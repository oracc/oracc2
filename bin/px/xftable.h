#ifndef XFT_H_
#define XFT_H_

typedef struct xft
{
  const char **atts;
  const char **thead_atts;
  List *tr;
} xft;

struct xft_tr
{
  const char **atts;
  List *thtd;
};
  
struct xft_thtd
{
  const char *name;
  const char **atts;  
};

extern xft*xft_table(const char **atts);
extern void xft_thead(xft *xp, const char **atts);
extern void xft_tr(xft *xp, const char **atts);
extern void xft_thtd(xft *xp, const char *name, const char **atts);
extern void xft_print(struct frag *frag);

#endif/*XFT_H_*/
