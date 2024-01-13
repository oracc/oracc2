#ifndef PSIG_H_
#define PSIG_H_

typedef struct psig
{
  const char *type;	/* The phrase type, e.g., psu, nsa */
  void *data; 		/* Type-specific data for the phrase */
  struct msig **m; 	/* The lemma signatures for the phrase */
  struct wloc **w; 	/* The word-locations for the phrase */
} Psig;

#endif/*PSIG_H_*/
