
/**
 * @file ncrunch.h
 * @author Andrew Fields
 *
 * This file contains all of the main function and structure declarations
 * for the program.
 */

#pragma once

#include <ncrunch/hash.h>

#define NCRUNCH_VERSION_MAJOR 1
#define NCRUNCH_VERSION_MINOR 0

/******************************************************************************
 * TFL FUNCTIONS AND STRUCTURES
 * Functions defined in teams.c
 ******************************************************************************
 */

/**
 * @enum tfl_type
 * @brief The field types for the team field list (tfl)
 *
 * The types are used for the tfl, which also defines the type for each
 * of the team's entries.
 */

enum tfl_type {
	TEAM_FIELD_INVALID = 0,	/* this needs to be 0 (calloc initialized array) */
	TEAM_FIELD_STRING,
	TEAM_FIELD_DOUBLE
};

/**
 * @struct tfl_entry
 * @brief The structure used to keep track of the mapping from field name
 * to type, and vice-versa.
 *
 * Fields:\n
 * - name: The name of the field. This will be set from the top line of the flatf,
 *   which is the heading line. The name is copied, so it needs to be free'd.
 * - type: The associated type for this field. It should only ever be TEAM_FIELD_STRING
 *   or TEAM_FIELD_DOUBLE. TEAM_FIELD_INVALID is only used for error-checking purposes.
 *   The type is determined by the first team entry's field types.
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
void tfl_destroy(void);

/******************************************************************************
 * TEAM FUNCTIONS AND STRUCTURES
 * Functions defined in teams.c
 ******************************************************************************
 */

#define TEAMS_MAXTEAMS	64
#define TEAMS_INVALID	TEAMS_MAXTEAMS

/**
 * @union team_field
 * @brief Container for the field data.
 *
 * The team_field union contains the actual data for one of a team's fields.
 * The field's type can be determined by checking the type of the equivalent
 * id in the tfl. For instance:\n
 * @code
 * size_t teamid = 0;
 * size_t fieldid = 1; //The field we want
 * enum tfl_type = tfl_get_type(fieldid);
 * if (type == TEAM_FIELD_STRING)
 * 	team_set_string(teamid, fieldid, "easy");
 * @endcode
 *
 * Fields:\n
 * - data_s: String data
 * - data_d: Numeric data
 */

union team_field {
	char *data_s;
	double data_d;
};

/**
 * @struct team
 * @brief A container for a team's fields
 *
 * Each team_field in fields corresponds to the same id in the tfl. The tfl
 * stores all of the type information.
 *
 * Fields:\n
 * - name: The hashed value of the name
 * - fields: The field list for the team
 */

struct team {
	struct mdigest name;
	union team_field *fields;
};

size_t team_create(void);
void team_destroy(size_t id);
int team_set_string(size_t id, size_t field, const char *str);
int team_set_double(size_t id, size_t field, double val);
void teams_destroy(void);
size_t teams_num_teams(void);

/******************************************************************************
 * FLAT FILE FUNCTIONS
 ******************************************************************************
 */

int flatf_read(const char *filename);
