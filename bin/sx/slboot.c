#include <oraccsys.h>
#include <oid.h>
#include "sx.h"

static unsigned char *get_arg(unsigned char *cmd, int len);
static void show_atcmd(unsigned char *cmd, unsigned char *arg);
static void un_eol_sp(unsigned char *eol);

static Pool *p = NULL;
int verbose = 0;

static unsigned char *
get_arg(unsigned char *cmd, int len)
{
  unsigned char *arg = cmd + len;
  *arg++ = '\0';
  while (isspace(*arg))
    ++arg;
  un_eol_sp(arg);
  if (verbose)
    show_atcmd(cmd, arg);
  return pool_copy(arg, p);
}

static void
show_atcmd(unsigned char *cmd, unsigned char *arg)
{
  fprintf(stderr, "slboot: %s with arg '%s'\n", cmd, arg);
}

static void
show_hash(const unsigned char *k, void *v)
{
  fprintf(stdout, "%s\t%s\n", k, (char*)v);
}

static void
hash_qs(const unsigned char *k, void *v, void *h)
{
  unsigned char *oids = list_to_str((List *)v);
  hash_add(h, k, pool_copy(oids,p));
}

static void
un_eol_sp(unsigned char *eol)
{
  eol += strlen((ccp)eol);
  while (isspace(eol[-1]))
    *--eol = '\0';
}

Hash *
slboot(const char *asl)
{
  Hash *bsl = NULL;
  Hash *oids = NULL;
  Hash *q = NULL;
  const char *dom = "sl";
  int lnum = 0;
  FILE *fp = xfopen(asl, "r");
  if (fp)
    {
      bsl = hash_create(1024);
      q = hash_create(128);
      unsigned char *lp, *aka, *oid, *sign, *v;
      int pending_no_oid = 0, sign_warned = 0;
      while ((lp = loadoneline(fp, NULL)))
	{
	  ++lnum;
	  ++lp;
	  if (*lp && strchr("adfsv", *lp))
	    {
	      if ('v' == *lp && isspace(lp[1]))
		{
		  v = get_arg(lp, 1);
		  if (!oid && !sign_warned++)
		    fprintf(stderr, "%s:%d: `%s' has no OID\n", asl, lnum, sign);
		  else
		    {
		      int len = strlen((ccp)v);
		      if ('?' == v[len-1])
			v[--len] = '\0';
		      if (len > 3 && v[len-1] == 0x93 && v[len-2] == 0x82 && v[len-3] == 0xE2)
			{
			  /* SUB_X needs to be registered with its
			     SIGN as a ';qv' and also have the OID
			     added to a list of known signs for this
			     SUB_X */
			  char buf[len+strlen((ccp)sign)+strlen("();qv0")];
			  sprintf(buf, "%s(%s);qv", v, sign);
			  hash_add(bsl, pool_copy((uccp)buf,p), oid);
			  unsigned char vq[len+3];
			  sprintf((char*)vq, "%s;q", v);
			  List *qs = hash_find(q, vq);
			  if (!qs)
			    hash_add(q, pool_copy(vq,p), qs = list_create(LIST_SINGLE));
			  list_add(qs, oid);
			}
		      else
			hash_add(bsl, v, oid);
		    }
		}
	      else if ((('s' == *lp && !strncmp((ccp)lp, "sign", 4))
			|| ('s' == *lp && !strncmp((ccp)lp, "sign", 4)))
		       && isspace(lp[4]))
		{
		  sign_warned = 0;
		  sign = get_arg(lp, 4);
		  if (!oids)
		    oids = oid_domain_hash(NULL, NULL, dom);
		  if (!(oid = hash_find(oids, sign)))
		    pending_no_oid = lnum;
		  else
		    {
		      hash_add(bsl, oid, sign);
		      hash_add(bsl, sign, oid);
		    }
		}
	      else if ('a' == *lp && !strncmp((ccp)lp, "aka", 3) && isspace(lp[3]))
		{
		  if (!oid)
		    {
		      aka = get_arg(lp, 3);
		      if (!(oid = hash_find(oids, aka)))
			fprintf(stderr, "%s:%d: sign `%s'/ aka `%s' has no OID\n",
				asl, pending_no_oid, sign, aka);
		      else
			{
			  hash_add(bsl, oid, sign);
			  hash_add(bsl, sign, oid);
			  hash_add(bsl, aka, oid);
			}
		    }
		}
	      else if ('d' == *lp && !strncmp((ccp)lp, "domain", 6) && isspace(lp[6]))
		{
		  dom = (ccp)pool_copy(get_arg(lp, 6), p);
		}
	    }
	}
    }
  hash_exec_user_key_data(q, hash_qs, bsl);
  return bsl;
}

int
main(int argc, char *const *argv)
{
  p = pool_init();
  Hash *h = slboot("00lib/osl.asl");
  hash_exec2(h, show_hash);
}
