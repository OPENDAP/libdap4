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
#define	SCAN_INT	257
#define	SCAN_FLOAT	258
#define	SCAN_STR	259
#define	SCAN_ID	260
#define	SCAN_FIELD	261
#define	SCAN_EQUAL	262
#define	SCAN_NOT_EQUAL	263
#define	SCAN_GREATER	264
#define	SCAN_GREATER_EQL	265
#define	SCAN_LESS	266
#define	SCAN_LESS_EQL	267
#define	SCAN_REGEXP	268


extern YYSTYPE exprlval;
