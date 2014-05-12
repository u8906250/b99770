#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <iconv.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef	__WIN32__
#include "win32/mmap.h"
#else
#include <sys/mman.h>
#endif	//__WIN32__
#include "utils.h"

char gErrmsg[256];

void
dump_text(char *data, int len)
{
	int     i;
	for(i=0;i<len;i++) {
		if (isprint(data[i]) ||
				data[i] == '\n' ||
				data[i] == '\r')
			putc( data[i], stdout );
		else
			//printf("\033[1;33m.\033[0m");
			printf(".");
	}
	printf("\n");
}

void
dump_data(unsigned char *data, int len)
{
	int     i;
	int     j;
	int     k;

	for(i=0;i<len;i+=16) {
		printf("%8.8x ", i);
		k = len - i;
		if (k > 16)
			k = 16;
		k += i;
		for(j=i; j<(i+16); j++) {
			if (j < k)
				printf("%2.2x ", data[j] );
			else
				printf("   ");
		}
		printf("  ");
		for(j=i; j<k; j++) {
			printf("%c", isprint(data[j]) ? data[j] : '.' );
		}
		printf("\n");
	}
}
#if 0
int
cp (char *srcpath, char *dstpath)
{
	int srcfd = 0, dstfd = 0;
	struct stat st;
	void *mem = NULL;
	int rc = -1;

	// open srcpath and mmap to memory
	if (stat (srcpath, &st) < 0) {
		ERROR ("edprobe", "stat() %s", strerror (errno));
		goto END;
	}
	if (!S_ISREG (st.st_mode))
		goto END;

	srcfd = open (srcpath, O_RDONLY);
	if (srcfd < 0) {
		ERROR ("edprobe", "open() %s", strerror (errno));
		goto END;
	}

	mem = mmap (NULL, st.st_size, PROT_READ, MAP_SHARED, srcfd, 0);
	if (mem == MAP_FAILED)
		goto END;

	// open dstpath and ready to write data
	dstfd = open (dstpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (dstfd < 0) {
		ERROR ("edprobe", "open() %s", strerror (errno));
		goto END;
	}

	// dump data
	rc = write (dstfd, mem, st.st_size);

END:
	if (mem && mem != MAP_FAILED)
		munmap (mem, st.st_size);

	if (srcfd > 0)
		close (srcfd);

	if (dstfd > 0)
		close (dstfd);

	return rc;
}


int
mv (char *srcpath, char *dstpath)
{
	unlink (dstpath);
	if (rename (srcpath, dstpath) == 0)
		return 0;
	if (cp (srcpath, dstpath) < 0)
		return -1;
	unlink (srcpath);

	return 0;
}
#endif

void *
memmem (const void *haystack, size_t haystack_len,
	const void *needle, size_t needle_len)
{
	size_t i;

	if (needle_len > haystack_len)
		return NULL;

	for (i = 0; i <= haystack_len - needle_len; i++)
		if (!memcmp (needle, &((const char *) haystack)[i], needle_len))
			return (void *) (&((const char *) haystack)[i]);

	return NULL;
}

char
*ed_string_to_html (char *str)
{
	int     i,j;
	int     len;
	char    *dst;
	int     c;

	if (!str)
		return NULL;

	len = strlen(str);

	dst = malloc( len * 3 + 1 );
	if (!dst)
		return NULL;

	for(i=0,j=0;i<len;i++,j++) {
		c = str[i];

		if ( (c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') ||
				(c >= '0' && c <= '9') ) {

			dst[j] = str[i];

		} else
			if ( c == ' ') {

				dst[j] = '+';

			} else {

				dst[j] = '%';
				j++;
				sprintf(&dst[j], "%2.2X", (unsigned char )str[i] );
				j++;
			}

	}
	dst[j] = '\0';
	return dst;
}

int
charset_convert(const char      *srcenc,
		const char      *dstenc,
		char            *buf,
		int             len,
		char            **ret)
{
	size_t inbytesleft, outbytesleft;
	char *inbuf, *outbuf;
	iconv_t cd;

	inbuf = buf;
	*ret = outbuf = malloc (len * 4);
	inbytesleft = len;
	outbytesleft = (len * 4);

	if ((cd = iconv_open (dstenc, srcenc)) < 0) {
		printf ("iconv_open() error\n");
		goto MY_ERROR;
	}

	if (iconv (cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft) < 0) {
		printf ("iconv() error\n");
		goto MY_ERROR;
	}

	if (iconv_close (cd) < 0) {
		printf ("iconv_close() error\n");
		goto MY_ERROR;
	}

	return ((len * 4) - outbytesleft);
MY_ERROR:
	free(outbuf);
	outbuf = NULL;
	*ret = buf;
	return 0;
}

// html
void
add2list (struct curl_slist **list, const char *ptr)
{
	struct curl_slist *newlist = curl_slist_append(*list, ptr);
	if(newlist)
		*list = newlist;
}

size_t
write_cb (void *ptr, size_t size, size_t nmemb, void *stream)
{
	struct doc *docbuf = (struct doc *)stream;
	if (!docbuf->data) {
		docbuf->data = (char *)malloc (size*nmemb+1);
		memcpy (docbuf->data, ptr, size*nmemb);
		docbuf->len  = size*nmemb;
	} else {
		docbuf->data = (char *) realloc (docbuf->data, docbuf->len + size*nmemb + 1);
		memcpy (&docbuf->data[docbuf->len], ptr, size*nmemb);
		docbuf->len  += size*nmemb;
	}
	return size*nmemb;
}



