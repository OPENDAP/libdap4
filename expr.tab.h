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
#define	INT	257
#define	FLOAT	258
#define	STR	259
#define	ID	260
#define	FIELD	261
#define	EQUAL	262
#define	NOT_EQUAL	263
#define	GREATER	264
#define	GREATER_EQL	265
#define	LESS	266
#define	LESS_EQL	267
#define	REGEXP	268


extern YYSTYPE exprlval;
