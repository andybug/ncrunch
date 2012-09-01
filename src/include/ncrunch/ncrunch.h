
#pragma once

#include <ncrunch/hash.h>

#define NCRUNCH_VERSION_MAJOR 1
#define NCRUNCH_VERSION_MINOR 0

/**
 *
 */

enum tfl_type {
	TEAM_FIELD_INVALID = 0,	/* this needs to be 0 (calloc initialized array) */
	TEAM_FIELD_STRING,
	TEAM_FIELD_DOUBLE
};

/**
 *
 */

struct tfl_entry {
	char *name;
	enum tfl_type type;
};

int tfl_create(size_t num_fields);
size_t tfl_num_fields(void);
int tfl_set_name(size_t id, const char *name);
int tfl_set_type(size_t id, enum tfl_type type);
const char *tfl_get_name(size_t id);
enum tfl_type tfl_get_type(size_t id);
int tfl_find(const char *name, size_t * id);

int tfl_destroy(void);

#define TEAMS_MAXTEAMS	64
#define TEAMS_INVALID	TEAMS_MAXTEAMS

/**
 *
 */

union team_field {
	char *data_s;
	double data_d;
};

/**
 *
 */

struct team {
	struct mdigest name;
	union team_field *fields;
};

size_t team_create(void);
int team_destroy(size_t id);
int team_set_string(size_t id, size_t field, const char *str);
int team_set_double(size_t id, size_t field, double val);

int teams_destroy(void);
size_t teams_num_teams(void);

/* Functions for reading flat file that contains team data */

int flatf_read(const char *filename);
