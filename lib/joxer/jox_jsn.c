#include <joxer.h>
#include <json.h>

static FILE *f_jsn;

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
jox_jsn_ea(const char *pname, Rats *ratts)
{
  int i;
  jw_object_o();
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

  if (ratts)
    for (i = 0; ratts->atts[i]; i+=2)
      {
	jw_member(ratts->atts[i]);
	jw_string(ratts->atts[i+1]);
      }
}

void
jox_jsn_ee(const char *pname)
{
  jw_object_c();
  jw_object_c();
}

void
jox_jsn_et(const char *pname, Rats *ratts, const char *ch)
{
  int i;
  jw_strmem(pname,ch);
  if (ratts)
    for (i = 0; ratts->atts[i]; i+=2)
      {
	jw_member(ratts->atts[i]);
	jw_string(ratts->atts[i+1]);
      }
}

void
jox_jsn_ao(const char *name)
{
  jw_member(name);
  jw_array_o();
}

void
jox_jsn_ac(void)
{
  jw_array_c();
}
