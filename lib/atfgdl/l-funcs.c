static void
ag_user_action(void)
{
  if (agflextrace)
      fprintf(stderr, "%s:%d: aglex: text=::%s:: start=%d; tra_text=%d\n", curragfile, aglineno, agtext, YY_START, tra_text);

  aglloc.file = curragfile;
  aglloc.line = aglineno;
}

static void
ag_l_lex_init(const char *file, int line)
{
  curragfile = agfile = file;
  aglineno = line;
}

#if 0
static int
ag_line_save(const char *t)
{
  extern const char *curr_line_tok;
  curr_line_tok = (ccp)hpool_copy((uccp)t, ag_hpool);
  return 0;
}
#endif

#if 0
static int
ag_gdl_init(void)
{
  gdl_new_word(gdl_get_tree());
  return 0;
}
#endif

Tree *
ag_read_atf(char *atf, size_t len)
{
  YY_BUFFER_STATE ag_buffer;
  if (!(ag_buffer = ag_scan_buffer(atf, len)))
    {
      fprintf(stderr, "ag_read_atf: ag_scan_buffer failed\n");
      return NULL;
    }
  yy_start = s_next = 0;
  atf_init(); /* this really is atf_init--initialize the ATF structure for the build */
  Tree *tp = agyacc();
  ag_wrapup_buffer();
  return tp;
}

