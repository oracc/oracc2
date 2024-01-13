#ifndef MSIG_H_
#define MSIG_H_

typedef struct msig
{
  const char *type;	/* The phrase type, e.g., psu, nsa */
  void *data; 		/* Type-specific data for the phrase */
  struct lsig **l; 	/* The lemma signatures for the phrase */
  struct wloc **w; 	/* The word-locations for the phrase */
} Msig;

#endif/*MSIG_H_*/
