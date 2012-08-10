
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

enum team_field_type {
	TEAM_FIELD_STRING,
	TEAM_FIELD_DOUBLE,
	TEAM_FIELD_INVALID
};


/**
 *
 */

struct team_field_list {
	const char **field_name;
	enum team_field_type *field_type;
	size_t num_fields;
};

int team_field_list_create(size_t num_fields);
size_t team_field_list_num_fields(void);
int team_field_list_set_name(size_t id, const char *name);
int team_field_list_set_type(size_t id, enum team_field_type type);
const char *team_field_list_get_name(size_t id);
enum team_field_type team_field_list_get_type(size_t id);


/**
 *
 */

union team_field {
	const char* data_s;
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

int team_create(size_t id, const char *name);
int team_destroy(size_t id);
int team_set_string(size_t id, size_t field, const char *str);
int team_set_double(size_t id, size_t field, double val);

int teams_destroy(void);

/* Functions for reading flat file that contains team data */


int flatf_read(const char* filename);


