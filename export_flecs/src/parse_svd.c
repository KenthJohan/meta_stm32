/*
https://jsonformatter.org/xml-viewer

*/
#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <flecs.h>
#include <mxml.h>
#include "parse_svd.h"
#include "printer.h"

typedef struct {
	mxml_node_t *name;
	mxml_node_t *description;
	mxml_node_t *registers;
	mxml_node_t *address;
	mxml_node_t *size;
} svd_peripheral_t;

typedef struct {
	mxml_node_t *name;
	mxml_node_t *description;
	mxml_node_t *fields;
	mxml_node_t *address;
	mxml_node_t *access;
	mxml_node_t *size;
} svd_register_t;

typedef struct {
	mxml_node_t *name;
	mxml_node_t *description;
	mxml_node_t *bitOffset;
	mxml_node_t *bitWidth;
	mxml_node_t *access;
} svd_field_t;

void iterate_fields(ecs_world_t *world, ecs_entity_t parent, mxml_node_t *node, mxml_node_t *top)
{
	mxml_node_t *child = mxmlGetFirstChild(node);
	while (child != NULL) {
		if (mxmlGetType(child) != MXML_TYPE_ELEMENT) {
			child = mxmlGetNextSibling(child);
			continue;
		}
		svd_field_t field;
		field.name = mxmlFindElement(child, top, "name", NULL, NULL, MXML_DESCEND_FIRST);
		field.description = mxmlFindElement(child, top, "description", NULL, NULL, MXML_DESCEND_FIRST);
		field.bitOffset = mxmlFindElement(child, top, "bitOffset", NULL, NULL, MXML_DESCEND_FIRST);
		field.bitWidth = mxmlFindElement(child, top, "bitWidth", NULL, NULL, MXML_DESCEND_FIRST);
		field.access = mxmlFindElement(child, top, "access", NULL, NULL, MXML_DESCEND_FIRST);
		ecs_entity_t f = 0;
		if (field.name) {
			char const *name = mxmlGetOpaque(field.name);
			f = ecs_entity_init(world,
			&(ecs_entity_desc_t){
			.parent = parent,
			.name = name,
			});
		}
		if (f) {
			if (field.description) {
				char const *brief = mxmlGetOpaque(field.description);
				if (brief) {
					ecs_doc_set_brief(world, f, brief);
				}
			}
			if (field.bitOffset && field.bitWidth) {
				// result_flecs_field(mxmlGetOpaque(field.bitOffset), mxmlGetOpaque(field.bitWidth), mxmlGetOpaque(field.access));
			}
		}
		child = mxmlGetNextSibling(child);
	}
}

void iterate_registers(ecs_world_t *world, ecs_entity_t parent, mxml_node_t *node, mxml_node_t *top)
{
	mxml_node_t *child = mxmlGetFirstChild(node);
	while (child != NULL) {
		if (mxmlGetType(child) != MXML_TYPE_ELEMENT) {
			child = mxmlGetNextSibling(child);
			continue;
		}
		svd_register_t regs;
		regs.name = mxmlFindElement(child, top, "name", NULL, NULL, MXML_DESCEND_FIRST);
		regs.description = mxmlFindElement(child, top, "description", NULL, NULL, MXML_DESCEND_FIRST);
		regs.fields = mxmlFindElement(child, top, "fields", NULL, NULL, MXML_DESCEND_FIRST);
		regs.address = mxmlFindElement(child, top, "addressOffset", NULL, NULL, MXML_DESCEND_FIRST);
		regs.access = mxmlFindElement(child, top, "access", NULL, NULL, MXML_DESCEND_FIRST);
		regs.size = mxmlFindElement(child, top, "size", NULL, NULL, MXML_DESCEND_FIRST);

		ecs_entity_t reg = 0;

		if (regs.name) {
			char const *name = mxmlGetOpaque(regs.name);
			reg = ecs_entity_init(world,
			&(ecs_entity_desc_t){
			.parent = parent,
			.name = name,
			});
		}

		if (reg) {
			if (regs.description) {
				char const *brief = mxmlGetOpaque(regs.description);
				if (brief) {
					ecs_doc_set_brief(world, reg, brief);
				}
			}
			if (regs.fields) {
				iterate_fields(world, reg, regs.fields, top);
			}
		}

		child = mxmlGetNextSibling(child);
	}
}

void iterate_peripherals(ecs_world_t *world, ecs_entity_t parent, mxml_node_t *node, mxml_node_t *top)
{
	mxml_node_t *child = mxmlGetFirstChild(node);
	while (child != NULL) {
		if (mxmlGetType(child) != MXML_TYPE_ELEMENT) {
			child = mxmlGetNextSibling(child);
			continue;
		}
		svd_peripheral_t peripheral;
		peripheral.name = mxmlFindElement(child, top, "name", NULL, NULL, MXML_DESCEND_FIRST);
		peripheral.address = mxmlFindElement(child, top, "baseAddress", NULL, NULL, MXML_DESCEND_FIRST);

		mxmlElementSetAttr(child, "name", mxmlGetOpaque(peripheral.name));
		char const *a = mxmlElementGetAttr(child, "derivedFrom");
		if (a) {
			mxml_node_t *g = mxmlFindElement(top, top, "peripheral", "name", a, MXML_DESCEND_ALL);
			if (g) {
				printf("Derived from %s\n", mxmlGetOpaque(mxmlFindElement(g, top, "name", NULL, NULL, MXML_DESCEND_FIRST)));
				peripheral.description = mxmlFindElement(g, top, "description", NULL, NULL, MXML_DESCEND_FIRST);
				peripheral.registers = mxmlFindElement(g, top, "registers", NULL, NULL, MXML_DESCEND_FIRST);
				mxml_node_t *ablock = mxmlFindElement(g, top, "addressBlock", NULL, NULL, MXML_DESCEND_FIRST);
				peripheral.size = mxmlFindElement(ablock, top, "size", NULL, NULL, MXML_DESCEND_FIRST);
			}
		} else {
			peripheral.description = mxmlFindElement(child, top, "description", NULL, NULL, MXML_DESCEND_FIRST);
			peripheral.registers = mxmlFindElement(child, top, "registers", NULL, NULL, MXML_DESCEND_FIRST);
			mxml_node_t *ablock = mxmlFindElement(child, top, "addressBlock", NULL, NULL, MXML_DESCEND_FIRST);
			peripheral.size = mxmlFindElement(ablock, top, "size", NULL, NULL, MXML_DESCEND_FIRST);
		}
		ecs_entity_t per = 0;
		if (peripheral.name) {
			char const *name = mxmlGetOpaque(peripheral.name);
			per = ecs_entity_init(world,
			&(ecs_entity_desc_t){
			.parent = parent,
			.name = name,
			});
		}
		if (per) {
			if (peripheral.description) {
				char const *brief = mxmlGetOpaque(peripheral.description);
				if (brief) {
					ecs_doc_set_brief(world, per, brief);
				}
			}
			if (peripheral.registers) {
				iterate_registers(world, per, peripheral.registers, top);
			}
		}

		child = mxmlGetNextSibling(child);
	}
}

void if_file_exists(char const *filename)
{
	FILE *f = fopen(filename, "r");
	if (f) {
		fclose(f);
	} else {
		printf("File not found: %s\n", filename);
		exit(1);
	}
}

int parse_svd(ecs_world_t *world, char const *filename, ecs_entity_t parent)
{
	mxml_node_t *tree;
	mxml_options_t *options = mxmlOptionsNew();
	mxmlOptionsSetTypeValue(options, MXML_TYPE_OPAQUE);
	if_file_exists(filename);
	tree = mxmlLoadFilename(NULL, options, filename);
	mxml_node_t *node = tree;
	node = mxmlFindElement(node, tree, "peripherals", NULL, NULL, MXML_DESCEND_ALL);
	iterate_peripherals(world, parent, node, tree);
	return tree != NULL;
}
