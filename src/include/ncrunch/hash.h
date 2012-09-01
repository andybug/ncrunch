
#pragma once

#include <openssl/sha.h>

/**
 * Stores the message digests for hashing calls
 */

struct mdigest {
	unsigned char md[SHA256_DIGEST_LENGTH];
};

void hash_string(const char *str, size_t len, struct mdigest *digest);
void hash_stringi(const char *str, struct mdigest *digest);
void hash_show(const struct mdigest *digest);
