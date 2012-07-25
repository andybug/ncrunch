
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

	return 0;

#else	/* OS not supported */
	return -1;

#endif
}


static int _check_installed_version(void)
{
	char exe_location[FILENAME_MAX];

#ifdef __linux__
	int error;
	int release = -1;

	error = _get_exe_location(exe_location);
	if (error < 0) {
		return -1;
	}

	if (strncmp("/usr/bin/", exe_location, 9) == 0) {	/* RELEASE */
		release = 1;
	}

	else if (strncmp("/opt/", exe_location, 5) == 0) {	/* RELEASE */
		release = 1;
	}

	else {							/* DEBUG */
		release = 0;
	}


	/* ensure that the installed location is equivalent to the build type;
	 * this will make sure that directory paths throughout the program will
	 * read from the right locations (configs, etc.)
	 */

	if (release && NCRUNCH_DEV) {	/* release location but dev build */
		fprintf(stderr, "%s: developer build running from release location\n", __func__);
		return -2;
	}

	else if (!release && NCRUNCH_RELEASE) {	/* dev location but release build */
		fprintf(stderr, "%s: release build running from developer location\n", __func__);
		return -3;
	}

	/* everything's ok */
	return 0;


#else	/* OS not supported */
	return -1;
#endif
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
	int error;

	printf("ncaacrunch:\tAndrew Fields 2012\n\t\t<andybug10@gmail.com>\n");

	error = _check_installed_version();
	if (error < 0) {
		return -1;
	}

	return 0;
}

