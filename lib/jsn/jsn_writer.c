#include <json.h>
#include <stck.h>

static FILE *jwfp;
static Stck *jstack;
static enum jstate jstate;

int jw_format = 1;

enum jstate
jw_state(void)
{
  return jstate;
}

void
jw_init(FILE *fp)
{
  jwfp = fp;
  jstack = stck_init(32);
  jstate = jstart;
}

void
jw_term(void)
{
  stck_term(jstack);
  jstack = NULL;
}

void
jw_value(void)
{
  if (jelement == jstate)
    fputc(',', jwfp);
  else
    jstate = jelement;
}

void
jw_array_o(void)
{
  if (jelement == jstate)
    fputc(',', jwfp);
  else
    stck_push(jstack, jelement);
  jstate = jarray;
  fputc('[', jwfp);
}

void
jw_array_c(void)
{
  jstate = stck_pop(jstack);
  fputc(']', jwfp);
}

void
jw_object_o(void)
{
  if (jelement == jstate)
    fputc(',', jwfp);
  else
    stck_push(jstack, jelement);
  jstate = jobject;
  fputc('{', jwfp);
  if (jw_format)
    fputc('\n',jwfp);
}

void
jw_object_c(void)
{
  jstate = stck_pop(jstack);
  if (jw_format)
    fputc('\n',jwfp);
  fputc('}', jwfp);
}

void
jw_member(const char *name)
{
  if (jelement == jstate)
    fputc(',', jwfp);
  else
    stck_push(jstack, jelement);
  fprintf(jwfp, "\"%s\" : ", name);
  jstate = jmember;
}

void
jw_string(const char *s)
{
  jw_value();
  fprintf(jwfp, "\"%s\"", jsonify((const unsigned char *)s));
}

void
jw_strmem(const char *m, const char *v)
{
  jw_member(m);
  jw_string(v);
}

void
jw_number(int i)
{
  jw_value();
  fprintf(jwfp, "%d", i);
}

void
jw_true(void)
{
  jw_value();
  fputs("true", jwfp);
}

void
jw_false(void)
{
  jw_value();
  fputs("false", jwfp);
}

void
jw_null(void)
{
  jw_value();
  fputs("null", jwfp);
}
