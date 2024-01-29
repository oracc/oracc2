#ifndef OSE_H_
#define OSE_H_

#include "ose_types.h"

enum oce_files { oce_byid , oce_summary , oce_tok , oce_xmd , oce_bad };

struct oce_config
{
  const char *tag;
  enum  oce_files f;
  const char *ext;
  const char **proxies;
};

extern void ose_ce_cfg(const char *project, const char *index,const char *ce_tag, 
		       const char *ce_ext, enum oce_files ce_type, 
		       const char **proxies);
void ose_wid2loc8(const char *id, const char *lang, struct location8 *l8p);
const char *ose_dir(const char *project, const char *xname);
const char *ose_file(const char *project, const char *xname, const char *fname);

#endif /*OSE_H_*/
