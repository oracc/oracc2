#include <oraccsys.h>
#include <gvl.h>

Hash *gvl_curr_script;
Hash *gvl_known_scripts;
Hash *gvl_tried_scripts;

void
gvl_set_script(const char *scripttype)
{
  if (gvl_known_scripts
      && (gvl_curr_script = hash_find(gvl_known_scripts, (uccp)scripttype)))
    return;
  if (!hash_find(gvl_tried_scripts, (uccp)scripttype))
    {
      if (!gvl_known_scripts)
	{
	  gvl_known_scripts = hash_create(5);
	  gvl_tried_scripts = hash_create(5);
	}
      gvl_curr_script = oiv_load(oiv_style_to_file(scripttype));
      if (gvl_curr_script)
	hash_add(gvl_known_scripts, (uccp)scripttype, gvl_curr_script);
      else
	hash_add(gvl_tried_scripts, (uccp)scripttype, "");
    }
}
