
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <ncrunch/ncrunch.h>



static struct team_field_list team_fields = {
			.field_name = NULL,
			.field_type = NULL,
			.num_fields = 0 };


int team_field_list_create(size_t num_fields)
{
	assert(team_fields.num_fields == 0);

	team_fields.field_name = calloc(team_fields.num_fields, sizeof(char*));
	team_fields.field_type = calloc(team_fields.num_fields, sizeof(char*));
	team_fields.num_fields = num_fields;

	return 0;
}

size_t team_field_list_num_fields(void)
{
	return team_fields.num_fields;
}

int team_field_list_set_name(size_t id, const char *name)
{
	if (id >= team_fields.num_fields)
		return -1;

	team_fields.field_name[id] = strdup(name);
	return 0;
}

int team_field_list_set_type(size_t id, enum team_field_type type)
{
	if (id >= team_fields.num_fields)
		return -1;

	team_fields.field_type[id] = type;
	return 0;
}

const char *team_field_list_get_name(size_t id)
{
	if (id >= team_fields.num_fields)
		return NULL;

	return team_fields.field_name[id];
}

enum team_field_type team_field_list_get_type(size_t id)
{
	if (id >= team_fields.num_fields)
		return TEAM_FIELD_INVALID;

	return team_fields.field_type[id];
}

