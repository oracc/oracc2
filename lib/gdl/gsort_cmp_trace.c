static FILE *gsort_cmp_trace_fp = NULL;

static void
gsort_cmp_trace_init(void)
{
  if (!(gsort_cmp_trace_fp = fopen("gsort.log", "w")))
    fprintf(stderr, "gsort_cmp_trace: unable to write to gsort.log; gsort_trace disabled\n");
  gsort_trace = 1;
}

static void
gsort_cmp_trace_term(void)
{
  if (gsort_cmp_trace_fp)
    fclose(gsort_cmp_trace_fp);
}

static int
gsort_cmp_trace_cmp(GS_head *h1, GS_head *h2)
{
  int i, ret = 0;

  for (i = 0; i < h1->n && i < h2->n; ++i)
    if ((ret = gsort_cmp_item(h1->i[i], h2->i[i])))
      return ret;

  if (h1->i[i] || h2->i[i])
    return h1->i[i] ? 1 : -1;

  /* if we are still here the grapheme sequences are identical when ignoring case.
     
     sll_has_sign_indicator looks for key uppercase letters to detect
     sign names so we use that to try to resolve via lower/upper case
   */
  if ((ret = strcmp((ccp)h1->s, (ccp)h2->s)))
    {
      if (sll_has_sign_indicator(h1->s)
	  && sll_has_sign_indicator(h2->s))
	return ret;
      else if (sll_has_sign_indicator(h1->s))
	return 1;
      else if (sll_has_sign_indicator(h2->s))
	return -1;
      else
	return ret;
    }
  return ret;
}

static int
gsort_cmp_trace(GS_head *h1, GS_head *h2)
{
  static int ncmp = 0;
  int ret = gsort_cmp_trace_cmp(h1, h2);
  if (gsort_cmp_trace_fp)
    {
      fprintf(gsort_cmp_trace_fp, "###\n%d\t", ++ncmp);
      gsort_show_sub(gsort_cmp_trace_fp, h1);
      fputs("\n\t", gsort_cmp_trace_fp);
      gsort_show_sub(gsort_cmp_trace_fp, h2);
      fprintf(gsort_cmp_trace_fp, "\n>>%d\n###\n", ret);
    }
  return ret;
}
