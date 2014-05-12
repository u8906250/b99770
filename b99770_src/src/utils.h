#ifndef	__UTILS_INCLUDE__
#define	__UTILS_INCLUDE__
#include <curl/curl.h>

#define	MY_USER_AGENT	"b99770"
#define	SAFE_FREE(MEM)	{if ((MEM)) free ((MEM)); (MEM) = NULL;}

extern char gErrmsg[256];

struct doc {
	char *data;
	int len;
};


void
dump_text(char *data, int len);

void
dump_data(unsigned char *data, int len);

void *
memmem (const void *haystack, size_t haystack_len,
	const void *needle, size_t needle_len);

char
*ed_string_to_html (char *str);
/*
 * inline utility
 */
static inline void
safe_free (void *_ptr)
{
	void    **ptr = (void **)_ptr;
	if (!ptr)
		return;
	if (*ptr)
		free (*ptr);
	*ptr = NULL;
}


static inline int
seek_first_meet_char (char *data, int len, char c)
{                       
	int i;          
	for (i=0 ; i<len; i++) {
		if (data[i] == c)
			return i;
	}               
	return -1;      
}                       
static inline int       
seek_oppo_brackets (char *data, int len, char left, char right)
{               
	int i;
	int top = 0;    
	int escape_mode = 0;    
	int text_mode = 0;      

	if (data[0] != left)
		return 0;
	top ++;
	for (i=1; i<len; i++) {
		if (data[i] == left && !escape_mode && !text_mode)
			top ++;
		if (data[i] == right && !escape_mode && !text_mode)
			top --;
		if (top <= 0)
			return i-1;
		if (!escape_mode && data[i] == '"')
			text_mode = (text_mode) ? 0 : 1;
		escape_mode = (!escape_mode && data[i] == '\\') ? 1 : 0;
	}
	return 0;
}

int
charset_convert(const char      *srcenc,
		const char      *dstenc,
		char            *buf,
		int             len,
		char            **ret);

void
add2list (struct curl_slist **list, const char *ptr);

size_t
write_cb (void *ptr, size_t size, size_t nmemb, void *stream);

int
get_cover_url (char *ourl, char *coverUrl);
#endif	//__UTILS_INCLUDE__
