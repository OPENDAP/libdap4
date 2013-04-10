/* Extended regular expression matching and search library.
   Copyright (C) 2002,2003,2004,2005,2006,2007 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Isamu Hasegawa <isamu@yamato.ibm.com>.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. */

static reg_errcode_t re_compile_internal (regex_t *preg, const char * pattern,
					  size_t length, reg_syntax_t syntax);
static void re_compile_fastmap_iter (regex_t *bufp,
				     const re_dfastate_t *init_state,
				     char *fastmap);
static reg_errcode_t init_dfa (re_dfa_t *dfa, size_t pat_len);
#ifdef RE_ENABLE_I18N
static void free_charset (re_charset_t *cset);
#endif /* RE_ENABLE_I18N */
static void free_workarea_compile (regex_t *preg);
static reg_errcode_t create_initial_state (re_dfa_t *dfa);
#ifdef RE_ENABLE_I18N
static void optimize_utf8 (re_dfa_t *dfa);
#endif
static reg_errcode_t analyze (regex_t *preg);
static reg_errcode_t preorder (bin_tree_t *root,
			       reg_errcode_t (fn (void *, bin_tree_t *)),
			       void *extra);
static reg_errcode_t postorder (bin_tree_t *root,
				reg_errcode_t (fn (void *, bin_tree_t *)),
				void *extra);
static reg_errcode_t optimize_subexps (void *extra, bin_tree_t *node);
static reg_errcode_t lower_subexps (void *extra, bin_tree_t *node);
static bin_tree_t *lower_subexp (reg_errcode_t *err, regex_t *preg,
				 bin_tree_t *node);
static reg_errcode_t calc_first (void *extra, bin_tree_t *node);
static reg_errcode_t calc_next (void *extra, bin_tree_t *node);
static reg_errcode_t link_nfa_nodes (void *extra, bin_tree_t *node);
static Idx duplicate_node (re_dfa_t *dfa, Idx org_idx, unsigned int constraint);
static Idx search_duplicated_node (const re_dfa_t *dfa, Idx org_node,
				   unsigned int constraint);
static reg_errcode_t calc_eclosure (re_dfa_t *dfa);
static reg_errcode_t calc_eclosure_iter (re_node_set *new_set, re_dfa_t *dfa,
					 Idx node, bool root);
static reg_errcode_t calc_inveclosure (re_dfa_t *dfa);
static Idx fetch_number (re_string_t *input, re_token_t *token,
			 reg_syntax_t syntax);
static int peek_token (re_token_t *token, re_string_t *input,
			reg_syntax_t syntax) internal_function;
static bin_tree_t *parse (re_string_t *regexp, regex_t *preg,
			  reg_syntax_t syntax, reg_errcode_t *err);
static bin_tree_t *parse_reg_exp (re_string_t *regexp, regex_t *preg,
				  re_token_t *token, reg_syntax_t syntax,
				  Idx nest, reg_errcode_t *err);
static bin_tree_t *parse_branch (re_string_t *regexp, regex_t *preg,
				 re_token_t *token, reg_syntax_t syntax,
				 Idx nest, reg_errcode_t *err);
static bin_tree_t *parse_expression (re_string_t *regexp, regex_t *preg,
				     re_token_t *token, reg_syntax_t syntax,
				     Idx nest, reg_errcode_t *err);
static bin_tree_t *parse_sub_exp (re_string_t *regexp, regex_t *preg,
				  re_token_t *token, reg_syntax_t syntax,
				  Idx nest, reg_errcode_t *err);
static bin_tree_t *parse_dup_op (bin_tree_t *dup_elem, re_string_t *regexp,
				 re_dfa_t *dfa, re_token_t *token,
				 reg_syntax_t syntax, reg_errcode_t *err);
static bin_tree_t *parse_bracket_exp (re_string_t *regexp, re_dfa_t *dfa,
				      re_token_t *token, reg_syntax_t syntax,
				      reg_errcode_t *err);
static reg_errcode_t parse_bracket_element (bracket_elem_t *elem,
					    re_string_t *regexp,
					    re_token_t *token, int token_len,
					    re_dfa_t *dfa,
					    reg_syntax_t syntax,
					    bool accept_hyphen);
static reg_errcode_t parse_bracket_symbol (bracket_elem_t *elem,
					  re_string_t *regexp,
					  re_token_t *token);
#ifdef RE_ENABLE_I18N
static reg_errcode_t build_equiv_class (bitset_t sbcset,
					re_charset_t *mbcset,
					Idx *equiv_class_alloc,
					const unsigned char *name);
static reg_errcode_t build_charclass (RE_TRANSLATE_TYPE trans,
				      bitset_t sbcset,
				      re_charset_t *mbcset,
				      Idx *char_class_alloc,
				      const unsigned char *class_name,
				      reg_syntax_t syntax);
#else  /* not RE_ENABLE_I18N */
static reg_errcode_t build_equiv_class (bitset_t sbcset,
					const unsigned char *name);
static reg_errcode_t build_charclass (RE_TRANSLATE_TYPE trans,
				      bitset_t sbcset,
				      const unsigned char *class_name,
				      reg_syntax_t syntax);
#endif /* not RE_ENABLE_I18N */
static bin_tree_t *build_charclass_op (re_dfa_t *dfa,
				       RE_TRANSLATE_TYPE trans,
				       const unsigned char *class_name,
				       const unsigned char *extra,
				       bool non_match, reg_errcode_t *err);
static bin_tree_t *create_tree (re_dfa_t *dfa,
				bin_tree_t *left, bin_tree_t *right,
				re_token_type_t type);
static bin_tree_t *create_token_tree (re_dfa_t *dfa,
				      bin_tree_t *left, bin_tree_t *right,
				      const re_token_t *token);
static bin_tree_t *duplicate_tree (const bin_tree_t *src, re_dfa_t *dfa);
static void free_token (re_token_t *node);
static reg_errcode_t free_tree (void *extra, bin_tree_t *node);
static reg_errcode_t mark_opt_subexp (void *extra, bin_tree_t *node);

/* This table gives an error message for each of the error codes listed
   in regex.h.  Obviously the order here has to be same as there.
   POSIX doesn't require that we do anything for REG_NOERROR,
   but why not be nice?  */

static const char __re_error_msgid[] =
  {
#define REG_NOERROR_IDX	0
    gettext_noop ("Success")	/* REG_NOERROR */
    "\0"
#define REG_NOMATCH_IDX (REG_NOERROR_IDX + sizeof "Success")
    gettext_noop ("No match")	/* REG_NOMATCH */
    "\0"
#define REG_BADPAT_IDX	(REG_NOMATCH_IDX + sizeof "No match")
    gettext_noop ("Invalid regular expression") /* REG_BADPAT */
    "\0"
#define REG_ECOLLATE_IDX (REG_BADPAT_IDX + sizeof "Invalid regular expression")
    gettext_noop ("Invalid collation character") /* REG_ECOLLATE */
    "\0"
#define REG_ECTYPE_IDX	(REG_ECOLLATE_IDX + sizeof "Invalid collation character")
    gettext_noop ("Invalid character class name") /* REG_ECTYPE */
    "\0"
#define REG_EESCAPE_IDX	(REG_ECTYPE_IDX + sizeof "Invalid character class name")
    gettext_noop ("Trailing backslash") /* REG_EESCAPE */
    "\0"
#define REG_ESUBREG_IDX	(REG_EESCAPE_IDX + sizeof "Trailing backslash")
    gettext_noop ("Invalid back reference") /* REG_ESUBREG */
    "\0"
#define REG_EBRACK_IDX	(REG_ESUBREG_IDX + sizeof "Invalid back reference")
    gettext_noop ("Unmatched [ or [^")	/* REG_EBRACK */
    "\0"
#define REG_EPAREN_IDX	(REG_EBRACK_IDX + sizeof "Unmatched [ or [^")
    gettext_noop ("Unmatched ( or \\(") /* REG_EPAREN */
    "\0"
#define REG_EBRACE_IDX	(REG_EPAREN_IDX + sizeof "Unmatched ( or \\(")
    gettext_noop ("Unmatched \\{") /* REG_EBRACE */
    "\0"
#define REG_BADBR_IDX	(REG_EBRACE_IDX + sizeof "Unmatched \\{")
    gettext_noop ("Invalid content of \\{\\}") /* REG_BADBR */
    "\0"
#define REG_ERANGE_IDX	(REG_BADBR_IDX + sizeof "Invalid content of \\{\\}")
    gettext_noop ("Invalid range end")	/* REG_ERANGE */
    "\0"
#define REG_ESPACE_IDX	(REG_ERANGE_IDX + sizeof "Invalid range end")
    gettext_noop ("Memory exhausted") /* REG_ESPACE */
    "\0"
#define REG_BADRPT_IDX	(REG_ESPACE_IDX + sizeof "Memory exhausted")
    gettext_noop ("Invalid preceding regular expression") /* REG_BADRPT */
    "\0"
#define REG_EEND_IDX	(REG_BADRPT_IDX + sizeof "Invalid preceding regular expression")
    gettext_noop ("Premature end of regular expression") /* REG_EEND */
    "\0"
#define REG_ESIZE_IDX	(REG_EEND_IDX + sizeof "Premature end of regular expression")
    gettext_noop ("Regular expression too big") /* REG_ESIZE */
    "\0"
#define REG_ERPAREN_IDX	(REG_ESIZE_IDX + sizeof "Regular expression too big")
    gettext_noop ("Unmatched ) or \\)") /* REG_ERPAREN */
  };

static const size_t __re_error_msgid_idx[] =
  {
    REG_NOERROR_IDX,
    REG_NOMATCH_IDX,
    REG_BADPAT_IDX,
    REG_ECOLLATE_IDX,
    REG_ECTYPE_IDX,
    REG_EESCAPE_IDX,
    REG_ESUBREG_IDX,
    REG_EBRACK_IDX,
    REG_EPAREN_IDX,
    REG_EBRACE_IDX,
    REG_BADBR_IDX,
    REG_ERANGE_IDX,
    REG_ESPACE_IDX,
    REG_BADRPT_IDX,
    REG_EEND_IDX,
    REG_ESIZE_IDX,
    REG_ERPAREN_IDX
  };

/* Entry points for GNU code.  */

/* re_compile_pattern is the GNU regular expression compiler: it
   compiles PATTERN (of length LENGTH) and puts the result in BUFP.
   Returns 0 if the pattern was valid, otherwise an error string.

   Assumes the `allocated' (and perhaps `buffer') and `translate' fields
   are set in BUFP on entry.  */

#ifdef _LIBC
const char *
re_compile_pattern (pattern, length, bufp)
    const char *pattern;
    size_t length;
    struct re_pattern_buffer *bufp;
#else /* size_t might promote */
const char *
re_compile_pattern (const char *pattern, size_t length,
		    struct re_pattern_buffer *bufp)
#endif
{
  reg_errcode_t ret;

  /* And GNU code determines whether or not to get register information
     by passing null for the REGS argument to re_match, etc., not by
     setting no_sub, unless RE_NO_SUB is set.  */
  bufp->no_sub = !!(re_syntax_options & RE_NO_SUB);

  /* Match anchors at newline.  */
  bufp->newline_anchor = 1;

  ret = re_compile_internal (bufp, pattern, length, re_syntax_options);

  if (!ret)
    return NULL;
  return gettext (__re_error_msgid + __re_error_msgid_idx[(int) ret]);
}
#ifdef _LIBC
weak_alias (__re_compile_pattern, re_compile_pattern)
#endif

/* Set by `re_set_syntax' to the current regexp syntax to recognize.  Can
   also be assigned to arbitrarily: each pattern buffer stores its own
   syntax, so it can be changed between regex compilations.  */
/* This has no initializer because initialized variables in Emacs
   become read-only after dumping.  */
reg_syntax_t re_syntax_options;


/* Specify the precise syntax of regexps for compilation.  This provides
   for compatibility for various utilities which historically have
   different, incompatible syntaxes.

   The argument SYNTAX is a bit mask comprised of the various bits
   defined in regex.h.  We return the old syntax.  */

reg_syntax_t
re_set_syntax (syntax)
    reg_syntax_t syntax;
{
  reg_syntax_t ret = re_syntax_options;

  re_syntax_options = syntax;
  return ret;
}
#ifdef _LIBC
weak_alias (__re_set_syntax, re_set_syntax)
#endif

int
re_compile_fastmap (bufp)
    struct re_pattern_buffer *bufp;
{
  re_dfa_t *dfa = (re_dfa_t *) bufp->buffer;
  char *fastmap = bufp->fastmap;

  memset (fastmap, '\0', sizeof (char) * SBC_MAX);
  re_compile_fastmap_iter (bufp, dfa->init_state, fastmap);
  if (dfa->init_state != dfa->init_state_word)
    re_compile_fastmap_iter (bufp, dfa->init_state_word, fastmap);
  if (dfa->init_state != dfa->init_state_nl)
    re_compile_fastmap_iter (bufp, dfa->init_state_nl, fastmap);
  if (dfa->init_state != dfa->init_state_begbuf)
    re_compile_fastmap_iter (bufp, dfa->init_state_begbuf, fastmap);
  bufp->fastmap_accurate = 1;
  return 0;
}
#ifdef _LIBC
weak_alias (__re_compile_fastmap, re_compile_fastmap)
#endif

static _inline void
__attribute ((always_inline))
re_set_fastmap (char *fastmap, bool icase, int ch)
{
  fastmap[ch] = 1;
  if (icase)
    fastmap[tolower (ch)] = 1;
}

/* Helper function for re_compile_fastmap.
   Compile fastmap for the initial_state INIT_STATE.  */

static void
re_compile_fastmap_iter (regex_t *bufp, const re_dfastate_t *init_state,
			 char *fastmap)
{
  re_dfa_t *dfa = (re_dfa_t *) bufp->buffer;
  Idx node_cnt;
  bool icase = (dfa->mb_cur_max == 1 && (bufp->syntax & RE_ICASE));
  for (node_cnt = 0; node_cnt < init_state->nodes.nelem; ++node_cnt)
    {
      Idx node = init_state->nodes.elems[node_cnt];
      re_token_type_t type = dfa->nodes[node].type;

      if (type == CHARACTER)
	{
	  re_set_fastmap (fastmap, icase, dfa->nodes[node].opr.c);
#ifdef RE_ENABLE_I18N
	  if ((bufp->syntax & RE_ICASE) && dfa->mb_cur_max > 1)
	    {
	      unsigned char buf[MB_LEN_MAX];
	      unsigned char *p;
	      wchar_t wc;
	      mbstate_t state;

	      p = buf;
	      *p++ = dfa->nodes[node].opr.c;
	      while (++node < dfa->nodes_len
		     &&	dfa->nodes[node].type == CHARACTER
		     && dfa->nodes[node].mb_partial)
		*p++ = dfa->nodes[node].opr.c;
	      memset (&state, '\0', sizeof (state));
	      if (mbrtowc (&wc, (const char *) buf, p - buf,
			   &state) == p - buf
		  && (__wcrtomb ((char *) buf, towlower (wc), &state)
		      != (size_t) -1))
		re_set_fastmap (fastmap, false, buf[0]);
	    }
#endif
	}
      else if (type == SIMPLE_BRACKET)
	{
	  int i, ch;
	  for (i = 0, ch = 0; i < BITSET_WORDS; ++i)
	    {
	      int j;
	      bitset_word_t w = dfa->nodes[node].opr.sbcset[i];
	      for (j = 0; j < BITSET_WORD_BITS; ++j, ++ch)
		if (w & ((bitset_word_t) 1 << j))
		  re_set_fastmap (fastmap, icase, ch);
	    }
	}
#ifdef RE_ENABLE_I18N
      else if (type == COMPLEX_BRACKET)
	{
	  Idx i;
	  re_charset_t *cset = dfa->nodes[node].opr.mbcset;
	  if (cset->non_match || cset->ncoll_syms || cset->nequiv_classes
	      || cset->nranges || cset->nchar_classes)
	    {
# ifdef _LIBC
	      if (_NL_CURRENT_WORD (LC_COLLATE, _NL_COLLATE_NRULES) != 0)
		{
		  /* In this case we want to catch the bytes which are
		     the first byte of any collation elements.
		     e.g. In da_DK, we want to catch 'a' since "aa"
			  is a valid collation element, and don't catch
			  'b' since 'b' is the only collation element
			  which starts from 'b'.  */
		  const int32_t *table = (const int32_t *)
		    _NL_CURRENT (LC_COLLATE, _NL_COLLATE_TABLEMB);
		  for (i = 0; i < SBC_MAX; ++i)
		    if (table[i] < 0)
		      re_set_fastmap (fastmap, icase, i);
		}
# else
	      if (dfa->mb_cur_max > 1)
		for (i = 0; i < SBC_MAX; ++i)
		  if (__btowc (i) == WEOF)
		    re_set_fastmap (fastmap, icase, i);
# endif /* not _LIBC */
	    }
	  for (i = 0; i < cset->nmbchars; ++i)
	    {
	      char buf[256];
	      mbstate_t state;
	      memset (&state, '\0', sizeof (state));
	      if (__wcrtomb (buf, cset->mbchars[i], &state) != (size_t) -1)
		re_set_fastmap (fastmap, icase, *(unsigned char *) buf);
	      if ((bufp->syntax & RE_ICASE) && dfa->mb_cur_max > 1)
		{
		  if (__wcrtomb (buf, towlower (cset->mbchars[i]), &state)
		      != (size_t) -1)
		    re_set_fastmap (fastmap, false, *(unsigned char *) buf);
		}
	    }
	}
#endif /* RE_ENABLE_I18N */
      else if (type == OP_PERIOD
#ifdef RE_ENABLE_I18N
	       || type == OP_UTF8_PERIOD
#endif /* RE_ENABLE_I18N */
	       || type == END_OF_RE)
	{
	  memset (fastmap, '\1', sizeof (char) * SBC_MAX);
	  if (type == END_OF_RE)
	    bufp->can_be_null = 1;
	  return;
	}
    }
}

/* Entry point for POSIX code.  */
/* regcomp takes a regular expression as a string and compiles it.

   PREG is a regex_t *.  We do not expect any fields to be initialized,
   since POSIX says we shouldn't.  Thus, we set

     `buffer' to the compiled pattern;
     `used' to the length of the compiled pattern;
     `syntax' to RE_SYNTAX_POSIX_EXTENDED if the
       REG_EXTENDED bit in CFLAGS is set; otherwise, to
       RE_SYNTAX_POSIX_BASIC;
     `newline_anchor' to REG_NEWLINE being set in CFLAGS;
     `fastmap' to an allocated space for the fastmap;
     `fastmap_accurate' to zero;
     `re_nsub' to the number of subexpressions in PATTERN.

   PATTERN is the address of the pattern string.

   CFLAGS is a series of bits which affect compilation.

     If REG_EXTENDED is set, we use POSIX extended syntax; otherwise, we
     use POSIX basic syntax.

     If REG_NEWLINE is set, then . and [^...] don't match newline.
     Also, regexec will try a match beginning after every newline.

     If REG_ICASE is set, then we considers upper- and lowercase
     versions of letters to be equivalent when matching.

     If REG_NOSUB is set, then when PREG is passed to regexec, that
     routine will report only success or failure, and nothing about the
     registers.

   It returns 0 if it succeeds, nonzero if it doesn't.  (See regex.h for
   the return codes and their meanings.)  */

int
regcomp (preg, pattern, cflags)
    regex_t *_Restrict_ preg;
    const char *_Restrict_ pattern;
    int cflags;
{
  reg_errcode_t ret;
  reg_syntax_t syntax = ((cflags & REG_EXTENDED) ? RE_SYNTAX_POSIX_EXTENDED
			 : RE_SYNTAX_POSIX_BASIC);

  preg->buffer = NULL;
  preg->allocated = 0;
  preg->used = 0;

  /* Try to allocate space for the fastmap.  */
  preg->fastmap = re_malloc (char, SBC_MAX);
  if (BE (preg->fastmap == NULL, 0))
    return REG_ESPACE;

  syntax |= (cflags & REG_ICASE) ? RE_ICASE : 0;

  /* If REG_NEWLINE is set, newlines are treated differently.  */
  if (cflags & REG_NEWLINE)
    { /* REG_NEWLINE implies neither . nor [^...] match newline.  */
      syntax &= ~RE_DOT_NEWLINE;
      syntax |= RE_HAT_LISTS_NOT_NEWLINE;
      /* It also changes the matching behavior.  */
      preg->newline_anchor = 1;
    }
  else
    preg->newline_anchor = 0;
  preg->no_sub = !!(cflags & REG_NOSUB);
  preg->translate = NULL;

  ret = re_compile_internal (preg, pattern, strlen (pattern), syntax);

  /* POSIX doesn't distinguish between an unmatched open-group and an
     unmatched close-group: both are REG_EPAREN.  */
  if (ret == REG_ERPAREN)
    ret = REG_EPAREN;

  /* We have already checked preg->fastmap != NULL.  */
  if (BE (ret == REG_NOERROR, 1))
    /* Compute the fastmap now, since regexec cannot modify the pattern
       buffer.  This function never fails in this implementation.  */
    (void) re_compile_fastmap (preg);
  else
    {
      /* Some error occurred while compiling the expression.  */
      re_free (preg->fastmap);
      preg->fastmap = NULL;
    }

  return (int) ret;
}
#ifdef _LIBC
weak_alias (__regcomp, regcomp)
#endif

/* Returns a message corresponding to an error code, ERRCODE, returned
   from either regcomp or regexec.   We don't use PREG here.  */

#ifdef _LIBC
size_t
regerror (errcode, preg, errbuf, errbuf_size)
    int errcode;
    const regex_t *_Restrict_ preg;
    char *_Restrict_ errbuf;
    size_t errbuf_size;
#else /* size_t might promote */
size_t
regerror (int errcode, const regex_t *_Restrict_ preg,
	  char *_Restrict_ errbuf, size_t errbuf_size)
#endif
{
  const char *msg;
  size_t msg_size;

  if (BE (errcode < 0
	  || errcode >= (int) (sizeof (__re_error_msgid_idx)
			       / sizeof (__re_error_msgid_idx[0])), 0))
    /* Only error codes returned by the rest of the code should be passed
       to this routine.  If we are given anything else, or if other regex
       code generates an invalid error code, then the program has a bug.
       Dump core so we can fix it.  */
    abort ();

  msg = gettext (__re_error_msgid + __re_error_msgid_idx[errcode]);

  msg_size = strlen (msg) + 1; /* Includes the null.  */

  if (BE (errbuf_size != 0, 1))
    {
      size_t cpy_size = msg_size;
      if (BE (msg_size > errbuf_size, 0))
	{
	  cpy_size = errbuf_size - 1;
	  errbuf[cpy_size] = '\0';
	}
      memcpy (errbuf, msg, cpy_size);
    }

  return msg_size;
}
#ifdef _LIBC
weak_alias (__regerror, regerror)
#endif


#ifdef RE_ENABLE_I18N
/* This static array is used for the map to single-byte characters when
   UTF-8 is used.  Otherwise we would allocate memory just to initialize
   it the same all the time.  UTF-8 is the preferred encoding so this is
   a worthwhile optimization.  */
static const bitset_t utf8_sb_map =
{
  /* Set the first 128 bits.  */
# if 4 * BITSET_WORD_BITS < ASCII_CHARS
#  error "bitset_word_t is narrower than 32 bits"
# elif 3 * BITSET_WORD_BITS < ASCII_CHARS
  BITSET_WORD_MAX, BITSET_WORD_MAX, BITSET_WORD_MAX,
# elif 2 * BITSET_WORD_BITS < ASCII_CHARS
  BITSET_WORD_MAX, BITSET_WORD_MAX,
# elif 1 * BITSET_WORD_BITS < ASCII_CHARS
  BITSET_WORD_MAX,
# endif
  (BITSET_WORD_MAX
   >> (SBC_MAX % BITSET_WORD_BITS == 0
       ? 0
       : BITSET_WORD_BITS - SBC_MAX % BITSET_WORD_BITS))
};
#endif


static void
free_dfa_content (re_dfa_t *dfa)
{
  Idx i, j;

  if (dfa->nodes)
    for (i = 0; i < dfa->nodes_len; ++i)
      free_token (dfa->nodes + i);
  re_free (dfa->nexts);
  for (i = 0; i < dfa->nodes_len; ++i)
    {
      if (dfa->eclosures != NULL)
	re_node_set_free (dfa->eclosures + i);
      if (dfa->inveclosures != NULL)
	re_node_set_free (dfa->inveclosures + i);
      if (dfa->edests != NULL)
	re_node_set_free (dfa->edests + i);
    }
  re_free (dfa->edests);
  re_free (dfa->eclosures);
  re_free (dfa->inveclosures);
  re_free (dfa->nodes);

  if (dfa->state_table)
    for (i = 0; i <= dfa->state_hash_mask; ++i)
      {
	struct re_state_table_entry *entry = dfa->state_table + i;
	for (j = 0; j < entry->num; ++j)
	  {
	    re_dfastate_t *state = entry->array[j];
	    free_state (state);
	  }
        re_free (entry->array);
      }
  re_free (dfa->state_table);
#ifdef RE_ENABLE_I18N
  if (dfa->sb_char != utf8_sb_map)
    re_free (dfa->sb_char);
#endif
  re_free (dfa->subexp_map);
#ifdef DEBUG
  re_free (dfa->re_str);
#endif

  re_free (dfa);
}


/* Free dynamically allocated space used by PREG.  */

void
regfree (preg)
    regex_t *preg;
{
  re_dfa_t *dfa = (re_dfa_t *) preg->buffer;
  if (BE (dfa != NULL, 1))
    free_dfa_content (dfa);
  preg->buffer = NULL;
  preg->allocated = 0;

  re_free (preg->fastmap);
  preg->fastmap = NULL;

  re_free (preg->translate);
  preg->translate = NULL;
}
#ifdef _LIBC
weak_alias (__regfree, regfree)
#endif

/* Entry points compatible with 4.2 BSD regex library.  We don't define
   them unless specifically requested.  */

#if defined _REGEX_RE_COMP || defined _LIBC

/* BSD has one and only one pattern buffer.  */
static struct re_pattern_buffer re_comp_buf;

char *
# ifdef _LIBC
/* Make these definitions weak in libc, so POSIX programs can redefine
   these names if they don't use our functions, and still use
   regcomp/regexec above without link errors.  */
weak_function
# endif
re_comp (s)
     const char *s;
{
  reg_errcode_t ret;
  char *fastmap;

  if (!s)
    {
      if (!re_comp_buf.buffer)
	return gettext ("No previous regular expression");
      return 0;
    }

  if (re_comp_buf.buffer)
    {
      fastmap = re_comp_buf.fastmap;
      re_comp_buf.fastmap = NULL;
      __regfree (&re_comp_buf);
      memset (&re_comp_buf, '\0', sizeof (re_comp_buf));
      re_comp_buf.fastmap = fastmap;
    }

  if (re_comp_buf.fastmap == NULL)
    {
      re_comp_buf.fastmap = (char *) malloc (SBC_MAX);
      if (re_comp_buf.fastmap == NULL)
	return (char *) gettext (__re_error_msgid
				 + __re_error_msgid_idx[(int) REG_ESPACE]);
    }

  /* Since `re_exec' always passes NULL for the `regs' argument, we
     don't need to initialize the pattern buffer fields which affect it.  */

  /* Match anchors at newlines.  */
  re_comp_buf.newline_anchor = 1;

  ret = re_compile_internal (&re_comp_buf, s, strlen (s), re_syntax_options);

  if (!ret)
    return NULL;

  /* Yes, we're discarding `const' here if !HAVE_LIBINTL.  */
  return (char *) gettext (__re_error_msgid + __re_error_msgid_idx[(int) ret]);
}

#ifdef _LIBC
libc_freeres_fn (free_mem)
{
  __regfree (&re_comp_buf);
}
#endif

#endif /* _REGEX_RE_COMP */

/* Internal entry point.
   Compile the regular expression PATTERN, whose length is LENGTH.
   SYNTAX indicate regular expression's syntax.  */

static reg_errcode_t
re_compile_internal (regex_t *preg, const char * pattern, size_t length,
		     reg_syntax_t syntax)
{
  reg_errcode_t err = REG_NOERROR;
  re_dfa_t *dfa;
  re_string_t regexp;

  /* Initialize the pattern buffer.  */
  preg->fastmap_accurate = 0;
  preg->syntax = syntax;
  preg->not_bol = preg->not_eol = 0;
  preg->used = 0;
  preg->re_nsub = 0;
  preg->can_be_null = 0;
  preg->regs_allocated = REGS_UNALLOCATED;

  /* Initialize the dfa.  */
  dfa = (re_dfa_t *) preg->buffer;
  if (BE (preg->allocated < sizeof (re_dfa_t), 0))
    {
      /* If zero allocated, but buffer is non-null, try to realloc
	 enough space.  This loses if buffer's address is bogus, but
	 that is the user's responsibility.  If ->buffer is NULL this
	 is a simple allocation.  */
      dfa = re_realloc (preg->buffer, re_dfa_t, 1);
      if (dfa == NULL)
	return REG_ESPACE;
      preg->allocated = sizeof (re_dfa_t);
      preg->buffer = (unsigned char *) dfa;
    }
  preg->used = sizeof (re_dfa_t);

  err = init_dfa (dfa, length);
  if (BE (err != REG_NOERROR, 0))
    {
      free_dfa_content (dfa);
      preg->buffer = NULL;
      preg->allocated = 0;
      return err;
    }
#ifdef DEBUG
  /* Note: length+1 will not overflow since it is checked in init_dfa.  */
  dfa->re_str = re_malloc (char, length + 1);
  strncpy (dfa->re_str, pattern, length + 1);
#endif

  __libc_lock_init (dfa->lock);

  err = re_string_construct (&regexp, pattern, length, preg->translate,
			     syntax & RE_ICASE, dfa);
  if (BE (err != REG_NOERROR, 0))
    {
    re_compile_internal_free_return:
      free_workarea_compile (preg);
      re_string_destruct (&regexp);
      free_dfa_content (dfa);
      preg->buffer = NULL;
      preg->allocated = 0;
      return err;
    }

  /* Parse the regular expression, and build a structure tree.  */
  preg->re_nsub = 0;
  dfa->str_tree = parse (&regexp, preg, syntax, &err);
  if (BE (dfa->str_tree == NULL, 0))
    goto re_compile_internal_free_return;

  /* Analyze the tree and create the nfa.  */
  err = analyze (preg);
  if (BE (err != REG_NOERROR, 0))
    goto re_compile_internal_free_return;

#ifdef RE_ENABLE_I18N
  /* If possible, do searching in single byte encoding to speed things up.  */
  if (dfa->is_utf8 && !(syntax & RE_ICASE) && preg->translate == NULL)
    optimize_utf8 (dfa);
#endif

  /* Then create the initial state of the dfa.  */
  err = create_initial_state (dfa);

  /* Release work areas.  */
  free_workarea_compile (preg);
  re_string_destruct (&regexp);

  if (BE (err != REG_NOERROR, 0))
    {
      free_dfa_content (dfa);
      preg->buffer = NULL;
      preg->allocated = 0;
    }

  return err;
}

/* Initialize DFA.  We use the length of the regular expression PAT_LEN
   as the initial length of some arrays.  */

static reg_errcode_t
init_dfa (re_dfa_t *dfa, size_t pat_len)
{
  __re_size_t table_size;
#ifdef RE_ENABLE_I18N
  size_t max_i18n_object_size = MAX (sizeof (wchar_t), sizeof (wctype_t));
#else
  size_t max_i18n_object_size = 0;
#endif
  size_t max_object_size =
    MAX (sizeof (struct re_state_table_entry),
	 MAX (sizeof (re_token_t),
	      MAX (sizeof (re_node_set),
		   MAX (sizeof (regmatch_t),
			max_i18n_object_size))));

  memset (dfa, '\0', sizeof (re_dfa_t));

  /* Force allocation of str_tree_storage the first time.  */
  dfa->str_tree_storage_idx = BIN_TREE_STORAGE_SIZE;

  /* Avoid overflows.  The extra "/ 2" is for the table_size doubling
     calculation below, and for similar doubling calculations
     elsewhere.  And it's <= rather than <, because some of the
     doubling calculations add 1 afterwards.  */
  if (BE (SIZE_MAX / max_object_size / 2 <= pat_len, 0))
    return REG_ESPACE;

  dfa->nodes_alloc = pat_len + 1;
  dfa->nodes = re_malloc (re_token_t, dfa->nodes_alloc);

  /*  table_size = 2 ^ ceil(log pat_len) */
  for (table_size = 1; ; table_size <<= 1)
    if (table_size > pat_len)
      break;

  dfa->state_table = calloc (sizeof (struct re_state_table_entry), table_size);
  dfa->state_hash_mask = table_size - 1;

  dfa->mb_cur_max = MB_CUR_MAX;
#ifdef _LIBC
  if (dfa->mb_cur_max == 6
      && strcmp (_NL_CURRENT (LC_CTYPE, _NL_CTYPE_CODESET_NAME), "UTF-8") == 0)
    dfa->is_utf8 = 1;
  dfa->map_notascii = (_NL_CURRENT_WORD (LC_CTYPE, _NL_CTYPE_MAP_TO_NONASCII)
		       != 0);
#else
  if (strcmp (locale_charset (), "UTF-8") == 0)
    dfa->is_utf8 = 1;

  /* We check exhaustively in the loop below if this charset is a
     superset of ASCII.  */
  dfa->map_notascii = 0;
#endif

#ifdef RE_ENABLE_I18N
  if (dfa->mb_cur_max > 1)
    {
      if (dfa->is_utf8)
	dfa->sb_char = (re_bitset_ptr_t) utf8_sb_map;
      else
	{
	  int i, j, ch;

	  dfa->sb_char = (re_bitset_ptr_t) calloc (sizeof (bitset_t), 1);
	  if (BE (dfa->sb_char == NULL, 0))
	    return REG_ESPACE;

	  /* Set the bits corresponding to single byte chars.  */
	  for (i = 0, ch = 0; i < BITSET_WORDS; ++i)
	    for (j = 0; j < BITSET_WORD_BITS; ++j, ++ch)
	      {
		wint_t wch = __btowc (ch);
		if (wch != WEOF)
		  dfa->sb_char[i] |= (bitset_word_t) 1 << j;
# ifndef _LIBC
		if (isascii (ch) && wch != ch)
		  dfa->map_notascii = 1;
# endif
	      }
	}
    }
#endif

  if (BE (dfa->nodes == NULL || dfa->state_table == NULL, 0))
    return REG_ESPACE;
  return REG_NOERROR;
}

/* Initialize WORD_CHAR table, which indicate which character is
   "word".  In this case "word" means that it is the word construction
   character used by some operators like "\<", "\>", etc.  */

static void
internal_function
init_word_char (re_dfa_t *dfa)
{
  int i, j, ch;
  dfa->word_ops_used = 1;
  for (i = 0, ch = 0; i < BITSET_WORDS; ++i)
    for (j = 0; j < BITSET_WORD_BITS; ++j, ++ch)
      if (isalnum (ch) || ch == '_')
	dfa->word_char[i] |= (bitset_word_t) 1 << j;
}

/* Free the work area which are only used while compiling.  */

static void
free_workarea_compile (regex_t *preg)
{
  re_dfa_t *dfa = (re_dfa_t *) preg->buffer;
  bin_tree_storage_t *storage, *next;
  for (storage = dfa->str_tree_storage; storage; storage = next)
    {
      next = storage->next;
      re_free (storage);
    }
  dfa->str_tree_storage = NULL;
  dfa->str_tree_storage_idx = BIN_TREE_STORAGE_SIZE;
  dfa->str_tree = NULL;
  re_free (dfa->org_indices);
  dfa->org_indices = NULL;
}

/* Create initial states for all contexts.  */

static reg_errcode_t
create_initial_state (re_dfa_t *dfa)
{
  Idx first, i;
  reg_errcode_t err;
  re_node_set init_nodes;

  /* Initial states have the epsilon closure of the node which is
     the first node of the regular expression.  */
  first = dfa->str_tree->first->node_idx;
  dfa->init_node = first;
  err = re_node_set_init_copy (&init_nodes, dfa->eclosures + first);
  if (BE (err != REG_NOERROR, 0))
    return err;

  /* The back-references which are in initial states can epsilon transit,
     since in this case all of the subexpressions can be null.
     Then we add epsilon closures of the nodes which are the next nodes of
     the back-references.  */
  if (dfa->nbackref > 0)
    for (i = 0; i < init_nodes.nelem; ++i)
      {
	Idx node_idx = init_nodes.elems[i];
	re_token_type_t type = dfa->nodes[node_idx].type;

	Idx clexp_idx;
	if (type != OP_BACK_REF)
	  continue;
	for (clexp_idx = 0; clexp_idx < init_nodes.nelem; ++clexp_idx)
	  {
	    re_token_t *clexp_node;
	    clexp_node = dfa->nodes + init_nodes.elems[clexp_idx];
	    if (clexp_node->type == OP_CLOSE_SUBEXP
		&& clexp_node->opr.idx == dfa->nodes[node_idx].opr.idx)
	      break;
	  }
	if (clexp_idx == init_nodes.nelem)
	  continue;

	if (type == OP_BACK_REF)
	  {
	    Idx dest_idx = dfa->edests[node_idx].elems[0];
	    if (!re_node_set_contains (&init_nodes, dest_idx))
	      {
		re_node_set_merge (&init_nodes, dfa->eclosures + dest_idx);
		i = 0;
	      }
	  }
      }

  /* It must be the first time to invoke acquire_state.  */
  dfa->init_state = re_acquire_state_context (&err, dfa, &init_nodes, 0);
  /* We don't check ERR here, since the initial state must not be NULL.  */
  if (BE (dfa->init_state == NULL, 0))
    return err;
  if (dfa->init_state->has_constraint)
    {
      dfa->init_state_word = re_acquire_state_context (&err, dfa, &init_nodes,
						       CONTEXT_WORD);
      dfa->init_state_nl = re_acquire_state_context (&err, dfa, &init_nodes,
						     CONTEXT_NEWLINE);
      dfa->init_state_begbuf = re_acquire_state_context (&err, dfa,
							 &init_nodes,
							 CONTEXT_NEWLINE
							 | CONTEXT_BEGBUF);
      if (BE (dfa->init_state_word == NULL || dfa->init_state_nl == NULL
	      || dfa->init_state_begbuf == NULL, 0))
	return err;
    }
  else
    dfa->init_state_word = dfa->init_state_nl
      = dfa->init_state_begbuf = dfa->init_state;

  re_node_set_free (&init_nodes);
  return REG_NOERROR;
}

#ifdef RE_ENABLE_I18N
/* If it is possible to do searching in single byte encoding instead of UTF-8
   to speed things up, set dfa->mb_cur_max to 1, clear is_utf8 and change
   DFA nodes where needed.  */

static void
optimize_utf8 (re_dfa_t *dfa)
{
  Idx node;
  int i;
  bool mb_chars = false;
  bool has_period = false;

  for (node = 0; node < dfa->nodes_len; ++node)
    switch (dfa->nodes[node].type)
      {
      case CHARACTER:
	if (dfa->nodes[node].opr.c >= ASCII_CHARS)
	  mb_chars = true;
	break;
      case ANCHOR:
	switch (dfa->nodes[node].opr.idx)
	  {
	  case LINE_FIRST:
	  case LINE_LAST:
	  case BUF_FIRST:
	  case BUF_LAST:
	    break;
	  default:
	    /* Word anchors etc. cannot be handled.  */
	    return;
	  }
	break;
      case OP_PERIOD:
        has_period = true;
        break;
      case OP_BACK_REF:
      case OP_ALT:
      case END_OF_RE:
      case OP_DUP_ASTERISK:
      case OP_OPEN_SUBEXP:
      case OP_CLOSE_SUBEXP:
	break;
      case COMPLEX_BRACKET:
	return;
      case SIMPLE_BRACKET:
	/* Just double check.  */
	{
	  int rshift = (ASCII_CHARS % BITSET_WORD_BITS == 0
			? 0
			: BITSET_WORD_BITS - ASCII_CHARS % BITSET_WORD_BITS);
	  for (i = ASCII_CHARS / BITSET_WORD_BITS; i < BITSET_WORDS; ++i)
	    {
	      if (dfa->nodes[node].opr.sbcset[i] >> rshift != 0)
		return;
	      rshift = 0;
	    }
	}
	break;
      default:
	abort ();
      }

  if (mb_chars || has_period)
    for (node = 0; node < dfa->nodes_len; ++node)
      {
	if (dfa->nodes[node].type == CHARACTER
	    && dfa->nodes[node].opr.c >= ASCII_CHARS)
	  dfa->nodes[node].mb_partial = 0;
	else if (dfa->nodes[node].type == OP_PERIOD)
	  dfa->nodes[node].type = OP_UTF8_PERIOD;
      }

  /* The search can be in single byte locale.  */
  dfa->mb_cur_max = 1;
  dfa->is_utf8 = 0;
  dfa->has_mb_node = dfa->nbackref > 0 || has_period;
}
#endif

/* Analyze the structure tree, and calculate "first", "next", "edest",
   "eclosure", and "inveclosure".  */

static reg_errcode_t
analyze (regex_t *preg)
{
  re_dfa_t *dfa = (re_dfa_t *) preg->buffer;
  reg_errcode_t ret;

  /* Allocate arrays.  */
  dfa->nexts = re_malloc (Idx, dfa->nodes_alloc);
  dfa->org_indices = re_malloc (Idx, dfa->nodes_alloc);
  dfa->edests = re_malloc (re_node_set, dfa->nodes_alloc);
  dfa->eclosures = re_malloc (re_node_set, dfa->nodes_alloc);
  if (BE (dfa->nexts == NULL || dfa->org_indices == NULL || dfa->edests == NULL
	  || dfa->eclosures == NULL, 0))
    return REG_ESPACE;

  dfa->subexp_map = re_malloc (Idx, preg->re_nsub);
  if (dfa->subexp_map != NULL)
    {
      Idx i;
      for (i = 0; i < preg->re_nsub; i++)
	dfa->subexp_map[i] = i;
      preorder (dfa->str_tree, optimize_subexps, dfa);
      for (i = 0; i < preg->re_nsub; i++)
	if (dfa->subexp_map[i] != i)
	  break;
      if (i == preg->re_nsub)
	{
	  free (dfa->subexp_map);
	  dfa->subexp_map = NULL;
	}
    }

  ret = postorder (dfa->str_tree, lower_subexps, preg);
  if (BE (ret != REG_NOERROR, 0))
    return ret;
  ret = postorder (dfa->str_tree, calc_first, dfa);
  if (BE (ret != REG_NOERROR, 0))
    return ret;
  preorder (dfa->str_tree, calc_next, dfa);
  ret = preorder (dfa->str_tree, link_nfa_nodes, dfa);
  if (BE (ret != REG_NOERROR, 0))
    return ret;
  ret = calc_eclosure (dfa);
  if (BE (ret != REG_NOERROR, 0))
    return ret;

  /* We only need this during the prune_impossible_nodes pass in regexec.c;
     skip it if p_i_n will not run, as calc_inveclosure can be quadratic.  */
  if ((!preg->no_sub && preg->re_nsub > 0 && dfa->has_plural_match)
      || dfa->nbackref)
    {
      dfa->inveclosures = re_malloc (re_node_set, dfa->nodes_len);
      if (BE (dfa->inveclosures == NULL, 0))
        return REG_ESPACE;
      ret = calc_inveclosure (dfa);
    }

  return ret;
}

/* Our parse trees are very unbalanced, so we cannot use a stack to
   implement parse tree visits.  Instead, we use parent pointers and
   some hairy code in these two functions.  */
static reg_errcode_t
postorder (bin_tree_t *root, reg_errcode_t (fn (void *, bin_tree_t *)),
	   void *extra)
{
  bin_tree_t *node, *prev;

  for (node = root; ; )
    {
      /* Descend down the tree, preferably to the left (or to the right
	 if that's the only child).  */
      while (node->left || node->right)
	if (node->left)
          node = node->left;
        else
          node = node->right;

      do
	{
	  reg_errcode_t err = fn (extra, node);
	  if (BE (err != REG_NOERROR, 0))
	    return err;
          if (node->parent == NULL)
	    return REG_NOERROR;
	  prev = node;
	  node = node->parent;
	}
      /* Go up while we have a node that is reached from the right.  */
      while (node->right == prev || node->right == NULL);
      node = node->right;
    }
}

static reg_errcode_t
preorder (bin_tree_t *root, reg_errcode_t (fn (void *, bin_tree_t *)),
	  void *extra)
{
  bin_tree_t *node;

  for (node = root; ; )
    {
      reg_errcode_t err = fn (extra, node);
      if (BE (err != REG_NOERROR, 0))
	return err;

      /* Go to the left node, or up and to the right.  */
      if (node->left)
	node = node->left;
      else
	{
	  bin_tree_t *prev = NULL;
	  while (node->right == prev || node->right == NULL)
	    {
	      prev = node;
	      node = node->parent;
	      if (!node)
	        return REG_NOERROR;
	    }
	  node = node->right;
	}
    }
}

/* Optimization pass: if a SUBEXP is entirely contained, strip it and tell
   re_search_internal to map the inner one's opr.idx to this one's.  Adjust
   backreferences as well.  Requires a preorder visit.  */
static reg_errcode_t
optimize_subexps (void *extra, bin_tree_t *node)
{
  re_dfa_t *dfa = (re_dfa_t *) extra;

  if (node->token.type == OP_BACK_REF && dfa->subexp_map)
    {
      int idx = node->token.opr.idx;
      node->token.opr.idx = dfa->subexp_map[idx];
      dfa->used_bkref_map |= 1 << node->token.opr.idx;
    }

  else if (node->token.type == SUBEXP
           && node->left && node->left->token.type == SUBEXP)
    {
      Idx other_idx = node->left->token.opr.idx;

      node->left = node->left->left;
      if (node->left)
        node->left->parent = node;

      dfa->subexp_map[other_idx] = dfa->subexp_map[node->token.opr.idx];
      if (other_idx < BITSET_WORD_BITS)
	dfa->used_bkref_map &= ~((bitset_word_t) 1 << other_idx);
    }

  return REG_NOERROR;
}

/* Lowering pass: Turn each SUBEXP node into the appropriate concatenation
   of OP_OPEN_SUBEXP, the body of the SUBEXP (if any) and OP_CLOSE_SUBEXP.  */
static reg_errcode_t
lower_subexps (void *extra, bin_tree_t *node)
{
  regex_t *preg = (regex_t *) extra;
  reg_errcode_t err = REG_NOERROR;

  if (node->left && node->left->token.type == SUBEXP)
    {
      node->left = lower_subexp (&err, preg, node->left);
      if (node->left)
	node->left->parent = node;
    }
  if (node->right && node->right->token.type == SUBEXP)
    {
      node->right = lower_subexp (&err, preg, node->right);
      if (node->right)
	node->right->parent = node;
    }

  return err;
}

static bin_tree_t *
lower_subexp (reg_errcode_t *err, regex_t *preg, bin_tree_t *node)
{
  re_dfa_t *dfa = (re_dfa_t *) preg->buffer;
  bin_tree_t *body = node->left;
  bin_tree_t *op, *cls, *tree1, *tree;

  if (preg->no_sub
      /* We do not optimize empty subexpressions, because otherwise we may
	 have bad CONCAT nodes with NULL children.  This is obviously not
	 very common, so we do not lose much.  An example that triggers
	 this case is the sed "script" /\(\)/x.  */
      && node->left != NULL
      && (node->token.opr.idx >= BITSET_WORD_BITS
	  || !(dfa->used_bkref_map
	       & ((bitset_word_t) 1 << node->token.opr.idx))))
    return node->left;

  /* Convert the SUBEXP node to the concatenation of an
     OP_OPEN_SUBEXP, the contents, and an OP_CLOSE_SUBEXP.  */
  op = create_tree (dfa, NULL, NULL, OP_OPEN_SUBEXP);
  cls = create_tree (dfa, NULL, NULL, OP_CLOSE_SUBEXP);
  tree1 = body ? create_tree (dfa, body, cls, CONCAT) : cls;
  tree = create_tree (dfa, op, tree1, CONCAT);
  if (BE (tree == NULL || tree1 == NULL || op == NULL || cls == NULL, 0))
    {
      *err = REG_ESPACE;
      return NULL;
    }

  op->token.opr.idx = cls->token.opr.idx = node->token.opr.idx;
  op->token.opt_subexp = cls->token.opt_subexp = node->token.opt_subexp;
  return tree;
}

/* Pass 1 in building the NFA: compute FIRST and create unlinked automaton
   nodes.  Requires a postorder visit.  */
static reg_errcode_t
calc_first (void *extra, bin_tree_t *node)
{
  re_dfa_t *dfa = (re_dfa_t *) extra;
  if (node->token.type == CONCAT)
    {
      node->first = node->left->first;
      node->node_idx = node->left->node_idx;
    }
  else
    {
      node->first = node;
      node->node_idx = re_dfa_add_node (dfa, node->token);
      if (BE (node->node_idx == REG_MISSING, 0))
        return REG_ESPACE;
    }
  return REG_NOERROR;
}

/* Pass 2: compute NEXT on the tree.  Preorder visit.  */
static reg_errcode_t
calc_next (void *extra, bin_tree_t *node)
{
  switch (node->token.type)
    {
    case OP_DUP_ASTERISK:
      node->left->next = node;
      break;
    case CONCAT:
      node->left->next = node->right->first;
      node->right->next = node->next;
      break;
    default:
      if (node->left)
	node->left->next = node->next;
      if (node->right)
        node->right->next = node->next;
      break;
    }
  return REG_NOERROR;
}

/* Pass 3: link all DFA nodes to their NEXT node (any order will do).  */
static reg_errcode_t
link_nfa_nodes (void *extra, bin_tree_t *node)
{
  re_dfa_t *dfa = (re_dfa_t *) extra;
  Idx idx = node->node_idx;
  reg_errcode_t err = REG_NOERROR;

  switch (node->token.type)
    {
    case CONCAT:
      break;

    case END_OF_RE:
      assert (node->next == NULL);
      break;

    case OP_DUP_ASTERISK:
    case OP_ALT:
      {
	Idx left, right;
	dfa->has_plural_match = 1;
	if (node->left != NULL)
	  left = node->left->first->node_idx;
	else
	  left = node->next->node_idx;
	if (node->right != NULL)
	  right = node->right->first->node_idx;
	else
	  right = node->next->node_idx;
	assert (REG_VALID_INDEX (left));
	assert (REG_VALID_INDEX (right));
	err = re_node_set_init_2 (dfa->edests + idx, left, right);
      }
      break;

    case ANCHOR:
    case OP_OPEN_SUBEXP:
    case OP_CLOSE_SUBEXP:
      err = re_node_set_init_1 (dfa->edests + idx, node->next->node_idx);
      break;

    case OP_BACK_REF:
      dfa->nexts[idx] = node->next->node_idx;
      if (node->token.type == OP_BACK_REF)
	re_node_set_init_1 (dfa->edests + idx, dfa->nexts[idx]);
      break;

    default:
      assert (!IS_EPSILON_NODE (node->token.type));
      dfa->nexts[idx] = node->next->node_idx;
      break;
    }

  return err;
}

/* Duplicate the epsilon closure of the node ROOT_NODE.
   Note that duplicated nodes have constraint INIT_CONSTRAINT in addition
   to their own constraint.  */

static reg_errcode_t
internal_function
duplicate_node_closure (re_dfa_t *dfa, Idx top_org_node, Idx top_clone_node,
			Idx root_node, unsigned int init_constraint)
{
  Idx org_node, clone_node;
  bool ok;
  unsigned int constraint = init_constraint;
  for (org_node = top_org_node, clone_node = top_clone_node;;)
    {
      Idx org_dest, clone_dest;
      if (dfa->nodes[org_node].type == OP_BACK_REF)
	{
	  /* If the back reference epsilon-transit, its destination must
	     also have the constraint.  Then duplicate the epsilon closure
	     of the destination of the back reference, and store it in
	     edests of the back reference.  */
	  org_dest = dfa->nexts[org_node];
	  re_node_set_empty (dfa->edests + clone_node);
	  clone_dest = duplicate_node (dfa, org_dest, constraint);
	  if (BE (clone_dest == REG_MISSING, 0))
	    return REG_ESPACE;
	  dfa->nexts[clone_node] = dfa->nexts[org_node];
	  ok = re_node_set_insert (dfa->edests + clone_node, clone_dest);
	  if (BE (! ok, 0))
	    return REG_ESPACE;
	}
      else if (dfa->edests[org_node].nelem == 0)
	{
	  /* In case of the node can't epsilon-transit, don't duplicate the
	     destination and store the original destination as the
	     destination of the node.  */
	  dfa->nexts[clone_node] = dfa->nexts[org_node];
	  break;
	}
      else if (dfa->edests[org_node].nelem == 1)
	{
	  /* In case of the node can epsilon-transit, and it has only one
	     destination.  */
	  org_dest = dfa->edests[org_node].elems[0];
	  re_node_set_empty (dfa->edests + clone_node);
	  if (dfa->nodes[org_node].type == ANCHOR)
	    {
	      /* In case of the node has another constraint, append it.  */
	      if (org_node == root_node && clone_node != org_node)
		{
		  /* ...but if the node is root_node itself, it means the
		     epsilon closure have a loop, then tie it to the
		     destination of the root_node.  */
		  ok = re_node_set_insert (dfa->edests + clone_node, org_dest);
		  if (BE (! ok, 0))
		    return REG_ESPACE;
		  break;
		}
	      constraint |= dfa->nodes[org_node].opr.ctx_type;
	    }
	  clone_dest = duplicate_node (dfa, org_dest, constraint);
	  if (BE (clone_dest == REG_MISSING, 0))
	    return REG_ESPACE;
	  ok = re_node_set_insert (dfa->edests + clone_node, clone_dest);
	  if (BE (! ok, 0))
	    return REG_ESPACE;
	}
      else /* dfa->edests[org_node].nelem == 2 */
	{
	  /* In case of the node can epsilon-transit, and it has two
	     destinations. In the bin_tree_t and DFA, that's '|' and '*'.   */
	  org_dest = dfa->edests[org_node].elems[0];
	  re_node_set_empty (dfa->edests + clone_node);
	  /* Search for a duplicated node which satisfies the constraint.  */
	  clone_dest = search_duplicated_node (dfa, org_dest, constraint);
	  if (clone_dest == REG_MISSING)
	    {
	      /* There are no such a duplicated node, create a new one.  */
	      reg_errcode_t err;
	      clone_dest = duplicate_node (dfa, org_dest, constraint);
	      if (BE (clone_dest == REG_MISSING, 0))
		return REG_ESPACE;
	      ok = re_node_set_insert (dfa->edests + clone_node, clone_dest);
	      if (BE (! ok, 0))
		return REG_ESPACE;
	      err = duplicate_node_closure (dfa, org_dest, clone_dest,
					    root_node, constraint);
	      if (BE (err != REG_NOERROR, 0))
		return err;
	    }
	  else
	    {
	      /* There are a duplicated node which satisfy the constraint,
		 use it to avoid infinite loop.  */
	      ok = re_node_set_insert (dfa->edests + clone_node, clone_dest);
	      if (BE (! ok, 0))
		return REG_ESPACE;
	    }

	  org_dest = dfa->edests[org_node].elems[1];
	  clone_dest = duplicate_node (dfa, org_dest, constraint);
	  if (BE (clone_dest == REG_MISSING, 0))
	    return REG_ESPACE;
	  ok = re_node_set_insert (dfa->edests + clone_node, clone_dest);
	  if (BE (! ok, 0))
	    return REG_ESPACE;
	}
      org_node = org_dest;
      clone_node = clone_dest;
    }
  return REG_NOERROR;
}

/* Search for a node which is duplicated from the node ORG_NODE, and
   satisfies the constraint CONSTRAINT.  */

static Idx
search_duplicated_node (const re_dfa_t *dfa, Idx org_node,
			unsigned int constraint)
{
  Idx idx;
  for (idx = dfa->nodes_len - 1; dfa->nodes[idx].duplicated && idx > 0; --idx)
    {
      if (org_node == dfa->org_indices[idx]
	  && constraint == dfa->nodes[idx].constraint)
	return idx; /* Found.  */
    }
  return REG_MISSING; /* Not found.  */
}

/* Duplicate the node whose index is ORG_IDX and set the constraint CONSTRAINT.
   Return the index of the new node, or REG_MISSING if insufficient storage is
   available.  */

static Idx
duplicate_node (re_dfa_t *dfa, Idx org_idx, unsigned int constraint)
{
  Idx dup_idx = re_dfa_add_node (dfa, dfa->nodes[org_idx]);
  if (BE (dup_idx != REG_MISSING, 1))
    {
      dfa->nodes[dup_idx].constraint = constraint;
      if (dfa->nodes[org_idx].type == ANCHOR)
	dfa->nodes[dup_idx].constraint |= dfa->nodes[org_idx].opr.ctx_type;
      dfa->nodes[dup_idx].duplicated = 1;

      /* Store the index of the original node.  */
      dfa->org_indices[dup_idx] = org_idx;
    }
  return dup_idx;
}

static reg_errcode_t
calc_inveclosure (re_dfa_t *dfa)
{
  Idx src, idx;
  bool ok;
  for (idx = 0; idx < dfa->nodes_len; ++idx)
    re_node_set_init_empty (dfa->inveclosures + idx);

  for (src = 0; src < dfa->nodes_len; ++src)
    {
      Idx *elems = dfa->eclosures[src].elems;
      for (idx = 0; idx < dfa->eclosures[src].nelem; ++idx)
	{
	  ok = re_node_set_insert_last (dfa->inveclosures + elems[idx], src);
	  if (BE (! ok, 0))
	    return REG_ESPACE;
	}
    }

  return REG_NOERROR;
}

/* Calculate "eclosure" for all the node in DFA.  */

static reg_errcode_t
calc_eclosure (re_dfa_t *dfa)
{
  Idx node_idx;
  bool incomplete;
#ifdef DEBUG
  assert (dfa->nodes_len > 0);
#endif
  incomplete = false;
  /* For each nodes, calculate epsilon closure.  */
  for (node_idx = 0; ; ++node_idx)
    {
      reg_errcode_t err;
      re_node_set eclosure_elem;
      if (node_idx == dfa->nodes_len)
	{
	  if (!incomplete)
	    break;
	  incomplete = false;
	  node_idx = 0;
	}

#ifdef DEBUG
      assert (dfa->eclosures[node_idx].nelem != REG_MISSING);
#endif

      /* If we have already calculated, skip it.  */
      if (dfa->eclosures[node_idx].nelem != 0)
	continue;
      /* Calculate epsilon closure of `node_idx'.  */
      err = calc_eclosure_iter (&eclosure_elem, dfa, node_idx, true);
      if (BE (err != REG_NOERROR, 0))
	return err;

      if (dfa->eclosures[node_idx].nelem == 0)
	{
	  incomplete = true;
	  re_node_set_free (&eclosure_elem);
	}
    }
  return REG_NOERROR;
}

/* Calculate epsilon closure of NODE.  */

static reg_errcode_t
calc_eclosure_iter (re_node_set *new_set, re_dfa_t *dfa, Idx node, bool root)
{
  reg_errcode_t err;
  unsigned int constraint;
  Idx i;
  bool incomplete;
  bool ok;
  re_node_set eclosure;
  incomplete = false;
  err = re_node_set_alloc (&eclosure, dfa->edests[node].nelem + 1);
  if (BE (err != REG_NOERROR, 0))
    return err;

  /* This indicates that we are calculating this node now.
     We reference this value to avoid infinite loop.  */
  dfa->eclosures[node].nelem = REG_MISSING;

  constraint = ((dfa->nodes[node].type == ANCHOR)
		? dfa->nodes[node].opr.ctx_type : 0);
  /* If the current node has constraints, duplicate all nodes.
     Since they must inherit the constraints.  */
  if (constraint
      && dfa->edests[node].nelem
      && !dfa->nodes[dfa->edests[node].elems[0]].duplicated)
    {
      err = duplicate_node_closure (dfa, node, node, node, constraint);
      if (BE (err != REG_NOERROR, 0))
	return err;
    }

  /* Expand each epsilon destination nodes.  */
  if (IS_EPSILON_NODE(dfa->nodes[node].type))
    for (i = 0; i < dfa->edests[node].nelem; ++i)
      {
	re_node_set eclosure_elem;
	Idx edest = dfa->edests[node].elems[i];
	/* If calculating the epsilon closure of `edest' is in progress,
	   return intermediate result.  */
	if (dfa->eclosures[edest].nelem == REG_MISSING)
	  {
	    incomplete = true;
	    continue;
	  }
	/* If we haven't calculated the epsilon closure of `edest' yet,
	   calculate now. Otherwise use calculated epsilon closure.  */
	if (dfa->eclosures[edest].nelem == 0)
	  {
	    err = calc_eclosure_iter (&eclosure_elem, dfa, edest, false);
	    if (BE (err != REG_NOERROR, 0))
	      return err;
	  }
	else
	  eclosure_elem = dfa->eclosures[edest];
	/* Merge the epsilon closure of `edest'.  */
	re_node_set_merge (&eclosure, &eclosure_elem);
	/* If the epsilon closure of `edest' is incomplete,
	   the epsilon closure of this node is also incomplete.  */
	if (dfa->eclosures[edest].nelem == 0)
	  {
	    incomplete = true;
	    re_node_set_free (&eclosure_elem);
	  }
      }

  /* Epsilon closures include itself.  */
  ok = re_node_set_insert (&eclosure, node);
  if (BE (! ok, 0))
    return REG_ESPACE;
  if (incomplete && !root)
    dfa->eclosures[node].nelem = 0;
  else
    dfa->eclosures[node] = eclosure;
  *new_set = eclosure;
  return REG_NOERROR;
}

/* Functions for token which are used in the parser.  */

/* Fetch a token from INPUT.
   We must not use this function inside bracket expressions.  */

static void
internal_function
fetch_token (re_token_t *result, re_string_t *input, reg_syntax_t syntax)
{
  re_string_skip_bytes (input, peek_token (result, input, syntax));
}

/* Peek a token from INPUT, and return the length of the token.
   We must not use this function inside bracket expressions.  */

static int
internal_function
peek_token (re_token_t *token, re_string_t *input, reg_syntax_t syntax)
{
  unsigned char c;

  if (re_string_eoi (input))
    {
      token->type = END_OF_RE;
      return 0;
    }

  c = re_string_peek_byte (input, 0);
  token->opr.c = c;

  token->word_char = 0;
#ifdef RE_ENABLE_I18N
  token->mb_partial = 0;
  if (input->mb_cur_max > 1 &&
      !re_string_first_byte (input, re_string_cur_idx (input)))
    {
      token->type = CHARACTER;
      token->mb_partial = 1;
      return 1;
    }
#endif
  if (c == '\\')
    {
      unsigned char c2;
      if (re_string_cur_idx (input) + 1 >= re_string_length (input))
	{
	  token->type = BACK_SLASH;
	  return 1;
	}

      c2 = re_string_peek_byte_case (input, 1);
      token->opr.c = c2;
      token->type = CHARACTER;
#ifdef RE_ENABLE_I18N
      if (input->mb_cur_max > 1)
	{
	  wint_t wc = re_string_wchar_at (input,
					  re_string_cur_idx (input) + 1);
	  token->word_char = IS_WIDE_WORD_CHAR (wc) != 0;
	}
      else
#endif
	token->word_char = IS_WORD_CHAR (c2) != 0;

      switch (c2)
	{
	case '|':
	  if (!(syntax & RE_LIMITED_OPS) && !(syntax & RE_NO_BK_VBAR))
	    token->type = OP_ALT;
	  break;
	case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '8': case '9':
	  if (!(syntax & RE_NO_BK_REFS))
	    {
	      token->type = OP_BACK_REF;
	      token->opr.idx = c2 - '1';
	    }
	  break;
	case '<':
	  if (!(syntax & RE_NO_GNU_OPS))
	    {
	      token->type = ANCHOR;
	      token->opr.ctx_type = WORD_FIRST;
	    }
	  break;
	case '>':
	  if (!(syntax & RE_NO_GNU_OPS))
	    {
	      token->type = ANCHOR;
	      token->opr.ctx_type = WORD_LAST;
	    }
	  break;
	case 'b':
	  if (!(syntax & RE_NO_GNU_OPS))
	    {
	      token->type = ANCHOR;
	      token->opr.ctx_type = WORD_DELIM;
	    }
	  break;
	case 'B':
	  if (!(syntax & RE_NO_GNU_OPS))
	    {
	      token->type = ANCHOR;
	      token->opr.ctx_type = NOT_WORD_DELIM;
	    }
	  break;
	case 'w':
	  if (!(syntax & RE_NO_GNU_OPS))
	    token->type = OP_WORD;
	  break;
	case 'W':
	  if (!(syntax & RE_NO_GNU_OPS))
	    token->type = OP_NOTWORD;
	  break;
	case 's':
	  if (!(syntax & RE_NO_GNU_OPS))
	    token->type = OP_SPACE;
	  break;
	case 'S':
	  if (!(syntax & RE_NO_GNU_OPS))
	    token->type = OP_NOTSPACE;
	  break;
	case '`':
	  if (!(syntax & RE_NO_GNU_OPS))
	    {
	      token->type = ANCHOR;
	      token->opr.ctx_type = BUF_FIRST;
	    }
	  break;
	case '\'':
	  if (!(syntax & RE_NO_GNU_OPS))
	    {
	      token->type = ANCHOR;
	      token->opr.ctx_type = BUF_LAST;
	    }
	  break;
	case '(':
	  if (!(syntax & RE_NO_BK_PARENS))
	    token->type = OP_OPEN_SUBEXP;
	  break;
	case ')':
	  if (!(syntax & RE_NO_BK_PARENS))
	    token->type = OP_CLOSE_SUBEXP;
	  break;
	case '+':
	  if (!(syntax & RE_LIMITED_OPS) && (syntax & RE_BK_PLUS_QM))
	    token->type = OP_DUP_PLUS;
	  break;
	case '?':
	  if (!(syntax & RE_LIMITED_OPS) && (syntax & RE_BK_PLUS_QM))
	    token->type = OP_DUP_QUESTION;
	  break;
	case '{':
	  if ((syntax & RE_INTERVALS) && (!(syntax & RE_NO_BK_BRACES)))
	    token->type = OP_OPEN_DUP_NUM;
	  break;
	case '}':
	  if ((syntax & RE_INTERVALS) && (!(syntax & RE_NO_BK_BRACES)))
	    token->type = OP_CLOSE_DUP_NUM;
	  break;
	default:
	  break;
	}
      return 2;
    }

  token->type = CHARACTER;
#ifdef RE_ENABLE_I18N
  if (input->mb_cur_max > 1)
    {
      wint_t wc = re_string_wchar_at (input, re_string_cur_idx (input));
      token->word_char = IS_WIDE_WORD_CHAR (wc) != 0;
    }
  else
#endif
    token->word_char = IS_WORD_CHAR (token->opr.c);

  switch (c)
    {
    case '\n':
      if (syntax & RE_NEWLINE_ALT)
	token->type = OP_ALT;
      break;
    case '|':
      if (!(syntax & RE_LIMITED_OPS) && (syntax & RE_NO_BK_VBAR))
	token->type = OP_ALT;
      break;
    case '*':
      token->type = OP_DUP_ASTERISK;
      break;
    case '+':
      if (!(syntax & RE_LIMITED_OPS) && !(syntax & RE_BK_PLUS_QM))
	token->type = OP_DUP_PLUS;
      break;
    case '?':
      if (!(syntax & RE_LIMITED_OPS) && !(syntax & RE_BK_PLUS_QM))
	token->type = OP_DUP_QUESTION;
      break;
    case '{':
      if ((syntax & RE_INTERVALS) && (syntax & RE_NO_BK_BRACES))
	token->type = OP_OPEN_DUP_NUM;
      break;
    case '}':
      if ((syntax & RE_INTERVALS) && (syntax & RE_NO_BK_BRACES))
	token->type = OP_CLOSE_DUP_NUM;
      break;
    case '(':
      if (syntax & RE_NO_BK_PARENS)
	token->type = OP_OPEN_SUBEXP;
      break;
    case ')':
      if (syntax & RE_NO_BK_PARENS)
	token->type = OP_CLOSE_SUBEXP;
      break;
    case '[':
      token->type = OP_OPEN_BRACKET;
      break;
    case '.':
      token->type = OP_PERIOD;
      break;
    case '^':
      if (!(syntax & (RE_CONTEXT_INDEP_ANCHORS | RE_CARET_ANCHORS_HERE)) &&
	  re_string_cur_idx (input) != 0)
	{
	  char prev = re_string_peek_byte (input, -1);
	  if (!(syntax & RE_NEWLINE_ALT) || prev != '\n')
	    break;
	}
      token->type = ANCHOR;
      token->opr.ctx_type = LINE_FIRST;
      break;
    case '$':
      if (!(syntax & RE_CONTEXT_INDEP_ANCHORS) &&
	  re_string_cur_idx (input) + 1 != re_string_length (input))
	{
	  re_token_t next;
	  re_string_skip_bytes (input, 1);
	  peek_token (&next, input, syntax);
	  re_string_skip_bytes (input, -1);
	  if (next.type != OP_ALT && next.type != OP_CLOSE_SUBEXP)
	    break;
	}
      token->type = ANCHOR;
      token->opr.ctx_type = LINE_LAST;
      break;
    default:
      break;
    }
  return 1;
}

/* Peek a token from INPUT, and return the length of the token.
   We must not use this function out of bracket expressions.  */

static int
internal_function
peek_token_bracket (re_token_t *token, re_string_t *input, reg_syntax_t syntax)
{
  unsigned char c;
  if (re_string_eoi (input))
    {
      token->type = END_OF_RE;
      return 0;
    }
  c = re_string_peek_byte (input, 0);
  token->opr.c = c;

#ifdef RE_ENABLE_I18N
  if (input->mb_cur_max > 1 &&
      !re_string_first_byte (input, re_string_cur_idx (input)))
    {
      token->type = CHARACTER;
      return 1;
    }
#endif /* RE_ENABLE_I18N */

  if (c == '\\' && (syntax & RE_BACKSLASH_ESCAPE_IN_LISTS)
      && re_string_cur_idx (input) + 1 < re_string_length (input))
    {
      /* In this case, '\' escape a character.  */
      unsigned char c2;
      re_string_skip_bytes (input, 1);
      c2 = re_string_peek_byte (input, 0);
      token->opr.c = c2;
      token->type = CHARACTER;
      return 1;
    }
  if (c == '[') /* '[' is a special char in a bracket exps.  */
    {
      unsigned char c2;
      int token_len;
      if (re_string_cur_idx (input) + 1 < re_string_length (input))
	c2 = re_string_peek_byte (input, 1);
      else
	c2 = 0;
      token->opr.c = c2;
      token_len = 2;
      switch (c2)
	{
	case '.':
	  token->type = OP_OPEN_COLL_ELEM;
	  break;
	case '=':
	  token->type = OP_OPEN_EQUIV_CLASS;
	  break;
	case ':':
	  if (syntax & RE_CHAR_CLASSES)
	    {
	      token->type = OP_OPEN_CHAR_CLASS;
	      break;
	    }
	  /* else fall through.  */
	default:
	  token->type = CHARACTER;
	  token->opr.c = c;
	  token_len = 1;
	  break;
	}
      return token_len;
    }
  switch (c)
    {
    case '-':
      token->type = OP_CHARSET_RANGE;
      break;
    case ']':
      token->type = OP_CLOSE_BRACKET;
      break;
    case '^':
      token->type = OP_NON_MATCH_LIST;
      break;
    default:
      token->type = CHARACTER;
    }
  return 1;
}

/* Functions for parser.  */

/* Entry point of the parser.
   Parse the regular expression REGEXP and return the structure tree.
   If an error is occured, ERR is set by error code, and return NULL.
   This function build the following tree, from regular expression <reg_exp>:
	   CAT
	   / \
	  /   \
   <reg_exp>  EOR

   CAT means concatenation.
   EOR means end of regular expression.  */

static bin_tree_t *
parse (re_string_t *regexp, regex_t *preg, reg_syntax_t syntax,
       reg_errcode_t *err)
{
  re_dfa_t *dfa = (re_dfa_t *) preg->buffer;
  bin_tree_t *tree, *eor, *root;
  re_token_t current_token;
  dfa->syntax = syntax;
  fetch_token (&current_token, regexp, syntax | RE_CARET_ANCHORS_HERE);
  tree = parse_reg_exp (regexp, preg, &current_token, syntax, 0, err);
  if (BE (*err != REG_NOERROR && tree == NULL, 0))
    return NULL;
  eor = create_tree (dfa, NULL, NULL, END_OF_RE);
  if (tree != NULL)
    root = create_tree (dfa, tree, eor, CONCAT);
  else
    root = eor;
  if (BE (eor == NULL || root == NULL, 0))
    {
      *err = REG_ESPACE;
      return NULL;
    }
  return root;
}

/* This function build the following tree, from regular expression
   <branch1>|<branch2>:
	   ALT
	   / \
	  /   \
   <branch1> <branch2>

   ALT means alternative, which represents the operator `|'.  */

static bin_tree_t *
parse_reg_exp (re_string_t *regexp, regex_t *preg, re_token_t *token,
	       reg_syntax_t syntax, Idx nest, reg_errcode_t *err)
{
  re_dfa_t *dfa = (re_dfa_t *) preg->buffer;
  bin_tree_t *tree, *branch = NULL;
  tree = parse_branch (regexp, preg, token, syntax, nest, err);
  if (BE (*err != REG_NOERROR && tree == NULL, 0))
    return NULL;

  while (token->type == OP_ALT)
    {
      fetch_token (token, regexp, syntax | RE_CARET_ANCHORS_HERE);
      if (token->type != OP_ALT && token->type != END_OF_RE
	  && (nest == 0 || token->type != OP_CLOSE_SUBEXP))
	{
	  branch = parse_branch (regexp, preg, token, syntax, nest, err);
	  if (BE (*err != REG_NOERROR && branch == NULL, 0))
	    return NULL;
	}
      else
	branch = NULL;
      tree = create_tree (dfa, tree, branch, OP_ALT);
      if (BE (tree == NULL, 0))
	{
	  *err = REG_ESPACE;
	  return NULL;
	}
    }
  return tree;
}

/* This function build the following tree, from regular expression
   <exp1><exp2>:
	CAT
	/ \
       /   \
   <exp1> <exp2>

   CAT means concatenation.  */

static bin_tree_t *
parse_branch (re_string_t *regexp, regex_t *preg, re_token_t *token,
	      reg_syntax_t syntax, Idx nest, reg_errcode_t *err)
{
  bin_tree_t *tree, *expr;
  re_dfa_t *dfa = (re_dfa_t *) preg->buffer;
  tree = parse_expression (regexp, preg, token, syntax, nest, err);
  if (BE (*err != REG_NOERROR && tree == NULL, 0))
    return NULL;

  while (token->type != OP_ALT && token->type != END_OF_RE
	 && (nest == 0 || token->type != OP_CLOSE_SUBEXP))
    {
      expr = parse_expression (regexp, preg, token, syntax, nest, err);
      if (BE (*err != REG_NOERROR && expr == NULL, 0))
	{
	  return NULL;
	}
      if (tree != NULL && expr != NULL)
	{
	  tree = create_tree (dfa, tree, expr, CONCAT);
	  if (tree == NULL)
	    {
	      *err = REG_ESPACE;
	      return NULL;
	    }
	}
      else if (
