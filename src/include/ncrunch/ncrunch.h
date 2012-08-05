
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


union team_field {
	const char* data_s;
	double data_f;
};


/**
 *
 */

enum team_field_type {
	TEAM_FIELD_STR,
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
int team_field_list_set_name(int id, const char *name);
int team_field_list_set_type(int id, enum team_field_type type);
const char *team_field_list_get_name(int id);
enum team_field_type team_field_list_get_type(int id);


/**
 *
 */

struct team {
	const char* name;
	struct ncrunch_crypto_digest name_hash;

};


/* Functions for reading flat file that contains team data */


int ncrunch_flatf_read(const char* filename);


