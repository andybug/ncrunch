
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
 * 
 * TODO: memory map the file
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


struct _flatf_token {
	const char* str;	/* pointer into buffer */
	struct _flatf_token* next;
};


static size_t _flatf_tokenize_line(char* buffer, struct _flatf_token** token_list)
{
	
}



/**
 *
 */

int ncrunch_flatf_read(const char* filename)
{
	int fd;
	char buffer[FLATF_READBUFSIZE];
	size_t count;


	fd = _flatf_open(filename);
	if (fd < 0) {
		return -1;
	}

	count = _flatf_read_line(fd, buffer, FLATF_READBUFSIZE);
	printf("read %d: %s\n", count, buffer);

	_flatf_close(fd);

	return 0;
}

