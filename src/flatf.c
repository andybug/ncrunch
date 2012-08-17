
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

static int _open_file(const char* filename)
{
	int fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {	/* error */
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
		exit(-1); /* just die for now... */
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

static size_t _read_line(int fd, char* buffer)
{
	size_t count = 0;
	char c;

	while (read(fd, &c, 1)) {

		if (count == FLATF_READBUFSIZE) {	/* line won't fit in buffer */
			fprintf(stderr, "%s: Line too long for buffer!\n", __func__);
			return 0;
		}

		if (c == '\n') {	/* line is over, null-term. the string */
			buffer[count] = '\0';
			break;
		}
		else {
			buffer[count] = c;
			count++;
		}
	}
			
	return count;
}


/**
 * Token structure for breaking up lines from the file
 * The str pointer points into the buffer - DO NOT FREE IT
 */

struct _token {
	const char* str;	/* pointer into buffer */
	struct _token* next;
};


/**
 * Tokenizes the line contained in a buffer into a token list
 * The token list is allocated in the function
 * Be sure to call _deallocate_tokens() afterwords
 *
 * @param buffer The buffer that contains a line to be tokenized, it will be modified
 * during the tokenization process
 *
 * @param token_list An unallocated token to be used as the resulting token list.
 * The token will be allocated inside the function
 *
 * @return The number of characters read, not including the null terminator
 */

static size_t _tokenize_line(char* buffer, struct _token** token_list)
{
	struct _token *list = NULL;
	struct _token *token = NULL;
	size_t count = 0;
	char *str;

	str = strtok(buffer, "\t");

	while (str) {
		if (!list) {
			list = malloc(sizeof(*list));
			token = list;
		} else {
			token->next = malloc(sizeof(struct _token));
			token = token->next;
		}

		token->str = str;
		token->next = NULL;
		count++;

		str = strtok(NULL, "\t");
	}

	if (count) {
		*token_list = list;
	}

	return count;
}


/**
 * Deallocates a token list
 */

static void _deallocate_tokens(struct _token **token_list)
{
	struct _token *token = *token_list;
	struct _token *next;

	while (token) {
		next = token->next;
		free(token);
		token = next;
	}

	*token_list = NULL;
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
	struct _token *tokens, *token;
	size_t num_tokens;
	size_t i;

	count = _read_line(fd, buf);
	if (!count) {
		/* no data or too much data! */
		fprintf(stderr, "%s: failed to read fields line\n", __func__);
		return 0;
	}

	num_tokens = _tokenize_line(buf, &tokens);
	if (!num_tokens) {
		/* not formatted correctly */
		fprintf(stderr, "%s: fields line incorrectly formatted\n", __func__);
		_deallocate_tokens(&tokens);
		return 0;
	}

	
	tfl_create(num_tokens);
	token = tokens;

	for (i = 0; i < num_tokens; i++) {
		tfl_set_name(i, token->str);
		token = token->next;
	}

	_deallocate_tokens(&tokens);
	return num_tokens;	
}


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
		fprintf(stderr, "%s: could not find required 'name' field\n", __func__);
		return NULL;
	}

	for (i = 0; i < nameid; i++) {
		list = list->next;
	}

	return list->str;
}


/**
 * Returns whether a string consists only of alpha characters
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
 *
 */

static int _set_fields(struct _token *list, size_t teamid)
{
	enum tfl_type type;
	size_t id = 0;
	double conv;

	while (list) {
		type = tfl_get_type(id);

		if (_isAlpha(list->str)) {
			if (type == TEAM_FIELD_DOUBLE) {
				fprintf(stderr, "%s: token '%s' is not numeric!\n", __func__, list->str);
				return -1;
			}

			else if (type == TEAM_FIELD_INVALID) {
				tfl_set_type(id, TEAM_FIELD_STRING);
			}

			team_set_string(teamid, id, list->str);
		}

		else if (_isNumeric(list->str)) {
			if (type == TEAM_FIELD_STRING) {
				fprintf(stderr, "%s: token '%s' is not alpha!\n", __func__, list->str);
				return -2;
			}

			else if (type == TEAM_FIELD_INVALID) {
				tfl_set_type(id, TEAM_FIELD_DOUBLE);
			}

			conv = atof(list->str);
			team_set_double(teamid, id, conv);
		}

		else {
			fprintf(stderr, "%s: illegal value '%s'\n", __func__, list->str);
			return -3;
		}

		list = list->next;
		id++;
	}

	return 0;
}


/**
 *
 */

static int _create_team(struct _token *list)
{
	const char *name;
	int err;
	size_t teamid;

	name = _get_team_name(list);
	if (!name) {
		return -1;
	}

	err = team_create(name, &teamid);
	if (err) {
		fprintf(stderr, "%s: unable to create team '%s'\n", __func__, name);
		return -2;
	}

	err = _set_fields(list, teamid);
	if (err) {
		fprintf(stderr, "%s: unable to read field for team '%s'\n", __func__, name);
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
	struct _token *tokens, *token;
	size_t num_tokens;
	size_t i;

	count = _read_line(fd, buf);
	if (!count) {
		return toread;
	}

	num_tokens = _tokenize_line(buf, &tokens);
	if (num_tokens != toread) {
		fprintf(stderr, "%s: team only has %lu/%lu fields\n", __func__, num_tokens, toread);
		_deallocate_tokens(&tokens);
		return (num_tokens - toread);
	}

	token = tokens;
	for (i = 0; i < num_tokens; i++) {
		token = token->next;
	}

	_create_team(tokens);
	_deallocate_tokens(&tokens);
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


	fd = _open_file(filename);
	if (fd < 0) {
		fprintf(stderr, "%s: could not open file %s\n", __func__, filename);
		return -1;
	}

	/* read heading line which contains the variable names */
	count = _read_fields_list(fd, buf);
	if (count == 0) {
		return -2;
	}

	do {
		count = _read_team(fd, buf);
	} while (!count);


	_close_file(fd);

	return 0;
}

