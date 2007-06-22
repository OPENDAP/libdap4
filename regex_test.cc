#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <regex.h>
int main () { printf ("%d\n", 8 * sizeof (regoff_t)); return 0; }
