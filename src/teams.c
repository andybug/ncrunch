
#include <stdlib.h>
#include <assert.h>
#include <string.h>

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
	team_fields.num_fields = team_fields.num_fields;

	return 0;
}

int team_field_list_set_name(int id, const char *name)
{
	size_t index;

	if (id < 0)
		return -1;

	index = (size_t) id;
	if (index >= team_fields.num_fields)
		return -2;

	team_fields.field_name[index] = strdup(name);
	return 0;
}

int team_field_list_set_type(int id, enum team_field_type type)
{
	size_t index;

	if (id < 0)
		return -1;

	index = (size_t) id;
	if (index >= team_fields.num_fields)
		return -2;

	team_fields.field_type[index] = type;
	return 0;
}

const char *team_field_list_get_name(int id)
{
	size_t index;

	if (id < 0)
		return NULL;

	index = (size_t) id;
	if (index >= team_fields.num_fields)
		return NULL;

	return team_fields.field_name[index];
}

enum team_field_type team_field_list_get_type(int id)
{
	size_t index;

	if (id < 0)
		return TEAM_FIELD_INVALID;

	index = (size_t) id;
	if (index >= team_fields.num_fields)
		return TEAM_FIELD_INVALID;

	return team_fields.field_type[index];
}

