
/**
 * @file main.c
 * @author Andrew Fields
 *
 * Entry point of the program. The arguments are processed and each step of the
 * data translations are controlled from here.\n
 *
 * The basic flow is:\n
 * arguments -> flatf -> algo -> database -> web
 *
 * @see main()
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include <ncrunch/ncrunch.h>

/**
 * @struct switch_handler
 * @brief Maps a switch character (-x) to a function that handles it
 *
 * Fields:\n
 * - _switch: The character that is read from the command line ('a' for switch -a)
 * - takes_arg:True if the switch expects to receive the next argument as input
 *   (-f file.txt)
 * - handler: The function that will be called when the switch is passed on the
 *   command line. If takes_arg is set, the next argument will be passed in as
 *   a string to the handler. (-f file.txt -> _f_switch_handler(str = file.txt))
 */

struct switch_handler {
	char _switch;
	unsigned char takes_arg;
	void (*handler) (const char *);
};

/* prototypes for handler functions */

static void _switch_version(const char *arg);
static void _switch_flatf(const char *arg);

/**
 * @brief The list of argument handlers
 *
 * This list maps each switch to its handler function
 */

static struct switch_handler handlers[] = {
	{._switch = 'v',.takes_arg = 0,.handler = _switch_version},
	{._switch = 'f',.takes_arg = 1,.handler = _switch_flatf},
	{._switch = 0,.takes_arg = 1,.handler = _switch_flatf},
	{._switch = 27,.takes_arg = 0,.handler = NULL}
};

/**
 * @brief Set to the switch_handler that is currently expecting a param
 *
 * If a switch handler has the takes_arg fields set and is called, the active_switch
 * variable will point to that handler so that the next argument from the command
 * line will be processed by the preceding switch's handler
 */

static struct switch_handler *active_switch = NULL;

/**
 * @brief The name of the flat file from the command line
 *
 * It can be set with either the -f flag or simply ncrunch [flatf]
 */

static const char *flatf_name = NULL;

/**
 * @brief Handles the version switch and exits
 */

static void _switch_version(const char *arg)
{
	if (arg) {
		fprintf(stderr, "%s: -v does not take an argument\n", __func__);
		exit(EXIT_FAILURE);
	}
#ifdef NCRUNCH_RELEASE
	int major = NCRUNCH_VERSION_MAJOR;
	int minor = NCRUNCH_VERSION_MINOR;

	printf("ncrunch v%d.%d", minor, major);
	printf(" by Andrew Fields - 2012\n");
#else
	printf("ncrunch debug build: built %s %s\n", __DATE__, __TIME__);
#endif

	exit(EXIT_SUCCESS);
}

/**
 * @brief Handles the flatf name switch/default case
 */

static void _switch_flatf(const char *arg)
{
	flatf_name = arg;
}

/**
 * @brief Finds the handler that handles the switch given
 */

static struct switch_handler *_find_handler(char _switch)
{
	size_t i = 0;

	while (handlers[i]._switch != 27) {
		if (handlers[i]._switch == _switch)
			return &handlers[i];
		i++;
	}

	fprintf(stderr, "%s: '%c' is not a valid switch\n", __func__, _switch);
	exit(EXIT_FAILURE);

	return NULL;
}

/**
 * @brief Handles a simple switch (-a)
 *
 * Sets this switch to the active_switch if it expects an argument to follow,
 * otherwise the handler for the switch is called
 *
 * @param _switch The single switch from the command line (-x)
 */

static void _process_single_switch(const char *_switch)
{
	struct switch_handler *handler;

	handler = _find_handler(_switch[1]);

	if (handler->takes_arg) {
		active_switch = handler;
	} else {
		handler->handler(NULL);
	}
}

/**
 * @brief Handles a switch that has multiple flags (-abc)
 *
 * Each switch in the chain is checked to ensure that they do not take an
 * argument, then the handler for each is called
 *
 * @param _switch The switch chain from the command line
 */

static void _process_chained_switch(const char *_switch)
{
	struct switch_handler *handler;
	const char *switchstr = &_switch[1];

	while (*switchstr) {
		handler = _find_handler(*switchstr);

		if (handler->takes_arg) {
			fprintf(stderr,
				"%s: can't chain switches that require arguments\n",
				__func__);
			exit(EXIT_FAILURE);
		}

		handler->handler(NULL);
		switchstr++;
	}
}

/**
 * @brief Handles a data argument
 *
 * If there is an active switch (one that is waiting for a data argument), the
 * handler for the active switch is called with str for its data. Otherwise,
 * the default data handler is called (_find_handler(0))
 *
 * @param str The data argument
 */

static void _process_non_switch(const char *str)
{
	struct switch_handler *handler;

	if (active_switch) {
		/* if a switch is expecting an argument, give it to them */
		active_switch->handler(str);
		active_switch = NULL;
	} else {
		/* otherwise, call default handler */
		handler = _find_handler(0);
		handler->handler(str);
	}
}

/**
 * @brief Handles the processing of a switch from the command line (e.g. -x)
 *
 * Determines whether the switch is chained (-abc) or single (-f), then calls
 * the functions that handle these types
 *
 * @arg _switch The enite argument from the command line ("-abc")
 */

static void _process_switch(const char *_switch)
{
	size_t len;

	if (active_switch) {
		/* can't have another switch when expecting an argument */
		fprintf(stderr, "%s: switch '%c' expected argument to follow\n",
			__func__, active_switch->_switch);
		exit(EXIT_FAILURE);
	}

	len = strlen(_switch);

	if (len > 2) {
		/* more than one flag grouped together */
		_process_chained_switch(_switch);
	} else if (len == 2) {
		/* just one flag */
		_process_single_switch(_switch);
	} else {
		/* not valid */
		fprintf(stderr, "WTF\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Handles the processing of flags and file names passed to the program
 *
 * First determines if each arg is a switch or non-switch, then calls the 
 * appropriate processing functions
 *
 * @param argc Number of arguments, including called location (argv[0])
 * @param argv Vector of string arguments
 */

static void _process_args(int argc, char **argv)
{
	int arg = 1;

	while (arg < argc) {

		if (argv[arg][0] == '-') {
			_process_switch(argv[arg]);
		} else {
			_process_non_switch(argv[arg]);
		}

		arg++;
	}
}

/**
 * @brief Callback for the atexit() function, cleans up allocations
 *
 * Only used in debug mode to make sure we aren't losing any allocations
 */

static void _exit_handler(void)
{
#ifdef NCRUNCH_DEBUG
	teams_destroy();
	tfl_destroy();
#endif
}

int main(int argc, char **argv)
{
	int error;

	_process_args(argc, argv);

	/* install our exit callback function */
	atexit(_exit_handler);
	error = flatf_read(flatf_name);

	return error;
}
