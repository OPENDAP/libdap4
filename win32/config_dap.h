/*  This file is typically generated during configuation      */
/*  time, but that is not the case under win32.  This file    */
/*  was generated under linux on an x86 and then stripped     */
/*  down to the bare essentials and modified where necessary  */
/*  to form something appropriate for win32.                  */

#ifndef _DODS_CONFIG_H_
#define _DODS_CONFIG_H_

#define HAVE_ALLOCA 1
#define HAVE_ALLOCA_H 1
#define HAVE_MKTIME 1
#define STDC_HEADERS 1
#define CNAME "DODS"
#define CVER "$VERSION"
#define DVR "DODS/3.4.3"

#define SIZEOF_CHAR 1
#define SIZEOF_DOUBLE 8
#define SIZEOF_FLOAT 4
#define SIZEOF_INT 4
#define SIZEOF_LONG 4

#define HAVE_STRDUP 1
#define HAVE_FCNTL_H 1
#define HAVE_LIMITS_H 1

#define sleep(n) Sleep(n * 1000)

/*  Replaces dods-deployed stat.h defines only found on unixes  */
#define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)
#define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)

#ifndef DODS_ROOT
#define DODS_ROOT "C:/DODS"
#endif

#define DODS_BISON_VER 128

#define not_used 

#endif


