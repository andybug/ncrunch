
/**
 * @file teams.c
 * @author Andrew Fields
 *
 * This file contains all of the controlling functions for the team fields list
 * (aka tfl) and the teams list. The functions are declared in ncrunch.h.
 *
 * @see ncrunch.h
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <ncrunch/ncrunch.h>

#define TFL_MAXFIELDS  16

static struct tfl_entry *tfl = NULL;
static size_t num_fields = 0;

static struct team teams[TEAMS_MAXTEAMS];
static size_t num_teams = 0;

/**
 * @brief Allocates the team field list for the specified number of fields
 *
 * @retval 0 Success
 */

int tfl_create(size_t num_fields_)
{
	assert(num_fields == 0);
	assert(num_fields_ <= TFL_MAXFIELDS);

	tfl = calloc(num_fields_, sizeof(struct tfl_entry));
	num_fields = num_fields_;

	return 0;
}

/**
 * @brief Get the number of fields in the team field list
 */

size_t tfl_num_fields(void)
{
	return num_fields;
}

/**
 * @brief Set the name of a field in the list
 *
 * @param id The id of the field
 * @param name The name for the field (Copied)
 * @return Negative on error
 * @retval 0 Success
 * @retval -1 Failure
 */

int tfl_set_name(size_t id, const char *name)
{
	if (id >= num_fields)
		return -1;

	tfl[id].name = strdup(name);
	return 0;
}

/**
 * @brief Set the type of a field in the list
 *
 * @param id The id of the field
 * @param type The type to set the field to
 * @return Negative on error
 * @retval 0 Success
 * @retval -1 Failure
 */

int tfl_set_type(size_t id, enum tfl_type type)
{
	if (id >= num_fields)
		return -1;

	tfl[id].type = type;
	return 0;
}

/**
 * @brief Get the name of a field
 *
 * @param id The field's id
 * @return Returns a const* to the field's name. The returned string should
 * not be modified. The string should not be free'd.
 * @retval NULL Failure
 */

const char *tfl_get_name(size_t id)
{
	if (id >= num_fields)
		return NULL;

	return tfl[id].name;
}

/**
 * @brief Get the type of a field
 *
 * @param id The field's id
 * @return The type of the field
 * @retval TEAM_FIELD_INVALID Error
 */

enum tfl_type tfl_get_type(size_t id)
{
	if (id >= num_fields)
		return TEAM_FIELD_INVALID;

	return tfl[id].type;
}

/**
 * @brief Locates a field by name
 *
 * @param name The name to match to a field name
 * @param id The id of a match, will be set if one is found
 * @return Negative if field not found
 * @retval 0 Success
 * @retval -1 Failure
 */

int tfl_find(const char *name, size_t * id)
{
	size_t i;

	for (i = 0; i < num_fields; i++) {
		/* FIXME: make this strcmpi - have to implement */
		if (strcmp(tfl[i].name, name) == 0) {
			*id = i;
			return 0;
		}
	}

	return -1;
}

/**
 * @brief Cleans up the allocations made for the team field list
 *
 * @attention Debug purposes only. This is used for Valgrind testing.
 */

void tfl_destroy(void)
{
	size_t i;

	if (!tfl) {
		/* not allocated */
		num_fields = 0;
		return;
	}

	for (i = 0; i < num_fields; i++) {
		free(tfl[i].name);
	}

	free(tfl);
	tfl = NULL;
	num_fields = 0;
}

/**
 * @brief Adds a team to the team list
 * 
 * @return Returns the id of the created team
 * @retval TEAMS_INVALID Error
 */

size_t team_create(void)
{
	struct team *team;
	size_t id = num_teams;

	if (num_teams >= TEAMS_MAXTEAMS) {
		fprintf(stderr, "%s: Too many teams! Max: %d\n", __func__,
			TEAMS_MAXTEAMS);
		return TEAMS_INVALID;
	}

	team = &teams[id];
	team->fields = calloc(num_fields, sizeof(union team_field));

	num_teams++;
	return id;
}

/**
 * @brief Cleans up the allocations for the team
 *
 * @attention Debug purposes only. Does not remove the team from the teams list.
 */

void team_destroy(size_t id)
{
	struct team *team;
	size_t i;

	if (id >= num_teams) {
		fprintf(stderr, "%s: id %lu out of range\n", __func__, id);
		return;
	}

	team = &teams[id];

	for (i = 0; i < num_fields; i++) {
		if (tfl[i].type == TEAM_FIELD_STRING)
			free(team->fields[i].data_s);
	}

	free(team->fields);
	memset(team, 0, sizeof(struct team));
}

/**
 * @brief Destroys each team that has been created
 *
 * @attention For debug purposes only.
 */

void teams_destroy(void)
{
	size_t id;

	for (id = 0; id < num_teams; id++) {
		team_destroy(id);
	}

	num_teams = 0;
	printf("Destroyed %lu team(s)\n", id);
}

/**
 * @brief Get the number of teams
 */

size_t teams_num_teams(void)
{
	return num_teams;
}

/**
 * @brief Sets a field in a team to a string value.
 *
 * @attention The string is copied.
 * @param id The team's id
 * @param field The field's id
 * @param str The string to be copied into the field
 * @return Returns negative on error
 * @retval 0 Success
 * @retval -1 Failure
 */

int team_set_string(size_t id, size_t field, const char *str)
{
	struct team *team;
	enum tfl_type type;

	if (id >= num_teams) {
		fprintf(stderr, "%s: id %lu out of range\n", __func__, id);
		return -1;
	}

	team = &teams[id];
	type = tfl_get_type(field);

	if (type == TEAM_FIELD_INVALID) {
		fprintf(stderr, "%s: invalid field id %lu\n", __func__, field);
		return -1;
	} else if (type == TEAM_FIELD_DOUBLE) {
		fprintf(stderr, "%s: trying to write string to double field\n",
			__func__);
		return -1;
	}

	team->fields[field].data_s = strdup(str);
	return 0;
}

/**
 * @brief Sets a field in a team to a double value
 *
 * @param id The team's id
 * @param field The field's id
 * @param val The value to set the field to
 * @return Returns negative on error
 * @retval 0 Success
 * @retval -1 Failure
 */

int team_set_double(size_t id, size_t field, double val)
{
	struct team *team;
	enum tfl_type type;

	if (id >= num_teams) {
		fprintf(stderr, "%s: id %lu out of range\n", __func__, id);
		return -1;
	}

	team = &teams[id];
	type = tfl_get_type(field);

	if (type == TEAM_FIELD_INVALID) {
		fprintf(stderr, "%s: invalid field id %lu\n", __func__, field);
		return -1;
	} else if (type == TEAM_FIELD_STRING) {
		fprintf(stderr, "%s: trying to write double to string field\n",
			__func__);
		return -1;
	}

	team->fields[field].data_d = val;
	return 0;
}
