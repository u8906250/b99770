
enum {
	/*----- tag -------*/
	TAG_HTML=0,
	TAG_TR,
	TAG_TD,
	TAG_TH,
	TAG_A,
	TAG_DIV,
	TAG_SPAN,
	TAG_SCRIPT
};

#define REG_TAG 8
struct html_tag_t {
	const int       tag;
	const char      *name;
	const int       len;
	const char      *end;
	const int       elen;
};

static struct html_tag_t html_tag[REG_TAG] = {
	{TAG_HTML,	"html",	4,	"/html",	5},
	{TAG_TR,	"tr",	2,	"/tr",		3},
	{TAG_TD,	"td",	2,	"/td",		3},
	{TAG_TH,	"th",	2,	"/th",		3},
	{TAG_A,		"a",	1,	"/a",		2},
	{TAG_DIV,	"div",	3,	"/div",		4},
	{TAG_SPAN,	"span",	4,	"/span",	5},
	{TAG_SCRIPT,	"script",	6,	"/script",		7}
};


enum {
	ATTR_HREF=0,
	ATTR_CLASS,
	ATTR_ID,
	ATTR_TITLE
};
#define REG_ATTR   4
typedef struct html_attr_t {
	const int       tag;
	const char      *name;
	const int       len;
} html_attr_t;
static html_attr_t html_attr[REG_ATTR] = {
	{ATTR_HREF,	"href",		4},
	{ATTR_CLASS,	"class",	5},
	{ATTR_ID,	"id",		2},
	{ATTR_TITLE,	"title",	5}
};



typedef struct html_hdr_t {
	int     status;
	char    *data;
	int     len;
	int     tag;
} html_hdr_t;

enum {  
	HTML_CLOSE=0,
	HTML_ESTABLISH, 
	HTML_TAG,
	HTML_DATA,      
	HTML_DATA_INCOMPLETE
};

int
html_get_tag_content (char *s, char *buf, int size);

int
html_parser (char *data, int len, int (*cb_func)(void *, void **, void *), void *parent);

int
html_attr_parser (char *data, int len, int (*cb_func)(void *, void **, void *), void *parent);
