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
