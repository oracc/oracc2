#ifndef WHAT_H_
#define WHAT_H_

struct content_opts
{
  const char *chunk_id; /* extract the chunk with the given xml:id */
  const char *frag_id;  /* set the onload frag to the given xml:id */
  const char *hilite_id;/* add 'select' to @class on the element with given xml:id */
  const char *title;    /* title for HTML header */
  int echo;		/* echo input to output */
  int ga;		/* output google analytics header */
  int html;		/* input is in HTML: echo the header as well as the chunk */
  int sigs;		/* edit pop1sig to have project */
  int unwrap;		/* input is HTML page: return only the content of BODY */
  int wrap;		/* input is an HTML fragment: add a standard wrapper to it */
};

#include "../isp/isp.h"

typedef int (whatfunc)(Isp *ip);

struct whatfuncstab
{
  const char *name;
  whatfunc *func;
};

extern whatfunc what_html, what_score, what_sig, what_sources;

extern int what(Isp *ip);
extern int what_content(Isp *ip, struct content_opts *cop, const char *input);
extern int what_not(Isp *ip, const char *no);
struct content_opts *what_content_init(void);

#endif/*WHAT_H_*/
