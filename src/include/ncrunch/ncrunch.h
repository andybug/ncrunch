
#pragma once

#include <ncrunch/crypto.h>



#define NCRUNCH_VERSION_MAJOR 1
#define NCRUNCH_VERSION_MINOR 0


/* TODO: Make NCRUNCH_DEV and RELEASE be configured through CMake  */

#if (NCRUNCH_RELEASE == 1)
	#define NCRUNCH_DEBUG 0
#elif (NCRUNCH_DEBUG == 1)
	#define NCRUNCH_RELEASE 0
#endif



/**
 *
 */


union ncrunch_team_field {
	const char* data_s;
	double data_f;
};



/**
 *
 */

struct ncrunch_team {
	const char* name;
	struct ncrunch_crypto_digest name_hash;

};


/* Functions for reading flat file that contains team data */


int ncrunch_flatf_read(const char* filename);


