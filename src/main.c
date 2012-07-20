
#include <stdio.h>
#include <string.h>

#include <openssl/sha.h>

#include <ncaacrunch.h>


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


int main(void)
{
	char buffer[256];
	unsigned char digest[SHA256_DIGEST_LENGTH];
	size_t len;
	
	printf("ncaacrunch:\tAndrew Fields 2012\n\t\t<andybug10@gmail.com>\n");
	printf("Version %d.%d\n", NCRUNCH_VERSION_MAJOR, NCRUNCH_VERSION_MINOR);

	gets(buffer);
	len = strlen(buffer);
	
	SHA256((unsigned char*) buffer, len, digest);
	show_digest(digest);

	return 0;
}

