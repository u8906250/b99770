#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "libhtml.h"

#define TEMPLATE_SEEK_HTML_TAG(NAME,TOP)\
struct NAME##_t *\
seek_##NAME (char *data, int len)\
{\
	int i;\
	for (i=0; i<TOP; i++) {\
		if ((strncasecmp(data, NAME[i].name, NAME[i].len)))\
		continue;\
		return &NAME[i];\
	}\
	return NULL;\
}

TEMPLATE_SEEK_HTML_TAG (html_tag, REG_TAG)
TEMPLATE_SEEK_HTML_TAG (html_attr, REG_ATTR)

char gLibhtmlErrmsg[256];
//utils
int
html_get_tag_content (char *s, char *buf, int size)
{
	char *start, *end;
	if ((start=strchr(s, '>')) && (end=strrchr(s, '<')) && (start++) && end-start > 0 && end-start < size) {
		memcpy (buf, start, end-start);
		buf[end-start] = '\0';
		return 1;
	}
	return 0;
}

//main
int
html_attr_parser (char *data, int len, int (*cb_func)(void *, void **, void *), void *parent)
{
html_hdr_t *hdr = NULL;
html_attr_t *hattr= NULL;
void *arg = NULL;
char *now = data;
char *end = data+len;
char *left, *right;
int mv = 0;

	if (!(hdr=malloc(sizeof(html_hdr_t)))) {
                sprintf (gLibhtmlErrmsg, "%s:%d %s", __FILE__, __LINE__, strerror(errno));
		return -1;
	}

	hdr->status    = HTML_ESTABLISH;
	hdr->data      = NULL;
	hdr->len       = 0;
	hdr->tag       = -1;

	if (!cb_func ((void*)hdr, &arg, parent)) {
		safe_free (&hdr);
		return 0;
	}

	if ((mv=seek_first_meet_char(now, end-now, '<')) == -1)
		goto CLOSE_HTML;

	now += mv;
	if ((mv=seek_oppo_brackets(now, end-now, '<', '>')) <= 0)
		goto CLOSE_HTML;
	now ++;
	end = now + mv;

	while (now < end) {
		if ((mv=seek_first_meet_char(now, end-now, ' ')) == -1)
			break;
		now += mv;
		if (!(hattr=seek_html_attr(now+1, end-now))) {
			now ++;
			continue;
		}
		hdr->status    = HTML_TAG;
		hdr->tag       = hattr->tag;
		if (!cb_func ((void*)hdr, &arg, parent)) {
			now ++;
			continue;
		}
		if ((mv=seek_first_meet_char(now, end-now, '=')) == -1)
			break;
		now += (mv+1);
		left = right = NULL;
		while (now < end) {
			if (now[0] == '"' || now[0] == '\'') {
				if (!left)
					left = now;
				else
					if (!right)
						right = now;
			}
			if (left && right)
				break;
			now ++;
		}
		if (left && right && right-left > 2) {
			hdr->status    = HTML_DATA;
			hdr->data      = left+1;
			hdr->len       = right-left-1;
			cb_func ((void*)hdr, &arg, parent);
		}
	}


CLOSE_HTML:
	hdr->status    = HTML_CLOSE;
	hdr->data      = NULL;
	hdr->len       = 0;
	hdr->tag       = -1;

	cb_func ((void*)hdr, &arg, parent);
	safe_free (&hdr);

	return 0;
}


int
html_parser (char *data, int len, int (*cb_func)(void *, void **, void *), void *parent)
{
html_hdr_t *hdr = NULL;
struct html_tag_t *htag= NULL;
void *arg = NULL;
char *now = data;
char *end = data+len;
int mv = 0;

	if (!(hdr=malloc(sizeof(html_hdr_t)))) {
                sprintf (gLibhtmlErrmsg, "%s:%d %s", __FILE__, __LINE__, strerror(errno));
		return -1;
	}

	hdr->status    = HTML_ESTABLISH;
	hdr->data      = NULL;
	hdr->len       = 0;
	hdr->tag       = -1;

	if (!cb_func ((void*)hdr, &arg, parent)) {
		safe_free (&hdr);
		return 0;
	}

	while (now < end) {
		if ((mv=seek_first_meet_char(now, end-now, '<')) == -1)
			break;
		now += mv;
		if ((mv=seek_oppo_brackets(now, end-now, '<', '>')) <= 0) {
			if (end-now <= 0)
				goto SEEKED;
			//DEBUG("libhtml", "Data incomplete, lost \">\"");
			hdr->status    = HTML_DATA_INCOMPLETE;
			hdr->data      = now;
			hdr->len       = end-now;
			cb_func ((void*)hdr, &arg, parent); 
			goto DATA_INCOMPLETE_CLOSE;
		}
		if (!(htag=seek_html_tag(now+1, mv))) {
			now += (mv + 2);
			continue;
		}
		hdr->status    = HTML_TAG;
		hdr->tag       = htag->tag;


		if (!cb_func ((void*)hdr, &arg, parent)) {
			if (htag->tag == TAG_SCRIPT) {
				char *ptr;
				if (!(ptr=memmem(now, end-now, "</script", 8))) {
					goto DATA_INCOMPLETE_CLOSE;
				}
				now = ptr+8;
			} else {
				now += (mv + 2);
			}
			continue;
		}
		if (!htag->end) {
			hdr->status    = HTML_DATA;
			hdr->data      = now;
			hdr->len       = mv+2;
			cb_func ((void*)hdr, &arg, parent);
			now += (mv + 2);
			continue;
		} else { // seek end of tag
			char *bgn = now;
			now += (mv + 2);
			int layer = 0;
			int haslayer = 0;
			while (now < end) {
				if ((mv=seek_first_meet_char(now, end-now, '<')) == -1)
					break;
				now += mv;
				if ((mv=seek_oppo_brackets(now, end-now, '<', '>')) <= 0) {
					if (end-bgn <= 0)
						goto SEEKED;
					//DEBUG("libhtml", "Data incomplete, lost \">\"");
					hdr->status    = HTML_DATA_INCOMPLETE;
					hdr->data      = bgn;
					hdr->len       = end-bgn;
					cb_func ((void*)hdr, &arg, parent);
					goto DATA_INCOMPLETE_CLOSE;
				}
				if (!strncasecmp(now+1, htag->name, htag->len)) {
					layer ++;
					haslayer ++;
					now += (mv + 2);
					continue;
				}
				if (strncasecmp(now+1, htag->end, htag->elen)) {
					if (!strncasecmp(now, "<script", 7)) {
						char *ptr;
						if (!(ptr=memmem(now, end-now, "</script", 8))) {
							goto DATA_INCOMPLETE_CLOSE;
						}
						now = ptr+8;
					} else {
						now += (mv + 2);
					}
					continue;
				}
				if (layer > 0) {
					layer --;
					now += (mv + 2);
					continue;
				}
				hdr->status    = HTML_DATA;
				hdr->data      = bgn;
				hdr->len       = (now-bgn) + (mv+2);
				cb_func ((void*)hdr, &arg, parent);
				if (!haslayer) {
					now += (mv + 2);
				} else {
					now = bgn + 2;
				}
				goto SEEKED;
			}
			if (end-bgn <= 0)
				goto SEEKED;
			//DEBUG("libhtml", "Data incomplete, lost \"%s\"", htag->end);
			hdr->status    = HTML_DATA_INCOMPLETE;
			hdr->data      = bgn;
			hdr->len       = end-bgn;
			cb_func ((void*)hdr, &arg, parent);
			goto DATA_INCOMPLETE_CLOSE;
SEEKED:
			continue;

		}
	}

	hdr->status    = HTML_CLOSE;
	hdr->data      = NULL;
	hdr->len       = 0;
	hdr->tag       = -1;
	cb_func ((void*)hdr, &arg, parent);
	safe_free (&hdr);
	return 0;

DATA_INCOMPLETE_CLOSE:
	hdr->status    = HTML_CLOSE;
	hdr->data      = NULL;
	hdr->len       = 0;
	hdr->tag       = -1;
	cb_func ((void*)hdr, &arg, parent);
	safe_free (&hdr);
	return 0;

}
