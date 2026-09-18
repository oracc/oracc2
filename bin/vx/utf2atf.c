#include <oraccsys.h>
#include "vx.h"

static const char *wcs2atf_w(wchar_t w);

unsigned char *
utf2atf(const unsigned char *src)
{
  size_t mylen = 0;
  wchar_t *wc = utf2wcs(src,&mylen);
  if (wc)
    {
      size_t aplen = 0;
      char *apmem = NULL;
      FILE *ap = open_memstream(&apmem, &aplen);
      int i;
      for (i = 0; i < mylen; ++i)
	if (wc[i] < 128)
	  fputc(wc[i], ap);
	else
	  fputs(wcs2atf_w(wc[i]), ap);
      fclose(ap);
      return (ucp)apmem;
    }
  else
    return NULL;
}

const char *
wcs2atf_w(wchar_t w)
{
  const char *a;
  switch (w)
    {
    case 0x0161: /*U_sz*/
      a = "sz";
      break;
    case 0x0160: /*U_SZ */
      a = "SZ";
      break;
    case 0x2080: /*U_s0 */
      a = "0";
      break;
    case 0x2081: /*U_s1 */
      a = "1";
      break;
    case 0x2082: /*U_s2 */
      a = "2";
      break;
    case 0x2083: /*U_s3 */
      a = "3";
      break;
    case 0x2084: /*U_s4 */
      a = "4";
      break;
    case 0x2085: /*U_s5 */
      a = "5";
      break;
    case 0x2086: /*U_s6 */
      a = "6";
      break;
    case 0x2087: /*U_s7 */
      a = "7";
      break;
    case 0x2088: /*U_s8 */
      a = "8";
      break;
    case 0x2089: /*U_s9 */
      a = "9";
      break;
    case 0x2093: /*U_s_x*/
      a = "x";
      break;
    case 0x014b: /*U_eng */
      a = "g";
      break;
    case 0x014a: /*U_ENG */
      a = "G";
      break;
    case 0x1e2b: /*U_heth */
      a = "h";
      break;
    case 0x1e2a: /*U_HETH */
      a = "H";
      break;
    case 0x1e63: /*U_sadhe*/
      a = "s,";
      break;
    case 0x1e62: /*U_SADHE*/
      a = "S,";
      break;
    case 0x015b: /*U_sin*/
      a = "s'";
      break;
    case 0x015a: /*U_SIN*/
      a = "S'";
      break;
    case 0x1e6d: /*U_tet*/
      a = "t,";
      break;
    case 0x1e6c: /*U_TET*/
      a = "T,";
      break;
    case 0x02be: /*U_aleph*/
      a = "'";
      break;
    case 0x02bf: /*U_ayin*/
      a = "`";
      break;
    case 0x00d7: /*U_times*/
      a = "x";
      break;
    default:
      fprintf(stderr, "wcs2atf: unhandled character %ld\n", (long)w);
      wchar_t ww[2] = { w, (wchar_t)'\0' };
      a = (ccp)wcs2utf((const wchar_t*)&ww,1);
      break;
    }
  return a;
}

