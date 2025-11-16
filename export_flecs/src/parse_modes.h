#pragma once
#include <flecs.h>

int parse_modes(ecs_world_t *world, char const * filename, ecs_entity_t gpios, ecs_entity_t signals);