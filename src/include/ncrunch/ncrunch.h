
#pragma once

#include <ncrunch/crypto.h>



#define NCRUNCH_VERSION_MAJOR 1
#define NCRUNCH_VERSION_MINOR 0


/* TODO: Make NCRUNCH_DEV and RELEASE be configured through CMake  */

#if (NCRUNCH_RELEASE == 1)
	#define NCRUNCH_DEBUG 0
#elif (NCRUNCH_DEBUG == 1)
	#define NCRUNCH_RELEASE 0
#endif





/**
 *
 */

enum tfl_type {
	TEAM_FIELD_INVALID = 0, /* this needs to be 0 (calloc initialized array) */
	TEAM_FIELD_STRING,
	TEAM_FIELD_DOUBLE
};


/**
 *
 */

struct tfl {
	char **names;
	enum tfl_type *types;
	size_t num_fields;
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
int tfl_find(const char *name, size_t *id);

int tfl_destroy(void);


/**
 *
 */

union team_field {
	char* data_s;
	double data_d;
};


/**
 *
 */

struct team {
	char *name;
	struct ncrunch_crypto_digest name_hash;
	union team_field *fields;
};

int team_create(const char *name, size_t *teamid);
int team_destroy(size_t id);
int team_set_string(size_t id, size_t field, const char *str);
int team_set_double(size_t id, size_t field, double val);

int teams_destroy(void);
size_t teams_num_teams(void);

/* Functions for reading flat file that contains team data */


int flatf_read(const char* filename);


