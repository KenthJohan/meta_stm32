#include "parse_mcu.h"
#include <mxml.h>
#include <Ec.h>
#include "str.h"

static ecs_entity_t lookup_gpio(ecs_world_t *world, char const *xml_name)
{
	char name[64] = {0};
	str_copy_gpio_name(name, xml_name);
	char buf[256] = {0};
	snprintf(buf, sizeof(buf), "gpios.%s", name);
	ecs_entity_t gpio = ecs_entity_init(world,
	&(ecs_entity_desc_t){
	.name = buf,
	});
	return gpio;
}

static ecs_entity_t lookup_pad(ecs_world_t *world, ecs_entity_t parent, char const *xml_position)
{
	char padpos[64] = {0};
	str_copy_position(padpos, xml_position);
	char buf[256] = {0};
	snprintf(buf, sizeof(buf), "pad_%s", padpos);
	ecs_entity_t pad = ecs_entity_init(world,
	&(ecs_entity_desc_t){
	.parent = parent,
	.name = buf,
	});
	return pad;
}

static void iterate_pins(ecs_world_t *world, mxml_node_t *node, mxml_node_t *top)
{
	ecs_entity_t pads = ecs_entity_init(world, &(ecs_entity_desc_t){.name = "pads"});
	mxml_node_t *node1 = node;
	while (1) {
		node1 = mxmlFindElement(node1, top, "Pin", NULL, NULL, MXML_DESCEND_NONE);
		if (node1 == NULL) {
			break;
		}
		ecs_entity_t pad = lookup_pad(world, pads, mxmlElementGetAttr(node1, "Position"));

		char const * type = mxmlElementGetAttr(node1, "Type");
		if (strcmp(type, "I/O") == 0) {
			ecs_entity_t gpio = lookup_gpio(world, mxmlElementGetAttr(node1, "Name"));
			ecs_add_pair(world, pad, EcAttached, gpio);
		}
	}
}

int parse_mcu(ecs_world_t *world, char const *filename)
{
	mxml_node_t *tree;
	mxml_options_t *options = mxmlOptionsNew();
	mxmlOptionsSetTypeValue(options, MXML_TYPE_OPAQUE);
	tree = mxmlLoadFilename(NULL, options, filename);
	ecs_assert(tree != NULL, ECS_INTERNAL_ERROR, NULL);
	mxml_node_t *node = tree;
	node = mxmlFindElement(node, tree, "Mcu", NULL, NULL, MXML_DESCEND_ALL);
	ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);
	iterate_pins(world, mxmlGetFirstChild(node), tree);
	return tree != NULL;
}
