#ifndef BISON_EXPR_TAB_H
# define BISON_EXPR_TAB_H

#ifndef YYSTYPE
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
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	SCAN_STR	257
# define	SCAN_WORD	258
# define	SCAN_EQUAL	259
# define	SCAN_NOT_EQUAL	260
# define	SCAN_GREATER	261
# define	SCAN_GREATER_EQL	262
# define	SCAN_LESS	263
# define	SCAN_LESS_EQL	264
# define	SCAN_REGEXP	265


extern YYSTYPE exprlval;

#endif /* not BISON_EXPR_TAB_H */
