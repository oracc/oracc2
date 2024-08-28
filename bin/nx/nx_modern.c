#include <string.h>
#include "nx.h"

static char *nx_decimal(nx_num *c);
static char *render_litres(double val);
static char *render_grams(double val);
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
      else
	{
	  fprintf(stderr,"nx: modern unit %s not handled\n",meu);
	  return render_raw(val,meu);
	}
    }
}

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
  return strdup(buf);
}

static char *
render_litres(double val)
{
  static char buf[64];
  const char *meu = "l";

  if (val < 1)
    sprintf(buf,"%.03f%s",val,meu);
  else
    sprintf(buf,"%.02f%s",val,meu);
  return strdup(buf);
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
      sprintf(buf,"%.3f%s",val,meu);
    }
  else
    {
      meu = "g";
      sprintf(buf,"%.f%s",val,meu);
    }
  return strdup(buf);
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
      sprintf(buf,"%.3f%s",val,meu);
    }
  else if (val > 1000)
    {
      meu = "m";
      val /= 1000;
      sprintf(buf,"%.3f%s",val,meu);
    }
  else if (val > 100)
    {
      meu = "cm";
      val /= 100;
      sprintf(buf,"%.2f%s",val,meu);
    }
  else
    {
      meu = "mm";
      sprintf(buf,"%.f%s",val,meu);
    }
  return strdup(buf);
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
      sprintf(buf,"%.2f%s",val,meu);
    }
  else
    {
      meu = "m^2";
      sprintf(buf,"%.2f%s",val,meu);
    }
  return strdup(buf);
}

static char *
render_raw(double val,const char *meu)
{
  static char buf[64];
  sprintf(buf,"%.2f%s",val,meu);
  return strdup(buf);
}
