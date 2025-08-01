#include <oraccsys.h>

static char *CGI_decode_url(const char *p);
const char *lang = NULL, *project = NULL, *map_proj = NULL;
int verbose = 1;

static void
print_hdr(void)
{
  puts("Content-type: text/html; charset=utf-8\nAccess-Control-Allow-Origin: *\n\n");
  fflush(stdout);
}

static void
try_map(char *sig,char *map_file,const char*projlang)
{
  char *s, buf[1024];
  FILE *map = NULL;

  map = fopen(map_file,"r");
  if (!map)
    {
      fprintf(stderr, "wsigx: map file %s not found\n", map_file);
      printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/></head><body><p>No such map file %s</p></body></html>",map_file);
      exit(1);
    }
  if (verbose)
    fprintf(stderr, "wsigx: looking for %s in %s\n", sig, map_file);
  while ((s = fgets(buf,1024,map)))
    {
      char*tab = strchr(s,'\t') /*, *doll */;
      *tab = '\0';
      if (!strcmp(s,sig))
	{
	  char html[1024];
	  ++tab;
	  tab[strlen(tab)-1] = '\0';
	  sprintf(html,"%s/www/%s/cbd/%s/%s.html",oracc(),map_proj,lang,tab);

	  if (!access(html,R_OK))
	    {
	      if (verbose)
		fprintf(stderr,"wsigx: %s: %s: found--/bin/cat %s\n", map_file, sig, html);
	      execlp("/bin/cat","cat",html,(char*)NULL);
	    }
	  else
	    {
	      if (verbose)
		fprintf(stderr,"wsigx: %s: %s: not found, returning error page\n", map_file, sig);
	      printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/></head><body><p>Signature %s points to non-existent HTML file %s</p><p>map file=%s; projlang=%s</p></body></html>",sig,html,map_file,lang);
	      exit(1);
	    }
	}
    }
  fprintf(stderr,"wsigx: %s: no sig %s\n", map_file, sig);
}

static int
hex(int digit) {
    switch(digit) {

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return digit - '0';

    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        return 10 + digit - 'A';

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        return 10 + digit - 'a';

    default:
        return -1;
    }
}

char *
CGI_decode_url(const char *p) {
    char *out;
    int i, k, L, R;

    if (p == 0) {
        return 0;
    }
    out = (char *)malloc(strlen(p) + 1);
    for (i = k = 0; p[i] != 0; i++) {
        switch(p[i]) {

        case '+':
            out[k++] = ' ';
            continue;

        case '%':
            if ((L = hex(p[i + 1])) >= 0 &&
                (R = hex(p[i + 2])) >= 0)
            {
                out[k++] = (L << 4) + R;
                i += 2;
                continue;
            }
            break;
        }
        out[k++] = p[i];
    }
    out[k] = 0;
    return out;
}

static const char *
find_pos(const char *sig)
{
  static char buf[5];
  const char *p = NULL;
  if ((p = strstr(sig, "}::")))
    {
    }
  else
    {
      const char *epos;
      p = strstr(sig, "//");
      while (*p && ']' != *++p)
	;
      if (!*p)
	return NULL;
      ++p;
      if ((epos = strchr(p, '\'')))
	{
	  if (epos - p < 5)
	    {
	      strncpy(buf,p,epos-p);
	      buf[epos-p] = '\0';
	    }
	  else
	    *buf = 'X';
	}
      else
	{
	  char *bufp = buf;
	  while (isalpha(*p) && bufp - buf < 5)
	    *bufp++ = *p++;
	  *bufp = '\0';
	}
    }
  return buf;
}

static char *
find_lang(const char *sig, char **endp)
{
  static char buf[128];
  const char *p = strchr(sig, '%');
  if (p)
    {
      const char *c = strchr(++p, ':');
      if (c - p < 128)
	{
	  *endp = (char*)c;
	  strncpy(buf, p, c-p);
	  buf[c-p] = '\0';
	}
      else
	fprintf(stderr, "wsigx: lang too long (%s)\n", p);
    }
  else
    *buf = '\0';
  return buf;
}

static char *
find_proj(const char *sig)
{
  static char buf[128];
  const char *at = strchr(sig, '@');
  if (at)
    {
      const char *p = strchr(++at, '%');
      if (p - at < 128)
	{
	  strncpy(buf, at, p-at);
	  buf[p-at] = '\0';
	}
      else
	fprintf(stderr, "wsigx: project too long (%s)\n", p);
    }
  else
    *buf = '\0';
  return buf;
}

static char *
make_map(const char *sig, const char *argproj)
{
  const char *pos = find_pos(sig);
  const char *proj = argproj;
  char *mapbuf = NULL;

  if (!proj)
    proj = find_proj(sig);
  
  if (strlen(pos) == 2 && pos[1] == 'N')
    lang = "qpn";

  map_proj = proj;
  mapbuf = malloc(20+(2*strlen(lang))+strlen(proj)+strlen(oracc_builds()));
  sprintf(mapbuf, "%s/www/%s/cbd/%s/%s.map", oracc_builds(), proj, lang, lang);
  return mapbuf;
}

int
main(int argc, char **argv)
{
  char *map = NULL;
  char *sig = argv[2], *usig, *msig, *trysig, *postproj, *script = NULL, *hyph = NULL;
  wchar_t *wsig = NULL;
  size_t nwchar = 0, i;
  char *locale = NULL;
  char *langend = NULL;

  project = argv[1];
  
  print_hdr();
  if ((locale = setlocale(LC_ALL, ORACC_LOCALE)))
    fprintf(stderr, "locale=%s\n", locale);
  else
    fprintf(stderr, "setlocale(LC_ALL, \"en_GB\") failed\n");

  usig = CGI_decode_url(sig);

  /* fprintf(stderr,"sig: %s\nusig: %s\n", sig, usig); */

  wsig = malloc((strlen(usig)+1) * sizeof(wchar_t));
  nwchar = mbstowcs(wsig, usig, strlen(usig)+1);

  if (*wsig != 0x2623)
    {
      fprintf(stderr,"wsigx: signature with no biohazard symbol\n");
      printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/></head><body><p>No HTML file found for signature %s</p></body></html>",
	     argv[1]);
      fflush(stdout);
      return 0;
    }
  else
    {
      while (*wsig == 0x2623)
	{
	  ++wsig; /* skip biohazard symbol */
	  --nwchar;
	}
    }
  for (i=0; i < nwchar; ++i)
    {
      switch(wsig[i])
	{
	case 0xb1:
	  wsig[i] = L'+';
	  break;
	case 0xb4:
	  wsig[i] = L'\'';
	  break;
	case 0x20e5:
	  wsig[i] = L'\\';
	  break;
	}
    }
  msig = malloc(2*strlen(usig)+1);
  wcstombs(msig, wsig, 2*strlen(usig));
  
  trysig = malloc(strlen(project) + strlen(msig) + 1);
  postproj = msig;
  while (*postproj && '%' != *postproj)
    ++postproj;
  sprintf(trysig, "@%s%s", project, postproj);

  lang = find_lang(trysig, &langend);

  if ((script = strrchr(lang, '-')))
    if (isdigit(script[1]))
      {
	char *langp = NULL;
	*script = '\0';
	langp = langend - 4; /* langp now points at - of script extension */
	while (*langend)
	  *langp++ = *langend++;
	*langp = '\0';
      }  

  if (!strcmp(project, "neo") && ((hyph = strchr(lang, '-'))))
    {
      char *langp = NULL;
      *hyph = '\0';
      langp = langend - 9; /* langp now points at first - of -x-stdbab vel sim */
      while (*langend)
	*langp++ = *langend++;
      *langp = '\0';
    }
  
  map = make_map(trysig, project);
  try_map(trysig, map, lang);
  map = make_map(msig, NULL);
  try_map(msig, map, lang);

  printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/></head><body><p>No HTML file found for signature %s in project %s</p></body></html>",
	 msig, project);
  fflush(stdout);
  return 1;
}

int major_version = 1, minor_version = 0;
const char *usage_string = "wsigx [WORDSIG]";
const char *prog = "wsigx";
void help(void) { }
