/*
https://jsonformatter.org/xml-viewer
*/
#include "parse_modes.h"
#include <mxml.h>
#include "str.h"
#include <Ec.h>



/*
<PinSignal Name="I2S1_CK"> : node
    <SpecificParameter Name="GPIO_AF">
        <PossibleValue>(return this)</PossibleValue>
    </SpecificParameter>
</PinSignal>
*/
static char const *node_extract_af(mxml_node_t *node, mxml_node_t *top)
{
	node = mxmlFindElement(node, top, "SpecificParameter", "Name", "GPIO_AF", MXML_DESCEND_ALL);
	if (node == NULL) {
		return "";
	}
	node = mxmlFindElement(node, top, "PossibleValue", NULL, NULL, MXML_DESCEND_ALL);
	if (node == NULL) {
		return "";
	}
	return mxmlGetOpaque(node);
}

static void node_get_uniqie_signals(ecs_world_t *world, ecs_entity_t parent, ecs_entity_t c, mxml_node_t *node, mxml_node_t *top)
{
	while (1) {
		node = mxmlFindElement(node, top, "PinSignal", NULL, NULL, MXML_DESCEND_NONE);
		if (node == NULL) {
			break;
		}
		char const *signame = mxmlElementGetAttr(node, "Name");
		ecs_entity_t e = ecs_entity_init(world,
		&(ecs_entity_desc_t){
		.parent = parent,
		.name = signame,
		.add = (ecs_id_t[]){c, 0},
		});
	}
}

static void node_print_signals(ecs_world_t *world, ecs_entity_t parent, mxml_node_t *node, mxml_node_t *top)
{
	ecs_entity_t c = ecs_new(world);
	mxml_node_t *node1 = node;
	while (1) {
		node1 = mxmlFindElement(node1, top, "GPIO_Pin", NULL, NULL, MXML_DESCEND_NONE);
		if (node1 == NULL) {
			break;
		}
		node_get_uniqie_signals(world, parent, c, mxmlGetFirstChild(node1), top);
	}

}

/*
<PinSignal Name="EVENTOUT"></PinSignal> : node iteration 1
<PinSignal Name="I2C1_SMBA"></PinSignal> : node iteration 2
<PinSignal Name="I2S1_CK"></PinSignal> : node iteration 3
*/
static void node_print_pins_af(ecs_world_t *world, ecs_entity_t parent, mxml_node_t *node, mxml_node_t *top)
{
	while (1) {
		node = mxmlFindElement(node, top, "PinSignal", NULL, NULL, MXML_DESCEND_NONE);
		if (node == NULL) {
			break;
		}
		char const *signame = mxmlElementGetAttr(node, "Name");
		char afstr[16] = {0};
		str_copy_gpioaf(afstr, sizeof(afstr), node_extract_af(node, top));
		char path[256] = {0};
		snprintf(path, sizeof(path), "signals.%s", signame);
		ecs_entity_t sig = ecs_entity_init(world,
		&(ecs_entity_desc_t){
		.name = path,
		});
		snprintf(path, sizeof(path), "%s_%s", afstr, signame);
		ecs_entity_t af = ecs_entity_init(world,
		&(ecs_entity_desc_t){
		.parent = parent,
		.name = path,
		.add = (ecs_id_t[]){ecs_pair(EcAttached, sig), 0},
		});
	}
}

static void node_print_gpios(ecs_world_t *world, ecs_entity_t parent, mxml_node_t *node, mxml_node_t *top)
{
	mxml_node_t *node1 = node;
	while (1) {
		node1 = mxmlFindElement(node1, top, "GPIO_Pin", NULL, NULL, MXML_DESCEND_NONE);
		if (node1 == NULL) {
			break;
		}
		char buf[256] = {0};
		str_copy_gpio_name(buf, mxmlElementGetAttr(node1, "Name"));
		ecs_entity_t gpio = ecs_entity_init(world,
		&(ecs_entity_desc_t){
		.parent = parent,
		.name = buf,
		});
		node_print_pins_af(world, gpio, mxmlGetFirstChild(node1), top);
	}
}

int parse_modes(ecs_world_t *world, char const *filename, ecs_entity_t gpios, ecs_entity_t signals)
{
	mxml_node_t *tree;
	mxml_options_t *options = mxmlOptionsNew();
	mxmlOptionsSetTypeValue(options, MXML_TYPE_OPAQUE);
	tree = mxmlLoadFilename(NULL, options, filename);
	if (tree == NULL) {
		return -1;
	}


	mxml_node_t *node = tree;
	node = mxmlFindElement(node, tree, "IP", NULL, NULL, MXML_DESCEND_ALL);

	node_print_signals(world, signals, mxmlGetFirstChild(node), tree);
	node_print_gpios(world, gpios, mxmlGetFirstChild(node), tree);

	return 0;
}
