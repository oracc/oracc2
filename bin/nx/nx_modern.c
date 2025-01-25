#include <string.h>
#include "nx.h"

int trace_modern = 0;

static char *nx_decimal(nx_num *c);
static char *render_cmsq(double val);
static char *render_grams(double val);
static char *render_litres(double val);
static char *render_mm(double val);
static char *render_msq(double val);
static char *render_raw(double val, const char *meu);

char *
nx_modern(struct nx_num *mev, const char *meu)
{
  if (!strcmp(meu, "u"))
    return nx_decimal(mev);
  else
    {
      double val = ((double)mev->n) / mev->d;
      if (!strcmp(meu,"l"))
	return render_litres(val);
      else if (!strcmp(meu,"g"))
	return render_grams(val);
      else if (!strcmp(meu,"mm"))
	return render_mm(val);
      else if (!strcmp(meu,"m2"))
	return render_msq(val);
      else if (!strcmp(meu,"cm2"))
	return render_cmsq(val);
      else
	{
	  fprintf(stderr,"nx: modern unit %s not handled\n",meu);
	  return render_raw(val,meu);
	}
    }
}

static void
trimfinal0(char*buf)
{
  char *dot;
  if (buf && (dot = strchr(buf, '.')))
    {
      char *end = buf + strlen(buf);
      while (end > dot && '0' == end[-1])
	*--end = '\0';
      if ('.' == end[-1])
	end[-1] = '\0';
    }
}

#if 0
static void
trimfinal0(char*buf)
{
  if (buf)
    {
      char *z00 = strstr(buf,".00");
      if (z00)
	{
	  if (z00[3])
	    {
	      char *dest = z00;
	      z00 += 3;
	      while (*z00)
		*dest++ = *z00++;
	      *dest = '\0';
	    }
	  else
	    *z00 = '\0';
	}
    }
}
#endif

static char *
nx_decimal(nx_num *c)
{
  char buf[64];
  nx_simplify(c);
  if (c->d > 1)
    {
      if (c->n > c->d)
	sprintf(buf, "%lld %lld/%d", c->n/c->d, c->n % c->d, c->d);
      else
	sprintf(buf, "%d/%d", (int)c->n, c->d);
    }
  else
    sprintf(buf, "%lld",c->n);
  trimfinal0(buf);
  return (char*)pool_copy((ucp)buf, nxp->p);
}

static char *
render_litres(double val)
{
  static char buf[64];
  const char *meu = "l";

  if (val < 1)
    sprintf(buf,"%.03f",val);
  else
    sprintf(buf,"%.02f",val);
  trimfinal0(buf);
  strcat(buf,meu);
  return (char*)pool_copy((ucp)buf, nxp->p);
}

static char *
render_grams(double val)
{
  static char buf[64];
  const char *meu = NULL;
  if (val > 1000)
    {
      meu = "kg";
      val /= 1000;
      sprintf(buf,"%.3f",val);
    }
  else
    {
      meu = "g";
      sprintf(buf,"%.f",val);
    }
  trimfinal0(buf);
  strcat(buf,meu);
  return (char*)pool_copy((ucp)buf, nxp->p);
}

static char *
render_mm(double val)
{
  static char buf[64];
  const char *meu = NULL;
  if (val > 1000000)
    {
      meu = "km";
      val /= 1000000;
      sprintf(buf,"%.3f",val);
    }
  else if (val > 1000)
    {
      meu = "m";
      val /= 1000;
      sprintf(buf,"%.3f",val);
    }
  else if (val > 100)
    {
      meu = "cm";
      val /= 100;
      sprintf(buf,"%.2f",val);
    }
  else
    {
      meu = "mm";
      sprintf(buf,"%.f",val);
    }
  trimfinal0(buf);
  strcat(buf,meu);
  return (char*)pool_copy((ucp)buf, nxp->p);
}

static char *
render_msq(double val)
{
  static char buf[64];
  const char *meu = NULL;
  if (val > 10000)
    {
      meu = "ha";
      val /= 10000;
      sprintf(buf,"%.2f",val);
    }
  else
    {
      meu = "m2";
      sprintf(buf,"%.2f",val);
    }
  trimfinal0(buf);
  strcat(buf,meu);
  return (char*)pool_copy((ucp)buf, nxp->p);
}

static char *
render_cmsq(double val)
{
  static char buf[64];
  const char *meu = NULL;
  if (val > 1000000)
    {
      meu = "ha";
      val /= 100000000;
      sprintf(buf,"%.2f",val);
    }
  else
    {
      meu = "m2";
      val /= 10000;
      sprintf(buf,"%.2f",val);
    }
  trimfinal0(buf);
  strcat(buf,meu);
  return (char*)pool_copy((ucp)buf, nxp->p);
}

static char *
render_raw(double val,const char *meu)
{
  static char buf[64];
  sprintf(buf,"%.2f",val);
  trimfinal0(buf);
  strcat(buf,meu);
  return (char*)pool_copy((ucp)buf, nxp->p);
}
