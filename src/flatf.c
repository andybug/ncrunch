
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <ncrunch/ncrunch.h>

#define FLATF_READBUFSIZE 256

/**
 * Opens a file and returns the file descriptor, or a negative to indicate 
 * an error.
 *
 * @return Negative if error
 */

static int _open_file(const char *filename)
{
	int fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {		/* error */
		/* TODO: get error from errno and put in nice print outs */
		return -1;
	}

	return fd;
}

/**
 * Simply closes a file...
 *
 * @return Negative if error
 */

static int _close_file(int fd)
{
	int err;

	err = close(fd);
	if (err) {
		exit(-1);	/* just die for now... */
		/* TODO: error checking */
		return -1;
	}

	return 0;
}

/**
 * Reads a line from the flat file and returns the number of characters read
 * A null-terminator is added to the end of the string
 * The max number of chars that can be read is therefore (len - 1)
 * 
 * TODO: memory map the file
 *
 * @param buffer The buffer that is to be read into
 * @return The number of characters read into the buffer
 */

static size_t _read_line(int fd, char *buffer)
{
	size_t count = 0;
	char c;

	while (read(fd, &c, 1)) {

		if (count == FLATF_READBUFSIZE) {	/* line won't fit in buffer */
			fprintf(stderr, "%s: Line too long for buffer!\n",
				__func__);
			return 0;
		}

		if (c == '\n') {	/* line is over, null-term. the string */
			buffer[count] = '\0';
			break;
		} else {
			buffer[count] = c;
			count++;
		}
	}

	return count;
}

/**
 * A token from the current line in the buffer
 *
 * The str pointer points into the buffer - DO NOT FREE IT
 */

struct token {
	const char *str;	/* pointer into buffer */
	struct token *next;
};

/**
 * Contains a list of tokens from a line in the flatf
 *
 * Create by calling _tokenize_line()
 * Cleaned up by calling _deallocate_tokens()
 */

struct tokenlist {
	struct token *head;
	size_t num_tokens;
};

/**
 * Tokenizes the line contained in a buffer into a token list
 *
 * Each token in the list will be allocated inside the function
 * Be sure to call _deallocate_tokens() afterwords
 *
 * @param buffer The buffer that contains a line to be tokenized, it will be modified
 * during the tokenization process
 *
 * @param list The container for the resulting tokens; the individual tokens
 * will be allocated as needed. Call _deallocate_tokens() when done to clean
 * up the list.
 *
 * @return The number of characters read, not including the null terminator
 */

static size_t _tokenize_line(char *buffer, struct tokenlist *list)
{
	struct token *token = NULL;
	size_t count = 0;
	char *str;

	/* go ahead and reset the list, they better have deallocated! */
	memset(list, 0, sizeof(struct tokenlist));

	str = strtok(buffer, "\t");

	while (str) {
		if (!list->head) {
			list->head = malloc(sizeof(struct token));
			token = list->head;
		} else {
			token->next = malloc(sizeof(struct token));
			token = token->next;
		}

		token->str = str;
		token->next = NULL;
		count++;

		str = strtok(NULL, "\t");
	}

	list->num_tokens = count;
	return count;
}

/**
 * Deallocates the tokens in a token list
 */

static void _deallocate_tokens(struct tokenlist *list)
{
	struct token *token = list->head;
	struct token *next;

	while (token) {
		next = token->next;
		free(token);
		token = next;
	}

	list->head = NULL;
	list->num_tokens = 0;
}

/**
 * Reads the first line of the flat file and adds each heading as a field in the
 * team fields list.
 *
 * @param buf The buffer to use for reading
 * @return The number of fields added to the team field list
 */

static size_t _read_fields_list(int fd, char *buf)
{
	size_t count;
	struct tokenlist list;
	struct token *token;
	size_t num_tokens;
	size_t i;

	count = _read_line(fd, buf);
	if (!count) {
		/* no data or too much data! */
		fprintf(stderr, "%s: failed to read fields line\n", __func__);
		return 0;
	}

	num_tokens = _tokenize_line(buf, &list);
	if (!num_tokens) {
		/* not formatted correctly */
		fprintf(stderr, "%s: fields line incorrectly formatted\n",
			__func__);
		_deallocate_tokens(&list);
		return 0;
	}

	tfl_create(num_tokens);
	token = list.head;

	for (i = 0; i < num_tokens; i++) {
		tfl_set_name(i, token->str);
		token = token->next;
	}

	_deallocate_tokens(&list);
	return num_tokens;
}

#if 0
/**
 * Finds the string name associated with the team's name
 *
 * @param list The list of tokens read from the flatf for this team
 * @return Returns the string name of the currently being read team
 */

static const char *_get_team_name(struct _token *list)
{
	size_t i;
	size_t nameid;
	int err;

	err = tfl_find("name", &nameid);
	if (err) {
		fprintf(stderr, "%s: could not find required 'name' field\n",
			__func__);
		return NULL;
	}

	for (i = 0; i < nameid; i++) {
		list = list->next;
	}

	return list->str;
}
#endif

/**
 * Checks whether a string consists only of alpha characters
 *
 * @param str The string to be tested
 * @return 1 if string contains only valid alpha chars; or 0 otherwise
 */

static int _isAlpha(const char *str)
{
	while (*str) {
		if (!isalpha(*str) && !ispunct(*str) && *str != ' ') {
			return 0;
		}

		str++;
	}

	return 1;
}

/**
 * Returns whether a string consists only of numbers
 *
 * @param str The string to be tested
 * @return 1 if string contains only valid number chars; or 0 otherwise
 */

static int _isNumeric(const char *str)
{
	while (*str) {
		if (!isdigit(*str) && *str != '.') {
			return 0;
		}

		str++;
	}

	return 1;
}

/**
 * Sets a team's field to a string value
 *
 * @param teamid The team that contains the field
 * @param fieldid The field that is to be set
 * @param str The string value to set the field to (copy)
 * @return Negative on error
 */

static int _set_alpha_field(size_t teamid, size_t fieldid, const char *str)
{
	enum tfl_type type = tfl_get_type(fieldid);

	if (type == TEAM_FIELD_DOUBLE) {
		fprintf(stderr, "%s: token '%s' is not numeric!\n", __func__,
			str);
		return -1;
	} else if (type == TEAM_FIELD_INVALID) {
		tfl_set_type(fieldid, TEAM_FIELD_STRING);
	}

	team_set_string(teamid, fieldid, str);

	return 0;
}

/**
 * Sets a team's field to a double value
 *
 * @param teamid The team that contains the field
 * @param fieldid The field that is to be set
 * @param str The string containing a numeric value
 * @return Negative on error
 */

static int _set_numeric_field(size_t teamid, size_t fieldid, const char *str)
{
	double conv;
	enum tfl_type type = tfl_get_type(fieldid);

	if (type == TEAM_FIELD_STRING) {
		fprintf(stderr, "%s: token '%s' is not alpha!\n", __func__,
			str);
		return -2;
	} else if (type == TEAM_FIELD_INVALID) {
		tfl_set_type(fieldid, TEAM_FIELD_DOUBLE);
	}

	conv = atof(str);
	team_set_double(teamid, fieldid, conv);

	return 0;
}

/**
 * Sets a team's field values from the tokenized field listing
 *
 * @param list The tokens from the team's line in the flatf file
 * @param teamid The team to have its fields set
 * @return Negative on error
 */

static int _set_fields(const struct tokenlist *list, size_t teamid)
{
	size_t id = 0;
	int err = 0;
	struct token *token = list->head;

	while (token) {
		if (_isAlpha(token->str)) {
			err = _set_alpha_field(teamid, id, token->str);
		} else if (_isNumeric(token->str)) {
			err = _set_numeric_field(teamid, id, token->str);
		} else {
			fprintf(stderr, "%s: illegal value '%s'\n", __func__,
				token->str);
			err = -3;
		}

		if (err)
			break;

		token = token->next;
		id++;
	}

	return err;
}

/**
 * Retrieves a new teamid and has the fields set from the tokenized line
 *
 * @param list The tokenized line representing a team from the flatf
 * @return Negative on error
 */

static int _create_team(const struct tokenlist *list)
{
	int err;
	size_t teamid;

	teamid = team_create();
	if (teamid == TEAMS_INVALID) {
		fprintf(stderr, "%s: unable to create team\n", __func__);
		return -2;
	}

	err = _set_fields(list, teamid);
	if (err) {
		fprintf(stderr, "%s: unable to read field for team\n",
			__func__);
		return -3;
	}

	return 0;
}

/**
 * Reads a line from the file and interprets it as team data.
 *
 * @param buf The buffer to use for reading
 * @return The difference between the number of fields read and the number of fields
 * in the team field list
 */

static size_t _read_team(int fd, char *buf)
{
	size_t count;
	size_t toread = tfl_num_fields();
	struct tokenlist list;
	struct token *token;
	size_t num_tokens;
	size_t i;

	count = _read_line(fd, buf);
	if (!count) {
		return toread;
	}

	num_tokens = _tokenize_line(buf, &list);
	if (num_tokens != toread) {
		fprintf(stderr, "%s: team only has %lu/%lu fields\n", __func__,
			num_tokens, toread);
		_deallocate_tokens(&list);
		return (num_tokens - toread);
	}

	token = list.head;
	for (i = 0; i < num_tokens; i++) {
		token = token->next;
	}

	_create_team(&list);
	_deallocate_tokens(&list);
	return 0;
}

/**
 * Reads the flat file; The first line of the file is interpreted as the field list
 * (since the headings of the columns correspond to the data below). The team field
 * list is populated from this line. The rest of the lines are used to fill out the
 * individual teams.
 *
 * @return Negative on error
 */

int flatf_read(const char *filename)
{
	int fd;
	char buf[FLATF_READBUFSIZE];
	size_t count;
	size_t diff;

	fd = _open_file(filename);
	if (fd < 0) {
		fprintf(stderr, "%s: could not open file '%s'\n", __func__,
			filename);
		return -1;
	}

	/* read heading line which contains the variable names */
	count = _read_fields_list(fd, buf);
	if (count == 0) {
		return -2;
	}

	do {
		diff = _read_team(fd, buf);
	} while (!diff);

	_close_file(fd);

	return 0;
}
