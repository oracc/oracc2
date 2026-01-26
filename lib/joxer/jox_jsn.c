#include <joxer.h>
#include <stck.h>
#include <json.h>

static FILE *f_jsn;
static Stck *ao_stack = NULL;

void
jox_jsn_output(FILE *fp)
{
  f_jsn = fp;  
}

void
jox_jsn_ch(const char *ch)
{
  jw_member("_t_");
  jw_string(ch);
}

void
jox_jsn_ea(const char *pname, Rats *rats)
{
  int i;
  jw_object_o();
  if (pname)
    {
      jw_member(pname);
      jw_object_o();
    }
  if (jsn_xmlns_atts)
    {
      int i;
      for (i = 0; jsn_xmlns_atts[i].ns; ++i)
	{
	  jw_member(jsn_xmlns_atts[i].ns);
	  jw_string(jsn_xmlns_atts[i].uri);
	}
      jsn_xmlns_atts = NULL;
    }

  if (rats)
    for (i = 0; rats->atts[i]; i+=2)
      {
	jw_member(rats->atts[i]);
	jw_string(rats->atts[i+1]);
      }
}

void
jox_jsn_ee(const char *pname)
{
  if (pname)
    jw_object_c();
  jw_object_c();
}

void
jox_jsn_eaa(const char *pname, Rats *rats)
{
  int i;
  jw_member(pname);
  jw_object_o();
  if (jsn_xmlns_atts)
    {
      int i;
      for (i = 0; jsn_xmlns_atts[i].ns; ++i)
	{
	  jw_member(jsn_xmlns_atts[i].ns);
	  jw_string(jsn_xmlns_atts[i].uri);
	}
      jsn_xmlns_atts = NULL;
    }

  if (rats)
    for (i = 0; rats->atts[i]; i+=2)
      {
	jw_member(rats->atts[i]);
	jw_string(rats->atts[i+1]);
      }
}

void
jox_jsn_eea(const char *pname)
{
  jw_object_c();
}

void
jox_jsn_eaaa(const char *pname, Rats *rats)
{
  int i;
  jw_member(pname);
  jw_array_o();
  if (jsn_xmlns_atts)
    {
      int i;
      for (i = 0; jsn_xmlns_atts[i].ns; ++i)
	{
	  jw_member(jsn_xmlns_atts[i].ns);
	  jw_string(jsn_xmlns_atts[i].uri);
	}
      jsn_xmlns_atts = NULL;
    }

  if (rats)
    for (i = 0; rats->atts[i]; i+=2)
      {
	jw_member(rats->atts[i]);
	jw_string(rats->atts[i+1]);
      }
}

void
jox_jsn_eeaa(const char *pname)
{
  jw_array_c();
}

void
jox_jsn_eto(const char *pname, Rats *rats, const char *ch)
{
  jw_object_o();
  jox_jsn_et(pname, rats, ch);
  jw_object_c();
}

void
jox_jsn_et(const char *pname, Rats *rats, const char *ch)
{
  int i;
  jw_strmem(pname,ch);
  if (rats)
    for (i = 0; rats->atts[i]; i+=2)
      {
	jw_member(rats->atts[i]);
	jw_string(rats->atts[i+1]);
      }
}

/* We need to protect against two arrays being opened in a row, so
 * jox_jsn_ao/ac maintain an autocreated and autodestroyed stack which
 * records whether an array has been wrapped in an object.
 */
void
jox_jsn_ao(const char *name)
{
  if (jw_state() == jarray)
    {
      if (ao_stack == NULL)
	ao_stack = stck_init(32);
      stck_push(ao_stack, 1);
      jw_object_o();
    }
  else if (ao_stack)
    stck_push(ao_stack, 0);
  jw_member(name);
  jw_array_o();
}

void
jox_jsn_ac(void)
{
  jw_array_c();  
  if (ao_stack)
    {
      uintptr_t need_o_c = stck_pop(ao_stack);
      if (need_o_c == 1)
	jw_object_c();
      if (!stck_len(ao_stack))
	{
	  stck_term(ao_stack);
	  ao_stack = NULL;
	}
    }
}
