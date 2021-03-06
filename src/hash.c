
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include <ncrunch/hash.h>



/**
 * Hashes a string into the digest
 *
 * @param str The input string
 * @param len The length of the string; if 0 is passed, length will be determined
 * @param digest The resulting message digest
 */

void hash_string(const char *str, size_t len, struct mdigest* md)
{
	if (len == 0) {
		len = strlen(str);
	}

	SHA256((const unsigned char*) str, len, md->md);
}


/**
 * Hashes a string, ignoring case
 *
 * @param str The string to be hashed
 * @param md The resulting message digest
 */

void hash_stringi(const char *str, struct mdigest *md)
{
	SHA256_CTX ctx;
	unsigned char data;

	SHA256_Init(&ctx);
	
	while (*str) {
		data = (unsigned char) tolower(*str);
		SHA256_Update(&ctx, &data, 1);
		str++;
	}

	SHA256_Final(md->md, &ctx);
}


/**
 * Prints the message digest to stdout in hex form [0-9][a-f]. No new line is added
 */

void hash_show(const struct mdigest* md)
{
	int i;
	unsigned int high, low;
	static const char trans[] = 	{ '0', '1', '2', '3',
					  '4', '5', '6', '7',
					  '8', '9', 'a', 'b',
					  'c', 'd', 'e', 'f' };

	for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		high = (md->md[i] >> 4) & 0x0f;
		low = md->md[i] & 0x0f;

		putchar(trans[high]);
		putchar(trans[low]);
	}
}

