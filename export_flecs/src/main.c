/*
https://jsonformatter.org/xml-viewer

*/
#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <Ec.h>
#include "parse_svd.h"
#include "parse_mcu.h"
#include "parse_modes.h"



int main(int argc, char *argv[])
{
	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsRest);
	ECS_IMPORT(world, FlecsStats);
	ECS_IMPORT(world, Ec);
	ecs_set(world, EcsWorld, EcsRest, {.port = 0});

	ecs_entity_t peripherals = ecs_entity_init(world, &(ecs_entity_desc_t){.name = "peripherals"});
	ecs_entity_t gpios = ecs_entity_init(world, &(ecs_entity_desc_t){.name = "gpios"});
	ecs_entity_t signals = ecs_entity_init(world, &(ecs_entity_desc_t){.name = "signals"});

	parse_svd(world, "../meta/svd/stm32c0/STM32C051.svd", peripherals);
	parse_modes(world, "../meta/IP/GPIO-STM32C051xx_gpio_v1_0_Modes.xml", gpios, signals);
	parse_mcu(world, "../meta/mcu/STM32C051F8Px.xml");

	ecs_query_t *q = ecs_query_init(world,
	&(ecs_query_desc_t){
	.terms = {
	{.id = ecs_pair(EcsChildOf, gpios)},
	{.id = ecs_pair(EcAttached, EcsWildcard), .oper = EcsNot},
	}});
	ecs_iter_t it = ecs_query_iter(world, q);
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			//printf("GPIO without attached signals: %s\n", ecs_get_name(world, e));
			ecs_doc_set_color(world, e, "#FF0000");
		}
	}
	ecs_query_fini(q);

	char *json = ecs_world_to_json(world, &(ecs_world_to_json_desc_t){
	                                      .serialize_builtin = false,
	                                      .serialize_modules = false,
	                                      });
	if (json) {
		FILE *f = fopen("output.json", "w");
		if (f) {
			fputs(json, f);
			fclose(f);
		}
		ecs_os_free(json);
	}

	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");
	while (1) {
		ecs_progress(world, 0.0f);
		ecs_sleepf(0.016f);
	}
	ecs_fini(world);
	return 0;
}
