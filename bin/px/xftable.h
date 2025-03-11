#ifndef XFT_H_
#define XFT_H_

typedef struct xft
{
  const char *name;
  const char **atts;
  struct xft *thead;
  List *td;
} xft;

extern xft*xft_table(const char **atts);
extern void xft_thead(xft *xp, const char **atts);
extern void xft_thtd(xft *xp, const char *name, const char **atts);
extern void xft_print(struct frag *frag);

#endif/*XFT_H_*/
