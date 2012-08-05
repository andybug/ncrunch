
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

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

static int _flatf_open(const char* filename)
{
	int fd;

	assert(filename);

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

static int _flatf_close(int fd)
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
 * @param len The size of the buffer
 * @return The number of characters read into the buffer
 */

static size_t _flatf_read_line(int fd, char* buffer, size_t len)
{
	size_t count = 0;
	char c;


	while (read(fd, &c, 1)) {

		if (count == len) {	/* line won't fit in buffer */
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

struct _flatf_token {
	const char* str;	/* pointer into buffer */
	struct _flatf_token* next;
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

static size_t _flatf_tokenize_line(char* buffer, struct _flatf_token** token_list)
{
	struct _flatf_token *list = NULL;
	struct _flatf_token *token = NULL;
	size_t count = 0;
	char *str;

	str = strtok(buffer, "\t");

	while (str) {
		if (!list) {
			list = malloc(sizeof(*list));
			token = list;
		} else {
			token->next = malloc(sizeof(struct _flatf_token));
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

static void _deallocate_tokens(struct _flatf_token **token_list)
{
	struct _flatf_token *token = *token_list;
	struct _flatf_token *next;

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

static size_t _flatf_read_fields_list(int fd, char *buf)
{
	size_t count;
	struct _flatf_token *tokens, *token;
	size_t num_tokens;
	size_t i;

	count = _flatf_read_line(fd, buf, FLATF_READBUFSIZE);
	if (!count) {
		/* no data or too much data! */
		fprintf(stderr, "%s: failed to read fields line\n", __func__);
		return 0;
	}

	num_tokens = _flatf_tokenize_line(buf, &tokens);
	if (!num_tokens) {
		/* not formatted correctly */
		fprintf(stderr, "%s: fields line incorrectly formatted\n", __func__);
		_deallocate_tokens(&tokens);
		return 0;
	}

	
	team_field_list_create(num_tokens);
	token = tokens;

	for (i = 0; i < num_tokens; i++) {
		team_field_list_set_name(i, token->str);
		token = token->next;
	}

	_deallocate_tokens(&tokens);
	return num_tokens;	
}


/**
 * Reads the flat file; The first line of the file is interpreted as the field list
 * (since the headings of the columns correspond to the data below). The team field
 * list is populated from this line. The rest of the lines are used to fill out the
 * individual teams.
 *
 * @return Negative on error
 */

int ncrunch_flatf_read(const char* filename)
{
	int fd;
	char buffer[FLATF_READBUFSIZE];
	size_t count;

	struct _flatf_token *token_list;
	size_t token_list_length;


	fd = _flatf_open(filename);
	if (fd < 0) {
		fprintf(stderr, "%s: could not open file %s\n", __func__, filename);
		return -1;
	}

	/* read heading line which contains the variable names */
	count = _flatf_read_fields_list(fd, buffer);

	count = _flatf_read_line(fd, buffer, FLATF_READBUFSIZE);
	token_list_length = _flatf_tokenize_line(buffer, &token_list);
	_deallocate_tokens(&token_list);

	_flatf_close(fd);

	return 0;
}

