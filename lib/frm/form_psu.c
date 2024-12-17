#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype128.h>
#include "mesg.h"
#include "memo.h"
#include "pool.h"
#include "bits.h"
#include "lng.h"
#include "form.h"

/* Simple parsing for PSUs
 */
int
form_parse_psu(const Uchar *file, size_t line, Uchar *lp, struct form *formp)
{
  char *psu_cdc = strstr((ccp)lp, "}::");
  if (psu_cdc)
    {
      char *psu_form, *psu_ngram, *psu_cfgw, **psu_parts;
      *psu_cdc = '\0';
      psu_cdc += 3;
      psu_cfgw = strstr((ccp)lp, "+=");
      if (psu_cfgw)
	{
	  char *s = psu_cfgw;
	  /* terminate the form/parts */
	  while (isspace(s[-1]))
	    --s;
	  *s = '\0';
	  psu_cfgw += 2;
	  /* move psu_cfgw to cf of cfgw */
	  while (isspace(*psu_cfgw))
	    ++psu_cfgw;
	  psu_form = (char*)lp+1;
	  while (isspace(*psu_form))
	    ++psu_form;
	  psu_ngram = (char*)strstr(psu_form, " = "); /* spaces obligatory here */
	  if (psu_ngram)
	    {
	      s = psu_ngram;
	      /* terminate the form */
	      while (isspace(s[-1]))
		--s;
	      *s = '\0';
	      psu_ngram += 3;
	      while (isspace(*psu_ngram))
		++psu_ngram;
	    }
	}
      int n;
      char *plusplus;
      for (n = 1, plusplus = psu_cdc; (plusplus = strstr(plusplus, "++")); ++n, ++plusplus)
	;
      psu_parts = calloc(n+1, sizeof(char*));
      for (n = 0, plusplus = psu_cdc; plusplus; ++n)
	{
	  if ('+' == *plusplus)
	    {
	      *plusplus = '\0';    
	      plusplus += 2;
	    }
	  psu_parts[n] = plusplus;
	  plusplus = strstr(plusplus, "++");
	}
      psu_parts[n] = NULL;

      /* Everything is marshalled, now fill in the Form */
      formp->parts = memo_new_array(formspmem, n);
      Form *fmem = memo_new_array(formsmem, n);
      int i;
      for (i = 0; i < n; ++i)
	{
	  formp->parts[i] = &fmem[i];
	  form_parse(file, line, (ucp)psu_parts[i], formp->parts[i], NULL);
	}
      form_parse(file, line, (ucp)psu_cfgw, formp, NULL);
      formp->psu_ngram = (ucp)psu_ngram;
      formp->form = (uccp)psu_form;
      formp->project = formp->parts[0]->project;
      formp->lang = formp->parts[0]->lang;
      formp->core = formp->parts[0]->core;
      bit_set(formp->flags, FORM_FLAGS_IS_PSU);
    }

#if 0
  char * tmp = strstr((char *)lp, " += ");
  if (tmp)
    {
      char *tmp2;
      *tmp = '\0';
      tmp2 = strstr((char *)lp, " = ");
      if (tmp2)
	{
	  /*psu_form = &lp[1];*/
	  *tmp2 = '\0';
	  lp = (Uchar *)(tmp2 + 3);
	  *tmp = ' ';
	}
    }
#endif
  
  return 0;
}
