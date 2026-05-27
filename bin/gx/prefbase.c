#include <oraccsys.h>
#include <roco.h>
#include <cbd.h>

static Hash *prefs;
static Roco *prefs_roco;
static char *prefs_fn;

static char *
prefs_file()
{
  const char *epsd_bases = "/pub/epsd2/preferred-bases.tab";
  char fn[strlen(oracc())+strlen(epsd_bases)+1];
  strcpy(fn, oracc());
  strcat(fn, epsd_bases);
  return prefs_fn = strdup(fn);
}

void
pref_init()
{
  prefs_roco = roco_load1(prefs_file());
  prefs = roco_hash(prefs_roco);
  cbd_set_prefs(prefs);
}

void
pref_term(void)
{
  roco_destroy(prefs_roco);
  hash_free(prefs, NULL);
  free(prefs_fn);
}
