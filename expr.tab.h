typedef union {
    bool boolean;
    int op;
    char id[ID_MAX];

    value val;

    bool_func b_func;
    btp_func bt_func;

    int_list *int_l_ptr;
    int_list_list *int_ll_ptr;
    
    rvalue *rval_ptr;
    rvalue_list *r_val_l_ptr;
} YYSTYPE;
#define	INT	258
#define	FLOAT	259
#define	STR	260
#define	ID	261
#define	FIELD	262
#define	EQUAL	263
#define	NOT_EQUAL	264
#define	GREATER	265
#define	GREATER_EQL	266
#define	LESS	267
#define	LESS_EQL	268
#define	REGEXP	269


extern YYSTYPE exprlval;
