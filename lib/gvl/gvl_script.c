#include <oraccsys.h>
#include <gvl.h>

Hash *gvl_curr_ivs;
static Hash *gvl_known_scripts;
static Hash *gvl_tried_scripts;

struct oiv_data *
gvl_get_script(const char *ucode)
{
  return hash_find(gvl_curr_ivs, (uccp)ucode);
}

void
gvl_set_script(const char *scripttype)
{
  if (gvl_known_scripts
      && (gvl_curr_ivs = hash_find(gvl_known_scripts, (uccp)scripttype)))
    return;
  if (!hash_find(gvl_tried_scripts, (uccp)scripttype))
    {
      if (!gvl_known_scripts)
	{
	  gvl_known_scripts = hash_create(5);
	  gvl_tried_scripts = hash_create(5);
	}
      gvl_curr_ivs = oiv_load(oiv_style_to_file(scripttype));
      if (gvl_curr_ivs)
	hash_add(gvl_known_scripts, (uccp)scripttype, gvl_curr_ivs);
      else
	hash_add(gvl_tried_scripts, (uccp)scripttype, "");
    }
}
