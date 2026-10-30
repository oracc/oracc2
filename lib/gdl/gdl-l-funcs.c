int
gdl_lex_state(void)
{
  return YY_START;
}

void
gdl_lex_closers(void)
{
  if (CIN)
      mesg_vwarning(gdlfile,gdllineno,"missing '|' in compound (discovered at end of line)");
  else if (QIN)
    mesg_vwarning(gdlfile,gdllineno,"missing ')' in qv (discovered at end of line)");
  GIN=CIN=QIN=0;
  cvq_reset();
}

void
gdl_lex_init(const char *file, int line)
{
  CIN = GIN = QIN = 0;
  cvq_top = -1;
  gdl_l_lex_init(file, line);
}

void
gdl_decr_qin(void)
{
  if (--QIN < 0)
    { /* underflow condition */
      QIN = 0;
      BEGIN 0;
    }
  else
    {
      BEGIN cvq_pop();
    }
  if (gdlflextrace)
    fprintf(stderr, "gdl_decr_qin: QIN=%d; new state=%d\n", QIN, YY_START);
}

void
gdl_incr_qin(void)
{
  ++QIN;
  BEGIN cvq_push(Q);
  if (gdlflextrace)
    fprintf(stderr, "gdl_incr_qin: QIN=%d; new state=%d\n", QIN, YY_START);
}

int
cvq_push(int s)
{
  if (++cvq_top < CVQ_MAX)
    {
      cvq_stack[cvq_top] = YY_START;
      return s;
    }
  else
    fprintf(stderr, "%s:%d: cvq_push: cvq stack overflow (CVQ_MAX=%d)\n", gdlfile, gdllineno, CVQ_MAX);
  return 0;
}

int
cvq_pop(void)
{
  if (cvq_top >= 0)
    return cvq_stack[cvq_top--];
  else
    fprintf(stderr, "%s:%d: cvq_pop: cvq stack underflow\n", gdlfile, gdllineno);
   return 0;
}

void
cvq_reset(void)
{
  cvq_top = -1;
  BEGIN 0;
}
