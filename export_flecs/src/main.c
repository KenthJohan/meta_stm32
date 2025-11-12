/*
https://jsonformatter.org/xml-viewer

*/
#include <stdlib.h>
#include <stdio.h>
#include <flecs.h>
#include <Ec.h>
#include "parse_svd.h"
#include "parse_modes.h"
#include "printer.h"

#include <mxml.h>



void generate_file()
{
	ecs_world_t *world = ecs_mini();
	result_t result = {0};
	result.color_fields = "#6b93d6";
	result.color_peripherals = "#9b2316";
	result.color_registers = "#2b6316";
	result.doc_mode = 1;
	result.file = fopen("../meta_flecs/STM32C051.flecs", "w");
	if (result.file == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	fprintf(result.file, "@color #AA99AF\n");
	fprintf(result.file, "xmcu {}\n");
	fprintf(result.file, "module xmcu\n");
	fprintf(result.file, "module STM32C051\n\n");
	parse_svd_init(&result, "../meta/svd/stm32c0/STM32C051.svd");
	//parse_modes_init(world, &result, "config/GPIO-STM32G03x_gpio_v1_0_Modes.xml");
	ecs_fini(world);
	fclose(result.file);
}

const char* find_last_slash(const char* str)
{
	if (str == NULL) {
		return NULL;
	}
	
	const char* last_slash = NULL;
	for (const char* p = str; *p != '\0'; p++) {
		if (*p == '/') {
			last_slash = p;
		}
	}
	
	return last_slash;
}

void read_file_line_by_line(const char* filename)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error opening file: %s\n", filename);
		return;
	}
	
	char line[1024];
	while (fgets(line, sizeof(line), file)) {
		printf("%s", line);
	}
	
	fclose(file);
}


int main(int argc, char *argv[])
{
	generate_file();

	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsRest);
	ECS_IMPORT(world, FlecsStats);
	ECS_IMPORT(world, Ec);
	ecs_script_run_file(world, "../meta_flecs/STM32C051.flecs");
	ecs_set(world, EcsWorld, EcsRest, {.port = 0});

	read_file_line_by_line("../meta/svds.txt");

	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");
	while (1) {
		ecs_progress(world, 0.0f);
		ecs_sleepf(0.016f);
	}
	ecs_fini(world);
	return 0;
}
