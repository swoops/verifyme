#include <stdio.h>
#include <openssl/rsa.h>
#include <dlfcn.h>
#include <string.h> // memcmp

#define _GNU_SOURCE
#define fileloc "/tmp/alwaysverify"

char *alwaysbuff = NULL;
size_t alwayssize = 0;

static char *getfile() {
	FILE *fp = fopen(fileloc, "r");
	if (!fp) {
		fprintf(stderr, "Failed to open %s\n", fileloc);
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	alwayssize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *ret = (char *) malloc(alwayssize);
	if (!ret) {
		return NULL;
	}
	if(fread(ret, sizeof(char), alwayssize, fp) != alwayssize ) {
		free(ret);
		return NULL;
	}
	return ret;
}

int RSA_verify(int type, const unsigned char *m, unsigned int m_length,
		const unsigned char *sigbuf, unsigned int siglen, RSA *rsa){

	static int (*rverify)(int, const unsigned char*, unsigned int, const
		unsigned char*, unsigned int, RSA *) = NULL;
	if (!alwaysbuff) {
		alwaysbuff = getfile();
	}
	if (!rverify) {
		rverify = dlsym(RTLD_NEXT, "RSA_verify");
	}
	if (alwaysbuff) {
		if(siglen == alwayssize) {
			if ( memcmp(alwaysbuff, sigbuf, alwayssize) == 0 ) {
				return 1;
			}
		}
	} else {
		fprintf(stderr, "Couldn't open file\n");
	}
	return rverify(type, m, m_length, sigbuf, siglen, rsa);
}

__attribute__((destructor)) static void e() {
	free(alwaysbuff);
}
