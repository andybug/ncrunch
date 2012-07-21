
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <openssl/sha.h>

#include <ncaacrunch.h>



static int _get_exe_location(char location[FILENAME_MAX])
{
#ifdef __linux__
	int error;

	error = readlink("/proc/self/exe", location, FILENAME_MAX);
	if (error < 0) {
		fprintf(stderr, "%s: Unable to read /proc/self/exe\n", __func__);
		return -1;
	}
#endif

	return 0;
}


static void show_digest(unsigned char* digest)
{
	int high, low;
	int i;
	static char trans[] = { '0', '1', '2', '3',
				'4', '5', '6', '7',
				'8', '9', 'a', 'b',
				'c', 'd', 'e', 'f' };


	for(i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		high = (digest[i] & 0xf0) >> 4;
		low = digest[i] & 0x0f;

		putchar(trans[high]);
		putchar(trans[low]);
	}

	putchar('\n');
}


int main(int argc, char** argv)
{
	char buffer[256];
	unsigned char digest[SHA256_DIGEST_LENGTH];
	size_t len;

	char exe_location[FILENAME_MAX];
	

	printf("ncaacrunch:\tAndrew Fields 2012\n\t\t<andybug10@gmail.com>\n");

	_get_exe_location(exe_location);
	printf("exe location = %s\n", exe_location);

	gets(buffer);
	len = strlen(buffer);
	
	SHA256((unsigned char*) buffer, len, digest);
	show_digest(digest);

	return 0;
}

