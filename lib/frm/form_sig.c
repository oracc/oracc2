#include <oraccsys.h>
#include <lng.h>
#include "xcl.h"
#include "ilem_form.h"
#include "form.h"
#include "xli.h"
#include "xcl.h"

static struct xcl_context *xxcp;

void
form_set_xcp(struct xcl_context *xcp)
{
  xxcp = xcp;
}

extern int bootstrap_mode, lem_extended;

int form_cbd_sux_norm = 0;

#ifndef strdup
extern char *strdup(const char *);
#endif


static int
cbd_cof_len(Form *fp)
{
  int len = strlen((ccp)fp->norm) + 3;
  Form *cof = (Form*)fp->cof_id;
  int i;
  for (i = 0; cof->parts[i]; ++i)
    len += 3 + strlen((ccp)cof->parts[i]->norm);
  return len;
}

static int
cbd_form_len(Form *fp)
{
#define flen(x) (fp->x?strlen((ccp)fp->x):0)
  int len = flen(form)+flen(norm)+flen(lang)+flen(base)+flen(cont)
    +flen(morph)+flen(morph2)+flen(stem) + strlen(" $ % / + # ## *0");

  /* This oversizes the return value because it counts cof-head->norm
     twice, but that is benign */
  if (fp->cof_id)
    len += cbd_cof_len(fp);
  
  return len;
}

static unsigned char *
cof_norm(Form *fp)
{
  char buf[cbd_cof_len(fp)];
  uintptr_t self = (uintptr_t)fp;
  Form *head = (Form*)fp->cof_id;
  if (self == head->cof_id)
    sprintf(buf, "$%s", head->norm);
  else
    sprintf(buf, "$(%s)", head->norm);
  int i;
  for (i = 0; head->parts[i]; ++i)
    {
      if (self == (uintptr_t)head->parts[i])
	sprintf(buf+strlen(buf), " $%s", head->parts[i]->norm);
      else
	sprintf(buf+strlen(buf), " $(%s)", head->parts[i]->norm);
    }
  return (ucp)strdup(buf);
}

unsigned char *
form_cbd(Form *fp, Pool *p, int with_lang)
{
#if 1
  char buf[cbd_form_len(fp)];
#else
  char buf[flen(form)+flen(norm)+flen(lang)+flen(base)+flen(cont)
	   +flen(morph)+flen(morph2)+flen(stem) + strlen(" $ % / + # ## *0")];
#endif
  
  sprintf((char*)buf,"%s",fp->form);

  if (fp->cof_id)
    {
      unsigned char *cofnorm = cof_norm(fp);
      sprintf((char*)(buf+strlen((char*)buf))," %s", cofnorm);
      free(cofnorm);
    }
  else if (fp->norm && (form_cbd_sux_norm || strncmp((ccp)fp->lang, "sux", 3)))
    sprintf((char*)(buf+strlen((char*)buf))," $%s",fp->norm);

  if (with_lang)
    sprintf((char*)(buf+strlen((char*)buf))," %%%s",fp->lang);

  if (fp->base)
    sprintf((char*)(buf+strlen((char*)buf))," /%s",fp->base);

  if (fp->cont && *fp->cont)
    sprintf((char*)(buf+strlen((char*)buf))," +%s",fp->cont);

  if (fp->morph)
    sprintf((char*)(buf+strlen((char*)buf))," #%s",fp->morph);

  if (fp->morph2)
    sprintf((char*)(buf+strlen((char*)buf))," ##%s",fp->morph2);

  if (fp->stem && *fp->stem)
    sprintf((char*)(buf+strlen((char*)buf))," *%s",fp->stem);

  return pool_copy((uccp)buf, p);
}

static unsigned char *
sig_one(Pool *p, struct ilem_form *ifp, Form *fp, int tail)
{
  unsigned char buf[1024];
  const unsigned char *form_to_use;
  int wild_form = 0;

  /* 20260127: remove struct xcl_context from form_sig functions to
     make them easier to use outside the lemmatizer; require setting
     xxcp using form_set_xcp(struct xcl_context*) to set up
     xli_ilem */
  if (ifp && lem_extended)
    xli_ilem(xxcp, ifp, fp);
  
  if (strstr((const char *)fp->lang, "-949"))
    wild_form = 1;
  
  if (!fp->project)
    /*fp->project = (const Uchar *)xcp->project*/
    fprintf(stderr, "sig_one: internal error: must set fp->project\n");
  
  if (BIT_ISSET(fp->flags, FORM_FLAGS_NORM_IS_CF)) 
    fp->cf = fp->norm;

  if (BIT_ISSET(fp->flags, FORM_FLAGS_NEW_BY_PROJ)
      || BIT_ISSET(fp->flags, FORM_FLAGS_NEW_BY_LANG))
    form_to_use = fp->form;
  else
    form_to_use = (Uchar*)(wild_form 
			   ? "*" 
			   : (fp->oform ? (char*)fp->oform : (char*)fp->form));

  if (!fp->cf && fp->pos && !strcmp((char *)fp->pos,"n"))
    {
      fp->cf = (const Uchar *)"n";
      fp->gw = fp->sense = (const Uchar *)"n";
      fp->epos = (const Uchar *)"n";
    }
  
  sprintf((char*)buf,"@%s%%%s:%s=%s[%s//%s]%s'%s",
	  (char*)(fp->project),
	  (char*)(fp->lang),
	  (char*)(tail ? (Uchar*)"" : form_to_use),
	  (char*)(fp->cf ? fp->cf : (Uchar*)"X"),
	  (char*)(fp->gw ? fp->gw : (Uchar*)"X"),
	  (char*)(fp->sense ? fp->sense : (Uchar*)"X"),
	  (char*)(fp->pos ? fp->pos : (Uchar*)"X"),
	  (char*)(fp->epos ? fp->epos : (Uchar*)"X"));

  if (!fp->base && fp->core && BIT_ISSET(fp->core->features,LF_BASE))
    {
      if (fp->pos && !strcmp((char *)fp->pos, "n"))
	{
	  fp->base = fp->form;
	  fp->morph = (const Uchar *)"~";
	  fp->cont = (const Uchar *)"";
	  fp->norm = (const Uchar *)"n";
	}
      else if (ifp && ifp->fcount)
	{
	  fp->base = ifp->finds[0]->f2.base;
	  fp->cont = ifp->finds[0]->f2.cont;
	  fp->morph = ifp->finds[0]->f2.morph;
	}
      else
	fp->base = fp->cont = fp->morph = (Uchar*)"X";
    }

  if (!fp->norm)
    fp->norm = (Uchar*)"X";
  
  sprintf((char*)(buf+strlen((char*)buf)),"$%s",fp->norm);

  if (fp->base)
    sprintf((char*)(buf+strlen((char*)buf)),"/%s",fp->base);

  if (fp->cont && *fp->cont)
    sprintf((char*)(buf+strlen((char*)buf)),"+%s",fp->cont);

  if (fp->morph)
    sprintf((char*)(buf+strlen((char*)buf)),"#%s",fp->morph);

  if (fp->morph2)
    sprintf((char*)(buf+strlen((char*)buf)),"##%s",fp->morph2);

  if (fp->stem)
    sprintf((char*)(buf+strlen((char*)buf)),"*%s",fp->stem);

  if (fp->rws)
    sprintf((char*)(buf+strlen((char*)buf)),"@%s",fp->rws);

  return (unsigned char *)strdup((char*)buf);
}

unsigned char *
form_sig_sub(Pool *p, Form *fp)
{
  unsigned char *ret = NULL;
  if (fp)
    {
      struct ilem_form *ifp = fp->owner;

      if (bit_get(fp->flags, FORM_FLAGS_IS_PSU))
	ret = form_psu_sig(p, fp);
      else if (fp->parts) /* COF */
	{
	  unsigned char *tmp = NULL;
	  tmp = sig_one(p, ifp, fp, 0);
	  if (tmp) 
	    {
	      List *parts = list_create(LIST_SINGLE);
	      int i;
	      list_add(parts, tmp);
	      for (i = 0; fp->parts[i]; ++i)
		{
		  fp->parts[i]->tail_sig = tmp = sig_one(p, ifp, fp->parts[i], 1);
		  if (tmp)
		    list_add(parts, tmp);
		  else
		    return NULL;
		}
	      ret = list_to_str2(parts, "&&");
	    }
	}
      else
	ret = sig_one(p, ifp, fp, 0);
    }

  return ret;
}

unsigned char *
form_sig(Pool *p, Form *fp)
{
  unsigned char *ret = NULL;
  unsigned char *res = form_sig_sub(p, fp);
  if (res)
    {
      ret = pool_copy(res, p);
      free(res);
    }
  return ret;
}

#if 0
static void
append_sig_sans_form(unsigned char *buf,unsigned char *sig)
{
  unsigned char *dst = buf+strlen((char*)buf);
  while (*sig && ':' != *sig)
    *dst++ = *sig++;
  *dst++ = *sig++;
  while (*sig && '=' != *sig)
    ++sig;
  while (*sig)
    *dst++ = *sig++;
  *dst = '\0';
}
#endif

static char *
tabless(const unsigned char *s)
{
  static char buf[1024], *tab;
  strcpy(buf,(char*)s);
  tab = strchr(buf,'\t');
  if (tab)
    *tab = '\0';
  return buf;
}

unsigned char *
form_psu_sig(Pool *p, Form *fp)
{
  unsigned char psu_buf[4096];

  sprintf((char*)psu_buf, "%s[%s//%s]%s'%s", fp->cf, fp->gw, fp->sense, fp->pos, fp->epos);
  fp->sig = pool_copy(psu_buf, p);
  
  if (fp->parts)
    { 
      int i;
      char *amp = NULL;
      unsigned char psu_form[1024], buf[2048];

      *psu_form = *buf = '\0';

      for (i = 0; fp->parts[i]; ++i)
        {
	  /* New PSU form generation */
	  if (fp->parts[i]->cof_id)
	    {
	      if (!fp->parts[i]->tail_sig)
		{
		  if (BIT_ISSET(fp->flags, FORM_FLAGS_NEW_BY_PROJ)
		      || BIT_ISSET(fp->flags, FORM_FLAGS_NEW_BY_LANG))
		    strcat((char*)psu_form, (char*)fp->parts[i]->form);
		  else if (fp->parts[i]->oform)
		    strcat((char*)psu_form, (char*)fp->parts[i]->oform);
		  else
		    strcat((char*)psu_form, (char*)fp->parts[i]->form);
		}
	    }
	  else
	    {
	      if (BIT_ISSET(fp->flags, FORM_FLAGS_NEW_BY_PROJ)
		       || BIT_ISSET(fp->flags, FORM_FLAGS_NEW_BY_LANG))
		strcat((char*)psu_form, (char*)fp->parts[i]->form);
	      else if (fp->parts[i]->oform)
		strcat((char*)psu_form, (char*)fp->parts[i]->oform);
	      else
		strcat((char*)psu_form, (char*)fp->parts[i]->form);
	    }

	  strcat((char*)psu_form, " ");

          if (i)
	    strcat((char*)buf, "++");

          if (fp->parts[i]->tail_sig)
            strcat((char*)buf, (char*)fp->parts[i]->tail_sig);
          else
            {
              if (!fp->parts[i]->sig)
                /* passing NULL as arg2 means do not run the extended lemmatization--this is ok
                 * because all the parts will have been subjected to that anyway by now
		 *
		 * New in o2 port: we don't use the ilem arg but use
		 * fp->owner instead; if things misbehave check that
		 * fp->parts[i]->owner is NULL.
		 */
                fp->parts[i]->sig = form_sig(p, fp->parts[i]);

              if ((amp = strstr((char*)fp->parts[i]->sig, "&&")))
                {
                  int len = strlen((char*)buf) + (amp - (char*)fp->parts[i]->sig);
                  strncat((char*)buf, (char*)fp->parts[i]->sig, amp - (char*)fp->parts[i]->sig);
                  buf[len] = '\0';
                }
              else
                strcat((char*)buf,tabless(fp->parts[i]->sig));
            }
        }
      sprintf((char*)psu_buf,"{%s= %s += %s}::%s",psu_form,fp->psu_ngram,fp->sig,buf);
    }
  else
    sprintf((char*)psu_buf,"{%s}::",fp->psu_ngram);

  return (ucp)strdup((ccp)psu_buf);
}
