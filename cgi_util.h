
// External definitions for functions defined in libnetio.a.
//
// 2/22/95 jhrg

char *name_path(char *path);
char *fmakeword(FILE *f, const char stop, int *cl) ;
void ErrMsgT(const char *Msgt);
void set_mime_test();
void set_mime_binary();

extern "C" {
    FILE *NetConnect(char *AbsAddress, char *PostData);
    FILE *NetExecute(char *AbsAddress, char *PostData);
}

/* Make *SURE* that any change to VERSION_LENGTH here is also made in HTTP.c */

#define VERSION_LENGTH 		20    /* Number of chars in protocol version */
extern char DVersion[VERSION_LENGTH]; /* version string from DODS MIME docs */
