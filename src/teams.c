
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <ncrunch/ncrunch.h>


#define TEAMS_MAXTEAMS 64


static struct team_field_list team_fields = {
			.field_name = NULL,
			.field_type = NULL,
			.num_fields = 0 };


static struct team teams[TEAMS_MAXTEAMS];
static size_t num_teams = 0; 



int team_field_list_create(size_t num_fields)
{
	assert(team_fields.num_fields == 0);

	team_fields.field_name = calloc(num_fields, sizeof(char*));
	team_fields.field_type = calloc(num_fields, sizeof(char*));
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


/**
 *
 */

int team_create(size_t id, const char *name)
{
	struct team *team;

	if (id >= TEAMS_MAXTEAMS) {
		fprintf(stderr, "%s: Too many teams! Max: %d\n", __func__, TEAMS_MAXTEAMS);
		return -1;
	}

	team = &teams[id];
	team->name = strdup(name);
	ncrunch_crypto_hash_string(name, 0, &team->name_hash);
	team->fields = calloc(team_fields.num_fields, sizeof(union team_field));

	num_teams++;
	return 0;
}


/**
 * Cleans up the allocations for the team
 *
 * DEBUG only
 */

int team_destroy(size_t id)
{
	struct team *team;

	if (id >= num_teams) {
		fprintf(stderr, "%s: id %lu out of range\n", __func__, id);
		return -1;
	}

	team = &teams[id];
	free(team->name);
	/* FIXME: iterate and free all string fields */
	free(team->fields);
	memset(team, 0, sizeof(struct team));

	return 0;
}


/**
 * Destroys each team that has been created
 *
 * DEBUG only
 */

int teams_destroy(void)
{
	size_t id;
	int err;

	for (id = 0; id < num_teams; id++) {
		err = team_destroy(id);

		if (err)
			return -1;
	}

	printf("Destroyed %lu team(s)\n", id);
	return 0;
}


/**
 *
 */

int team_set_string(size_t id, size_t field, const char *str)
{
	struct team *team;
	enum team_field_type type;

	if (id >= num_teams) {
		fprintf(stderr, "%s: id %lu out of range\n", __func__, id);
		return -1;
	}

	team = &teams[id];
	type = team_field_list_get_type(field);

	if (type == TEAM_FIELD_INVALID) {
		fprintf(stderr, "%s: invalid field id %lu\n", __func__, field);
		return -2;
	}

	else if (type == TEAM_FIELD_DOUBLE) {
		fprintf(stderr, "%s: trying to write string to double field\n", __func__);
		return -3;
	}

	team->fields[field].data_s = strdup(str);
	return 0;
}


/**
 *
 */

int team_set_double(size_t id, size_t field, double val)
{
	struct team *team;
	enum team_field_type type;

	if (id >= num_teams) {
		fprintf(stderr, "%s: id %lu out of range\n", __func__, id);
		return -1;
	}

	team = &teams[id];
	type = team_field_list_get_type(field);

	if (type == TEAM_FIELD_INVALID) {
		fprintf(stderr, "%s: invalid field id %lu\n", __func__, field);
		return -2;
	}

	else if (type == TEAM_FIELD_STRING) {
		fprintf(stderr, "%s: trying to write double to string field\n", __func__);
		return -3;
	}

	team->fields[field].data_d = val;
	return 0;
}

