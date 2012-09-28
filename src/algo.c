
/**
 * @file algo.c
 * @author Andrew Fields
 *
 * This file handles the interactions with the Lua library and the algorithm
 * scripts. The basic idea is that the team data is read into the C program,
 * which then gets the information to the processing algorithm through this
 * file, which makes use of the Lua API. In addition, this file handles getting
 * the generated information back from the scripts and into the C program.
 *
 * @see algo_exec
 */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>

#include <ncrunch/ncrunch.h>

static int algo_fd = -1;

/**
 * @brief The allocation function used by the Lua state
 *
 * Maps Lua realloc calls to the stdlib's realloc function
 *
 * @param ud Unused
 * @param ptr The pointer to reallocate
 * @param osize Unused
 * @param nsize The new size for the allocation
 * @return The realloc'd pointer
 * @retval NULL Error
 */

static void *_lua_realloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	(void)ud;
	(void)osize;

	if (nsize == 0) {
		free(ptr);
		return NULL;
	} else {
		return realloc(ptr, nsize);
	}
}

/**
 * @brief Handles reads for the Lua state
 *
 * Maps Lua read calls to Unix's read function
 *
 * @param L The Lua state
 * @param data The output buffer
 * @param size Set to the number of bytes read
 * @return The string value that was read (pointer into data)
 * @retval NULL Error
 */

static const char *_lua_read(lua_State * L, void *data, size_t * size)
{
	ssize_t num_read;

	num_read = read(algo_fd, data, 512);
	if (num_read < 0) {
		*size = 0;
		return NULL;
	}

	*size = (size_t) num_read;
	return (const char *)data;
}

/**
 * @brief Pushes a team onto the Lua stack
 *
 * Adds a team onto the Lua stack to be used as an entry in the teams
 * array. This array is what the Lua scripts operate on
 *
 * @param state The Lua state
 * @param id The id of the team to push
 */

static void _push_team(lua_State * state, size_t id)
{
	const char *fieldname;
	enum tfl_type fieldtype;
	size_t fieldid;
	size_t num_fields;
	double val_d;
	const char *val_s;

	lua_newtable(state);

	num_fields = tfl_num_fields();
	for (fieldid = 0; fieldid < num_fields; fieldid++) {
		fieldname = tfl_get_name(fieldid);
		fieldtype = tfl_get_type(fieldid);

		lua_pushstring(state, fieldname);
		if (fieldtype == TEAM_FIELD_STRING) {
			val_s = team_get_string(id, fieldid);
			lua_pushstring(state, val_s);
		} else if (fieldtype == TEAM_FIELD_DOUBLE) {
			val_d = team_get_double(id, fieldid);
			lua_pushnumber(state, val_d);
		} else {
			fprintf(stderr, "%s: field has unknown type\n",
				__func__);
			lua_pushnumber(state, 0.0);
		}

		lua_rawset(state, -3);
	}
}

/**
 * @brief Creates the teams table on the Lua stack
 *
 * A table containing all of the teams and their fields is pushed onto the Lua
 * stack.
 *
 * @param state The Lua state
 */

static void _push_teams(lua_State * state)
{
	size_t id;
	size_t num_teams;

	lua_newtable(state);

	num_teams = teams_num_teams();
	for (id = 0; id < num_teams; id++) {
		lua_pushnumber(state, id + 1);
		_push_team(state, id);
		lua_rawset(state, -3);
	}

	lua_setglobal(state, "teams");
}

/**
 * @brief Executes an algo script
 *
 * The script is loaded and called with the teams table as a parameter. The
 * script can use the teams table for calculation and can return a result.
 * FIXME output data!
 *
 * @param script The script to execute, set by -a flag
 * @return Returns negative on error
 */

int algo_exec(const char *script)
{
	lua_State *state;
	int err;
	char buf[512];
	double ret;

	state = lua_newstate(_lua_realloc, NULL);
	if (!state) {
		fprintf(stderr, "%s: could not create lua state\n", __func__);
		return -1;
	}

	luaL_openlibs(state);

	algo_fd = open(script, O_RDONLY);
	if (algo_fd < 0) {
		fprintf(stderr, "%s: could not open file '%s'\n", __func__,
			script);
		return -2;
	}

	err = lua_load(state, _lua_read, buf, "Algo", NULL);
	if (err != LUA_OK) {
		fprintf(stderr, "%s: could not parse '%s'\n", __func__, script);
		return -3;
	}

	_push_teams(state);
	lua_call(state, 0, 1);

	ret = lua_tonumber(state, -1);
	printf("Lua: %f\n", ret);

	lua_pop(state, 1);
	lua_close(state);
	close(algo_fd);
	algo_fd = -1;

	return 0;
}
