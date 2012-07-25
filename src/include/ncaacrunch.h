
#pragma once

#define NCRUNCH_VERSION_MAJOR 1
#define NCRUNCH_VERSION_MINOR 0


/* FIXME */
#ifdef CMAKE_RELEASE
#define NCRUNCH_RELEASE	1
#define NCRUNCH_DEV	0
#else
#define NCRUNCH_DEV	1
#define NCRUNCH_RELEASE	0
#endif

