#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "libhtml.h"
#include "utils.h"

struct page {
	int isCDefaultImg;
};
int
cover_html_attr_cb (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct page *page = (struct page *) parent;

	if (hdr->status == HTML_ESTABLISH) {
                return 1;
        } else
        if (hdr->status == HTML_TAG) {
                switch (hdr->tag) {
                        case ATTR_CLASS:
                                return 1;
                        default:
                                return 0;
                }
                return 0;
        } else
        if (hdr->status == HTML_DATA) {
		if (hdr->tag == ATTR_CLASS) {
			if (!memcmp(hdr->data, "cDefaultImg", 4)) {
				page->isCDefaultImg = 1;
			}
		}

	} else
        if (hdr->status == HTML_CLOSE) {
        }

	return 0;
}
/*



int
html_attr_href_cb (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
char *href = (char *) parent;

	if (hdr->status == HTML_ESTABLISH) {
                return 1;
        } else
        if (hdr->status == HTML_TAG) {
                switch (hdr->tag) {
                        case ATTR_HREF:
                                return 1;
                        default:
                                return 0;
                }
                return 0;
        } else
        if (hdr->status == HTML_DATA) {
		if (hdr->tag == ATTR_HREF) {
			memcpy (href, hdr->data, hdr->len);
			href[hdr->len] = '\0';
		}

	} else
        if (hdr->status == HTML_CLOSE) {
        }

	return 0;
}
int
html_a_cb (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
FILE *fp = (FILE *) parent;

	if (hdr->status == HTML_ESTABLISH) {
		return 1;
	} else
	if (hdr->status == HTML_TAG) {
		switch (hdr->tag) {
			case TAG_A:
				return 1;
			default:
				return 0;
		}
		return 0;
	} else
	if (hdr->status == HTML_DATA) {
		if (hdr->tag == TAG_A) {
			char *start, *end;
			char *ptr = hdr->data;
			ptr[hdr->len] = '\0';
			if ((start=strchr(ptr, '>')) && start++ && start[0] && (end=strchr(start, '<')) && end-start > 0) {
				char comicName[128];
				char href[128]="";
				html_attr_parser (hdr->data, hdr->len, html_attr_href_cb, href);
				if (href[0]) {
					memcpy (comicName, start, end-start);
					comicName[end-start] = '\0';
					char *name = NULL;
					int  name_len = 0;
					name_len = charset_convert("gbk", "utf-8", comicName, end-start, &name);
					if(name_len > 0) {
						name[name_len]='\0';
						char picurl[128];
						if (get_cover_url (href[0], picurl)) {

						} else {
							fprintf (fp, "%s\t%s\r\n", name, href);
						}
						free(name);
					}
				}
			}
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
		//DEBUG ("webcat", "%s", "HTML_DATA_INCOMPLETE");
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}
*/

int
cover_html_cb (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;

	if (hdr->status == HTML_ESTABLISH) {
		return 1;
	} else
	if (hdr->status == HTML_TAG) {
		switch (hdr->tag) {
			case TAG_DIV:
				return 1;
			default:
				return 0;
		}
		return 0;
	} else
	if (hdr->status == HTML_DATA) {
		if (hdr->tag == TAG_DIV) {
			struct page page = {0};
			html_attr_parser (hdr->data, hdr->len, cover_html_attr_cb, &page);
			if (page.isCDefaultImg) {
				dump_text (hdr->data, hdr->len);
				page.isCDefaultImg = 0;
			}
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
		//DEBUG ("webcat", "%s", "HTML_DATA_INCOMPLETE");
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
get_cover_url (char *ourl, char *coverUrl)
{
	CURL *curl = NULL;
	//struct curl_slist *headers = NULL;
	CURLcode res;
	struct doc docbuf = {NULL, 0};
	char url[1024] = "http://mh.99770.cc";
	strcat (url, ourl);

	//add2list(&headers, "Accept-Language: en-us,en;q=0.5");
	//add2list(&headers, "Accept-Charset: utf-8");

	if (!(curl=curl_easy_init())) {
                sprintf (gErrmsg, "%s:%d curl_easy_init() fail", __FILE__, __LINE__);
                return -1;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_URL, url)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
	if ((res=curl_easy_setopt(curl, CURLOPT_USERAGENT, MY_USER_AGENT)) != CURLE_OK ||
	    //(res=curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers))  != CURLE_OK ||
	    (res=curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb)) != CURLE_OK ||
            (res=curl_easy_setopt(curl, CURLOPT_WRITEDATA, &docbuf)) != CURLE_OK){
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }

	if ((res = curl_easy_perform(curl) != CURLE_OK)) {
		sprintf (gErrmsg, "%s:%d curl_easy_perform: %s", __FILE__, __LINE__, curl_easy_strerror(res));
		goto ERR;
	}
	
	docbuf.data[docbuf.len] = '\0';
	//printf ("%s\n", docbuf.data);
	html_parser (docbuf.data, docbuf.len, cover_html_cb, NULL);
	//dump_text ((unsigned char *)docbuf.data, docbuf.len);

	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	curl_easy_cleanup(curl);
	return 0;
ERR:
	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	curl_easy_cleanup(curl);
	return -1;
}
