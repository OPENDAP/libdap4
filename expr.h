/*
  Types for the expr parser.

  11/4/95 jhrg
*/

/*
  $Log: expr.h,v $
  Revision 1.2  1996/02/01 17:43:15  jimg
  Added support for lists as operands in constraint expressions.

  Revision 1.1  1996/01/31 18:55:15  jimg
  Added to repository.

 */

#include <String.h>
#include <SLList.h>

#include "config_dap.h"

#include "BaseType.h"

typedef SLList<int> IntList;
typedef SLList<IntList *> IntListList;

typedef struct {
    Type type;			/* Type is an enum defined in BaseType.h */
    union {
	int32 i;
	float64 f;
	String *s;
    } v;
} value;

typedef SLList<BaseType *> RValList;
