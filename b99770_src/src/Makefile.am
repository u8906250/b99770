
AUTOMAKE_OPTIONS=foreign

bin_PROGRAMS    = b99770

b99770_SOURCES	= main.c getcover.c libhtml.c utils.c

b99770_CFLAGS	= -Wall -O2 -I../../curl-7.28.1/include

b99770_LDADD	= ../../curl-7.28.1/lib/.libs/libcurl.a -lssl -lcrypto -lz -lldap -lssh2 -lidn -lrt ../../c-ares-1.9.1/lib/libcares.a

