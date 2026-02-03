#include <oraccsys.h>
#include <oracclocale.h>
#include <gvl.h>

/* Filter to rewrite cuneiform as HTML IVS codes */

#undef CHXDEBUG

const char *gvl_script_type = "gudea";

int
main(int argc, char **argv)
{
  unsigned char *lp;
  int l = 0;
  setlocale(LC_ALL, ORACC_LOCALE);
  gvl_set_script(gvl_script_type);
  while ((lp = loadoneline(stdin, NULL)))
    {
#ifdef CHXDEBUG
      fprintf(stderr, "line %d read with length %d\n", ++l, strlen(lp));
#endif
      /* gvl_ivs inserts IVSs according to the current script-type */
      int nivs = 0;
      unsigned char *out = gvl_ivs(lp, &nivs);
#ifdef CHXDEBUG
      if (out)
	fprintf(stderr, "gvl_ivs returned length %d\n", strlen(out));
      else
	fprintf(stderr, "gvl_ivs returned NULL\n");
#endif      
      if (nivs > 0)
	{
	  size_t needlen = mbstowcs(NULL,(ccp)out,0)+1;
	  wchar_t *w = calloc(needlen * sizeof(wchar_t), sizeof(wchar_t));
	  size_t convlen = mbstowcs(w, (ccp)out, needlen-1);
	  if (needlen - convlen != 1)
	    {
	      fprintf(stderr, "cunhtmlx: mbstowcs conversion failed.\n");
	      fputs((ccp)lp, stdout);
	    }
	  else
	    {
	      int i;
	      for (i = 0; w[i]; ++i)
		{
		  if (w[i] >= 0x12000 && w[i] < 0xe0100)
		    {
		      if (w[i+1] >= 0xe0100 && w[i+1] <= 0xe01ef)
			{
			  char key[10] = {
			    '\0', '\0', '\0', '\0', '\0',
			    '\0', '\0', '\0', '\0', '\0'
			  };
			  char w0[5] = { '\0', '\0', '\0', '\0', '\0' };
			  char w1[5] = { '\0', '\0', '\0', '\0', '\0' };
			  strcpy(w0,(ccp)utf8ify(w[i]));
			  strcpy(w1,(ccp)utf8ify(w[i+1]));
			  strcpy(key,w0);
			  strcpy(key+4,w1);
			  char *ident = hash_find(gvl_curr_ivs,(uccp)key);
			  if (ident)
			    {
			      if (*ident < 128)
				fprintf(stdout, "<span class=\"ivs-%s\"><span>%s</span></span>", ident, key);
			      else
				fputs(w0, stdout);
			    }
			  else
			    {
			      fprintf(stderr, "cunhtmlx.c: no ident found for IVS %x+%x\n", w[i], w[i+1]);
			      fputs(key, stdout);
			    }
			}
		      else
			fputs((ccp)utf8ify(w[i]), stdout);
		    }
		  else if (w[i] >= 128)
		    {
		      fputs((ccp)utf8ify(w[i]), stdout);
		    }
		  else
		    fputc((char)w[i], stdout);
		}
	    }
	}
      else
	fputs((ccp)lp, stdout);
      fputc('\n', stdout);
      free(out);
    }
}
void help(void){}
int opts(int optch, const char*optarg){return 0;}
