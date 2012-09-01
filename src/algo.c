
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>



static int algo_fd = -1;




/**
 *
 */

static void *_lua_realloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	(void) ud;
	(void) osize;

	if (nsize == 0) {
		free(ptr);
		return NULL;
	}

	else {
		return realloc(ptr, nsize);
	}
}


/**
 *
 */

static const char *_lua_read(lua_State *L, void *data, size_t *size)
{
	ssize_t num_read;


	num_read = read(algo_fd, data, 512);
	if (num_read < 0) {
		*size = 0;
		return NULL;
	}

	*size = (size_t) num_read;
	return (const char *) data;
}


int algo_exec(const char *script)
{
	lua_State *state;
	int err;
	char buf[512];


	state = lua_newstate(_lua_realloc, NULL);
	if (!state) {
		fprintf(stderr, "%s: could not create lua state\n", __func__);
		return -1;
	}

	algo_fd = open(script, O_RDONLY);
	if (algo_fd < 0) {
		fprintf(stderr, "%s: could not open file '%s'\n", __func__, script);
		return -2;
	}

	err = lua_load(state, _lua_read, buf, "Algo", NULL);
	if (err != LUA_OK) {
		fprintf(stderr, "%s: could not parse '%s'\n", __func__, script);
		return -3;
	}

	luaL_openlibs(state);
	lua_call(state, 0, 0);

	lua_close(state);

	close(algo_fd);
	algo_fd = -1;
	
	return 0;
}

