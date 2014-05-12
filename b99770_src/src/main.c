#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include <curl/curl.h>
//#include "../../c-ares-1.9.1/include/ares.h"
#include "curl/curl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include "libhtml.h"
#include "utils.h"

char g_save_path[1024]="";
char g_save_tmp_path[1024]="";

struct page {
	int isW100;
	int isCComicTop;
	int isCUpComicTop;
	int isCTArea;
	int isCTAreaName;
	int isCVolList;
	int isCComicPageChange;
	int isCComicList;
	int isT2;
	int isT4;
	int isT5;
	FILE *fp;
	char *date;
	int pages;
};

int
html_attr_cb (void *_hdr, void **arg, void *parent)
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
			/*
			if (!memcmp(hdr->data, "w100", hdr->len)) {
				page->isW100 = 1;
			} else
			*/
			if (!memcmp(hdr->data, "cComicTop", hdr->len)) {
				page->isCComicTop = 1;
			} else
			if (!memcmp(hdr->data, "cUpComicTop", hdr->len)) {
				page->isCUpComicTop = 1;
			} else
			if (!memcmp(hdr->data, "t2", hdr->len)) {
				page->isT2 = 1;
			} else
			if (!memcmp(hdr->data, "t4", hdr->len)) {
				page->isT4 = 1;
			} else
			if (!memcmp(hdr->data, "t5", hdr->len)) {
				page->isT5 = 1;
			} else
			if (!memcmp(hdr->data, "cTArea", hdr->len)) {
				page->isCTArea = 1;
			} else
			if (!memcmp(hdr->data, "cTAreaName", hdr->len)) {
				page->isCTAreaName = 1;
			} else
			//if (!memcmp(hdr->data, "cVolList", hdr->len)) {
			if (!memcmp(hdr->data, "cVolList", 8)) {
				page->isCVolList = 1;
			} else
			if (!memcmp(hdr->data, "cComicPageChange", hdr->len)) {
				page->isCComicPageChange = 1;
			} else
			if (!memcmp(hdr->data, "cComicList", hdr->len)) {
				page->isCComicList = 1;
			}
		}
	} else
        if (hdr->status == HTML_CLOSE) {
        }

	return 0;
}

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
html_attr_href_cb2 (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct {char title[1024]; char href[1024];} *s = parent;

	if (hdr->status == HTML_ESTABLISH) {
                return 1;
        } else
        if (hdr->status == HTML_TAG) {
                switch (hdr->tag) {
                        case ATTR_HREF:
                                return 1;
			case ATTR_TITLE:
				return 1;
                        default:
                                return 0;
                }
                return 0;
        } else
        if (hdr->status == HTML_DATA) {
		if (hdr->tag == ATTR_HREF) {
			memcpy (s->href, hdr->data, hdr->len);
			s->href[hdr->len] = '\0';
		} else
		if (hdr->tag == ATTR_TITLE) {
			memcpy (s->title, hdr->data, hdr->len);
			s->title[hdr->len] = '\0';
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
						if (get_cover_url (href, picurl)) {

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
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
html_a_cb2 (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct page *page = (struct page *) parent;

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
			struct {char title[1024]; char href[1024];} s;
			html_attr_parser (hdr->data, hdr->len, html_attr_href_cb2, &s);
			fprintf (page->fp, "%s\t%s\r\n", s.title, s.href);
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
html_area_a_cb (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct page *page = (struct page *) parent;

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
			char *buf = malloc (hdr->len+1);
			memcpy (buf, hdr->data, hdr->len);
			buf[hdr->len] = '\0';
			char *ptr = buf;
			if ((start=strchr(ptr, '>')) && start++ && start[0] && (end=strchr(start, '<')) && end-start > 0) {
				char comicName[128];
				char href[128]="";
				html_attr_parser (hdr->data, hdr->len, html_attr_href_cb, href);
				if (href[0] && strncmp(href, "javascript", 10)) {
					memcpy (comicName, start, end-start);
					comicName[end-start] = '\0';
					fprintf (page->fp, "%s\t%s\r\n", comicName, href);
				}
			}
			free (buf);
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
html_area_a_cb2 (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct page *page = (struct page *) parent;

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
			char *buf = malloc (hdr->len+1);
			memcpy (buf, hdr->data, hdr->len);
			buf[hdr->len] = '\0';
			char *ptr = buf;
			if ((start=strchr(ptr, '>')) && start++ && start[0] && (end=strchr(start, '<')) && end-start > 0) {
				char comicName[128];
				char href[128]="";
				html_attr_parser (hdr->data, hdr->len, html_attr_href_cb, href);
				if (href[0] && strncmp(href, "javascript", 10)) {
					memcpy (comicName, start, end-start);
					comicName[end-start] = '\0';
					fprintf (page->fp, "\r\n%s\t%s", comicName, href);
				}
			}
			free (buf);
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
html_area_a_cb3 (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct page *page = (struct page *) parent;

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
			char *buf = malloc (hdr->len+1);
			memcpy (buf, hdr->data, hdr->len);
			buf[hdr->len] = '\0';
			char *ptr = buf;
			if ((start=strchr(ptr, '>')) && start++ && start[0] && (end=strchr(start, '<')) && end-start > 0) {
				char author[128];
				memcpy (author, start, end-start);
				author[end-start] = '\0';
				fprintf (page->fp, "\t%s", author);
			}
			free (buf);
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
html_area_cb (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct page *page = (struct page *) parent;

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
			page->isCTAreaName = 0;
			html_attr_parser (hdr->data, hdr->len, html_attr_cb, parent);
			if (page->isCTAreaName) {
				char *buf = malloc (hdr->len+1);
				memcpy (buf, hdr->data, hdr->len);
				buf[hdr->len] = '\0';
				char areaName[128];
				if (html_get_tag_content(buf, areaName, sizeof(areaName))) {
					fprintf (page->fp, "%s\r\n", areaName);			
				}
				free (buf);
				page->isCTAreaName = 0;
			} else {
				html_parser (hdr->data, hdr->len, html_area_a_cb, parent);
			}
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
html_top_cb (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct page *page = (struct page *) parent;

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
			html_attr_parser (hdr->data, hdr->len, html_attr_cb, parent);
			if (page->isCTArea) {
				html_parser (hdr->data+1, hdr->len-1, html_area_cb, parent);
				page->isCTArea = 0;
			}
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
html_newest_cb (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct page *page = (struct page *) parent;

	if (hdr->status == HTML_ESTABLISH) {
		return 1;
	} else
	if (hdr->status == HTML_TAG) {
		switch (hdr->tag) {
			case TAG_SPAN:
				return 1;
			default:
				return 0;
		}
		return 0;
	} else
	if (hdr->status == HTML_DATA) {
		if (hdr->tag == TAG_SPAN) {
			html_attr_parser (hdr->data, hdr->len, html_attr_cb, parent);
			if (page->isT5) {
				char *buf = malloc (hdr->len+1);
				memcpy (buf, hdr->data, hdr->len);
				buf[hdr->len] = '\0';
				char date[128];
				if (html_get_tag_content(buf, date, sizeof(date))) {
					strtok (date, " ");
					if (!page->date || strcmp(page->date, date)) {
						fprintf (page->fp, "\r\n%s", date);
						SAFE_FREE (page->date);
						page->date = strdup (date);
					} 
				}
				free (buf);
				page->isT5 = 0;
			}
			if (page->isT4) {
				html_parser (hdr->data+1, hdr->len-1, html_area_a_cb3, parent);
				page->isT4 = 0;
			}
			if (page->isT2) {
				html_parser (hdr->data+1, hdr->len-1, html_area_a_cb2, parent);
				page->isT2 = 0;
			}
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
html_cb (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct page *page = (struct page *) parent;

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
			html_attr_parser (hdr->data, hdr->len, html_attr_cb, parent);
			if (page->isW100) {
				FILE *fp = fopen (".top", "w");
				html_parser (hdr->data, hdr->len, html_a_cb, fp);
				fclose (fp);
				struct stat st;
				if (stat (".top", &st) != -1 && st.st_size > 0) {
					rename (".top", "top");
				}
				page->isW100 = 0;
			} else
			if (page->isCComicTop) {
				char *tmppath = g_save_tmp_path[0]? g_save_tmp_path: "/tmp/.top";
				char *savepath = g_save_path[0]? g_save_path: "/tmp/top";
				page->fp = fopen (tmppath, "w");
				html_parser (hdr->data, hdr->len, html_top_cb, parent);
				fclose (page->fp);
				struct stat st;
				if (stat (tmppath, &st) != -1 && st.st_size > 0) {
					rename (tmppath, savepath);
				}
				page->isCComicTop = 0;
			} else
			if (page->isCUpComicTop) {
				char *tmppath = g_save_tmp_path[0]? g_save_tmp_path: "/tmp/.newest";
				char *savepath = g_save_path[0]? g_save_path: "/tmp/newest";
				page->fp = fopen (tmppath, "w");
				html_parser (hdr->data, hdr->len, html_newest_cb, parent);
				fclose (page->fp);
				struct stat st;
				if (stat (tmppath, &st) != -1 && st.st_size > 0) {
					rename (tmppath, savepath);
				}
				page->isCUpComicTop = 0;
			} else
			if (page->isCComicPageChange) {
				hdr->data[hdr->len] = '\0';
				char *start, *end;
				if ((end=strstr(hdr->data, "</b><span"))) {
					end[0] = '\0';
				       	if ((start=strrchr(hdr->data, '>')) && (start++) && start[0]) {
						page->pages = atoi(start);
					}
				}
				page->isCComicPageChange = 0;
			}
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
html_cb2 (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct page *page = (struct page *) parent;

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
			html_attr_parser (hdr->data, hdr->len, html_attr_cb, parent);
			if (page->isCComicList) {
				html_parser (hdr->data, hdr->len, html_a_cb2, parent);
				page->isCComicList = 0;
			}

		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
b99770_top (char *url)
{
	CURL *curl = NULL;
	//struct curl_slist *headers = NULL;
	CURLcode res;
	struct doc docbuf = {NULL, 0};

	//add2list(&headers, "Accept-Language: en-us,en;q=0.5");
	//add2list(&headers, "Accept-Charset: utf-8");

	if (!(curl=curl_easy_init())) {
                sprintf (gErrmsg, "%s:%d curl_easy_init() fail", __FILE__, __LINE__);
                return -1;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "61.31.233.1")) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
/*
	struct curl_slist *host = NULL;
	host = curl_slist_append(NULL, "mh.99770.cc:80:58.215.241.45");
        if ((res=curl_easy_setopt(curl, CURLOPT_RESOLVE, host)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
	}
*/
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
	struct page page = {0};
	html_parser (docbuf.data, docbuf.len, html_cb, &page);
	//dump_text ((unsigned char *)docbuf.data, docbuf.len);

	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return 0;
ERR:
	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return -1;
}

//newest
int
b99770_newest (char *url)
{
	CURL *curl = NULL;
	//struct curl_slist *headers = NULL;
	CURLcode res;
	struct doc docbuf = {NULL, 0};

	//add2list(&headers, "Accept-Language: en-us,en;q=0.5");
	//add2list(&headers, "Accept-Charset: utf-8");

	if (!(curl=curl_easy_init())) {
                sprintf (gErrmsg, "%s:%d curl_easy_init() fail", __FILE__, __LINE__);
                return -1;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "61.31.233.1")) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
/*
	struct curl_slist *host = NULL;
	host = curl_slist_append(NULL, "mh.99770.cc:80:58.215.241.45");
        if ((res=curl_easy_setopt(curl, CURLOPT_RESOLVE, host)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
	}
*/
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
	struct page page = {0};
	html_parser (docbuf.data, docbuf.len, html_cb, &page);
	//dump_text ((unsigned char *)docbuf.data, docbuf.len);
	SAFE_FREE (page.date);

	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return 0;
ERR:
	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return -1;
}

//list
int
b99770_list_getpages (char *url)
{
	CURL *curl = NULL;
	//struct curl_slist *headers = NULL;
	CURLcode res;
	struct doc docbuf = {NULL, 0};

	//add2list(&headers, "Accept-Language: en-us,en;q=0.5");
	//add2list(&headers, "Accept-Charset: utf-8");

	if (!(curl=curl_easy_init())) {
                sprintf (gErrmsg, "%s:%d curl_easy_init() fail", __FILE__, __LINE__);
                return -1;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "61.31.233.1")) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
/*
	struct curl_slist *host = NULL;
	host = curl_slist_append(NULL, "mh.99770.cc:80:58.215.241.45");
        if ((res=curl_easy_setopt(curl, CURLOPT_RESOLVE, host)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
	}
*/
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
	struct page page = {0};
	html_parser (docbuf.data, docbuf.len, html_cb, &page);
	//dump_text ((unsigned char *)docbuf.data, docbuf.len);
	SAFE_FREE (page.date);

	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return page.pages;
ERR:
	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return -1;
}

int
b99770_list_pages (char *url, FILE *fp)
{
	CURL *curl = NULL;
	//struct curl_slist *headers = NULL;
	CURLcode res;
	struct doc docbuf = {NULL, 0};

	//add2list(&headers, "Accept-Language: en-us,en;q=0.5");
	//add2list(&headers, "Accept-Charset: utf-8");

	if (!(curl=curl_easy_init())) {
                sprintf (gErrmsg, "%s:%d curl_easy_init() fail", __FILE__, __LINE__);
                return -1;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "61.31.233.1")) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
/*
	struct curl_slist *host = NULL;
	host = curl_slist_append(NULL, "mh.99770.cc:80:58.215.241.45");
        if ((res=curl_easy_setopt(curl, CURLOPT_RESOLVE, host)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
	}
*/
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
	struct page page = {0};
	page.fp = fp;
	html_parser (docbuf.data, docbuf.len, html_cb2, &page);
	//dump_text ((unsigned char *)docbuf.data, docbuf.len);
	SAFE_FREE (page.date);

	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return 0;
ERR:
	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return -1;
}

int
b99770_list (char *url)
{
	int pages = b99770_list_getpages(url);
	if (pages > 0) {
		int i;
		char *tmppath = g_save_tmp_path[0]? g_save_tmp_path: "/tmp/.list";
		char *savepath = g_save_path[0]? g_save_path: "/tmp/list";
		FILE *fp = fopen (tmppath, "w");
		for (i=1; i<= pages; i++) {
			char tmps[1024];
			snprintf (tmps, 1024, "%s%d/", url, i);
			fprintf (fp, "%d\r\n", i);
			b99770_list_pages (tmps, fp);
		}
		fclose (fp);
		struct stat st;
		if (stat (tmppath, &st) != -1 && st.st_size > 0) {
			rename (tmppath, savepath);
		}
	}
	return 0;
}

//comic
int
html_comic_cb (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
struct page *page = (struct page *) parent;

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
			html_attr_parser (hdr->data, hdr->len, html_attr_cb, parent);
			if (page->isCVolList) {
				html_parser (hdr->data, hdr->len, html_area_a_cb, page);
				page->isCVolList = 0;
			}
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}

int
b99770_comic (char *url)
{
	CURL *curl = NULL;
	//struct curl_slist *headers = NULL;
	CURLcode res;
	struct doc docbuf = {NULL, 0};

	//add2list(&headers, "Accept-Language: en-us,en;q=0.5");
	//add2list(&headers, "Accept-Charset: utf-8");

	if (!(curl=curl_easy_init())) {
                sprintf (gErrmsg, "%s:%d curl_easy_init() fail", __FILE__, __LINE__);
                return -1;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "61.31.233.1")) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
/*
	struct curl_slist *host = NULL;
	host = curl_slist_append(NULL, "mh.99770.cc:80:58.215.241.45");
        if ((res=curl_easy_setopt(curl, CURLOPT_RESOLVE, host)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
	}
*/
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

	struct page page = {0};
	char *tmppath = g_save_tmp_path[0]? g_save_tmp_path: "/tmp/.comic";
	char *savepath = g_save_path[0]? g_save_path: "/tmp/comic";
	page.fp = fopen (tmppath, "w");
	html_parser (docbuf.data, docbuf.len, html_comic_cb, &page);
	fclose (page.fp);
	struct stat st;
	if (stat (tmppath, &st) != -1 && st.st_size > 0) {
		rename (tmppath, savepath);
	}
	//dump_text ((unsigned char *)docbuf.data, docbuf.len);

	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return 0;
ERR:
	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return -1;
}

//vol
char *
get_vol_server (int dmi)
{
	switch (dmi) {
		case 1:
			return "http://58.215.241.39:9728/dm01/";
		case 2:
			return "http://58.215.241.39:9728/dm02/";
		case 3:
			return "http://58.215.241.39:9728/dm03/";
		case 4:
			return "http://58.215.241.206:9728/dm04/";
		case 5:
			return "http://58.215.241.39:9728/dm05/";
		case 6:
			return "http://58.215.241.39:9728/dm06/";
		case 7:
			return "http://58.215.241.205:9728/dm07/";
		case 8:
			return "http://58.215.241.205:9728/dm08/";
		case 9:
			return "http://58.215.241.206:9728/dm09/";
		case 10:
			return "http://58.215.241.205:9728/dm10/";
		case 11:
			return "http://58.215.241.39:9728/dm11/";
		case 12:
			return "http://58.215.241.206:9728/dm12/";
		case 13:
			return "http://58.215.241.39:9728/dm13/";
		case 14:
			return "http://173.231.57.238/dm14/";
		case 15:
			return "http://58.215.241.206:9728/dm15/";
		case 16:
			return "http://142.4.34.102/dm16/";
		default:
			break;
	}
	return NULL;
}

void
html_vol_sfiles (char *sfiles, int dmi)
{
	char *tmppath = g_save_tmp_path[0]? g_save_tmp_path: "/tmp/.vol";
	char *savepath = g_save_path[0]? g_save_path: "/tmp/vol";

	char *tok = strtok (sfiles, "|");
	char *server = get_vol_server (dmi);

	if (server) {
		FILE *fp = fopen (tmppath, "w");
		for (;;) {
			if (!tok)
				break;
			fprintf (fp, "%s%s\r\n", server, tok);
			//snprintf (path, sizeof(path), "%s%s", VOL_SERVER1, tok);
			//snprintf (tmps, sizeof(tmps), "cd BAKUMAN_vol_01; wget %s", path);
			//system (tmps);
			tok = strtok (NULL, "|");
		}
		fclose (fp);
		struct stat st;
		if (stat (tmppath, &st) != -1 && st.st_size > 0) {
			rename (tmppath, savepath);
		}
	}
}

int
html_vol_cb (void *_hdr, void **arg, void *parent)
{
html_hdr_t *hdr = (html_hdr_t *) _hdr;
	if (hdr->status == HTML_ESTABLISH) {
		return 1;
	} else
	if (hdr->status == HTML_TAG) {
		switch (hdr->tag) {
			case TAG_SCRIPT:
				return 1;
			default:
				return 0;
		}
		return 0;
	} else
	if (hdr->status == HTML_DATA) {
		if (hdr->tag == TAG_SCRIPT) {
			char *dup = malloc (hdr->len+1);
			memcpy (dup, hdr->data, hdr->len);
			dup[hdr->len] = '\0';
			char *now;
			char *end;
			int dmi = 0;
			if ((now=strstr(dup, "sPath=\"")) && (now+=7) && now[0] && (end=strchr(now, '"')) && end-now > 0 && end-now < 4) {
				char dm[4];
				memcpy (dm, now, end-now);
				dm[end-now] = '\0';
				dmi = atoi (dm);
			}
			if (dmi && (now=strstr(dup, "sFiles=\"")) && (now+=8) && now[0] && (end=strchr(now, '"')) && end-now > 0) {
				end[0] = '\0';
				html_vol_sfiles (now, dmi);
			}
			free (dup);
		}
	} else
	if (hdr->status == HTML_DATA_INCOMPLETE) {
	} else
	if (hdr->status == HTML_CLOSE) {
	}
	return 0;
}
int
b99770_vol (char *url)
{
	CURL *curl = NULL;
	//struct curl_slist *headers = NULL;
	CURLcode res;
	struct doc docbuf = {NULL, 0};
	//add2list(&headers, "Accept-Language: en-us,en;q=0.5");
	//add2list(&headers, "Accept-Charset: utf-8");

	if (!(curl=curl_easy_init())) {
                sprintf (gErrmsg, "%s:%d curl_easy_init() fail", __FILE__, __LINE__);
                return -1;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
        if ((res=curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "61.31.233.1")) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
        }
/*
	struct curl_slist *host = NULL;
	host = curl_slist_append(NULL, "mh.99770.cc:80:58.215.241.45");
        if ((res=curl_easy_setopt(curl, CURLOPT_RESOLVE, host)) != CURLE_OK) {
                sprintf (gErrmsg, "%s:%d curl_easy_setopt: %s", __FILE__, __LINE__, curl_easy_strerror(res));
                goto ERR;
	}
*/
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

	struct page page = {0};
	html_parser (docbuf.data, docbuf.len, html_vol_cb, &page);
	//dump_text ((unsigned char *)docbuf.data, docbuf.len);

	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return 0;
ERR:
	SAFE_FREE (docbuf.data);
	//curl_slist_free_all (headers);
	//curl_slist_free_all(host);
	curl_easy_cleanup(curl);
	return -1;
}

void
show_usage (char *path)
{
	printf("Usage:\n");
	printf("\t%s [-t top][-n newest][-l list][-c comic][-v vol][-s path]\n", path);
	printf("\t-t top, Specify top page (http://mh.99770.cc/top/)\n");
	printf("\t-n newest, Specify newest page (http://mh.99770.cc/comicupdate/)\n");
	printf("\t-l list, Specify list page (ex: http://mh.99770.cc/comiclist/1/)\n");
	printf("\t-c comic, Specify comic page\n");
	printf("\t-v vol, Get vol\n");
	printf("\t-s path, Specify save path\n");
}

int
main (int argc, char *argv[])
{
	int	i;
	int     option_index = 0;
	struct  option long_options[] = {
		{"help", 0, 0, 0},
		{"version", 0, 0, 0}
	};
	int	istop=0;
	int	isnewest=0;
	int	islist=0;
	int	iscomic=0;
	int	isvol=0;
	char	page[1024];

	while ((i=getopt_long(argc, argv, "t:n:l:c:v:s:", long_options, &option_index)) != -1) {
		if (i == 't') {
			istop++;
			strcpy (page, optarg);
		} else
		if (i == 'n') {
			isnewest++;
			strcpy (page, optarg);
		} else
		if (i == 'l') {
			islist++;
			strcpy (page, optarg);
		} else
		if (i == 'c') {
			iscomic++;
			strcpy (page, optarg);
		} else
		if (i == 'v') {
			isvol++;
			strcpy (page, optarg);
		} else
		if (i == 's') {
			strcpy (g_save_path, optarg);
			snprintf (g_save_tmp_path, sizeof(g_save_tmp_path), "%s.tmp", g_save_path);
		} else
		if (i == 0) {
			if (!strncmp(long_options[option_index].name,"help",4)) {
				show_usage (argv[0]);
				exit (EXIT_SUCCESS);
			}
			if (!strncmp(long_options[option_index].name,"version",7)) {
				printf ("b99770 1.0\n");
				exit (EXIT_SUCCESS);
			}
		}
	}
	if (istop) {
		b99770_top (page);
	} else
	if (isnewest) {
		b99770_newest (page);
	} else
	if (islist) {
		b99770_list (page);
	} else
	if (iscomic) {
		b99770_comic (page);
	} else
	if (isvol) {
		b99770_vol (page);
	} else {
		show_usage (argv[0]);
	}

	return 0;
}
