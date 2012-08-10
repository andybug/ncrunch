
#pragma once

#include <openssl/sha.h>



/**
 * Stores the message digests for hashing calls
 */

struct ncrunch_crypto_digest {
	unsigned char md[SHA256_DIGEST_LENGTH];	
};


void ncrunch_crypto_hash_string(const char* str, size_t len, struct ncrunch_crypto_digest* digest);
void ncrunch_crypto_show_hash(const struct ncrunch_crypto_digest* digest);


