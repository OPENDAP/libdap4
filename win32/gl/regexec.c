/* Extended regular expression matching and search library.
   Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007 Free Software Foundation,
   Inc.
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

static reg_errcode_t match_ctx_init (re_match_context_t *cache, int eflags,
				     Idx n) internal_function;
static void match_ctx_clean (re_match_context_t *mctx) internal_function;
static void match_ctx_free (re_match_context_t *cache) internal_function;
static reg_errcode_t match_ctx_add_entry (re_match_context_t *cache, Idx node,
					  Idx str_idx, Idx from, Idx to)
     internal_function;
static Idx search_cur_bkref_entry (const re_match_context_t *mctx, Idx str_idx)
     internal_function;
static reg_errcode_t match_ctx_add_subtop (re_match_context_t *mctx, Idx node,
					   Idx str_idx) internal_function;
static re_sub_match_last_t * match_ctx_add_sublast (re_sub_match_top_t *subtop,
						    Idx node, Idx str_idx)
     internal_function;
static void sift_ctx_init (re_sift_context_t *sctx, re_dfastate_t **sifted_sts,
			   re_dfastate_t **limited_sts, Idx last_node,
			   Idx last_str_idx)
     internal_function;
static reg_errcode_t re_search_internal (const regex_t *preg,
					 const char *string, Idx length,
					 Idx start, Idx last_start, Idx stop,
					 size_t nmatch, regmatch_t pmatch[],
					 int eflags) internal_function;
static regoff_t re_search_2_stub (struct re_pattern_buffer *bufp,
				  const char *string1, Idx length1,
				  const char *string2, Idx length2,
				  Idx start, regoff_t range,
				  struct re_registers *regs,
				  Idx stop, bool ret_len) internal_function;
static regoff_t re_search_stub (struct re_pattern_buffer *bufp,
				const char *string, Idx length, Idx start,
				regoff_t range, Idx stop,
				struct re_registers *regs,
				bool ret_len) internal_function;
static unsigned int re_copy_regs (struct re_registers *regs, regmatch_t *pmatch,
				  Idx nregs, int regs_allocated)
     internal_function;
static reg_errcode_t prune_impossible_nodes (re_match_context_t *mctx)
     internal_function;
static Idx check_matching (re_match_context_t *mctx, bool fl_longest_match,
			   Idx *p_match_first) internal_function;
static Idx check_halt_state_context (const re_match_context_t *mctx,
				     const re_dfastate_t *state, Idx idx)
     internal_function;
static void update_regs (const re_dfa_t *dfa, regmatch_t *pmatch,
			 regmatch_t *prev_idx_match, Idx cur_node,
			 Idx cur_idx, Idx nmatch) internal_function;
static reg_errcode_t push_fail_stack (struct re_fail_stack_t *fs,
				      Idx str_idx, Idx dest_node, Idx nregs,
				      regmatch_t *regs,
				      re_node_set *eps_via_nodes)
     internal_function;
static reg_errcode_t set_regs (const regex_t *preg,
			       const re_match_context_t *mctx,
			       size_t nmatch, regmatch_t *pmatch,
			       bool fl_backtrack) internal_function;
static reg_errcode_t free_fail_stack_return (struct re_fail_stack_t *fs)
     internal_function;

#ifdef RE_ENABLE_I18N
static int sift_states_iter_mb (const re_match_context_t *mctx,
				re_sift_context_t *sctx,
				Idx node_idx, Idx str_idx, Idx max_str_idx)
     internal_function;
#endif /* RE_ENABLE_I18N */
static reg_errcode_t sift_states_backward (const re_match_context_t *mctx,
					   re_sift_context_t *sctx)
     internal_function;
static reg_errcode_t build_sifted_states (const re_match_context_t *mctx,
					  re_sift_context_t *sctx, Idx str_idx,
					  re_node_set *cur_dest)
     internal_function;
static reg_errcode_t update_cur_sifted_state (const re_match_context_t *mctx,
					      re_sift_context_t *sctx,
					      Idx str_idx,
					      re_node_set *dest_nodes)
     internal_function;
static reg_errcode_t add_epsilon_src_nodes (const re_dfa_t *dfa,
					    re_node_set *dest_nodes,
					    const re_node_set *candidates)
     internal_function;
static bool check_dst_limits (const re_match_context_t *mctx,
			      const re_node_set *limits,
			      Idx dst_node, Idx dst_idx, Idx src_node,
			      Idx src_idx) internal_function;
static int check_dst_limits_calc_pos_1 (const re_match_context_t *mctx,
					int boundaries, Idx subexp_idx,
					Idx from_node, Idx bkref_idx)
     internal_function;
static int check_dst_limits_calc_pos (const re_match_context_t *mctx,
				      Idx limit, Idx subexp_idx,
				      Idx node, Idx str_idx,
				      Idx bkref_idx) internal_function;
static reg_errcode_t check_subexp_limits (const re_dfa_t *dfa,
					  re_node_set *dest_nodes,
					  const re_node_set *candidates,
					  re_node_set *limits,
					  struct re_backref_cache_entry *bkref_ents,
					  Idx str_idx) internal_function;
static reg_errcode_t sift_states_bkref (const re_match_context_t *mctx,
					re_sift_context_t *sctx,
					Idx str_idx, const re_node_set *candidates)
     internal_function;
static reg_errcode_t merge_state_array (const re_dfa_t *dfa,
					re_dfastate_t **dst,
					re_dfastate_t **src, Idx num)
     internal_function;
static re_dfastate_t *find_recover_state (reg_errcode_t *err,
					 re_match_context_t *mctx) internal_function;
static re_dfastate_t *transit_state (reg_errcode_t *err,
				     re_match_context_t *mctx,
				     re_dfastate_t *state) internal_function;
static re_dfastate_t *merge_state_with_log (reg_errcode_t *err,
					    re_match_context_t *mctx,
					    re_dfastate_t *next_state)
     internal_function;
static reg_errcode_t check_subexp_matching_top (re_match_context_t *mctx,
						re_node_set *cur_nodes,
						Idx str_idx) internal_function;
#if 0
static re_dfastate_t *transit_state_sb (reg_errcode_t *err,
					re_match_context_t *mctx,
					re_dfastate_t *pstate)
     internal_function;
#endif
#ifdef RE_ENABLE_I18N
static reg_errcode_t transit_state_mb (re_match_context_t *mctx,
				       re_dfastate_t *pstate)
     internal_function;
#endif /* RE_ENABLE_I18N */
static reg_errcode_t transit_state_bkref (re_match_context_t *mctx,
					  const re_node_set *nodes)
     internal_function;
static reg_errcode_t get_subexp (re_match_context_t *mctx,
				 Idx bkref_node, Idx bkref_str_idx)
     internal_function;
static reg_errcode_t get_subexp_sub (re_match_context_t *mctx,
				     const re_sub_match_top_t *sub_top,
				     re_sub_match_last_t *sub_last,
				     Idx bkref_node, Idx bkref_str)
     internal_function;
static Idx find_subexp_node (const re_dfa_t *dfa, const re_node_set *nodes,
			     Idx subexp_idx, int type) internal_function;
static reg_errcode_t check_arrival (re_match_context_t *mctx,
				    state_array_t *path, Idx top_node,
				    Idx top_str, Idx last_node, Idx last_str,
				    int type) internal_function;
static reg_errcode_t check_arrival_add_next_nodes (re_match_context_t *mctx,
						   Idx str_idx,
						   re_node_set *cur_nodes,
						   re_node_set *next_nodes)
     internal_function;
static reg_errcode_t check_arrival_expand_ecl (const re_dfa_t *dfa,
					       re_node_set *cur_nodes,
					       Idx ex_subexp, int type)
     internal_function;
static reg_errcode_t check_arrival_expand_ecl_sub (const re_dfa_t *dfa,
						   re_node_set *dst_nodes,
						   Idx target, Idx ex_subexp,
						   int type) internal_function;
static reg_errcode_t expand_bkref_cache (re_match_context_t *mctx,
					 re_node_set *cur_nodes, Idx cur_str,
					 Idx subexp_num, int type)
     internal_function;
static bool build_trtable (const re_dfa_t *dfa,
			   re_dfastate_t *state) internal_function;
#ifdef RE_ENABLE_I18N
static int check_node_accept_bytes (const re_dfa_t *dfa, Idx node_idx,
				    const re_string_t *input, Idx idx)
     internal_function;
# ifdef _LIBC
static unsigned int find_collation_sequence_value (const unsigned char *mbs,
						   size_t name_len)
     internal_function;
# endif /* _LIBC */
#endif /* RE_ENABLE_I18N */
static Idx group_nodes_into_DFAstates (const re_dfa_t *dfa,
				       const re_dfastate_t *state,
				       re_node_set *states_node,
				       bitset_t *states_ch) internal_function;
static bool check_node_accept (const re_match_context_t *mctx,
			       const re_token_t *node, Idx idx)
     internal_function;
static reg_errcode_t extend_buffers (re_match_context_t *mctx)
     internal_function;

/* Entry point for POSIX code.  */

/* regexec searches for a given pattern, specified by PREG, in the
   string STRING.

   If NMATCH is zero or REG_NOSUB was set in the cflags argument to
   `regcomp', we ignore PMATCH.  Otherwise, we assume PMATCH has at
   least NMATCH elements, and we set them to the offsets of the
   corresponding matched substrings.

   EFLAGS specifies `execution flags' which affect matching: if
   REG_NOTBOL is set, then ^ does not match at the beginning of the
   string; if REG_NOTEOL is set, then $ does not match at the end.

   We return 0 if we find a match and REG_NOMATCH if not.  */

int
regexec (preg, string, nmatch, pmatch, eflags)
    const regex_t *_Restrict_ preg;
    const char *_Restrict_ string;
    size_t nmatch;
    regmatch_t pmatch[_Restrict_arr_];
    int eflags;
{
  reg_errcode_t err;
  Idx start, length;
#ifdef _LIBC
  re_dfa_t *dfa = (re_dfa_t *) preg->buffer;
#endif

  if (eflags & ~(REG_NOTBOL | REG_NOTEOL | REG_STARTEND))
    return REG_BADPAT;

  if (eflags & REG_STARTEND)
    {
      start = pmatch[0].rm_so;
      length = pmatch[0].rm_eo;
    }
  else
    {
      start = 0;
      length = strlen (string);
    }

  __libc_lock_lock (dfa->lock);
  if (preg->no_sub)
    err = re_search_internal (preg, string, length, start, length,
			      length, 0, NULL, eflags);
  else
    err = re_search_internal (preg, string, length, start, length,
			      length, nmatch, pmatch, eflags);
  __libc_lock_unlock (dfa->lock);
  return err != REG_NOERROR;
}

#ifdef _LIBC
# include <shlib-compat.h>
versioned_symbol (libc, __regexec, regexec, GLIBC_2_3_4);

# if SHLIB_COMPAT (libc, GLIBC_2_0, GLIBC_2_3_4)
__typeof__ (__regexec) __compat_regexec;

int
attribute_compat_text_section
__compat_regexec (const regex_t *_Restrict_ preg,
		  const char *_Restrict_ string, size_t nmatch,
		  regmatch_t pmatch[], int eflags)
{
  return regexec (preg, string, nmatch, pmatch,
		  eflags & (REG_NOTBOL | REG_NOTEOL));
}
compat_symbol (libc, __compat_regexec, regexec, GLIBC_2_0);
# endif
#endif

/* Entry points for GNU code.  */

/* re_match, re_search, re_match_2, re_search_2

   The former two functions operate on STRING with length LENGTH,
   while the later two operate on concatenation of STRING1 and STRING2
   with lengths LENGTH1 and LENGTH2, respectively.

   re_match() matches the compiled pattern in BUFP against the string,
   starting at index START.

   re_search() first tries matching at index START, then it tries to match
   starting from index START + 1, and so on.  The last start position tried
   is START + RANGE.  (Thus RANGE = 0 forces re_search to operate the same
   way as re_match().)

   The parameter STOP of re_{match,search}_2 specifies that no match exceeding
   the first STOP characters of the concatenation of the strings should be
   concerned.

   If REGS is not NULL, and BUFP->no_sub is not set, the offsets of the match
   and all groups is stored in REGS.  (For the "_2" variants, the offsets are
   computed relative to the concatenation, not relative to the individual
   strings.)

   On success, re_match* functions return the length of the match, re_search*
   return the position of the start of the match.  Return value -1 means no
   match was found and -2 indicates an internal error.  */

regoff_t
re_match (bufp, string, length, start, regs)
    struct re_pattern_buffer *bufp;
    const char *string;
    Idx length, start;
    struct re_registers *regs;
{
  return re_search_stub (bufp, string, length, start, 0, length, regs, true);
}
#ifdef _LIBC
weak_alias (__re_match, re_match)
#endif

regoff_t
re_search (bufp, string, length, start, range, regs)
    struct re_pattern_buffer *bufp;
    const char *string;
    Idx length, start;
    regoff_t range;
    struct re_registers *regs;
{
  return re_search_stub (bufp, string, length, start, range, length, regs,
			 false);
}
#ifdef _LIBC
weak_alias (__re_search, re_search)
#endif

regoff_t
re_match_2 (bufp, string1, length1, string2, length2, start, regs, stop)
    struct re_pattern_buffer *bufp;
    const char *string1, *string2;
    Idx length1, length2, start, stop;
    struct re_registers *regs;
{
  return re_search_2_stub (bufp, string1, length1, string2, length2,
			   start, 0, regs, stop, true);
}
#ifdef _LIBC
weak_alias (__re_match_2, re_match_2)
#endif

regoff_t
re_search_2 (bufp, string1, length1, string2, length2, start, range, regs, stop)
    struct re_pattern_buffer *bufp;
    const char *string1, *string2;
    Idx length1, length2, start, stop;
    regoff_t range;
    struct re_registers *regs;
{
  return re_search_2_stub (bufp, string1, length1, string2, length2,
			   start, range, regs, stop, false);
}
#ifdef _LIBC
weak_alias (__re_search_2, re_search_2)
#endif

static regoff_t
internal_function
re_search_2_stub (struct re_pattern_buffer *bufp,
		  const char *string1, Idx length1,
		  const char *string2, Idx length2,
		  Idx start, regoff_t range, struct re_registers *regs,
		  Idx stop, bool ret_len)
{
  const char *str;
  regoff_t rval;
  Idx len = length1 + length2;
  char *s = NULL;

  if (BE (length1 < 0 || length2 < 0 || stop < 0 || len < length1, 0))
    return -2;

  /* Concatenate the strings.  */
  if (length2 > 0)
    if (length1 > 0)
      {
	s = re_malloc (char, len);

	if (BE (s == NULL, 0))
	  return -2;
#ifdef _LIBC
	memcpy (__mempcpy (s, string1, length1), string2, length2);
#else
	memcpy (s, string1, length1);
	memcpy (s + length1, string2, length2);
#endif
	str = s;
      }
    else
      str = string2;
  else
    str = string1;

  rval = re_search_stub (bufp, str, len, start, range, stop, regs,
			 ret_len);
  re_free (s);
  return rval;
}

/* The parameters have the same meaning as those of re_search.
   Additional parameters:
   If RET_LEN is true the length of the match is returned (re_match style);
   otherwise the position of the match is returned.  */

static regoff_t
internal_function
re_search_stub (struct re_pattern_buffer *bufp,
		const char *string, Idx length,
		Idx start, regoff_t range, Idx stop, struct re_registers *regs,
		bool ret_len)
{
  reg_errcode_t result;
  regmatch_t *pmatch;
  Idx nregs;
  regoff_t rval;
  int eflags = 0;
#ifdef _LIBC
  re_dfa_t *dfa = (re_dfa_t *) bufp->buffer;
#endif
  Idx last_start = start + range;

  /* Check for out-of-range.  */
  if (BE (start < 0 || start > length, 0))
    return -1;
  if (BE (length < last_start || (0 <= range && last_start < start), 0))
    last_start = length;
  else if (BE (last_start < 0 || (range < 0 && start <= last_start), 0))
    last_start = 0;

  __libc_lock_lock (dfa->lock);

  eflags |= (bufp->not_bol) ? REG_NOTBOL : 0;
  eflags |= (bufp->not_eol) ? REG_NOTEOL : 0;

  /* Compile fastmap if we haven't yet.  */
  if (start < last_start && bufp->fastmap != NULL && !bufp->fastmap_accurate)
    re_compile_fastmap (bufp);

  if (BE (bufp->no_sub, 0))
    regs = NULL;

  /* We need at least 1 register.  */
  if (regs == NULL)
    nregs = 1;
  else if (BE (bufp->regs_allocated == REGS_FIXED
	       && regs->num_regs <= bufp->re_nsub, 0))
    {
      nregs = regs->num_regs;
      if (BE (nregs < 1, 0))
	{
	  /* Nothing can be copied to regs.  */
	  regs = NULL;
	  nregs = 1;
	}
    }
  else
    nregs = bufp->re_nsub + 1;
  pmatch = re_malloc (regmatch_t, nregs);
  if (BE (pmatch == NULL, 0))
    {
      rval = -2;
      goto out;
    }

  result = re_search_internal (bufp, string, length, start, last_start, stop,
			       nregs, pmatch, eflags);

  rval = 0;

  /* I hope we needn't fill ther regs with -1's when no match was found.  */
  if (result != REG_NOERROR)
    rval = -1;
  else if (regs != NULL)
    {
      /* If caller wants register contents data back, copy them.  */
      bufp->regs_allocated = re_copy_regs (regs, pmatch, nregs,
					   bufp->regs_allocated);
      if (BE (bufp->regs_allocated == REGS_UNALLOCATED, 0))
	rval = -2;
    }

  if (BE (rval == 0, 1))
    {
      if (ret_len)
	{
	  assert (pmatch[0].rm_so == start);
	  rval = pmatch[0].rm_eo - start;
	}
      else
	rval = pmatch[0].rm_so;
    }
  re_free (pmatch);
 out:
  __libc_lock_unlock (dfa->lock);
  return rval;
}

static unsigned int
internal_function
re_copy_regs (struct re_registers *regs, regmatch_t *pmatch, Idx nregs,
	      int regs_allocated)
{
  int rval = REGS_REALLOCATE;
  Idx i;
  Idx need_regs = nregs + 1;
  /* We need one extra element beyond `num_regs' for the `-1' marker GNU code
     uses.  */

  /* Have the register data arrays been allocated?  */
  if (regs_allocated == REGS_UNALLOCATED)
    { /* No.  So allocate them with malloc.  */
      regs->start = re_malloc (regoff_t, need_regs);
      if (BE (regs->start == NULL, 0))
	return REGS_UNALLOCATED;
      regs->end = re_malloc (regoff_t, need_regs);
      if (BE (regs->end == NULL, 0))
	{
	  re_free (regs->start);
	  return REGS_UNALLOCATED;
	}
      regs->num_regs = need_regs;
    }
  else if (regs_allocated == REGS_REALLOCATE)
    { /* Yes.  If we need more elements than were already
	 allocated, reallocate them.  If we need fewer, just
	 leave it alone.  */
      if (BE (need_regs > regs->num_regs, 0))
	{
	  regoff_t *new_start = re_realloc (regs->start, regoff_t, need_regs);
	  regoff_t *new_end;
	  if (BE (new_start == NULL, 0))
	    return REGS_UNALLOCATED;
	  new_end = re_realloc (regs->end, regoff_t, need_regs);
	  if (BE (new_end == NULL, 0))
	    {
	      re_free (new_start);
	      return REGS_UNALLOCATED;
	    }
	  regs->start = new_start;
	  regs->end = new_end;
	  regs->num_regs = need_regs;
	}
    }
  else
    {
      assert (regs_allocated == REGS_FIXED);
      /* This function may not be called with REGS_FIXED and nregs too big.  */
      assert (regs->num_regs >= nregs);
      rval = REGS_FIXED;
    }

  /* Copy the regs.  */
  for (i = 0; i < nregs; ++i)
    {
      regs->start[i] = pmatch[i].rm_so;
      regs->end[i] = pmatch[i].rm_eo;
    }
  for ( ; i < regs->num_regs; ++i)
    regs->start[i] = regs->end[i] = -1;

  return rval;
}

/* Set REGS to hold NUM_REGS registers, storing them in STARTS and
   ENDS.  Subsequent matches using PATTERN_BUFFER and REGS will use
   this memory for recording register information.  STARTS and ENDS
   must be allocated using the malloc library routine, and must each
   be at least NUM_REGS * sizeof (regoff_t) bytes long.

   If NUM_REGS == 0, then subsequent matches should allocate their own
   register data.

   Unless this function is called, the first search or match using
   PATTERN_BUFFER will allocate its own register data, without
   freeing the old data.  */

void
re_set_registers (bufp, regs, num_regs, starts, ends)
    struct re_pattern_buffer *bufp;
    struct re_registers *regs;
    __re_size_t num_regs;
    regoff_t *starts, *ends;
{
  if (num_regs)
    {
      bufp->regs_allocated = REGS_REALLOCATE;
      regs->num_regs = num_regs;
      regs->start = starts;
      regs->end = ends;
    }
  else
    {
      bufp->regs_allocated = REGS_UNALLOCATED;
      regs->num_regs = 0;
      regs->start = regs->end = NULL;
    }
}
#ifdef _LIBC
weak_alias (__re_set_registers, re_set_registers)
#endif

/* Entry points compatible with 4.2 BSD regex library.  We don't define
   them unless specifically requested.  */

#if defined _REGEX_RE_COMP || defined _LIBC
int
# ifdef _LIBC
weak_function
# endif
re_exec (s)
     const char *s;
{
  return 0 == regexec (&re_comp_buf, s, 0, NULL, 0);
}
#endif /* _REGEX_RE_COMP */

/* Internal entry point.  */

/* Searches for a compiled pattern PREG in the string STRING, whose
   length is LENGTH.  NMATCH, PMATCH, and EFLAGS have the same
   meaning as with regexec.  LAST_START is START + RANGE, where
   START and RANGE have the same meaning as with re_search.
   Return REG_NOERROR if we find a match, and REG_NOMATCH if not,
   otherwise return the error code.
   Note: We assume front end functions already check ranges.
   (0 <= LAST_START && LAST_START <= LENGTH)  */

static reg_errcode_t
internal_function
re_search_internal (const regex_t *preg,
		    const char *string, Idx length,
		    Idx start, Idx last_start, Idx stop,
		    size_t nmatch, regmatch_t pmatch[],
		    int eflags)
{
  reg_errcode_t err;
  const re_dfa_t *dfa = (const re_dfa_t *) preg->buffer;
  Idx left_lim, right_lim;
  int incr;
  bool fl_longest_match;
  int match_kind;
  Idx match_first;
  Idx match_last = REG_MISSING;
  Idx extra_nmatch;
  bool sb;
  int ch;
#if defined _LIBC || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L)
  re_match_context_t mctx = { .dfa = dfa };
#else
  re_match_context_t mctx;
#endif
  char *fastmap = ((preg->fastmap != NULL && preg->fastmap_accurate
		    && start != last_start && !preg->can_be_null)
		   ? preg->fastmap : NULL);
  RE_TRANSLATE_TYPE t = preg->translate;

#if !(defined _LIBC || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
  memset (&mctx, '\0', sizeof (re_match_context_t));
  mctx.dfa = dfa;
#endif

  extra_nmatch = (nmatch > preg->re_nsub) ? nmatch - (preg->re_nsub + 1) : 0;
  nmatch -= extra_nmatch;

  /* Check if the DFA haven't been compiled.  */
  if (BE (preg->used == 0 || dfa->init_state == NULL
	  || dfa->init_state_word == NULL || dfa->init_state_nl == NULL
	  || dfa->init_state_begbuf == NULL, 0))
    return REG_NOMATCH;

#ifdef DEBUG
  /* We assume front-end functions already check them.  */
  assert (0 <= last_start && last_start <= length);
#endif

  /* If initial states with non-begbuf contexts have no elements,
     the regex must be anchored.  If preg->newline_anchor is set,
     we'll never use init_state_nl, so do not check it.  */
  if (dfa->init_state->nodes.nelem == 0
      && dfa->init_state_word->nodes.nelem == 0
      && (dfa->init_state_nl->nodes.nelem == 0
	  || !preg->newline_anchor))
    {
      if (start != 0 && last_start != 0)
        return REG_NOMATCH;
      start = last_start = 0;
    }

  /* We must check the longest matching, if nmatch > 0.  */
  fl_longest_match = (nmatch != 0 || dfa->nbackref);

  err = re_string_allocate (&mctx.input, string, length, dfa->nodes_len + 1,
			    preg->translate, preg->syntax & RE_ICASE, dfa);
  if (BE (err != REG_NOERROR, 0))
    goto free_return;
  mctx.input.stop = stop;
  mctx.input.raw_stop = stop;
  mctx.input.newline_anchor = preg->newline_anchor;

  err = match_ctx_init (&mctx, eflags, dfa->nbackref * 2);
  if (BE (err != REG_NOERROR, 0))
    goto free_return;

  /* We will log all the DFA states through which the dfa pass,
     if nmatch > 1, or this dfa has "multibyte node", which is a
     back-reference or a node which can accept multibyte character or
     multi character collating element.  */
  if (nmatch > 1 || dfa->has_mb_node)
    {
      /* Avoid overflow.  */
      if (BE (SIZE_MAX / sizeof (re_dfastate_t *) <= mctx.input.bufs_len, 0))
	{
	  err = REG_ESPACE;
	  goto free_return;
	}

      mctx.state_log = re_malloc (re_dfastate_t *, mctx.input.bufs_len + 1);
      if (BE (mctx.state_log == NULL, 0))
	{
	  err = REG_ESPACE;
	  goto free_return;
	}
    }
  else
    mctx.state_log = NULL;

  match_first = start;
  mctx.input.tip_context = (eflags & REG_NOTBOL) ? CONTEXT_BEGBUF
			   : CONTEXT_NEWLINE | CONTEXT_BEGBUF;

  /* Check incrementally whether of not the input string match.  */
  incr = (last_start < start) ? -1 : 1;
  left_lim = (last_start < start) ? last_start : start;
  right_lim = (last_start < start) ? start : last_start;
  sb = dfa->mb_cur_max == 1;
  match_kind =
    (fastmap
     ? ((sb || !(preg->syntax & RE_ICASE || t) ? 4 : 0)
	| (start <= last_start ? 2 : 0)
	| (t != NULL ? 1 : 0))
     : 8);

  for (;; match_first += incr)
    {
      err = REG_NOMATCH;
      if (match_first < left_lim || right_lim < match_first)
	goto free_return;

      /* Advance as rapidly as possible through the string, until we
	 find a plausible place to start matching.  This may be done
	 with varying efficiency, so there are various possibilities:
	 only the most common of them are specialized, in order to
	 save on code size.  We use a switch statement for speed.  */
      switch (match_kind)
	{
	case 8:
	  /* No fastmap.  */
	  break;

	case 7:
	  /* Fastmap with single-byte translation, match forward.  */
	  while (BE (match_first < right_lim, 1)
		 && !fastmap[t[(unsigned char) string[match_first]]])
	    ++match_first;
	  goto forward_match_found_start_or_reached_end;

	case 6:
	  /* Fastmap without translation, match forward.  */
	  while (BE (match_first < right_lim, 1)
		 && !fastmap[(unsigned char) string[match_first]])
	    ++match_first;

	forward_match_found_start_or_reached_end:
	  if (BE (match_first == right_lim, 0))
	    {
	      ch = match_first >= length
		       ? 0 : (unsigned char) string[match_first];
	      if (!fastmap[t ? t[ch] : ch])
		goto free_return;
	    }
	  break;

	case 4:
	case 5:
	  /* Fastmap without multi-byte translation, match backwards.  */
	  while (match_first >= left_lim)
	    {
	      ch = match_first >= length
		       ? 0 : (unsigned char) string[match_first];
	      if (fastmap[t ? t[ch] : ch])
		break;
	      --match_first;
	    }
	  if (match_first < left_lim)
	    goto free_return;
	  break;

	default:
	  /* In this case, we can't determine easily the current byte,
	     since it might be a component byte of a multibyte
	     character.  Then we use the constructed buffer instead.  */
	  for (;;)
	    {
	      /* If MATCH_FIRST is out of the valid range, reconstruct the
		 buffers.  */
	      __re_size_t offset = match_first - mctx.input.raw_mbs_idx;
	      if (BE (offset >= (__re_size_t) mctx.input.valid_raw_len, 0))
		{
		  err = re_string_reconstruct (&mctx.input, match_first,
					       eflags);
		  if (BE (err != REG_NOERROR, 0))
		    goto free_return;

		  offset = match_first - mctx.input.raw_mbs_idx;
		}
	      /* If MATCH_FIRST is out of the buffer, leave it as '\0'.
		 Note that MATCH_FIRST must not be smaller than 0.  */
	      ch = (match_first >= length
		    ? 0 : re_string_byte_at (&mctx.input, offset));
	      if (fastmap[ch])
		break;
	      match_first += incr;
	      if (match_first < left_lim || match_first > right_lim)
	        {
	          err = REG_NOMATCH;
	          goto free_return;
	        }
	    }
	  break;
	}

      /* Reconstruct the buffers so that the matcher can assume that
	 the matching starts from the beginning of the buffer.  */
      err = re_string_reconstruct (&mctx.input, match_first, eflags);
      if (BE (err != REG_NOERROR, 0))
	goto free_return;

#ifdef RE_ENABLE_I18N
     /* Don't consider this char as a possible match start if it part,
	yet isn't the head, of a multibyte character.  */
      if (!sb && !re_string_first_byte (&mctx.input, 0))
	continue;
#endif

      /* It seems to be appropriate one, then use the matcher.  */
      /* We assume that the matching starts from 0.  */
      mctx.state_log_top = mctx.nbkref_ents = mctx.max_mb_elem_len = 0;
      match_last = check_matching (&mctx, fl_longest_match,
				   start <= last_start ? &match_first : NULL);
      if (match_last != REG_MISSING)
	{
	  if (BE (match_last == REG_ERROR, 0))
	    {
	      err = REG_ESPACE;
	      goto free_return;
	    }
	  else
	    {
	      mctx.match_last = match_last;
	      if ((!preg->no_sub && nmatch > 1) || dfa->nbackref)
		{
		  re_dfastate_t *pstate = mctx.state_log[match_last];
		  mctx.last_node = check_halt_state_context (&mctx, pstate,
							     match_last);
		}
	      if ((!preg->no_sub && nmatch > 1 && dfa->has_plural_match)
		  || dfa->nbackref)
		{
		  err = prune_impossible_nodes (&mctx);
		  if (err == REG_NOERROR)
		    break;
		  if (BE (err != REG_NOMATCH, 0))
		    goto free_return;
		  match_last = REG_MISSING;
		}
	      else
		break; /* We found a match.  */
	    }
	}

      match_ctx_clean (&mctx);
    }

#ifdef DEBUG
  assert (match_last != REG_MISSING);
  assert (err == REG_NOERROR);
#endif

  /* Set pmatch[] if we need.  */
  if (nmatch > 0)
    {
      Idx reg_idx;

      /* Initialize registers.  */
      for (reg_idx = 1; reg_idx < nmatch; ++reg_idx)
	pmatch[reg_idx].rm_so = pmatch[reg_idx].rm_eo = -1;

      /* Set the points where matching start/end.  */
      pmatch[0].rm_so = 0;
      pmatch[0].rm_eo = mctx.match_last;
      /* FIXME: This function should fail if mctx.match_last exceeds
	 the maximum possible regoff_t value.  We need a new error
	 code REG_OVERFLOW.  */

      if (!preg->no_sub && nmatch > 1)
	{
	  err = set_regs (preg, &mctx, nmatch, pmatch,
			  dfa->has_plural_match && dfa->nbackref > 0);
	  if (BE (err != REG_NOERROR, 0))
	    goto free_return;
	}

      /* At last, add the offset to the each registers, since we slided
	 the buffers so that we could assume that the matching starts
	 from 0.  */
      for (reg_idx = 0; reg_idx < nmatch; ++reg_idx)
	if (pmatch[reg_idx].rm_so != -1)
	  {
#ifdef RE_ENABLE_I18N
	    if (BE (mctx.input.offsets_needed != 0, 0))
	      {
		pmatch[reg_idx].rm_so =
		  (pmatch[reg_idx].rm_so == mctx.input.valid_len
		   ? mctx.input.valid_raw_len
		   : mctx.input.offsets[pmatch[reg_idx].rm_so]);
		pmatch[reg_idx].rm_eo =
		  (pmatch[reg_idx].rm_eo == mctx.input.valid_len
		   ? mctx.input.valid_raw_len
		   : mctx.input.offsets[pmatch[reg_idx].rm_eo]);
	      }
#else
	    assert (mctx.input.offsets_needed == 0);
#endif
	    pmatch[reg_idx].rm_so += match_first;
	    pmatch[reg_idx].rm_eo += match_first;
	  }
      for (reg_idx = 0; reg_idx < extra_nmatch; ++reg_idx)
	{
	  pmatch[nmatch + reg_idx].rm_so = -1;
	  pmatch[nmatch + reg_idx].rm_eo = -1;
	}

      if (dfa->subexp_map)
        for (reg_idx = 0; reg_idx + 1 < nmatch; reg_idx++)
          if (dfa->subexp_map[reg_idx] != reg_idx)
            {
              pmatch[reg_idx + 1].rm_so
                = pmatch[dfa->subexp_map[reg_idx] + 1].rm_so;
              pmatch[reg_idx + 1].rm_eo
                = pmatch[dfa->subexp_map[reg_idx] + 1].rm_eo;
            }
    }

 free_return:
  re_free (mctx.state_log);
  if (dfa->nbackref)
    match_ctx_free (&mctx);
  re_string_destruct (&mctx.input);
  return err;
}

static reg_errcode_t
internal_function
prune_impossible_nodes (re_match_context_t *mctx)
{
  const re_dfa_t *const dfa = mctx->dfa;
  Idx halt_node, match_last;
  reg_errcode_t ret;
  re_dfastate_t **sifted_states;
  re_dfastate_t **lim_states = NULL;
  re_sift_context_t sctx;
#ifdef DEBUG
  assert (mctx->state_log != NULL);
#endif
  match_last = mctx->match_last;
  halt_node = mctx->last_node;

  /* Avoid overflow.  */
  if (BE (SIZE_MAX / sizeof (re_dfastate_t *) <= match_last, 0))
    return REG_ESPACE;

  sifted_states = re_malloc (re_dfastate_t *, match_last + 1);
  if (BE (sifted_states == NULL, 0))
    {
      ret = REG_ESPACE;
      goto free_return;
    }
  if (dfa->nbackref)
    {
      lim_states = re_malloc (re_dfastate_t *, match_last + 1);
      if (BE (lim_states == NULL, 0))
	{
	  ret = REG_ESPACE;
	  goto free_return;
	}
      while (1)
	{
	  memset (lim_states, '\0',
		  sizeof (re_dfastate_t *) * (match_last + 1));
	  sift_ctx_init (&sctx, sifted_states, lim_states, halt_node,
			 match_last);
	  ret = sift_states_backward (mctx, &sctx);
	  re_node_set_free (&sctx.limits);
	  if (BE (ret != REG_NOERROR, 0))
	      goto free_return;
	  if (sifted_states[0] != NULL || lim_states[0] != NULL)
	    break;
	  do
	    {
	      --match_last;
	      if (! REG_VALID_INDEX (match_last))
		{
		  ret = REG_NOMATCH;
		  goto free_return;
		}
	    } while (mctx->state_log[match_last] == NULL
		     || !mctx->state_log[match_last]->halt);
	  halt_node = check_halt_state_context (mctx,
						mctx->state_log[match_last],
						match_last);
	}
      ret = merge_state_array (dfa, sifted_states, lim_states,
			       match_last + 1);
      re_free (lim_states);
      lim_states = NULL;
      if (BE (ret != REG_NOERROR, 0))
	goto free_return;
    }
  else
    {
      sift_ctx_init (&sctx, sifted_states, lim_states, halt_node, match_last);
      ret = sift_states_backward (mctx, &sctx);
      re_node_set_free (&sctx.limits);
      if (BE (ret != REG_NOERROR, 0))
	goto free_return;
    }
  re_free (mctx->state_log);
  mctx->state_log = sifted_states;
  sifted_states = NULL;
  mctx->last_node = halt_node;
  mctx->match_last = match_last;
  ret = REG_NOERROR;
 free_return:
  re_free (sifted_states);
  re_free (lim_states);
  return ret;
}

/* Acquire an initial state and return it.
   We must select appropriate initial state depending on the context,
   since initial states may have constraints like "\<", "^", etc..  */

static _inline re_dfastate_t *
__attribute ((always_inline)) internal_function
acquire_init_state_context (reg_errcode_t *err, const re_match_context_t *mctx,
			    Idx idx)
{
  const re_dfa_t *const dfa = mctx->dfa;
  if (dfa->init_state->has_constraint)
    {
      unsigned int context;
      context = re_string_context_at (&mctx->input, idx - 1, mctx->eflags);
      if (IS_WORD_CONTEXT (context))
	return dfa->init_state_word;
      else if (IS_ORDINARY_CONTEXT (context))
	return dfa->init_state;
      else if (IS_BEGBUF_CONTEXT (context) && IS_NEWLINE_CONTEXT (context))
	return dfa->init_state_begbuf;
      else if (IS_NEWLINE_CONTEXT (context))
	return dfa->init_state_nl;
      else if (IS_BEGBUF_CONTEXT (context))
	{
	  /* It is relatively rare case, then calculate on demand.  */
	  return re_acquire_state_context (err, dfa,
					   dfa->init_state->entrance_nodes,
					   context);
	}
      else
	/* Must not happen?  */
	return dfa->init_state;
    }
  else
    return dfa->init_state;
}

/* Check whether the regular expression match input string INPUT or not,
   and return the index where the matching end.  Return REG_MISSING if
   there is no match, and return REG_ERROR in case of an error.
   FL_LONGEST_MATCH means we want the POSIX longest matching.
   If P_MATCH_FIRST is not NULL, and the match fails, it is set to the
   next place where we may want to try matching.
   Note that the matcher assume that the maching starts from the current
   index of the buffer.  */

static Idx
internal_function
check_matching (re_match_context_t *mctx, bool fl_longest_match,
		Idx *p_match_first)
{
  const re_dfa_t *const dfa = mctx->dfa;
  reg_errcode_t err;
  Idx match = 0;
  Idx match_last = REG_MISSING;
  Idx cur_str_idx = re_string_cur_idx (&mctx->input);
  re_dfastate_t *cur_state;
  bool at_init_state = p_match_first != NULL;
  Idx next_start_idx = cur_str_idx;

  err = REG_NOERROR;
  cur_state = acquire_init_state_context (&err, mctx, cur_str_idx);
  /* An initial state must not be NULL (invalid).  */
  if (BE (cur_state == NULL, 0))
    {
      assert (err == REG_ESPACE);
      return REG_ERROR;
    }

  if (mctx->state_log != NULL)
    {
      mctx->state_log[cur_str_idx] = cur_state;

      /* Check OP_OPEN_SUBEXP in the initial state in case that we use them
	 later.  E.g. Processing back references.  */
      if (BE (dfa->nbackref, 0))
	{
	  at_init_state = false;
	  err = check_subexp_matching_top (mctx, &cur_state->nodes, 0);
	  if (BE (err != REG_NOERROR, 0))
	    return err;

	  if (cur_state->has_backref)
	    {
	      err = transit_state_bkref (mctx, &cur_state->nodes);
	      if (BE (err != REG_NOERROR, 0))
	        return err;
	    }
	}
    }

  /* If the RE accepts NULL string.  */
  if (BE (cur_state->halt, 0))
    {
      if (!cur_state->has_constraint
	  || check_halt_state_context (mctx, cur_state, cur_str_idx))
	{
	  if (!fl_longest_match)
	    return cur_str_idx;
	  else
	    {
	      match_last = cur_str_idx;
	      match = 1;
	    }
	}
    }

  while (!re_string_eoi (&mctx->input))
    {
      re_dfastate_t *old_state = cur_state;
      Idx next_char_idx = re_string_cur_idx (&mctx->input) + 1;

      if (BE (next_char_idx >= mctx->input.bufs_len, 0)
          || (BE (next_char_idx >= mctx->input.valid_len, 0)
              && mctx->input.valid_len < mctx->input.len))
        {
          err = extend_buffers (mctx);
          if (BE (err != REG_NOERROR, 0))
	    {
	      assert (err == REG_ESPACE);
	      return REG_ERROR;
	    }
        }

      cur_state = transit_state (&err, mctx, cur_state);
      if (mctx->state_log != NULL)
	cur_state = merge_state_with_log (&err, mctx, cur_state);

      if (cur_state == NULL)
	{
	  /* Reached the invalid state or an error.  Try to recover a valid
	     state using the state log, if available and if we have not
	     already found a valid (even if not the longest) match.  */
	  if (BE (err != REG_NOERROR, 0))
	    return REG_ERROR;

	  if (mctx->state_log == NULL
	      || (match && !fl_longest_match)
	      || (cur_state = find_recover_state (&err, mctx)) == NULL)
	    break;
	}

      if (BE (at_init_state, 0))
	{
	  if (old_state == cur_state)
	    next_start_idx = next_char_idx;
	  else
	    at_init_state = false;
	}

      if (cur_state->halt)
	{
	  /* Reached a halt state.
	     Check the halt state can satisfy the current context.  */
	  if (!cur_state->has_constraint
	      || check_halt_state_context (mctx, cur_state,
					   re_string_cur_idx (&mctx->input)))
	    {
	      /* We found an appropriate halt state.  */
	      match_last = re_string_cur_idx (&mctx->input);
	      match = 1;

	      /* We found a match, do not modify match_first below.  */
	      p_match_first = NULL;
	      if (!fl_longest_match)
		break;
	    }
	}
    }

  if (p_match_first)
    *p_match_first += next_start_idx;

  return match_last;
}

/* Check NODE match the current context.  */

static bool
internal_function
check_halt_node_context (const re_dfa_t *dfa, Idx node, unsigned int context)
{
  re_token_type_t type = dfa->nodes[node].type;
  unsigned int constraint = dfa->nodes[node].constraint;
  if (type != END_OF_RE)
    return false;
  if (!constraint)
    return true;
  if (NOT_SATISFY_NEXT_CONSTRAINT (constraint, context))
    return false;
  return true;
}

/* Check the halt state STATE match the current context.
   Return 0 if not match, if the node, STATE has, is a halt node and
   match the context, return the node.  */

static Idx
internal_function
check_halt_state_context (const re_match_context_t *mctx,
			  const re_dfastate_t *state, Idx idx)
{
  Idx i;
  unsigned int context;
#ifdef DEBUG
  assert (state->halt);
#endif
  context = re_string_context_at (&mctx->input, idx, mctx->eflags);
  for (i = 0; i < state->nodes.nelem; ++i)
    if (check_halt_node_context (mctx->dfa, state->nodes.elems[i], context))
      return state->nodes.elems[i];
  return 0;
}

/* Compute the next node to which "NFA" transit from NODE("NFA" is a NFA
   corresponding to the DFA).
   Return the destination node, and update EPS_VIA_NODES;
   return REG_MISSING in case of errors.  */

static Idx
internal_function
proceed_next_node (const re_match_context_t *mctx, Idx nregs, regmatch_t *regs,
		   Idx *pidx, Idx node, re_node_set *eps_via_nodes,
		   struct re_fail_stack_t *fs)
{
  const re_dfa_t *const dfa = mctx->dfa;
  Idx i;
  bool ok;
  if (IS_EPSILON_NODE (dfa->nodes[node].type))
    {
      re_node_set *cur_nodes = &mctx->state_log[*pidx]->nodes;
      re_node_set *edests = &dfa->edests[node];
      Idx dest_node;
      ok = re_node_set_insert (eps_via_nodes, node);
      if (BE (! ok, 0))
	return REG_ERROR;
      /* Pick up a valid destination, or return REG_MISSING if none
	 is found.  */
      for (dest_node = REG_MISSING, i = 0; i < edests->nelem; ++i)
	{
	  Idx candidate = edests->elems[i];
	  if (!re_node_set_contains (cur_nodes, candidate))
	    continue;
          if (dest_node == REG_MISSING)
	    dest_node = candidate;

          else
	    {
	      /* In order to avoid infinite loop like "(a*)*", return the second
	         epsilon-transition if the first was already considered.  */
	      if (re_node_set_contains (eps_via_nodes, dest_node))
	        return candidate;

	      /* Otherwise, push the second epsilon-transition on the fail stack.  */
	      else if (fs != NULL
		       && push_fail_stack (fs, *pidx, candidate, nregs, regs,
				           eps_via_nodes))
		return REG_ERROR;

	      /* We know we are going to exit.  */
	      break;
	    }
	}
      return dest_node;
    }
  else
    {
      Idx naccepted = 0;
      re_token_type_t type = dfa->nodes[node].type;

#ifdef RE_ENABLE_I18N
      if (dfa->nodes[node].accept_mb)
	naccepted = check_node_accept_bytes (dfa, node, &mctx->input, *pidx);
      else
#endif /* RE_ENABLE_I18N */
      if (type == OP_BACK_REF)
	{
	  Idx subexp_idx = dfa->nodes[node].opr.idx + 1;
	  naccepted = regs[subexp_idx].rm_eo - regs[subexp_idx].rm_so;
	  if (fs != NULL)
	    {
	      if (regs[subexp_idx].rm_so == -1 || regs[subexp_idx].rm_eo == -1)
		return REG_MISSING;
	      else if (naccepted)
		{
		  char *buf = (char *) re_string_get_buffer (&mctx->input);
		  if (memcmp (buf + regs[subexp_idx].rm_so, buf + *pidx,
			      naccepted) != 0)
		    return REG_MISSING;
		}
	    }

	  if (naccepted == 0)
	    {
	      Idx dest_node;
	      ok = re_node_set_insert (eps_via_nodes, node);
	      if (BE (! ok, 0))
		return REG_ERROR;
	      dest_node = dfa->edests[node].elems[0];
	      if (re_node_set_contains (&mctx->state_log[*pidx]->nodes,
					dest_node))
		return dest_node;
	    }
	}

      if (naccepted != 0
	  || check_node_accept (mctx, dfa->nodes + node, *pidx))
	{
	  Idx dest_node = dfa->nexts[node];
	  *pidx = (naccepted == 0) ? *pidx + 1 : *pidx + naccepted;
	  if (fs && (*pidx > mctx->match_last || mctx->state_log[*pidx] == NULL
		     || !re_node_set_contains (&mctx->state_log[*pidx]->nodes,
					       dest_node)))
	    return REG_MISSING;
	  re_node_set_empty (eps_via_nodes);
	  return dest_node;
	}
    }
  return REG_MISSING;
}

static reg_errcode_t
internal_function
push_fail_stack (struct re_fail_stack_t *fs, Idx str_idx, Idx dest_node,
		 Idx nregs, regmatch_t *regs, re_node_set *eps_via_nodes)
{
  reg_errcode_t err;
  Idx num = fs->num++;
  if (fs->num == fs->alloc)
    {
      struct re_fail_stack_ent_t *new_array;
      new_array = realloc (fs->stack, (sizeof (struct re_fail_stack_ent_t)
				       * fs->alloc * 2));
      if (new_array == NULL)
	return REG_ESPACE;
      fs->alloc *= 2;
      fs->stack = new_array;
    }
  fs->stack[num].idx = str_idx;
  fs->stack[num].node = dest_node;
  fs->stack[num].regs = re_malloc (regmatch_t, nregs);
  if (fs->stack[num].regs == NULL)
    return REG_ESPACE;
  memcpy (fs->stack[num].regs, regs, sizeof (regmatch_t) * nregs);
  err = re_node_set_init_copy (&fs->stack[num].eps_via_nodes, eps_via_nodes);
  return err;
}

static Idx
internal_function
pop_fail_stack (struct re_fail_stack_t *fs, Idx *pidx, Idx nregs,
		regmatch_t *regs, re_node_set *eps_via_nodes)
{
  Idx num = --fs->num;
  assert (REG_VALID_INDEX (num));
  *pidx = fs->stack[num].idx;
  memcpy (regs, fs->stack[num].regs, sizeof (regmatch_t) * nregs);
  re_node_set_free (eps_via_nodes);
  re_free (fs->stack[num].regs);
  *eps_via_nodes = fs->stack[num].eps_via_nodes;
  return fs->stack[num].node;
}

/* Set the positions where the subexpressions are starts/ends to registers
   PMATCH.
   Note: We assume that pmatch[0] is already set, and
   pmatch[i].rm_so == pmatch[i].rm_eo == -1 for 0 < i < nmatch.  */

static reg_errcode_t
internal_function
set_regs (const regex_t *preg, const re_match_context_t *mctx, size_t nmatch,
	  regmatch_t *pmatch, bool fl_backtrack)
{
  const re_dfa_t *dfa = (const re_dfa_t *) preg->buffer;
  Idx idx, cur_node;
  re_node_set eps_via_nodes;
  struct re_fail_stack_t *fs;
  struct re_fail_stack_t fs_body = { 0, 2, NULL };
  regmatch_t *prev_idx_match;
  bool prev_idx_match_malloced = false;

#ifdef DEBUG
  assert (nmatch > 1);
  assert (mctx->state_log != NULL);
#endif
  if (fl_backtrack)
    {
      fs = &fs_body;
      fs->stack = re_malloc (struct re_fail_stack_ent_t, fs->alloc);
      if (fs->stack == NULL)
	return REG_ESPACE;
    }
  else
    fs = NULL;

  cur_node = dfa->init_node;
  re_node_set_init_empty (&eps_via_nodes);

  if (__libc_use_alloca (nmatch * sizeof (regmatch_t)))
    prev_idx_match = (regmatch_t *) alloca (nmatch * sizeof (regmatch_t));
  else
    {
      prev_idx_match = re_malloc (regmatch_t, nmatch);
      if (prev_idx_match == NULL)
	{
	  free_fail_stack_return (fs);
	  return REG_ESPACE;
	}
      prev_idx_match_malloced = true;
    }
  memcpy (prev_idx_match, pmatch, sizeof (regmatch_t) * nmatch);

  for (idx = pmatch[0].rm_so; idx <= pmatch[0].rm_eo ;)
    {
      update_regs (dfa, pmatch, prev_idx_match, cur_node, idx, nmatch);

      if (idx == pmatch[0].rm_eo && cur_node == mctx->last_node)
	{
	  Idx reg_idx;
	  if (fs)
	    {
	      for (reg_idx = 0; reg_idx < nmatch; ++reg_idx)
		if (pmatch[reg_idx].rm_so > -1 && pmatch[reg_idx].rm_eo == -1)
		  break;
	      if (reg_idx == nmatch)
		{
		  re_node_set_free (&eps_via_nodes);
		  if (prev_idx_match_malloced)
		    re_free (prev_idx_match);
		  return free_fail_stack_return (fs);
		}
	      cur_node = pop_fail_stack (fs, &idx, nmatch, pmatch,
					 &eps_via_nodes);
	    }
	  else
	    {
	      re_node_set_free (&eps_via_nodes);
	      if (prev_idx_match_malloced)
		re_free (prev_idx_match);
	      return REG_NOERROR;
	    }
	}

      /* Proceed to next node.  */
      cur_node = proceed_next_node (mctx, nmatch, pmatch, &idx, cur_node,
				    &eps_via_nodes, fs);

      if (BE (! REG_VALID_INDEX (cur_node), 0))
	{
	  if (BE (cur_node == REG_ERROR, 0))
	    {
	      re_node_set_free (&eps_via_nodes);
	      if (prev_idx_match_malloced)
		re_free (prev_idx_match);
	      free_fail_stack_return (fs);
	      return REG_ESPACE;
	    }
	  if (fs)
	    cur_node = pop_fail_stack (fs, &idx, nmatch, pmatch,
				       &eps_via_nodes);
	  else
	    {
	      re_node_set_free (&eps_via_nodes);
	      if (prev_idx_match_malloced)
		re_free (prev_idx_match);
	      return REG_NOMATCH;
	    }
	}
    }
  re_node_set_free (&eps_via_nodes);
  if (prev_idx_match_malloced)
    re_free (prev_idx_match);
  return free_fail_stack_return (fs);
}

static reg_errcode_t
internal_function
free_fail_stack_return (struct re_fail_stack_t *fs)
{
  if (fs)
    {
      Idx fs_idx;
      for (fs_idx = 0; fs_idx < fs->num; ++fs_idx)
	{
	  re_node_set_free (&fs->stack[fs_idx].eps_via_nodes);
	  re_free (fs->stack[fs_idx].regs);
	}
      re_free (fs->stack);
    }
  return REG_NOERROR;
}

static void
internal_function
update_regs (const re_dfa_t *dfa, regmatch_t *pmatch,
	     regmatch_t *prev_idx_match, Idx cur_node, Idx cur_idx, Idx nmatch)
{
  int type = dfa->nodes[cur_node].type;
  if (type == OP_OPEN_SUBEXP)
    {
      Idx reg_num = dfa->nodes[cur_node].opr.idx + 1;

      /* We are at the first node of this sub expression.  */
      if (reg_num < nmatch)
	{
	  pmatch[reg_num].rm_so = cur_idx;
	  pmatch[reg_num].rm_eo = -1;
	}
    }
  else if (type == OP_CLOSE_SUBEXP)
    {
      Idx reg_num = dfa->nodes[cur_node].opr.idx + 1;
      if (reg_num < nmatch)
	{
	  /* We are at the last node of this sub expression.  */
	  if (pmatch[reg_num].rm_so < cur_idx)
	    {
	      pmatch[reg_num].rm_eo = cur_idx;
	      /* This is a non-empty match or we are not inside an optional
		 subexpression.  Accept this right away.  */
	      memcpy (prev_idx_match, pmatch, sizeof (regmatch_t) * nmatch);
	    }
	  else
	    {
	      if (dfa->nodes[cur_node].opt_subexp
		  && prev_idx_match[reg_num].rm_so != -1)
		/* We transited through an empty match for an optional
		   subexpression, like (a?)*, and this is not the subexp's
		   first match.  Copy back the old content of the registers
		   so that matches of an inner subexpression are undone as
		   well, like in ((a?))*.  */
		memcpy (pmatch, prev_idx_match, sizeof (regmatch_t) * nmatch);
	      else
		/* We completed a subexpression, but it may be part of
		   an optional one, so do not update PREV_IDX_MATCH.  */
		pmatch[reg_num].rm_eo = cur_idx;
	    }
	}
    }
}

/* This function checks the STATE_LOG from the SCTX->last_str_idx to 0
   and sift the nodes in each states according to the following rules.
   Updated state_log will be wrote to STATE_LOG.

   Rules: We throw away the Node `a' in the STATE_LOG[STR_IDX] if...
     1. When STR_IDX == MATCH_LAST(the last index in the state_log):
	If `a' isn't the LAST_NODE and `a' can't epsilon transit to
	the LAST_NODE, we throw away the node `a'.
     2. When 0 <= STR_IDX < MATCH_LAST and `a' accepts
	string `s' and transit to `b':
	i. If 'b' isn't in the STATE_LOG[STR_IDX+strlen('s')], we throw
	   away the node `a'.
	ii. If 'b' is in the STATE_LOG[STR_IDX+strlen('s')] but 'b' is
	    thrown away, we throw away the node `a'.
     3. When 0 <= STR_IDX < MATCH_LAST and 'a' epsilon transit to 'b':
	i. If 'b' isn't in the STATE_LOG[STR_IDX], we throw away the
	   node `a'.
	ii. If 'b' is in the STATE_LOG[STR_IDX] but 'b' is thrown away,
	    we throw away the node `a'.  */

#define STATE_NODE_CONTAINS(state,node) \
  ((state) != NULL && re_node_set_contains (&(state)->nodes, node))

static reg_errcode_t
internal_function
sift_states_backward (const re_match_context_t *mctx, re_sift_context_t *sctx)
{
  reg_errcode_t err;
  int null_cnt = 0;
  Idx str_idx = sctx->last_str_idx;
  re_node_set cur_dest;

#ifdef DEBUG
  assert (mctx->state_log != NULL && mctx->state_log[str_idx] != NULL);
#endif

  /* Build sifted state_log[str_idx].  It has the nodes which can epsilon
     transit to the last_node and the last_node itself.  */
  err = re_node_set_init_1 (&cur_dest, sctx->last_node);
  if (BE (err != REG_NOERROR, 0))
    return err;
  err = update_cur_sifted_state (mctx, sctx, str_idx, &cur_dest);
  if (BE (err != REG_NOERROR, 0))
    goto free_return;

  /* Then check each states in the state_log.  */
  while (str_idx > 0)
    {
      /* Update counters.  */
      null_cnt = (sctx->sifted_states[str_idx] == NULL) ? null_cnt + 1 : 0;
      if (null_cnt > mctx->max_mb_elem_len)
	{
	  memset (sctx->sifted_states, '\0',
		  sizeof (re_dfastate_t *) * str_idx);
	  re_node_set_free (&cur_dest);
	  return REG_NOERROR;
	}
      re_node_set_empty (&cur_dest);
      --str_idx;

      if (mctx->state_log[str_idx])
	{
	  err = build_sifted_states (mctx, sctx, str_idx, &cur_dest);
          if (BE (err != REG_NOERROR, 0))
	    goto free_return;
	}

      /* Add all the nodes which satisfy the following conditions:
	 - It can epsilon transit to a node in CUR_DEST.
	 - It is in CUR_SRC.
	 And update state_log.  */
      err = update_cur_sifted_state (mctx, sctx, str_idx, &cur_dest);
      if (BE (err != REG_NOERROR, 0))
	goto free_return;
    }
  err = REG_NOERROR;
 free_return:
  re_node_set_free (&cur_dest);
  return err;
}

static reg_errcode_t
internal_function
build_sifted_states (const re_match_context_t *mctx, re_sift_context_t *sctx,
		     Idx str_idx, re_node_set *cur_dest)
{
  const re_dfa_t *const dfa = mctx->dfa;
  const re_node_set *cur_src = &mctx->state_log[str_idx]->non_eps_nodes;
  Idx i;

  /* Then build the next sifted state.
     We build the next sifted state on `cur_dest', and update
     `sifted_states[str_idx]' with `cur_dest'.
     Note:
     `cur_dest' is the sifted state from `state_log[str_idx + 1]'.
     `cur_src' points the node_set of the old `state_log[str_idx]'
     (with the epsilon nodes pre-filtered out).  */
  for (i = 0; i < cur_src->nelem; i++)
    {
      Idx prev_node = cur_src->elems[i];
      int naccepted = 0;
      bool ok;

#ifdef DEBUG
      re_token_type_t type = dfa->nodes[prev_node].type;
      assert (!IS_EPSILON_NODE (type));
#endif
#ifdef RE_ENABLE_I18N
      /* If the node may accept `multi byte'.  */
      if (dfa->nodes[prev_node].accept_mb)
	naccepted = sift_states_iter_mb (mctx, sctx, prev_node,
					 str_idx, sctx->last_str_idx);
#endif /* RE_ENABLE_I18N */

      /* We don't check backreferences here.
	 See update_cur_sifted_state().  */
      if (!naccepted
	  && check_node_accept (mctx, dfa->nodes + prev_node, str_idx)
	  && STATE_NODE_CONTAINS (sctx->sifted_states[str_idx + 1],
				  dfa->nexts[prev_node]))
	naccepted = 1;

      if (naccepted == 0)
	continue;

      if (sctx->limits.nelem)
	{
	  Idx to_idx = str_idx + naccepted;
	  if (check_dst_limits (mctx, &sctx->limits,
				dfa->nexts[prev_node], to_idx,
				prev_node, str_idx))
	    continue;
	}
      ok = re_node_set_insert (cur_dest, prev_node);
      if (BE (! ok, 0))
	return REG_ESPACE;
    }

  return REG_NOERROR;
}

/* Helper functions.  */

static reg_errcode_t
internal_function
clean_state_log_if_needed (re_match_context_t *mctx, Idx next_state_log_idx)
{
  Idx top = mctx->state_log_top;

  if (next_state_log_idx >= mctx->input.bufs_len
      || (next_state_log_idx >= mctx->input.valid_len
	  && mctx->input.valid_len < mctx->input.len))
    {
      reg_errcode_t err;
      err = extend_buffers (mctx);
      if (BE (err != REG_NOERROR, 0))
	return err;
    }

  if (top < next_state_log_idx)
    {
      memset (mctx->state_log + top + 1, '\0',
	      sizeof (re_dfastate_t *) * (next_state_log_idx - top));
      mctx->state_log_top = next_state_log_idx;
    }
  return REG_NOERROR;
}

static reg_errcode_t
internal_function
merge_state_array (const re_dfa_t *dfa, re_dfastate_t **dst,
		   re_dfastate_t **src, Idx num)
{
  Idx st_idx;
  reg_errcode_t err;
  for (st_idx = 0; st_idx < num; ++st_idx)
    {
      if (dst[st_idx] == NULL)
	dst[st_idx] = src[st_idx];
      else if (src[st_idx] != NULL)
	{
	  re_node_set merged_set;
	  err = re_node_set_init_union (&merged_set, &dst[st_idx]->nodes,
					&src[st_idx]->nodes);
	  if (BE (err != REG_NOERROR, 0))
	    return err;
	  dst[st_idx] = re_acquire_state (&err, dfa, &merged_set);
	  re_node_set_free (&merged_set);
	  if (BE (err != REG_NOERROR, 0))
	    return err;
	}
    }
  return REG_NOERROR;
}

static reg_errcode_t
internal_function
update_cur_sifted_state (const re_match_context_t *mctx,
			 re_sift_context_t *sctx, Idx str_idx,
			 re_node_set *dest_nodes)
{
  const re_dfa_t *const dfa = mctx->dfa;
  reg_errcode_t err = REG_NOERROR;
  const re_node_set *candidates;
  candidates = ((mctx->state_log[str_idx] == NULL) ? NULL
		: &mctx->state_log[str_idx]->nodes);

  if (dest_nodes->nelem == 0)
    sctx->sifted_states[str_idx] = NULL;
  else
    {
      if (candidates)
	{
	  /* At first, add the nodes which can epsilon transit to a node in
	     DEST_NODE.  */
	  err = add_epsilon_src_nodes (dfa, dest_nodes, candidates);
	  if (BE (err != REG_NOERROR, 0))
	    return err;

	  /* Then, check the limitations in the current sift_context.  */
	  if (sctx->limits.nelem)
	    {
	      err = check_subexp_limits (dfa, dest_nodes, candidates, &sctx->limits,
					 mctx->bkref_ents, str_idx);
	      if (BE (err != REG_NOERROR, 0))
		return err;
	    }
	}

      sctx->sifted_states[str_idx] = re_acquire_state (&err, dfa, dest_nodes);
      if (BE (err != REG_NOERROR, 0))
	return err;
    }

  if (candidates && mctx->state_log[str_idx]->has_backref)
    {
      err = sift_states_bkref (mctx, sctx, str_idx, candidates);
      if (BE (err != REG_NOERROR, 0))
	return err;
    }
  return REG_NOERROR;
}

static reg_errcode_t
internal_function
add_epsilon_src_nodes (const re_dfa_t *dfa, re_node_set *dest_nodes,
		       const re_node_set *candidates)
{
  reg_errcode_t err = REG_NOERROR;
  Idx i;

  re_dfastate_t *state = re_acquire_state (&err, dfa, dest_nodes);
  if (BE (err != REG_NOERROR, 0))
    return err;

  if (!state->inveclosure.alloc)
    {
      err = re_node_set_alloc (&state->inveclosure, dest_nodes->nelem);
      if (BE (err != REG_NOERROR, 0))
        return REG_ESPACE;
      for (i = 0; i < dest_nodes->nelem; i++)
        re_node_set_merge (&state->inveclosure,
			   dfa->inveclosures + dest_nodes->elems[i]);
    }
  return re_node_set_add_intersect (dest_nodes, candidates,
				    &state->inveclosure);
}

static reg_errcode_t
internal_function
sub_epsilon_src_nodes (const re_dfa_t *dfa, Idx node, re_node_set *dest_nodes,
		       const re_node_set *candidates)
{
    Idx ecl_idx;
    reg_errcode_t err;
    re_node_set *inv_eclosure = dfa->inveclosures + node;
    re_node_set except_nodes;
    re_node_set_init_empty (&except_nodes);
    for (ecl_idx = 0; ecl_idx < inv_eclosure->nelem; ++ecl_idx)
      {
	Idx cur_node = inv_eclosure->elems[ecl_idx];
	if (cur_node == node)
	  continue;
	if (IS_EPSILON_NODE (dfa->nodes[cur_node].type))
	  {
	    Idx edst1 = dfa->edests[cur_node].elems[0];
	    Idx edst2 = ((dfa->edests[cur_node].nelem > 1)
			 ? dfa->edests[cur_node].elems[1] : REG_MISSING);
	    if ((!re_node_set_contains (inv_eclosure, edst1)
		 && re_node_set_contains (dest_nodes, edst1))
		|| (REG_VALID_NONZERO_INDEX (edst2)
		    && !re_node_set_contains (inv_eclosure, edst2)
		    && re_node_set_contains (dest_nodes, edst2)))
	      {
		err = re_node_set_add_intersect (&except_nodes, candidates,
						 dfa->inveclosures + cur_node);
		if (BE (err != REG_NOERROR, 0))
		  {
		    re_node_set_free (&except_nodes);
		    return err;
		  }
	      }
	  }
      }
    for (ecl_idx = 0; ecl_idx < inv_eclosure->nelem; ++ecl_idx)
      {
	Idx cur_node = inv_eclosure->elems[ecl_idx];
	if (!re_node_set_contains (&except_nodes, cur_node))
	  {
	    Idx idx = re_node_set_contains (dest_nodes, cur_node) - 1;
	    re_node_set_remove_at (dest_nodes, idx);
	  }
      }
    re_node_set_free (&except_nodes);
    return REG_NOERROR;
}

static bool
internal_function
check_dst_limits (const re_match_context_t *mctx, const re_node_set *limits,
		  Idx dst_node, Idx dst_idx, Idx src_node, Idx src_idx)
{
  const re_dfa_t *const dfa = mctx->dfa;
  Idx lim_idx, src_pos, dst_pos;

  Idx dst_bkref_idx = search_cur_bkref_entry (mctx, dst_idx);
  Idx src_bkref_idx = search_cur_bkref_entry (mctx, src_idx);
  for (lim_idx = 0; lim_idx < limits->nelem; ++lim_idx)
    {
      Idx subexp_idx;
      struct re_backref_cache_entry *ent;
      ent = mctx->bkref_ents + limits->elems[lim_idx];
      subexp_idx = dfa->nodes[ent->node].opr.idx;

      dst_pos = check_dst_limits_calc_pos (mctx, limits->elems[lim_idx],
					   subexp_idx, dst_node, dst_idx,
					   dst_bkref_idx);
      src_pos = check_dst_limits_calc_pos (mctx, limits->elems[lim_idx],
					   subexp_idx, src_node, src_idx,
					   src_bkref_idx);

      /* In case of:
	 <src> <dst> ( <subexp> )
	 ( <subexp> ) <src> <dst>
	 ( <subexp1> <src> <subexp2> <dst> <subexp3> )  */
      if (src_pos == dst_pos)
	continue; /* This is unrelated limitation.  */
      else
	return true;
    }
  return false;
}

static int
internal_function
check_dst_limits_calc_pos_1 (const re_match_context_t *mctx, int boundaries,
			     Idx subexp_idx, Idx from_node, Idx bkref_idx)
{
  const re_dfa_t *const dfa = mctx->dfa;
  const re_node_set *eclosures = dfa->eclosures + from_node;
  Idx node_idx;

  /* Else, we are on the boundary: examine the nodes on the epsilon
     closure.  */
  for (node_idx = 0; node_idx < eclosures->nelem; ++node_idx)
    {
      Idx node = eclosures->elems[node_idx];
      switch (dfa->nodes[node].type)
	{
	case OP_BACK_REF:
	  if (bkref_idx != REG_MISSING)
	    {
	      struct re_backref_cache_entry *ent = mctx->bkref_ents + bkref_idx;
	      do
	        {
		  Idx dst;
		  int cpos;

		  if (ent->node != node)
		    continue;

		  if (subexp_idx < BITSET_WORD_BITS
		      && !(ent->eps_reachable_subexps_map
			   & ((bitset_word_t) 1 << subexp_idx)))
		    continue;

		  /* Recurse trying to reach the OP_OPEN_SUBEXP and
		     OP_CLOSE_SUBEXP cases below.  But, if the
		     destination node is the same node as the source
		     node, don't recurse because it would cause an
		     infinite loop: a regex that exhibits this behavior
		     is ()\1*\1*  */
		  dst = dfa->edests[node].elems[0];
		  if (dst == from_node)
		    {
		      if (boundaries & 1)
		        return -1;
		      else /* if (boundaries & 2) */
		        return 0;
		    }

		  cpos =
		    check_dst_limits_calc_pos_1 (mctx, boundaries, subexp_idx,
						 dst, bkref_idx);
		  if (cpos == -1 /* && (boundaries & 1) */)
		    return -1;
		  if (cpos == 0 && (boundaries & 2))
		    return 0;

		  if (subexp_idx < BITSET_WORD_BITS)
		    ent->eps_reachable_subexps_map
		      &= ~((bitset_word_t) 1 << subexp_idx);
	        }
	      while (ent++->more);
	    }
	  break;

	case OP_OPEN_SUBEXP:
	  if ((boundaries & 1) && subexp_idx == dfa->nodes[node].opr.idx)
	    return -1;
	  break;

	case OP_CLOSE_SUBEXP:
	  if ((boundaries & 2) && subexp_idx == dfa->nodes[node].opr.idx)
	    return 0;
	  break;

	default:
	    break;
	}
    }

  return (boundaries & 2) ? 1 : 0;
}

static int
internal_function
check_dst_limits_calc_pos (const re_match_context_t *mctx, Idx limit,
			   Idx subexp_idx, Idx from_node, Idx str_idx,
			   Idx bkref_idx)
{
  struct re_backref_cache_entry *lim = mctx->bkref_ents + limit;
  int boundaries;

  /* If we are outside the range of the subexpression, return -1 or 1.  */
  if (str_idx < lim->subexp_from)
    return -1;

  if (lim->subexp_to < str_idx)
    return 1;

  /* If we are within the subexpression, return 0.  */
  boundaries = (str_idx == lim->subexp_from);
  boundaries |= (str_idx == lim->subexp_to) << 1;
  if (boundaries == 0)
    return 0;

  /* Else, examine epsilon closure.  */
  return check_dst_limits_calc_pos_1 (mctx, boundaries, subexp_idx,
				      from_node, bkref_idx);
}

/* Check the limitations of sub expressions LIMITS, and remove the nodes
   which are against limitations from DEST_NODES. */

static reg_errcode_t
internal_function
check_subexp_limits (const re_dfa_t *dfa, re_node_set *dest_nodes,
		     const re_node_set *candidates, re_node_set *limits,
		     struct re_backref_cache_entry *bkref_ents, Idx str_idx)
{
  reg_errcode_t err;
  Idx node_idx, lim_idx;

  for (lim_idx = 0; lim_idx < limits->nelem; ++lim_idx)
    {
      Idx subexp_idx;
      struct re_backref_cache_entry *ent;
      ent = bkref_ents + limits->elems[lim_idx];

      if (str_idx <= ent->subexp_from || ent->str_idx < str_idx)
	continue; /* This is unrelated limitation.  */

      subexp_idx = dfa->nodes[ent->node].opr.idx;
      if (ent->subexp_to == str_idx)
	{
	  Idx ops_node = REG_MISSING;
	  Idx cls_node = REG_MISSING;
	  for (node_idx = 0; node_idx < dest_nodes->nelem; ++node_idx)
	    {
	      Idx node = dest_nodes->elems[node_idx];
	      re_token_type_t type = dfa->nodes[node].type;
	      if (type == OP_OPEN_SUBEXP
		  && subexp_idx == dfa->nodes[node].opr.idx)
		ops_node = node;
	      else if (type == OP_CLOSE_SUBEXP
		       && subexp_idx == dfa->nodes[node].opr.idx)
		cls_node = node;
	    }

	  /* Check the
