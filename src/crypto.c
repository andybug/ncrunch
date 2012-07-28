
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <ncrunch/crypto.h>



/**
 * Hashes a string into the digest
 *
 * @param str The input string
 * @param len The length of the string; if 0 is passed, length will be determined
 * @param digest The resulting message digest
 */

void ncrunch_crypto_hash_string(const char* str, size_t len, struct ncrunch_crypto_digest* digest)
{
	assert(str);
	assert(digest);

	if (len == 0) {
		len = strlen(str);
	}

	SHA256((const unsigned char*) str, len, digest->md);
}



/**
 * Prints the message digest to stdout in hex form [0-9][a-f]. No new line is added
 */

void ncrunch_crypto_show_hash(const struct ncrunch_crypto_digest* digest)
{
	int i;
	unsigned int high, low;
	static const char trans[] = 	{ '0', '1', '2', '3',
					  '4', '5', '6', '7',
					  '8', '9', 'a', 'b',
					  'c', 'd', 'e', 'f' };

	for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		high = (digest->md[i] >> 4) & 0x0f;
		low = digest->md[i] & 0x0f;

		putchar(trans[high]);
		putchar(trans[low]);
	}
}

