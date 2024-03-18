#include <oraccsys.h>
#include "charsets.h"
#define append(s) strcpy(buf+buf_used,s); buf_used+=strlen(s)
#define wcharcat(s) buf[buf_used++] = s
int chartrie_suppress_errors = 0;
static int
akk_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x000c9:
        case 0x00042:
        case 0x000e1:
        case 0x0006c:
        case 0x000f9:
        case 0x02087:
        case 0x02089:
        case 0x00047:
        case 0x02088:
        case 0x0004a:
        case 0x00067:
        case 0x00030:
        case 0x00066:
        case 0x0004b:
        case 0x0014b:
        case 0x00071:
        case 0x00034:
        case 0x0014a:
        case 0x00039:
        case 0x000d9:
        case 0x00049:
        case 0x00079:
        case 0x00053:
        case 0x02086:
        case 0x02085:
        case 0x01e2b:
        case 0x02084:
        case 0x00061:
        case 0x01e6d:
        case 0x00055:
        case 0x00070:
        case 0x0006b:
        case 0x000e0:
        case 0x000cc:
        case 0x00032:
        case 0x000e9:
        case 0x00160:
        case 0x00041:
        case 0x00044:
        case 0x00058:
        case 0x01e2a:
        case 0x00077:
        case 0x00054:
        case 0x00033:
        case 0x00051:
        case 0x00045:
        case 0x000ec:
        case 0x00069:
        case 0x0006f:
        case 0x00038:
        case 0x000da:
        case 0x00062:
        case 0x00161:
        case 0x00031:
        case 0x01e62:
        case 0x00068:
        case 0x00057:
        case 0x00000:
        case 0x0005a:
        case 0x00052:
        case 0x000cd:
        case 0x02093:
        case 0x00059:
        case 0x00078:
        case 0x00072:
        case 0x000c0:
        case 0x00037:
        case 0x002be:
        case 0x000ed:
        case 0x02082:
        case 0x00035:
        case 0x0006d:
        case 0x0004e:
        case 0x000e8:
        case 0x00065:
        case 0x0015a:
        case 0x00075:
        case 0x02080:
        case 0x000c8:
        case 0x00074:
        case 0x01e6c:
        case 0x0015b:
        case 0x00036:
        case 0x0007a:
        case 0x00064:
        case 0x00050:
        case 0x00048:
        case 0x00073:
        case 0x02083:
        case 0x0004d:
        case 0x0006e:
        case 0x000fa:
        case 0x0004c:
        case 0x01e63:
        case 0x00043:
        case 0x02081:
        case 0x000c1:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%akk/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
akk_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x000c9:
          append("É");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x000c1:
          append("Á");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys akk_g_keys[] =
  {
    { "s'"	, "\xc5\x9b" },
    { "j"	, "\xc5\x8b" },
    { "J"	, "\xc5\x8a" },
    { "'"	, "\xca\xbe" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "S'"	, "\xc5\x9a" },
    { "t,"	, "\xe1\xb9\xad" },
    { "sz"	, "\xc5\xa1" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "T,"	, "\xe1\xb9\xac" },
    { "SZ"	, "\xc5\xa0" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "G"	, "G" },
    { "g"	, "g" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "K"	, "K" },
    { "q"	, "q" },
    { "4"	, "4" },
    { "9"	, "9" },
    { "I"	, "I" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "a"	, "a" },
    { "U"	, "U" },
    { "p"	, "p" },
    { "k"	, "k" },
    { "2"	, "2" },
    { "A"	, "A" },
    { "D"	, "D" },
    { "X"	, "X" },
    { "w"	, "w" },
    { "T"	, "T" },
    { "3"	, "3" },
    { "Q"	, "Q" },
    { "E"	, "E" },
    { "i"	, "i" },
    { "o"	, "o" },
    { "8"	, "8" },
    { "b"	, "b" },
    { "1"	, "1" },
    { "h"	, "h" },
    { "W"	, "W" },
    { "Z"	, "Z" },
    { "R"	, "R" },
    { "Y"	, "Y" },
    { "x"	, "x" },
    { "r"	, "r" },
    { "7"	, "7" },
    { "5"	, "5" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "e"	, "e" },
    { "u"	, "u" },
    { "t"	, "t" },
    { "6"	, "6" },
    { "z"	, "z" },
    { "d"	, "d" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "n"	, "n" },
    { "L"	, "L" },
    { "C"	, "C" },
    { NULL, NULL },
  };
struct charset akk_g =
  {
    "akk", c_akk , "g", m_graphemic,
    akk_g_keys,
    0,
    akk_g_val,
    akk_g_asc,
    0,
  };

static int
akk_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00027:
        case 0x0006d:
        case 0x0004e:
        case 0x00100:
        case 0x0015a:
        case 0x00065:
        case 0x0012a:
        case 0x00052:
        case 0x0005a:
        case 0x0016b:
        case 0x00072:
        case 0x00078:
        case 0x00059:
        case 0x002be:
        case 0x0002d:
        case 0x00068:
        case 0x00057:
        case 0x01e62:
        case 0x000ce:
        case 0x00000:
        case 0x00161:
        case 0x000ea:
        case 0x00062:
        case 0x000fb:
        case 0x000db:
        case 0x0012b:
        case 0x00112:
        case 0x0006e:
        case 0x0004c:
        case 0x01e63:
        case 0x00043:
        case 0x0016a:
        case 0x0004f:
        case 0x00064:
        case 0x00050:
        case 0x00048:
        case 0x00073:
        case 0x00101:
        case 0x0004d:
        case 0x000ee:
        case 0x00075:
        case 0x0003e:
        case 0x000ca:
        case 0x00063:
        case 0x01e6c:
        case 0x00074:
        case 0x0015b:
        case 0x0007a:
        case 0x00079:
        case 0x00053:
        case 0x0014a:
        case 0x000e2:
        case 0x00049:
        case 0x00047:
        case 0x00067:
        case 0x0004b:
        case 0x0014b:
        case 0x0003f:
        case 0x00071:
        case 0x00113:
        case 0x000c2:
        case 0x00042:
        case 0x0006c:
        case 0x00060:
        case 0x00051:
        case 0x0003c:
        case 0x00045:
        case 0x00069:
        case 0x0006f:
        case 0x00058:
        case 0x01e2a:
        case 0x00077:
        case 0x00054:
        case 0x0006b:
        case 0x00041:
        case 0x00160:
        case 0x00044:
        case 0x01e2b:
        case 0x01e6d:
        case 0x00061:
        case 0x00055:
        case 0x00070:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%akk/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
akk_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00027:
          append("'");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00100:
          append("=A");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0012a:
          append("=I");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x0016b:
          append("=u");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x000ce:
          append("^I");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x000ea:
          append("^e");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x000fb:
          append("^u");
          break;
        case 0x000db:
          append("^U");
          break;
        case 0x0012b:
          append("=i");
          break;
        case 0x00112:
          append("=E");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0016a:
          append("=U");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00101:
          append("=a");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x000ee:
          append("^i");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0003e:
          append(">");
          break;
        case 0x000ca:
          append("^E");
          break;
        case 0x00063:
          append("c");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x000e2:
          append("^a");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x0003f:
          append("?");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00113:
          append("=e");
          break;
        case 0x000c2:
          append("^A");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00060:
          append("`");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0003c:
          append("<");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys akk_n_keys[] =
  {
    { "^u"	, "\xc3\xbb" },
    { "S'"	, "\xc5\x9a" },
    { "^I"	, "\xc3\x8e" },
    { "T,"	, "\xe1\xb9\xac" },
    { "=i"	, "\xc4\xab" },
    { "^U"	, "\xc3\x9b" },
    { "j"	, "\xc5\x8b" },
    { "=A"	, "\xc4\x80" },
    { "sz"	, "\xc5\xa1" },
    { "^A"	, "\xc3\x82" },
    { "=I"	, "\xc4\xaa" },
    { "J"	, "\xc5\x8a" },
    { "=U"	, "\xc5\xaa" },
    { "^i"	, "\xc3\xae" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "=u"	, "\xc5\xab" },
    { "t,"	, "\xe1\xb9\xad" },
    { "^a"	, "\xc3\xa2" },
    { "=E"	, "\xc4\x92" },
    { "^e"	, "\xc3\xaa" },
    { "s'"	, "\xc5\x9b" },
    { "SZ"	, "\xc5\xa0" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "'"	, "\xca\xbe" },
    { "^E"	, "\xc3\x8a" },
    { "=e"	, "\xc4\x93" },
    { "=a"	, "\xc4\x81" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "e"	, "e" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "Y"	, "Y" },
    { "-"	, "-" },
    { "h"	, "h" },
    { "W"	, "W" },
    { "b"	, "b" },
    { "n"	, "n" },
    { "L"	, "L" },
    { "C"	, "C" },
    { "O"	, "O" },
    { "d"	, "d" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "u"	, "u" },
    { ">"	, ">" },
    { "c"	, "c" },
    { "t"	, "t" },
    { "z"	, "z" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "g"	, "g" },
    { "K"	, "K" },
    { "?"	, "?" },
    { "q"	, "q" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "`"	, "`" },
    { "Q"	, "Q" },
    { "<"	, "<" },
    { "E"	, "E" },
    { "i"	, "i" },
    { "o"	, "o" },
    { "X"	, "X" },
    { "w"	, "w" },
    { "T"	, "T" },
    { "k"	, "k" },
    { "A"	, "A" },
    { "D"	, "D" },
    { "a"	, "a" },
    { "U"	, "U" },
    { "p"	, "p" },
    { NULL, NULL },
  };
struct charset akk_n =
  {
    "akk", c_akk , "n", m_normalized,
    akk_n_keys,
    0,
    akk_n_val,
    akk_n_asc,
    0,
  };

static int
arc_a_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x0002a:
        case 0x00072:
        case 0x00037:
        case 0x002be:
        case 0x00035:
        case 0x00027:
        case 0x0006d:
        case 0x0002b:
        case 0x0005b:
        case 0x00161:
        case 0x00026:
        case 0x00062:
        case 0x00031:
        case 0x0002d:
        case 0x00068:
        case 0x00021:
        case 0x0005f:
        case 0x00000:
        case 0x0006e:
        case 0x01e63:
        case 0x0002e:
        case 0x0003e:
        case 0x01e25:
        case 0x00036:
        case 0x00074:
        case 0x0015b:
        case 0x0007a:
        case 0x00064:
        case 0x00073:
        case 0x00034:
        case 0x00039:
        case 0x00079:
        case 0x0006c:
        case 0x00067:
        case 0x00030:
        case 0x00071:
        case 0x0003f:
        case 0x00077:
        case 0x002bf:
        case 0x00033:
        case 0x00023:
        case 0x0003c:
        case 0x00038:
        case 0x0005d:
        case 0x01e6d:
        case 0x00070:
        case 0x0006b:
        case 0x00032:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%arc/a",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
arc_a_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x0002a:
          append("*");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x002be:
          append(")");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0002b:
          append("+");
          break;
        case 0x0005b:
          append("[");
          break;
        case 0x00161:
          append("$");
          break;
        case 0x00026:
          append("&");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00021:
          append("!");
          break;
        case 0x0005f:
          append("_");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x01e63:
          append("c");
          break;
        case 0x0002e:
          append(".");
          break;
        case 0x0003e:
          append(">");
          break;
        case 0x01e25:
          append("x");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x0015b:
          append("&");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x0003f:
          append("?");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x002bf:
          append("(");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00023:
          append("#");
          break;
        case 0x0003c:
          append("<");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0005d:
          append("]");
          break;
        case 0x01e6d:
          append("T");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00032:
          append("2");
          break;
        default:
          break;
	}
    }
  return buf;
}

static wchar_t *
arc_u2u(wchar_t *w, ssize_t len)
{
  static wchar_t *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
          free(buf);
          buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced <= len)
    {
      while  (buf_alloced <= len)
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced * sizeof(wchar_t));
    }

  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x0015b:
          wcharcat(0xFB2B);
          break;
        case 0x00074:
          wcharcat(0x05E9);
          break;
        case 0x01e6d:
          wcharcat(0x05D8);
          break;
        case 0x00070:
          wcharcat(0x05E4);
          break;
        case 0x0007a:
          wcharcat(0x05D6);
          break;
        case 0x00062:
          wcharcat(0x05D1);
          break;
        case 0x01e25:
          wcharcat(0x05D7);
          break;
        case 0x00161:
          wcharcat(0xFB2A);
          break;
        case 0x0006c:
          wcharcat(0x05DC);
          break;
        case 0x00071:
          wcharcat(0x05E7);
          break;
        case 0x00073:
          wcharcat(0x05E1);
          break;
        case 0x00068:
          wcharcat(0x05D4);
          break;
        case 0x0006b:
          wcharcat(0x05DB);
          break;
        case 0x00067:
          wcharcat(0x05D2);
          break;
        case 0x00064:
          wcharcat(0x05D3);
          break;
        case 0x00077:
          wcharcat(0x05D5);
          break;
        case 0x01e63:
          wcharcat(0x05E6);
          break;
        case 0x002be:
          wcharcat(0x05D0);
          break;
        case 0x002bf:
          wcharcat(0x05E2);
          break;
        case 0x00072:
          wcharcat(0x05E8);
          break;
        case 0x0006e:
          wcharcat(0x05E0);
          break;
        case 0x0006d:
          wcharcat(0x05DE);
          break;
        case 0x00079:
          wcharcat(0x05D9);
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys arc_a_keys[] =
  {
    { "h"	, "\x68" },
    { "k"	, "\x6b" },
    { "g"	, "\x67" },
    { "d"	, "\x64" },
    { "("	, "\xca\xbf" },
    { "q"	, "\x71" },
    { "n'"	, "\x6e" },
    { "s"	, "\x73" },
    { "$"	, "\xc5\xa1" },
    { "m'"	, "\x6d" },
    { "b"	, "\x62" },
    { "&"	, "\xc5\x9b" },
    { "l"	, "\x6c" },
    { "t"	, "\x74" },
    { "c"	, "\xe1\xb9\xa3" },
    { "p"	, "\x70" },
    { "z"	, "\x7a" },
    { "y"	, "\x79" },
    { "c'"	, "\xe1\xb9\xa3" },
    { "p'"	, "\x70" },
    { "m"	, "\x6d" },
    { "n"	, "\x6e" },
    { "k'"	, "\x6b" },
    { "w"	, "\x77" },
    { ")"	, "\xca\xbe" },
    { "x"	, "\xe1\xb8\xa5" },
    { "T"	, "\xe1\xb9\xad" },
    { "r"	, "\x72" },
    { "*"	, "*" },
    { "7"	, "7" },
    { "5"	, "5" },
    { "'"	, "'" },
    { "+"	, "+" },
    { "["	, "[" },
    { "1"	, "1" },
    { "-"	, "-" },
    { "!"	, "!" },
    { "_"	, "_" },
    { ""	, "" },
    { "."	, "." },
    { ">"	, ">" },
    { "6"	, "6" },
    { "4"	, "4" },
    { "9"	, "9" },
    { "0"	, "0" },
    { "?"	, "?" },
    { "3"	, "3" },
    { "#"	, "#" },
    { "<"	, "<" },
    { "8"	, "8" },
    { "]"	, "]" },
    { "2"	, "2" },
    { NULL, NULL },
  };
struct charset arc_a =
  {
    "arc", c_arc , "a", m_alphabetic,
    arc_a_keys,
    0,
    arc_a_val,
    arc_a_asc,
    arc_u2u,
  };

static int
egy_a_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00077:
        case 0x00054:
        case 0x01e2a:
        case 0x000ef:
        case 0x01e6e:
        case 0x00069:
        case 0x00038:
        case 0x00051:
        case 0x00033:
        case 0x0010d:
        case 0x01e6d:
        case 0x01e32:
        case 0x00055:
        case 0x00070:
        case 0x01e2b:
        case 0x00032:
        case 0x00160:
        case 0x00044:
        case 0x0006b:
        case 0x0a723:
        case 0x00039:
        case 0x0a722:
        case 0x00131:
        case 0x00049:
        case 0x00034:
        case 0x0032f:
        case 0x01e70:
        case 0x00079:
        case 0x00053:
        case 0x00357:
        case 0x00042:
        case 0x00323:
        case 0x0004b:
        case 0x00066:
        case 0x00030:
        case 0x00071:
        case 0x0014d:
        case 0x00047:
        case 0x00067:
        case 0x0004a:
        case 0x0016a:
        case 0x01e6f:
        case 0x0006e:
        case 0x01e0e:
        case 0x001e6:
        case 0x0a725:
        case 0x01e96:
        case 0x0032d:
        case 0x001e7:
        case 0x00036:
        case 0x00074:
        case 0x0015b:
        case 0x0010c:
        case 0x0007a:
        case 0x00075:
        case 0x01e25:
        case 0x00050:
        case 0x01e71:
        case 0x00048:
        case 0x00073:
        case 0x00101:
        case 0x0004d:
        case 0x00064:
        case 0x00046:
        case 0x00072:
        case 0x00059:
        case 0x002be:
        case 0x00037:
        case 0x00052:
        case 0x0005a:
        case 0x0016b:
        case 0x01e24:
        case 0x0006d:
        case 0x0004e:
        case 0x00100:
        case 0x0015a:
        case 0x01e0f:
        case 0x00035:
        case 0x000cf:
        case 0x00161:
        case 0x01e33:
        case 0x0006a:
        case 0x00062:
        case 0x0014c:
        case 0x0a724:
        case 0x00000:
        case 0x00331:
        case 0x00031:
        case 0x00068:
        case 0x00057:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%egy/a",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
egy_a_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x000ef:
          append("ï");
          break;
        case 0x01e6e:
          append("Ṯ");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x0010d:
          append("č");
          break;
        case 0x01e6d:
          append("ṭ");
          break;
        case 0x01e32:
          append("Ḳ");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00160:
          append("Š");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x0a723:
          append("ꜣ");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0a722:
          append("Ꜣ");
          break;
        case 0x00131:
          append("ı");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x0032f:
          append("̯");
          break;
        case 0x01e70:
          append("Ṱ");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x00357:
          append("͗");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00323:
          append("̣");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x0014d:
          append("ō");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x0016a:
          append("Ū");
          break;
        case 0x01e6f:
          append("ṯ");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x01e0e:
          append("Ḏ");
          break;
        case 0x001e6:
          append("Ǧ");
          break;
        case 0x0a725:
          append("ꜥ");
          break;
        case 0x01e96:
          append("ẖ");
          break;
        case 0x0032d:
          append("̭");
          break;
        case 0x001e7:
          append("ǧ");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x0015b:
          append("ś");
          break;
        case 0x0010c:
          append("Č");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x01e25:
          append("ḥ");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x01e71:
          append("ṱ");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00101:
          append("ā");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00046:
          append("F");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x002be:
          append("ʾ");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x0016b:
          append("ū");
          break;
        case 0x01e24:
          append("Ḥ");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00100:
          append("Ā");
          break;
        case 0x0015a:
          append("Ś");
          break;
        case 0x01e0f:
          append("ḏ");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x000cf:
          append("Ï");
          break;
        case 0x00161:
          append("š");
          break;
        case 0x01e33:
          append("ḳ");
          break;
        case 0x0006a:
          append("j");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0014c:
          append("Ō");
          break;
        case 0x0a724:
          append("Ꜥ");
          break;
        case 0x00331:
          append("̱");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00057:
          append("W");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys egy_a_keys[] =
  {
    { "w"	, "w" },
    { "T"	, "T" },
    { "8"	, "8" },
    { "Q"	, "Q" },
    { "3"	, "3" },
    { "p"	, "p" },
    { "2"	, "2" },
    { "D"	, "D" },
    { "k"	, "k" },
    { "9"	, "9" },
    { "4"	, "4" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "B"	, "B" },
    { "K"	, "K" },
    { "f"	, "f" },
    { "0"	, "0" },
    { "q"	, "q" },
    { "G"	, "G" },
    { "g"	, "g" },
    { "J"	, "J" },
    { "n"	, "n" },
    { "6"	, "6" },
    { "t"	, "t" },
    { "z"	, "z" },
    { "P"	, "P" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "d"	, "d" },
    { "F"	, "F" },
    { "r"	, "r" },
    { "Y"	, "Y" },
    { "7"	, "7" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "5"	, "5" },
    { "j"	, "j" },
    { "b"	, "b" },
    { "1"	, "1" },
    { "W"	, "W" },
    { NULL, NULL },
  };
struct charset egy_a =
  {
    "egy", c_egy , "a", m_alphabetic,
    egy_a_keys,
    0,
    egy_a_val,
    egy_a_asc,
    0,
  };

static int
egy_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00067:
        case 0x00047:
        case 0x02088:
        case 0x0004b:
        case 0x0006c:
        case 0x00042:
        case 0x02087:
        case 0x02089:
        case 0x00053:
        case 0x02086:
        case 0x00049:
        case 0x0006b:
        case 0x00044:
        case 0x00160:
        case 0x00041:
        case 0x02085:
        case 0x01e2b:
        case 0x00055:
        case 0x00070:
        case 0x02084:
        case 0x00061:
        case 0x00045:
        case 0x00069:
        case 0x00058:
        case 0x01e2a:
        case 0x00054:
        case 0x00068:
        case 0x00000:
        case 0x00062:
        case 0x00161:
        case 0x02082:
        case 0x00065:
        case 0x0006d:
        case 0x0004e:
        case 0x0005a:
        case 0x00052:
        case 0x00078:
        case 0x00072:
        case 0x02093:
        case 0x00064:
        case 0x02083:
        case 0x00073:
        case 0x0004d:
        case 0x00050:
        case 0x00048:
        case 0x00075:
        case 0x02080:
        case 0x0007a:
        case 0x00074:
        case 0x02081:
        case 0x0006e:
        case 0x0004c:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%egy/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
egy_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00067:
          append("g");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0004c:
          append("L");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys egy_g_keys[] =
  {
    { "sz"	, "\xc5\xa1" },
    { "SZ"	, "\xc5\xa0" },
    { "g"	, "g" },
    { "G"	, "G" },
    { "K"	, "K" },
    { "l"	, "l" },
    { "B"	, "B" },
    { "S"	, "S" },
    { "I"	, "I" },
    { "k"	, "k" },
    { "D"	, "D" },
    { "A"	, "A" },
    { "U"	, "U" },
    { "p"	, "p" },
    { "a"	, "a" },
    { "E"	, "E" },
    { "i"	, "i" },
    { "X"	, "X" },
    { "T"	, "T" },
    { "h"	, "h" },
    { "b"	, "b" },
    { "e"	, "e" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "Z"	, "Z" },
    { "R"	, "R" },
    { "x"	, "x" },
    { "r"	, "r" },
    { "d"	, "d" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "u"	, "u" },
    { "z"	, "z" },
    { "t"	, "t" },
    { "n"	, "n" },
    { "L"	, "L" },
    { NULL, NULL },
  };
struct charset egy_g =
  {
    "egy", c_egy , "g", m_graphemic,
    egy_g_keys,
    0,
    egy_g_val,
    egy_g_asc,
    0,
  };

static int
elx_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x02093:
        case 0x00072:
        case 0x00078:
        case 0x00052:
        case 0x0005a:
        case 0x0006d:
        case 0x0004e:
        case 0x00065:
        case 0x02082:
        case 0x00161:
        case 0x00062:
        case 0x00000:
        case 0x00068:
        case 0x0006e:
        case 0x0004c:
        case 0x02081:
        case 0x00074:
        case 0x0007a:
        case 0x00075:
        case 0x02080:
        case 0x00050:
        case 0x00048:
        case 0x02083:
        case 0x00073:
        case 0x0004d:
        case 0x00064:
        case 0x00049:
        case 0x00053:
        case 0x02086:
        case 0x02089:
        case 0x02087:
        case 0x00042:
        case 0x0006c:
        case 0x0004b:
        case 0x02088:
        case 0x00047:
        case 0x00067:
        case 0x00054:
        case 0x00058:
        case 0x00069:
        case 0x00045:
        case 0x02084:
        case 0x00061:
        case 0x00055:
        case 0x00070:
        case 0x02085:
        case 0x00041:
        case 0x00160:
        case 0x00044:
        case 0x0006b:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%elx/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
elx_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x02093:
          append("ₓ");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x0006b:
          append("k");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys elx_g_keys[] =
  {
    { "SZ"	, "\xc5\xa0" },
    { "sz"	, "\xc5\xa1" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "e"	, "e" },
    { "b"	, "b" },
    { "h"	, "h" },
    { "n"	, "n" },
    { "L"	, "L" },
    { "t"	, "t" },
    { "z"	, "z" },
    { "u"	, "u" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "d"	, "d" },
    { "I"	, "I" },
    { "S"	, "S" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "K"	, "K" },
    { "G"	, "G" },
    { "g"	, "g" },
    { "T"	, "T" },
    { "X"	, "X" },
    { "i"	, "i" },
    { "E"	, "E" },
    { "a"	, "a" },
    { "U"	, "U" },
    { "p"	, "p" },
    { "A"	, "A" },
    { "D"	, "D" },
    { "k"	, "k" },
    { NULL, NULL },
  };
struct charset elx_g =
  {
    "elx", c_elx , "g", m_graphemic,
    elx_g_keys,
    0,
    elx_g_val,
    elx_g_asc,
    0,
  };

static int
grc_a_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x003c4:
        case 0x003b1:
        case 0x003c2:
        case 0x003bc:
        case 0x003c5:
        case 0x003c6:
        case 0x003b8:
        case 0x003b7:
        case 0x003ba:
        case 0x003b5:
        case 0x003c9:
        case 0x003bf:
        case 0x003bd:
        case 0x003c3:
        case 0x003c0:
        case 0x003bb:
        case 0x003b6:
        case 0x003c8:
        case 0x003be:
        case 0x003b9:
        case 0x003b2:
        case 0x003c7:
        case 0x003b4:
        case 0x00000:
        case 0x003b3:
        case 0x003c1:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%grc/a",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
grc_a_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x003c4:
          append("t");
          break;
        case 0x003b1:
          append("a");
          break;
        case 0x003c2:
          append("s'");
          break;
        case 0x003bc:
          append("m");
          break;
        case 0x003c5:
          append("u");
          break;
        case 0x003c6:
          append("ph");
          break;
        case 0x003b8:
          append("th");
          break;
        case 0x003b7:
          append("e'");
          break;
        case 0x003ba:
          append("k");
          break;
        case 0x003b5:
          append("e");
          break;
        case 0x003c9:
          append("o'");
          break;
        case 0x003bf:
          append("o");
          break;
        case 0x003bd:
          append("n");
          break;
        case 0x003c3:
          append("s");
          break;
        case 0x003c0:
          append("p");
          break;
        case 0x003bb:
          append("l");
          break;
        case 0x003b6:
          append("z");
          break;
        case 0x003c8:
          append("p'");
          break;
        case 0x003be:
          append("x");
          break;
        case 0x003b9:
          append("i");
          break;
        case 0x003b2:
          append("b");
          break;
        case 0x003c7:
          append("ch");
          break;
        case 0x003b4:
          append("d");
          break;
        case 0x003b3:
          append("g");
          break;
        case 0x003c1:
          append("r");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys grc_a_keys[] =
  {
    { "s'"	, "\xcf\x82" },
    { "n"	, "\xce\xbd" },
    { "ph"	, "\xcf\x86" },
    { "th"	, "\xce\xb8" },
    { "ch"	, "\xcf\x87" },
    { "r"	, "\xcf\x81" },
    { "x"	, "\xce\xbe" },
    { "p'"	, "\xcf\x88" },
    { "o"	, "\xce\xbf" },
    { "m"	, "\xce\xbc" },
    { "i"	, "\xce\xb9" },
    { "e"	, "\xce\xb5" },
    { "u"	, "\xcf\x85" },
    { "o'"	, "\xcf\x89" },
    { "b"	, "\xce\xb2" },
    { "e'"	, "\xce\xb7" },
    { "l"	, "\xce\xbb" },
    { "t"	, "\xcf\x84" },
    { "a"	, "\xce\xb1" },
    { "p"	, "\xcf\x80" },
    { "z"	, "\xce\xb6" },
    { "k"	, "\xce\xba" },
    { "g"	, "\xce\xb3" },
    { "d"	, "\xce\xb4" },
    { "s"	, "\xcf\x83" },
    { ""	, "" },
    { NULL, NULL },
  };
struct charset grc_a =
  {
    "grc", c_grc , "a", m_alphabetic,
    grc_a_keys,
    0,
    grc_a_val,
    grc_a_asc,
    0,
  };

static int
hit_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00055:
        case 0x00070:
        case 0x02084:
        case 0x01e6d:
        case 0x00061:
        case 0x01e2b:
        case 0x02085:
        case 0x00044:
        case 0x000cc:
        case 0x00041:
        case 0x00032:
        case 0x000e9:
        case 0x00160:
        case 0x000e0:
        case 0x0006b:
        case 0x00054:
        case 0x00077:
        case 0x00058:
        case 0x01e2a:
        case 0x00038:
        case 0x00069:
        case 0x0006f:
        case 0x00045:
        case 0x000ec:
        case 0x00051:
        case 0x00033:
        case 0x02089:
        case 0x02087:
        case 0x000f9:
        case 0x0006c:
        case 0x000e1:
        case 0x00042:
        case 0x000c9:
        case 0x0004b:
        case 0x00030:
        case 0x00066:
        case 0x00071:
        case 0x00067:
        case 0x0004a:
        case 0x02088:
        case 0x00047:
        case 0x00049:
        case 0x00039:
        case 0x000d9:
        case 0x00034:
        case 0x00053:
        case 0x02086:
        case 0x00079:
        case 0x0007a:
        case 0x00036:
        case 0x01e6c:
        case 0x0015b:
        case 0x00074:
        case 0x00075:
        case 0x02080:
        case 0x000c8:
        case 0x02083:
        case 0x00073:
        case 0x0004d:
        case 0x00050:
        case 0x00048:
        case 0x00064:
        case 0x01e63:
        case 0x000fa:
        case 0x0006e:
        case 0x0004c:
        case 0x000c1:
        case 0x02081:
        case 0x00161:
        case 0x00062:
        case 0x000da:
        case 0x00000:
        case 0x00031:
        case 0x00057:
        case 0x00068:
        case 0x01e62:
        case 0x00072:
        case 0x00078:
        case 0x00059:
        case 0x000ed:
        case 0x002be:
        case 0x000c0:
        case 0x00037:
        case 0x02093:
        case 0x00052:
        case 0x0005a:
        case 0x000cd:
        case 0x0015a:
        case 0x000e8:
        case 0x00065:
        case 0x0006d:
        case 0x0004e:
        case 0x02082:
        case 0x00035:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%hit/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
hit_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00035:
          append("5");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys hit_g_keys[] =
  {
    { "s,"	, "\xe1\xb9\xa3" },
    { "SZ"	, "\xc5\xa0" },
    { "'"	, "\xca\xbe" },
    { "t,"	, "\xe1\xb9\xad" },
    { "s'"	, "\xc5\x9b" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "T,"	, "\xe1\xb9\xac" },
    { "S'"	, "\xc5\x9a" },
    { "U"	, "U" },
    { "p"	, "p" },
    { "a"	, "a" },
    { "D"	, "D" },
    { "A"	, "A" },
    { "2"	, "2" },
    { "k"	, "k" },
    { "T"	, "T" },
    { "w"	, "w" },
    { "X"	, "X" },
    { "8"	, "8" },
    { "i"	, "i" },
    { "o"	, "o" },
    { "E"	, "E" },
    { "Q"	, "Q" },
    { "3"	, "3" },
    { "l"	, "l" },
    { "B"	, "B" },
    { "K"	, "K" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "q"	, "q" },
    { "g"	, "g" },
    { "J"	, "J" },
    { "G"	, "G" },
    { "I"	, "I" },
    { "9"	, "9" },
    { "4"	, "4" },
    { "S"	, "S" },
    { "y"	, "y" },
    { "z"	, "z" },
    { "6"	, "6" },
    { "t"	, "t" },
    { "u"	, "u" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "d"	, "d" },
    { "n"	, "n" },
    { "L"	, "L" },
    { "b"	, "b" },
    { "1"	, "1" },
    { "W"	, "W" },
    { "h"	, "h" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "Y"	, "Y" },
    { "7"	, "7" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "e"	, "e" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "5"	, "5" },
    { NULL, NULL },
  };
struct charset hit_g =
  {
    "hit", c_hit , "g", m_graphemic,
    hit_g_keys,
    0,
    hit_g_val,
    hit_g_asc,
    0,
  };

static int
peo_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x000c7:
        case 0x00054:
        case 0x00058:
        case 0x00069:
        case 0x003b8:
        case 0x01e5a:
        case 0x00061:
        case 0x0010d:
        case 0x00076:
        case 0x00070:
        case 0x00055:
        case 0x00160:
        case 0x00041:
        case 0x00044:
        case 0x0006b:
        case 0x00049:
        case 0x00079:
        case 0x000e7:
        case 0x00053:
        case 0x00042:
        case 0x0006c:
        case 0x00066:
        case 0x0004b:
        case 0x00113:
        case 0x00056:
        case 0x00047:
        case 0x0004a:
        case 0x00067:
        case 0x00043:
        case 0x0016a:
        case 0x00112:
        case 0x0004c:
        case 0x0006e:
        case 0x0012b:
        case 0x0010c:
        case 0x00074:
        case 0x00063:
        case 0x0007a:
        case 0x00075:
        case 0x00048:
        case 0x00050:
        case 0x0004d:
        case 0x00101:
        case 0x00073:
        case 0x01e5b:
        case 0x00064:
        case 0x00078:
        case 0x00059:
        case 0x00046:
        case 0x00072:
        case 0x00398:
        case 0x0016b:
        case 0x0005a:
        case 0x00052:
        case 0x0004e:
        case 0x00100:
        case 0x0006d:
        case 0x0012a:
        case 0x02082:
        case 0x00062:
        case 0x00161:
        case 0x0006a:
        case 0x00000:
        case 0x00068:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%peo/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
peo_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x000c7:
          append("Ç");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x003b8:
          append("t'");
          break;
        case 0x01e5a:
          append("Ṛ");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x0010d:
          append("c'");
          break;
        case 0x00076:
          append("v");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00160:
          append("Š");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x000e7:
          append("c,");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00113:
          append("=e");
          break;
        case 0x00056:
          append("V");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0016a:
          append("Ū");
          break;
        case 0x00112:
          append("Ē");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0012b:
          append("=i");
          break;
        case 0x0010c:
          append("Č");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00063:
          append("c");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00101:
          append("ā");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x01e5b:
          append("ṛ");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00046:
          append("F");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00398:
          append("Θ");
          break;
        case 0x0016b:
          append("ū");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00100:
          append("Ā");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0012a:
          append("Ī");
          break;
        case 0x02082:
          append("2");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00161:
          append("$");
          break;
        case 0x0006a:
          append("j");
          break;
        case 0x00068:
          append("h");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys peo_g_keys[] =
  {
    { "2"	, "\xe2\x82\x82" },
    { "$"	, "\xc5\xa1" },
    { "t'"	, "\xce\xb8" },
    { "c'"	, "\xc4\x8d" },
    { "c,"	, "\xc3\xa7" },
    { "T"	, "T" },
    { "X"	, "X" },
    { "i"	, "i" },
    { "a"	, "a" },
    { "v"	, "v" },
    { "p"	, "p" },
    { "U"	, "U" },
    { "A"	, "A" },
    { "D"	, "D" },
    { "k"	, "k" },
    { "I"	, "I" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "f"	, "f" },
    { "K"	, "K" },
    { "V"	, "V" },
    { "G"	, "G" },
    { "J"	, "J" },
    { "g"	, "g" },
    { "C"	, "C" },
    { "L"	, "L" },
    { "n"	, "n" },
    { "t"	, "t" },
    { "c"	, "c" },
    { "z"	, "z" },
    { "u"	, "u" },
    { "H"	, "H" },
    { "P"	, "P" },
    { "M"	, "M" },
    { "s"	, "s" },
    { "d"	, "d" },
    { "x"	, "x" },
    { "Y"	, "Y" },
    { "F"	, "F" },
    { "r"	, "r" },
    { "Z"	, "Z" },
    { "R"	, "R" },
    { "N"	, "N" },
    { "m"	, "m" },
    { "b"	, "b" },
    { "j"	, "j" },
    { "h"	, "h" },
    { NULL, NULL },
  };
struct charset peo_g =
  {
    "peo", c_peo , "g", m_graphemic,
    peo_g_keys,
    0,
    peo_g_val,
    peo_g_asc,
    0,
  };

static int
peo_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00055:
        case 0x00070:
        case 0x00076:
        case 0x0010d:
        case 0x00061:
        case 0x00044:
        case 0x00041:
        case 0x00160:
        case 0x0006b:
        case 0x00054:
        case 0x000c7:
        case 0x00058:
        case 0x003b8:
        case 0x00069:
        case 0x01e5a:
        case 0x0006c:
        case 0x00042:
        case 0x00113:
        case 0x0004b:
        case 0x00066:
        case 0x00067:
        case 0x0004a:
        case 0x00047:
        case 0x00056:
        case 0x00049:
        case 0x00053:
        case 0x000e7:
        case 0x00079:
        case 0x0007a:
        case 0x00074:
        case 0x0010c:
        case 0x00075:
        case 0x00073:
        case 0x00101:
        case 0x0004d:
        case 0x00050:
        case 0x00048:
        case 0x01e5b:
        case 0x00064:
        case 0x0016a:
        case 0x0006e:
        case 0x0004c:
        case 0x00112:
        case 0x0012b:
        case 0x0006a:
        case 0x00161:
        case 0x00062:
        case 0x00000:
        case 0x00068:
        case 0x00398:
        case 0x00046:
        case 0x00072:
        case 0x00078:
        case 0x00059:
        case 0x00052:
        case 0x0005a:
        case 0x0016b:
        case 0x0012a:
        case 0x0006d:
        case 0x0004e:
        case 0x00100:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%peo/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
peo_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00076:
          append("v");
          break;
        case 0x0010d:
          append("c'");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00160:
          append("%");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x000c7:
          append("C,");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x003b8:
          append("t'");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x01e5a:
          append("R,");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00113:
          append("=e");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00056:
          append("V");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x000e7:
          append("c,");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x0010c:
          append("C'");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00101:
          append("=a");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x01e5b:
          append("r,");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x0016a:
          append("=U");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00112:
          append("=E");
          break;
        case 0x0012b:
          append("=i");
          break;
        case 0x0006a:
          append("j");
          break;
        case 0x00161:
          append("$");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00398:
          append("T'");
          break;
        case 0x00046:
          append("F");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x0016b:
          append("=u");
          break;
        case 0x0012a:
          append("=I");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00100:
          append("=A");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys peo_n_keys[] =
  {
    { "=E"	, "\xc4\x92" },
    { "c,"	, "\xc3\xa7" },
    { "c'"	, "\xc4\x8d" },
    { "t'"	, "\xce\xb8" },
    { "=a"	, "\xc4\x81" },
    { "=e"	, "\xc4\x93" },
    { "=A"	, "\xc4\x80" },
    { "$"	, "\xc5\xa1" },
    { "R,"	, "\xe1\xb9\x9a" },
    { "r,"	, "\xe1\xb9\x9b" },
    { "=i"	, "\xc4\xab" },
    { "C,"	, "\xc3\x87" },
    { "C'"	, "\xc4\x8c" },
    { "%"	, "\xc5\xa0" },
    { "=u"	, "\xc5\xab" },
    { "=I"	, "\xc4\xaa" },
    { "=U"	, "\xc5\xaa" },
    { "T'"	, "\xce\x98" },
    { "U"	, "U" },
    { "p"	, "p" },
    { "v"	, "v" },
    { "a"	, "a" },
    { "D"	, "D" },
    { "A"	, "A" },
    { "k"	, "k" },
    { "T"	, "T" },
    { "X"	, "X" },
    { "i"	, "i" },
    { "l"	, "l" },
    { "B"	, "B" },
    { "K"	, "K" },
    { "f"	, "f" },
    { "g"	, "g" },
    { "J"	, "J" },
    { "G"	, "G" },
    { "V"	, "V" },
    { "I"	, "I" },
    { "S"	, "S" },
    { "y"	, "y" },
    { "z"	, "z" },
    { "t"	, "t" },
    { "u"	, "u" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "d"	, "d" },
    { "n"	, "n" },
    { "L"	, "L" },
    { "j"	, "j" },
    { "b"	, "b" },
    { ""	, "" },
    { "h"	, "h" },
    { "F"	, "F" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "Y"	, "Y" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "m"	, "m" },
    { "N"	, "N" },
    { NULL, NULL },
  };
struct charset peo_n =
  {
    "peo", c_peo , "n", m_normalized,
    peo_n_keys,
    0,
    peo_n_val,
    peo_n_asc,
    0,
  };

static int
qca_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00071:
        case 0x00066:
        case 0x00030:
        case 0x0014b:
        case 0x0004b:
        case 0x0004a:
        case 0x00067:
        case 0x00047:
        case 0x02088:
        case 0x02087:
        case 0x02089:
        case 0x000f9:
        case 0x000e1:
        case 0x0006c:
        case 0x00042:
        case 0x000c9:
        case 0x02086:
        case 0x00053:
        case 0x00079:
        case 0x00049:
        case 0x000d9:
        case 0x00039:
        case 0x00034:
        case 0x0014a:
        case 0x00044:
        case 0x000e9:
        case 0x00032:
        case 0x00160:
        case 0x00041:
        case 0x000cc:
        case 0x000e0:
        case 0x0006b:
        case 0x00070:
        case 0x00055:
        case 0x00061:
        case 0x01e6d:
        case 0x02084:
        case 0x02085:
        case 0x01e2b:
        case 0x00038:
        case 0x0006f:
        case 0x00069:
        case 0x000ec:
        case 0x00045:
        case 0x00051:
        case 0x00033:
        case 0x00054:
        case 0x00077:
        case 0x01e2a:
        case 0x00058:
        case 0x00000:
        case 0x01e62:
        case 0x00057:
        case 0x00068:
        case 0x00031:
        case 0x00062:
        case 0x00161:
        case 0x000da:
        case 0x000e8:
        case 0x00065:
        case 0x0015a:
        case 0x0004e:
        case 0x0006d:
        case 0x00035:
        case 0x02082:
        case 0x00037:
        case 0x000c0:
        case 0x002be:
        case 0x000ed:
        case 0x00059:
        case 0x00078:
        case 0x00072:
        case 0x02093:
        case 0x000cd:
        case 0x0005a:
        case 0x00052:
        case 0x0004d:
        case 0x00073:
        case 0x02083:
        case 0x00048:
        case 0x00050:
        case 0x00064:
        case 0x0007a:
        case 0x01e6c:
        case 0x00074:
        case 0x0015b:
        case 0x00036:
        case 0x000c8:
        case 0x00075:
        case 0x02080:
        case 0x000c1:
        case 0x02081:
        case 0x01e63:
        case 0x0004c:
        case 0x0006e:
        case 0x000fa:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%qca/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qca_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00071:
          append("q");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x000fa:
          append("ú");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qca_g_keys[] =
  {
    { "t,"	, "\xe1\xb9\xad" },
    { "s'"	, "\xc5\x9b" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "SZ"	, "\xc5\xa0" },
    { "'"	, "\xca\xbe" },
    { "S'"	, "\xc5\x9a" },
    { "T,"	, "\xe1\xb9\xac" },
    { "j"	, "\xc5\x8b" },
    { "sz"	, "\xc5\xa1" },
    { "J"	, "\xc5\x8a" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "q"	, "q" },
    { "f"	, "f" },
    { "0"	, "0" },
    { "K"	, "K" },
    { "g"	, "g" },
    { "G"	, "G" },
    { "l"	, "l" },
    { "B"	, "B" },
    { "S"	, "S" },
    { "y"	, "y" },
    { "I"	, "I" },
    { "9"	, "9" },
    { "4"	, "4" },
    { "D"	, "D" },
    { "2"	, "2" },
    { "A"	, "A" },
    { "k"	, "k" },
    { "p"	, "p" },
    { "U"	, "U" },
    { "a"	, "a" },
    { "8"	, "8" },
    { "o"	, "o" },
    { "i"	, "i" },
    { "E"	, "E" },
    { "Q"	, "Q" },
    { "3"	, "3" },
    { "T"	, "T" },
    { "w"	, "w" },
    { "X"	, "X" },
    { "W"	, "W" },
    { "h"	, "h" },
    { "1"	, "1" },
    { "b"	, "b" },
    { "e"	, "e" },
    { "N"	, "N" },
    { "m"	, "m" },
    { "5"	, "5" },
    { "7"	, "7" },
    { "Y"	, "Y" },
    { "x"	, "x" },
    { "r"	, "r" },
    { "Z"	, "Z" },
    { "R"	, "R" },
    { "M"	, "M" },
    { "s"	, "s" },
    { "H"	, "H" },
    { "P"	, "P" },
    { "d"	, "d" },
    { "z"	, "z" },
    { "t"	, "t" },
    { "6"	, "6" },
    { "u"	, "u" },
    { "L"	, "L" },
    { "n"	, "n" },
    { NULL, NULL },
  };
struct charset qca_g =
  {
    "qca", c_qca , "g", m_graphemic,
    qca_g_keys,
    0,
    qca_g_val,
    qca_g_asc,
    0,
  };

static int
qcu_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00050:
        case 0x00048:
        case 0x02083:
        case 0x00073:
        case 0x0004d:
        case 0x00064:
        case 0x00036:
        case 0x00074:
        case 0x01e6c:
        case 0x0015b:
        case 0x0007a:
        case 0x00075:
        case 0x02080:
        case 0x000c8:
        case 0x02081:
        case 0x000c1:
        case 0x01e63:
        case 0x000fa:
        case 0x0006e:
        case 0x0004c:
        case 0x00000:
        case 0x00031:
        case 0x00057:
        case 0x00068:
        case 0x01e62:
        case 0x000da:
        case 0x00161:
        case 0x00062:
        case 0x0006d:
        case 0x0004e:
        case 0x0015a:
        case 0x000e8:
        case 0x00065:
        case 0x02082:
        case 0x00035:
        case 0x02093:
        case 0x00072:
        case 0x00059:
        case 0x00078:
        case 0x000ed:
        case 0x002be:
        case 0x000c0:
        case 0x00037:
        case 0x00052:
        case 0x0005a:
        case 0x000cd:
        case 0x000cc:
        case 0x00041:
        case 0x000e9:
        case 0x00160:
        case 0x00032:
        case 0x00044:
        case 0x0006b:
        case 0x000e0:
        case 0x02084:
        case 0x01e6d:
        case 0x00061:
        case 0x00055:
        case 0x00070:
        case 0x01e2b:
        case 0x02085:
        case 0x00069:
        case 0x0006f:
        case 0x00038:
        case 0x00051:
        case 0x00033:
        case 0x00045:
        case 0x000ec:
        case 0x00077:
        case 0x00054:
        case 0x00058:
        case 0x01e2a:
        case 0x0004b:
        case 0x00030:
        case 0x00066:
        case 0x00071:
        case 0x02088:
        case 0x00047:
        case 0x00067:
        case 0x0004a:
        case 0x000f9:
        case 0x02089:
        case 0x02087:
        case 0x00042:
        case 0x000c9:
        case 0x0006c:
        case 0x000e1:
        case 0x00079:
        case 0x00053:
        case 0x02086:
        case 0x00039:
        case 0x000d9:
        case 0x00049:
        case 0x00034:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%qcu/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qcu_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00034:
          append("4");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qcu_g_keys[] =
  {
    { "P"	, "P" },
    { "H"	, "H" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "d"	, "d" },
    { "6"	, "6" },
    { "t"	, "t" },
    { "z"	, "z" },
    { "u"	, "u" },
    { "n"	, "n" },
    { "L"	, "L" },
    { ""	, "" },
    { "1"	, "1" },
    { "W"	, "W" },
    { "h"	, "h" },
    { "b"	, "b" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "e"	, "e" },
    { "5"	, "5" },
    { "r"	, "r" },
    { "Y"	, "Y" },
    { "x"	, "x" },
    { "7"	, "7" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "A"	, "A" },
    { "2"	, "2" },
    { "D"	, "D" },
    { "k"	, "k" },
    { "a"	, "a" },
    { "U"	, "U" },
    { "p"	, "p" },
    { "i"	, "i" },
    { "o"	, "o" },
    { "8"	, "8" },
    { "Q"	, "Q" },
    { "3"	, "3" },
    { "E"	, "E" },
    { "w"	, "w" },
    { "T"	, "T" },
    { "X"	, "X" },
    { "K"	, "K" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "q"	, "q" },
    { "G"	, "G" },
    { "g"	, "g" },
    { "J"	, "J" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "9"	, "9" },
    { "I"	, "I" },
    { "4"	, "4" },
    { NULL, NULL },
  };
struct charset qcu_g =
  {
    "qcu", c_qcu , "g", m_graphemic,
    qcu_g_keys,
    0,
    qcu_g_val,
    qcu_g_asc,
    0,
  };

static int
qcu_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x0006b:
        case 0x00041:
        case 0x00160:
        case 0x00044:
        case 0x01e2b:
        case 0x01e6d:
        case 0x00061:
        case 0x00055:
        case 0x00070:
        case 0x00076:
        case 0x00051:
        case 0x00045:
        case 0x00069:
        case 0x0006f:
        case 0x00058:
        case 0x01e2a:
        case 0x00077:
        case 0x00054:
        case 0x00047:
        case 0x00056:
        case 0x00067:
        case 0x0004a:
        case 0x0004b:
        case 0x00066:
        case 0x00071:
        case 0x0003f:
        case 0x00113:
        case 0x000c2:
        case 0x00042:
        case 0x0006c:
        case 0x00060:
        case 0x00079:
        case 0x00053:
        case 0x000e2:
        case 0x00049:
        case 0x0004f:
        case 0x00064:
        case 0x00050:
        case 0x00048:
        case 0x00073:
        case 0x0004d:
        case 0x00101:
        case 0x000ee:
        case 0x00075:
        case 0x000ca:
        case 0x00063:
        case 0x01e6c:
        case 0x00074:
        case 0x0015b:
        case 0x0007a:
        case 0x0012b:
        case 0x00112:
        case 0x0006e:
        case 0x0004c:
        case 0x01e63:
        case 0x00043:
        case 0x0016a:
        case 0x00057:
        case 0x0002d:
        case 0x00068:
        case 0x01e62:
        case 0x000ce:
        case 0x00000:
        case 0x00161:
        case 0x0006a:
        case 0x000ea:
        case 0x00062:
        case 0x0002f:
        case 0x000fb:
        case 0x000db:
        case 0x00027:
        case 0x0006d:
        case 0x00100:
        case 0x0004e:
        case 0x0015a:
        case 0x00065:
        case 0x0012a:
        case 0x00052:
        case 0x0005a:
        case 0x0016b:
        case 0x00072:
        case 0x00046:
        case 0x00078:
        case 0x00059:
        case 0x002be:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%qcu/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qcu_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x0006b:
          append("k");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00076:
          append("v");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00056:
          append("V");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x0003f:
          append("?");
          break;
        case 0x00113:
          append("=e");
          break;
        case 0x000c2:
          append("^A");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00060:
          append("`");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x000e2:
          append("^a");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00101:
          append("=a");
          break;
        case 0x000ee:
          append("^i");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x000ca:
          append("^E");
          break;
        case 0x00063:
          append("c");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x0012b:
          append("=i");
          break;
        case 0x00112:
          append("=E");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0016a:
          append("=U");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x000ce:
          append("^I");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x0006a:
          append("j");
          break;
        case 0x000ea:
          append("^e");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0002f:
          append("/");
          break;
        case 0x000fb:
          append("^u");
          break;
        case 0x000db:
          append("^U");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00100:
          append("=A");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0012a:
          append("=I");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x0016b:
          append("=u");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00046:
          append("F");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x002be:
          append("'");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qcu_n_keys[] =
  {
    { "k"	, "k" },
    { "A"	, "A" },
    { "D"	, "D" },
    { "a"	, "a" },
    { "U"	, "U" },
    { "p"	, "p" },
    { "v"	, "v" },
    { "Q"	, "Q" },
    { "E"	, "E" },
    { "i"	, "i" },
    { "o"	, "o" },
    { "X"	, "X" },
    { "w"	, "w" },
    { "T"	, "T" },
    { "G"	, "G" },
    { "V"	, "V" },
    { "g"	, "g" },
    { "J"	, "J" },
    { "K"	, "K" },
    { "f"	, "f" },
    { "q"	, "q" },
    { "?"	, "?" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "`"	, "`" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "I"	, "I" },
    { "O"	, "O" },
    { "d"	, "d" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "u"	, "u" },
    { "c"	, "c" },
    { "t"	, "t" },
    { "z"	, "z" },
    { "n"	, "n" },
    { "L"	, "L" },
    { "C"	, "C" },
    { "W"	, "W" },
    { "-"	, "-" },
    { "h"	, "h" },
    { ""	, "" },
    { "j"	, "j" },
    { "b"	, "b" },
    { "/"	, "/" },
    { "'"	, "'" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "e"	, "e" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "r"	, "r" },
    { "F"	, "F" },
    { "x"	, "x" },
    { "Y"	, "Y" },
    { NULL, NULL },
  };
struct charset qcu_n =
  {
    "qcu", c_qcu , "n", m_normalized,
    qcu_n_keys,
    0,
    qcu_n_val,
    qcu_n_asc,
    0,
  };

static int
qeb_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00079:
        case 0x02086:
        case 0x00053:
        case 0x000d9:
        case 0x00039:
        case 0x00049:
        case 0x00034:
        case 0x00071:
        case 0x00030:
        case 0x00066:
        case 0x0004b:
        case 0x00047:
        case 0x02088:
        case 0x0004a:
        case 0x00067:
        case 0x000f9:
        case 0x02087:
        case 0x02089:
        case 0x00042:
        case 0x000c9:
        case 0x000e1:
        case 0x0006c:
        case 0x0006f:
        case 0x00069:
        case 0x00038:
        case 0x00033:
        case 0x00051:
        case 0x000ec:
        case 0x00045:
        case 0x00077:
        case 0x00054:
        case 0x01e2a:
        case 0x00058:
        case 0x000e9:
        case 0x00032:
        case 0x00160:
        case 0x00041:
        case 0x000cc:
        case 0x00044:
        case 0x0006b:
        case 0x000e0:
        case 0x00061:
        case 0x01e6d:
        case 0x02084:
        case 0x00070:
        case 0x00055:
        case 0x02085:
        case 0x01e2b:
        case 0x0004e:
        case 0x0006d:
        case 0x000e8:
        case 0x00065:
        case 0x0015a:
        case 0x00035:
        case 0x02082:
        case 0x02093:
        case 0x00037:
        case 0x000c0:
        case 0x002be:
        case 0x000ed:
        case 0x00059:
        case 0x00078:
        case 0x00072:
        case 0x000cd:
        case 0x0005a:
        case 0x00052:
        case 0x00000:
        case 0x01e62:
        case 0x00057:
        case 0x00068:
        case 0x00031:
        case 0x000da:
        case 0x00062:
        case 0x00161:
        case 0x02081:
        case 0x000c1:
        case 0x01e63:
        case 0x0004c:
        case 0x0006e:
        case 0x000fa:
        case 0x00048:
        case 0x00050:
        case 0x0004d:
        case 0x02083:
        case 0x00073:
        case 0x00064:
        case 0x01e6c:
        case 0x0015b:
        case 0x00074:
        case 0x00036:
        case 0x0007a:
        case 0x000c8:
        case 0x02080:
        case 0x00075:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%qeb/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qeb_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00079:
          append("y");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00075:
          append("u");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qeb_g_keys[] =
  {
    { "S,"	, "\xe1\xb9\xa2" },
    { "sz"	, "\xc5\xa1" },
    { "S'"	, "\xc5\x9a" },
    { "T,"	, "\xe1\xb9\xac" },
    { "'"	, "\xca\xbe" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "SZ"	, "\xc5\xa0" },
    { "s'"	, "\xc5\x9b" },
    { "t,"	, "\xe1\xb9\xad" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "9"	, "9" },
    { "I"	, "I" },
    { "4"	, "4" },
    { "q"	, "q" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "K"	, "K" },
    { "G"	, "G" },
    { "J"	, "J" },
    { "g"	, "g" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "o"	, "o" },
    { "i"	, "i" },
    { "8"	, "8" },
    { "3"	, "3" },
    { "Q"	, "Q" },
    { "E"	, "E" },
    { "w"	, "w" },
    { "T"	, "T" },
    { "X"	, "X" },
    { "2"	, "2" },
    { "A"	, "A" },
    { "D"	, "D" },
    { "k"	, "k" },
    { "a"	, "a" },
    { "p"	, "p" },
    { "U"	, "U" },
    { "N"	, "N" },
    { "m"	, "m" },
    { "e"	, "e" },
    { "5"	, "5" },
    { "7"	, "7" },
    { "Y"	, "Y" },
    { "x"	, "x" },
    { "r"	, "r" },
    { "Z"	, "Z" },
    { "R"	, "R" },
    { "W"	, "W" },
    { "h"	, "h" },
    { "1"	, "1" },
    { "b"	, "b" },
    { "L"	, "L" },
    { "n"	, "n" },
    { "H"	, "H" },
    { "P"	, "P" },
    { "M"	, "M" },
    { "s"	, "s" },
    { "d"	, "d" },
    { "t"	, "t" },
    { "6"	, "6" },
    { "z"	, "z" },
    { "u"	, "u" },
    { NULL, NULL },
  };
struct charset qeb_g =
  {
    "qeb", c_qeb , "g", m_graphemic,
    qeb_g_keys,
    0,
    qeb_g_val,
    qeb_g_asc,
    0,
  };

static int
qeb_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00027:
        case 0x0012a:
        case 0x0015a:
        case 0x00065:
        case 0x0004e:
        case 0x00100:
        case 0x0006d:
        case 0x0016b:
        case 0x00052:
        case 0x0005a:
        case 0x002be:
        case 0x00072:
        case 0x00078:
        case 0x00059:
        case 0x000ce:
        case 0x0002d:
        case 0x00068:
        case 0x00057:
        case 0x01e62:
        case 0x00000:
        case 0x000ea:
        case 0x00161:
        case 0x00062:
        case 0x000db:
        case 0x000fb:
        case 0x0012b:
        case 0x0004c:
        case 0x0006e:
        case 0x00112:
        case 0x0016a:
        case 0x00043:
        case 0x01e63:
        case 0x00064:
        case 0x0004f:
        case 0x0004d:
        case 0x00101:
        case 0x000ee:
        case 0x00073:
        case 0x00048:
        case 0x00050:
        case 0x000ca:
        case 0x00075:
        case 0x0007a:
        case 0x00063:
        case 0x0015b:
        case 0x01e6c:
        case 0x00074:
        case 0x00053:
        case 0x00079:
        case 0x00049:
        case 0x000e2:
        case 0x00067:
        case 0x00047:
        case 0x00113:
        case 0x00071:
        case 0x0004b:
        case 0x0006c:
        case 0x00042:
        case 0x000c2:
        case 0x00045:
        case 0x00051:
        case 0x0006f:
        case 0x00069:
        case 0x01e2a:
        case 0x00058:
        case 0x00054:
        case 0x00077:
        case 0x0006b:
        case 0x00044:
        case 0x00041:
        case 0x00160:
        case 0x01e2b:
        case 0x00070:
        case 0x00055:
        case 0x01e6d:
        case 0x00061:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%qeb/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qeb_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00027:
          append("'");
          break;
        case 0x0012a:
          append("=I");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00100:
          append("=A");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0016b:
          append("=u");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x000ce:
          append("^I");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x000ea:
          append("^e");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x000db:
          append("^U");
          break;
        case 0x000fb:
          append("^u");
          break;
        case 0x0012b:
          append("=i");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00112:
          append("=E");
          break;
        case 0x0016a:
          append("=U");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00101:
          append("=a");
          break;
        case 0x000ee:
          append("^i");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x000ca:
          append("^E");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00063:
          append("c");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x000e2:
          append("^a");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00113:
          append("=e");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x000c2:
          append("^A");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x00061:
          append("a");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qeb_n_keys[] =
  {
    { "=U"	, "\xc5\xaa" },
    { "^i"	, "\xc3\xae" },
    { "=I"	, "\xc4\xaa" },
    { "=u"	, "\xc5\xab" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "^A"	, "\xc3\x82" },
    { "sz"	, "\xc5\xa1" },
    { "=A"	, "\xc4\x80" },
    { "^u"	, "\xc3\xbb" },
    { "S'"	, "\xc5\x9a" },
    { "^U"	, "\xc3\x9b" },
    { "=i"	, "\xc4\xab" },
    { "^I"	, "\xc3\x8e" },
    { "T,"	, "\xe1\xb9\xac" },
    { "'"	, "\xca\xbe" },
    { "^E"	, "\xc3\x8a" },
    { "=e"	, "\xc4\x93" },
    { "=a"	, "\xc4\x81" },
    { "SZ"	, "\xc5\xa0" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "s'"	, "\xc5\x9b" },
    { "t,"	, "\xe1\xb9\xad" },
    { "^a"	, "\xc3\xa2" },
    { "=E"	, "\xc4\x92" },
    { "^e"	, "\xc3\xaa" },
    { "e"	, "e" },
    { "N"	, "N" },
    { "m"	, "m" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "Y"	, "Y" },
    { "-"	, "-" },
    { "h"	, "h" },
    { "W"	, "W" },
    { "b"	, "b" },
    { "L"	, "L" },
    { "n"	, "n" },
    { "C"	, "C" },
    { "d"	, "d" },
    { "O"	, "O" },
    { "M"	, "M" },
    { "s"	, "s" },
    { "H"	, "H" },
    { "P"	, "P" },
    { "u"	, "u" },
    { "z"	, "z" },
    { "c"	, "c" },
    { "t"	, "t" },
    { "S"	, "S" },
    { "y"	, "y" },
    { "I"	, "I" },
    { "g"	, "g" },
    { "G"	, "G" },
    { "q"	, "q" },
    { "K"	, "K" },
    { "l"	, "l" },
    { "B"	, "B" },
    { "E"	, "E" },
    { "Q"	, "Q" },
    { "o"	, "o" },
    { "i"	, "i" },
    { "X"	, "X" },
    { "T"	, "T" },
    { "w"	, "w" },
    { "k"	, "k" },
    { "D"	, "D" },
    { "A"	, "A" },
    { "p"	, "p" },
    { "U"	, "U" },
    { "a"	, "a" },
    { NULL, NULL },
  };
struct charset qeb_n =
  {
    "qeb", c_qeb , "n", m_normalized,
    qeb_n_keys,
    0,
    qeb_n_val,
    qeb_n_asc,
    0,
  };

static int
qka_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x02088:
        case 0x00047:
        case 0x00067:
        case 0x0004a:
        case 0x0014b:
        case 0x0004b:
        case 0x00066:
        case 0x00030:
        case 0x00071:
        case 0x00042:
        case 0x000c9:
        case 0x0006c:
        case 0x000e1:
        case 0x000f9:
        case 0x02089:
        case 0x02087:
        case 0x00079:
        case 0x00053:
        case 0x02086:
        case 0x0014a:
        case 0x00034:
        case 0x00039:
        case 0x000d9:
        case 0x00049:
        case 0x0006b:
        case 0x000e0:
        case 0x000cc:
        case 0x00041:
        case 0x000e9:
        case 0x00160:
        case 0x00032:
        case 0x00044:
        case 0x01e2b:
        case 0x02085:
        case 0x02084:
        case 0x01e6d:
        case 0x00061:
        case 0x00055:
        case 0x00070:
        case 0x00051:
        case 0x00033:
        case 0x00045:
        case 0x000ec:
        case 0x00069:
        case 0x0006f:
        case 0x00038:
        case 0x00058:
        case 0x01e2a:
        case 0x00077:
        case 0x00054:
        case 0x00031:
        case 0x00057:
        case 0x00068:
        case 0x01e62:
        case 0x00000:
        case 0x000da:
        case 0x00161:
        case 0x00062:
        case 0x02082:
        case 0x00035:
        case 0x0006d:
        case 0x0004e:
        case 0x0015a:
        case 0x000e8:
        case 0x00065:
        case 0x00052:
        case 0x0005a:
        case 0x000cd:
        case 0x02093:
        case 0x00072:
        case 0x00078:
        case 0x00059:
        case 0x000ed:
        case 0x00037:
        case 0x000c0:
        case 0x002be:
        case 0x00064:
        case 0x00050:
        case 0x00048:
        case 0x02083:
        case 0x00073:
        case 0x0004d:
        case 0x02080:
        case 0x00075:
        case 0x000c8:
        case 0x00036:
        case 0x01e6c:
        case 0x0015b:
        case 0x00074:
        case 0x0007a:
        case 0x02081:
        case 0x000c1:
        case 0x000fa:
        case 0x0006e:
        case 0x0004c:
        case 0x01e63:
        case 0x00043:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%qka/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qka_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x02088:
          append("₈");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00043:
          append("C");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qka_g_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "S'"	, "\xc5\x9a" },
    { "j"	, "\xc5\x8b" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "J"	, "\xc5\x8a" },
    { "t,"	, "\xe1\xb9\xad" },
    { "s'"	, "\xc5\x9b" },
    { "SZ"	, "\xc5\xa0" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "'"	, "\xca\xbe" },
    { "G"	, "G" },
    { "g"	, "g" },
    { "K"	, "K" },
    { "f"	, "f" },
    { "0"	, "0" },
    { "q"	, "q" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "4"	, "4" },
    { "9"	, "9" },
    { "I"	, "I" },
    { "k"	, "k" },
    { "A"	, "A" },
    { "2"	, "2" },
    { "D"	, "D" },
    { "a"	, "a" },
    { "U"	, "U" },
    { "p"	, "p" },
    { "Q"	, "Q" },
    { "3"	, "3" },
    { "E"	, "E" },
    { "i"	, "i" },
    { "o"	, "o" },
    { "8"	, "8" },
    { "X"	, "X" },
    { "w"	, "w" },
    { "T"	, "T" },
    { "1"	, "1" },
    { "W"	, "W" },
    { "h"	, "h" },
    { "b"	, "b" },
    { "5"	, "5" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "e"	, "e" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "Y"	, "Y" },
    { "7"	, "7" },
    { "d"	, "d" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "u"	, "u" },
    { "6"	, "6" },
    { "t"	, "t" },
    { "z"	, "z" },
    { "n"	, "n" },
    { "L"	, "L" },
    { "C"	, "C" },
    { NULL, NULL },
  };
struct charset qka_g =
  {
    "qka", c_qka , "g", m_graphemic,
    qka_g_keys,
    0,
    qka_g_val,
    qka_g_asc,
    0,
  };

static int
qpc_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x02082:
        case 0x00035:
        case 0x00027:
        case 0x0004e:
        case 0x00052:
        case 0x0005a:
        case 0x02093:
        case 0x00059:
        case 0x00037:
        case 0x002be:
        case 0x00031:
        case 0x00057:
        case 0x01e62:
        case 0x00000:
        case 0x02081:
        case 0x0004c:
        case 0x00043:
        case 0x00050:
        case 0x00048:
        case 0x02083:
        case 0x0004d:
        case 0x02080:
        case 0x00036:
        case 0x01e6c:
        case 0x00053:
        case 0x02086:
        case 0x0014a:
        case 0x00034:
        case 0x00039:
        case 0x00049:
        case 0x02088:
        case 0x00047:
        case 0x0004b:
        case 0x00030:
        case 0x00042:
        case 0x02089:
        case 0x02087:
        case 0x00033:
        case 0x00045:
        case 0x00038:
        case 0x00058:
        case 0x01e2a:
        case 0x00054:
        case 0x00041:
        case 0x00032:
        case 0x00160:
        case 0x00044:
        case 0x02085:
        case 0x02084:
        case 0x00055:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%qpc/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qpc_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x02082:
          append("₂");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00055:
          append("U");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qpc_g_keys[] =
  {
    { "SZ"	, "\xc5\xa0" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "J"	, "\xc5\x8a" },
    { "T,"	, "\xe1\xb9\xac" },
    { "5"	, "5" },
    { "'"	, "'" },
    { "N"	, "N" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "Y"	, "Y" },
    { "7"	, "7" },
    { "1"	, "1" },
    { "W"	, "W" },
    { "L"	, "L" },
    { "C"	, "C" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "M"	, "M" },
    { "6"	, "6" },
    { "S"	, "S" },
    { "4"	, "4" },
    { "9"	, "9" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "K"	, "K" },
    { "0"	, "0" },
    { "B"	, "B" },
    { "3"	, "3" },
    { "E"	, "E" },
    { "8"	, "8" },
    { "X"	, "X" },
    { "T"	, "T" },
    { "A"	, "A" },
    { "2"	, "2" },
    { "D"	, "D" },
    { "U"	, "U" },
    { NULL, NULL },
  };
struct charset qpc_g =
  {
    "qpc", c_qpc , "g", m_graphemic,
    qpc_g_keys,
    0,
    qpc_g_val,
    qpc_g_asc,
    0,
  };

static int
qpe_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x0005a:
        case 0x00052:
        case 0x002be:
        case 0x00037:
        case 0x00059:
        case 0x02093:
        case 0x00027:
        case 0x00035:
        case 0x02082:
        case 0x0004e:
        case 0x01e62:
        case 0x00057:
        case 0x00031:
        case 0x00000:
        case 0x0004c:
        case 0x02081:
        case 0x02080:
        case 0x01e6c:
        case 0x00036:
        case 0x0004d:
        case 0x02083:
        case 0x00048:
        case 0x00050:
        case 0x00034:
        case 0x0014a:
        case 0x00049:
        case 0x00039:
        case 0x02086:
        case 0x00053:
        case 0x00042:
        case 0x02087:
        case 0x02089:
        case 0x00047:
        case 0x02088:
        case 0x00030:
        case 0x0004b:
        case 0x01e2a:
        case 0x00058:
        case 0x00054:
        case 0x00045:
        case 0x00033:
        case 0x00038:
        case 0x02085:
        case 0x00055:
        case 0x02084:
        case 0x00044:
        case 0x00032:
        case 0x00160:
        case 0x00041:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%qpe/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qpe_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x0005a:
          append("Z");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00041:
          append("A");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qpe_g_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "J"	, "\xc5\x8a" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "SZ"	, "\xc5\xa0" },
    { "Z"	, "Z" },
    { "R"	, "R" },
    { "7"	, "7" },
    { "Y"	, "Y" },
    { "'"	, "'" },
    { "5"	, "5" },
    { "N"	, "N" },
    { "W"	, "W" },
    { "1"	, "1" },
    { "L"	, "L" },
    { "6"	, "6" },
    { "M"	, "M" },
    { "H"	, "H" },
    { "P"	, "P" },
    { "4"	, "4" },
    { "I"	, "I" },
    { "9"	, "9" },
    { "S"	, "S" },
    { "B"	, "B" },
    { "G"	, "G" },
    { "0"	, "0" },
    { "K"	, "K" },
    { "X"	, "X" },
    { "T"	, "T" },
    { "E"	, "E" },
    { "3"	, "3" },
    { "8"	, "8" },
    { "U"	, "U" },
    { "D"	, "D" },
    { "2"	, "2" },
    { "A"	, "A" },
    { NULL, NULL },
  };
struct charset qpe_g =
  {
    "qpe", c_qpe , "g", m_graphemic,
    qpe_g_keys,
    0,
    qpe_g_val,
    qpe_g_asc,
    0,
  };

static int
qpn_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x0007a:
        case 0x00074:
        case 0x01e6c:
        case 0x0015b:
        case 0x00036:
        case 0x02080:
        case 0x00075:
        case 0x000c8:
        case 0x02083:
        case 0x00073:
        case 0x0004d:
        case 0x00050:
        case 0x00048:
        case 0x00064:
        case 0x01e63:
        case 0x0006e:
        case 0x000fa:
        case 0x0004c:
        case 0x000c1:
        case 0x02081:
        case 0x00062:
        case 0x00161:
        case 0x0006a:
        case 0x000da:
        case 0x00000:
        case 0x00031:
        case 0x01e62:
        case 0x00068:
        case 0x00057:
        case 0x00059:
        case 0x00078:
        case 0x00072:
        case 0x00037:
        case 0x000c0:
        case 0x002be:
        case 0x000ed:
        case 0x02093:
        case 0x0005a:
        case 0x00052:
        case 0x000cd:
        case 0x00065:
        case 0x000e8:
        case 0x0015a:
        case 0x0006d:
        case 0x0004e:
        case 0x02082:
        case 0x00035:
        case 0x00055:
        case 0x00070:
        case 0x02084:
        case 0x00061:
        case 0x01e6d:
        case 0x02085:
        case 0x01e2b:
        case 0x00044:
        case 0x000cc:
        case 0x00032:
        case 0x000e9:
        case 0x00160:
        case 0x00041:
        case 0x000e0:
        case 0x0006b:
        case 0x00054:
        case 0x00077:
        case 0x00058:
        case 0x01e2a:
        case 0x00038:
        case 0x00069:
        case 0x0006f:
        case 0x00045:
        case 0x000ec:
        case 0x00033:
        case 0x00051:
        case 0x02087:
        case 0x02089:
        case 0x000f9:
        case 0x000e1:
        case 0x0006c:
        case 0x000c9:
        case 0x00042:
        case 0x00066:
        case 0x00030:
        case 0x0014b:
        case 0x0004b:
        case 0x00071:
        case 0x0004a:
        case 0x00067:
        case 0x00047:
        case 0x02088:
        case 0x00049:
        case 0x00039:
        case 0x000d9:
        case 0x00034:
        case 0x0014a:
        case 0x00053:
        case 0x02086:
        case 0x00079:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%qpn/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qpn_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x0007a:
          append("z");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x0006a:
          append("j");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00079:
          append("y");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qpn_g_keys[] =
  {
    { "'"	, "\xca\xbe" },
    { "SZ"	, "\xc5\xa0" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "s'"	, "\xc5\x9b" },
    { "t,"	, "\xe1\xb9\xad" },
    { "J"	, "\xc5\x8a" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "sz"	, "\xc5\xa1" },
    { "j"	, "\xc5\x8b" },
    { "S'"	, "\xc5\x9a" },
    { "T,"	, "\xe1\xb9\xac" },
    { "z"	, "z" },
    { "t"	, "t" },
    { "6"	, "6" },
    { "u"	, "u" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "d"	, "d" },
    { "n"	, "n" },
    { "L"	, "L" },
    { "b"	, "b" },
    { "1"	, "1" },
    { "h"	, "h" },
    { "W"	, "W" },
    { "Y"	, "Y" },
    { "x"	, "x" },
    { "r"	, "r" },
    { "7"	, "7" },
    { "Z"	, "Z" },
    { "R"	, "R" },
    { "e"	, "e" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "5"	, "5" },
    { "U"	, "U" },
    { "p"	, "p" },
    { "a"	, "a" },
    { "D"	, "D" },
    { "2"	, "2" },
    { "A"	, "A" },
    { "k"	, "k" },
    { "T"	, "T" },
    { "w"	, "w" },
    { "X"	, "X" },
    { "8"	, "8" },
    { "i"	, "i" },
    { "o"	, "o" },
    { "E"	, "E" },
    { "3"	, "3" },
    { "Q"	, "Q" },
    { "l"	, "l" },
    { "B"	, "B" },
    { "f"	, "f" },
    { "0"	, "0" },
    { "K"	, "K" },
    { "q"	, "q" },
    { "g"	, "g" },
    { "G"	, "G" },
    { "I"	, "I" },
    { "9"	, "9" },
    { "4"	, "4" },
    { "S"	, "S" },
    { "y"	, "y" },
    { NULL, NULL },
  };
struct charset qpn_g =
  {
    "qpn", c_qpn , "g", m_graphemic,
    qpn_g_keys,
    0,
    qpn_g_val,
    qpn_g_asc,
    0,
  };

static int
sux_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x000b0:
        case 0x01e63:
        case 0x00043:
        case 0x0006e:
        case 0x000fa:
        case 0x0004c:
        case 0x000c1:
        case 0x02081:
        case 0x0002c:
        case 0x0003b:
        case 0x0007a:
        case 0x00074:
        case 0x01e6c:
        case 0x00036:
        case 0x02080:
        case 0x00075:
        case 0x000c8:
        case 0x02083:
        case 0x00073:
        case 0x0004d:
        case 0x00050:
        case 0x00048:
        case 0x00064:
        case 0x0207b:
        case 0x0004f:
        case 0x00078:
        case 0x00059:
        case 0x00072:
        case 0x00046:
        case 0x00037:
        case 0x002be:
        case 0x000c0:
        case 0x000ed:
        case 0x02093:
        case 0x0005a:
        case 0x00052:
        case 0x000cd:
        case 0x00065:
        case 0x000e8:
        case 0x12471:
        case 0x0006d:
        case 0x0004e:
        case 0x00027:
        case 0x02082:
        case 0x00035:
        case 0x0003a:
        case 0x00062:
        case 0x00161:
        case 0x000da:
        case 0x00000:
        case 0x000b7:
        case 0x00031:
        case 0x01e62:
        case 0x00057:
        case 0x00068:
        case 0x00054:
        case 0x00077:
        case 0x00058:
        case 0x01e2a:
        case 0x00038:
        case 0x00069:
        case 0x0006f:
        case 0x0207a:
        case 0x00045:
        case 0x000ec:
        case 0x00051:
        case 0x00033:
        case 0x00055:
        case 0x00070:
        case 0x02084:
        case 0x00061:
        case 0x01e6d:
        case 0x02085:
        case 0x01e2b:
        case 0x00044:
        case 0x000cc:
        case 0x00032:
        case 0x000e9:
        case 0x00160:
        case 0x00041:
        case 0x000e0:
        case 0x0006b:
        case 0x00049:
        case 0x00039:
        case 0x000d9:
        case 0x0014a:
        case 0x00034:
        case 0x00053:
        case 0x02086:
        case 0x00079:
        case 0x02087:
        case 0x02089:
        case 0x000f9:
        case 0x000e1:
        case 0x0006c:
        case 0x000c9:
        case 0x00042:
        case 0x00030:
        case 0x00066:
        case 0x0014b:
        case 0x0004b:
        case 0x00071:
        case 0x00067:
        case 0x00047:
        case 0x02088:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%sux/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
sux_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x000b0:
          append("°");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x0002c:
          append(",");
          break;
        case 0x0003b:
          append(";");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x0207b:
          append("⁻");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00046:
          append("F");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x12471:
          append("𒑱");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0003a:
          append(":");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x000b7:
          append("·");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x0207a:
          append("⁺");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02088:
          append("₈");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys sux_g_keys[] =
  {
    { "S,"	, "\xe1\xb9\xa2" },
    { "J"	, "\xc5\x8a" },
    { "sz"	, "\xc5\xa1" },
    { "j"	, "\xc5\x8b" },
    { "T,"	, "\xe1\xb9\xac" },
    { "'"	, "\xca\xbe" },
    { "SZ"	, "\xc5\xa0" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "t,"	, "\xe1\xb9\xad" },
    { "C"	, "C" },
    { "n"	, "n" },
    { "L"	, "L" },
    { ","	, "," },
    { ";"	, ";" },
    { "z"	, "z" },
    { "t"	, "t" },
    { "6"	, "6" },
    { "u"	, "u" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "d"	, "d" },
    { "O"	, "O" },
    { "x"	, "x" },
    { "Y"	, "Y" },
    { "r"	, "r" },
    { "F"	, "F" },
    { "7"	, "7" },
    { "Z"	, "Z" },
    { "R"	, "R" },
    { "e"	, "e" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "5"	, "5" },
    { ":"	, ":" },
    { "b"	, "b" },
    { "1"	, "1" },
    { "W"	, "W" },
    { "h"	, "h" },
    { "T"	, "T" },
    { "w"	, "w" },
    { "X"	, "X" },
    { "8"	, "8" },
    { "i"	, "i" },
    { "o"	, "o" },
    { "E"	, "E" },
    { "Q"	, "Q" },
    { "3"	, "3" },
    { "U"	, "U" },
    { "p"	, "p" },
    { "a"	, "a" },
    { "D"	, "D" },
    { "2"	, "2" },
    { "A"	, "A" },
    { "k"	, "k" },
    { "I"	, "I" },
    { "9"	, "9" },
    { "4"	, "4" },
    { "S"	, "S" },
    { "y"	, "y" },
    { "l"	, "l" },
    { "B"	, "B" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "K"	, "K" },
    { "q"	, "q" },
    { "g"	, "g" },
    { "G"	, "G" },
    { NULL, NULL },
  };
struct charset sux_g =
  {
    "sux", c_sux , "g", m_graphemic,
    sux_g_keys,
    0,
    sux_g_val,
    sux_g_asc,
    0,
  };

static int
sux_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00061:
        case 0x01e6d:
        case 0x02084:
        case 0x00070:
        case 0x00055:
        case 0x02085:
        case 0x01e2b:
        case 0x00032:
        case 0x00160:
        case 0x00041:
        case 0x00044:
        case 0x0006b:
        case 0x00077:
        case 0x00054:
        case 0x01e2a:
        case 0x00058:
        case 0x0006f:
        case 0x0207a:
        case 0x00069:
        case 0x00038:
        case 0x00033:
        case 0x00045:
        case 0x0007e:
        case 0x02087:
        case 0x02089:
        case 0x00042:
        case 0x0006c:
        case 0x00071:
        case 0x00066:
        case 0x00030:
        case 0x0014b:
        case 0x0004b:
        case 0x00047:
        case 0x02088:
        case 0x00067:
        case 0x00039:
        case 0x00049:
        case 0x0014a:
        case 0x00034:
        case 0x00079:
        case 0x02086:
        case 0x00053:
        case 0x00074:
        case 0x01e6c:
        case 0x00036:
        case 0x0007a:
        case 0x02080:
        case 0x00075:
        case 0x00048:
        case 0x00050:
        case 0x0004d:
        case 0x0005c:
        case 0x00073:
        case 0x02083:
        case 0x0004f:
        case 0x0207b:
        case 0x00064:
        case 0x00043:
        case 0x01e63:
        case 0x000b0:
        case 0x00040:
        case 0x0004c:
        case 0x0006e:
        case 0x02081:
        case 0x0002e:
        case 0x00026:
        case 0x00062:
        case 0x00161:
        case 0x00000:
        case 0x01e62:
        case 0x0002d:
        case 0x00057:
        case 0x00068:
        case 0x00031:
        case 0x000b7:
        case 0x02093:
        case 0x00037:
        case 0x00078:
        case 0x00059:
        case 0x00072:
        case 0x0005a:
        case 0x00052:
        case 0x0004e:
        case 0x0006d:
        case 0x0002b:
        case 0x00065:
        case 0x00035:
        case 0x02082:
        case 0x00027:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%sux/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
sux_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00061:
          append("a");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x0207a:
          append("⁺");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x0007e:
          append("~");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x0005c:
          append("\x5c");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x0207b:
          append("⁻");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x000b0:
          append("°");
          break;
        case 0x00040:
          append("@");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x0002e:
          append(".");
          break;
        case 0x00026:
          append("&");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x000b7:
          append("·");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0002b:
          append("+");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00027:
          append("'");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys sux_n_keys[] =
  {
    { "J"	, "\xc5\x8a" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "sz"	, "\xc5\xa1" },
    { "j"	, "\xc5\x8b" },
    { "T,"	, "\xe1\xb9\xac" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "SZ"	, "\xc5\xa0" },
    { "t,"	, "\xe1\xb9\xad" },
    { "a"	, "a" },
    { "p"	, "p" },
    { "U"	, "U" },
    { "2"	, "2" },
    { "A"	, "A" },
    { "D"	, "D" },
    { "k"	, "k" },
    { "w"	, "w" },
    { "T"	, "T" },
    { "X"	, "X" },
    { "o"	, "o" },
    { "i"	, "i" },
    { "8"	, "8" },
    { "3"	, "3" },
    { "E"	, "E" },
    { "~"	, "~" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "q"	, "q" },
    { "f"	, "f" },
    { "0"	, "0" },
    { "K"	, "K" },
    { "G"	, "G" },
    { "g"	, "g" },
    { "9"	, "9" },
    { "I"	, "I" },
    { "4"	, "4" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "t"	, "t" },
    { "6"	, "6" },
    { "z"	, "z" },
    { "u"	, "u" },
    { "H"	, "H" },
    { "P"	, "P" },
    { "M"	, "M" },
    { "\x5c"	, "\x5c" },
    { "s"	, "s" },
    { "O"	, "O" },
    { "d"	, "d" },
    { "C"	, "C" },
    { "@"	, "@" },
    { "L"	, "L" },
    { "n"	, "n" },
    { "."	, "." },
    { "&"	, "&" },
    { "b"	, "b" },
    { "-"	, "-" },
    { "W"	, "W" },
    { "h"	, "h" },
    { "1"	, "1" },
    { "7"	, "7" },
    { "x"	, "x" },
    { "Y"	, "Y" },
    { "r"	, "r" },
    { "Z"	, "Z" },
    { "R"	, "R" },
    { "N"	, "N" },
    { "m"	, "m" },
    { "+"	, "+" },
    { "e"	, "e" },
    { "5"	, "5" },
    { "'"	, "'" },
    { NULL, NULL },
  };
struct charset sux_n =
  {
    "sux", c_sux , "n", m_normalized,
    sux_n_keys,
    0,
    sux_n_val,
    sux_n_asc,
    0,
  };

static int
xhu_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00043:
        case 0x01e63:
        case 0x0004c:
        case 0x0006e:
        case 0x02081:
        case 0x00036:
        case 0x00074:
        case 0x01e6c:
        case 0x0007a:
        case 0x00075:
        case 0x02080:
        case 0x00048:
        case 0x00050:
        case 0x0004d:
        case 0x02083:
        case 0x00073:
        case 0x00064:
        case 0x02093:
        case 0x002be:
        case 0x00037:
        case 0x00072:
        case 0x00078:
        case 0x00059:
        case 0x00052:
        case 0x0005a:
        case 0x0004e:
        case 0x0006d:
        case 0x00065:
        case 0x00035:
        case 0x02082:
        case 0x00027:
        case 0x00161:
        case 0x00062:
        case 0x00000:
        case 0x00057:
        case 0x00068:
        case 0x01e62:
        case 0x00031:
        case 0x00077:
        case 0x00054:
        case 0x01e2a:
        case 0x00058:
        case 0x0006f:
        case 0x00069:
        case 0x00038:
        case 0x00033:
        case 0x00051:
        case 0x00045:
        case 0x01e6d:
        case 0x00061:
        case 0x02084:
        case 0x00070:
        case 0x00055:
        case 0x01e2b:
        case 0x02085:
        case 0x00041:
        case 0x00032:
        case 0x00160:
        case 0x00044:
        case 0x0006b:
        case 0x00039:
        case 0x00049:
        case 0x0014a:
        case 0x00034:
        case 0x00079:
        case 0x02086:
        case 0x00053:
        case 0x02089:
        case 0x02087:
        case 0x00042:
        case 0x0006c:
        case 0x00071:
        case 0x0004b:
        case 0x0014b:
        case 0x00066:
        case 0x00030:
        case 0x02088:
        case 0x00047:
        case 0x00067:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%xhu/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
xhu_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00043:
          append("C");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00067:
          append("g");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys xhu_g_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "sz"	, "\xc5\xa1" },
    { "t,"	, "\xe1\xb9\xad" },
    { "'"	, "\xca\xbe" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "SZ"	, "\xc5\xa0" },
    { "C"	, "C" },
    { "L"	, "L" },
    { "n"	, "n" },
    { "6"	, "6" },
    { "t"	, "t" },
    { "z"	, "z" },
    { "u"	, "u" },
    { "H"	, "H" },
    { "P"	, "P" },
    { "M"	, "M" },
    { "s"	, "s" },
    { "d"	, "d" },
    { "7"	, "7" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "Y"	, "Y" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "N"	, "N" },
    { "m"	, "m" },
    { "e"	, "e" },
    { "5"	, "5" },
    { "b"	, "b" },
    { "W"	, "W" },
    { "h"	, "h" },
    { "1"	, "1" },
    { "w"	, "w" },
    { "T"	, "T" },
    { "X"	, "X" },
    { "o"	, "o" },
    { "i"	, "i" },
    { "8"	, "8" },
    { "3"	, "3" },
    { "Q"	, "Q" },
    { "E"	, "E" },
    { "a"	, "a" },
    { "p"	, "p" },
    { "U"	, "U" },
    { "A"	, "A" },
    { "2"	, "2" },
    { "D"	, "D" },
    { "k"	, "k" },
    { "9"	, "9" },
    { "I"	, "I" },
    { "4"	, "4" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "q"	, "q" },
    { "K"	, "K" },
    { "f"	, "f" },
    { "0"	, "0" },
    { "G"	, "G" },
    { "g"	, "g" },
    { NULL, NULL },
  };
struct charset xhu_g =
  {
    "xhu", c_xhu , "g", m_graphemic,
    xhu_g_keys,
    0,
    xhu_g_val,
    xhu_g_asc,
    0,
  };

static int
xur_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00074:
        case 0x01e6c:
        case 0x00036:
        case 0x0007a:
        case 0x02080:
        case 0x00075:
        case 0x00048:
        case 0x00050:
        case 0x0004d:
        case 0x00073:
        case 0x02083:
        case 0x00064:
        case 0x00043:
        case 0x01e63:
        case 0x0004c:
        case 0x0006e:
        case 0x02081:
        case 0x00062:
        case 0x00161:
        case 0x00000:
        case 0x01e62:
        case 0x00057:
        case 0x00068:
        case 0x00031:
        case 0x02093:
        case 0x002be:
        case 0x00037:
        case 0x00059:
        case 0x00078:
        case 0x00072:
        case 0x0005a:
        case 0x00052:
        case 0x0004e:
        case 0x0006d:
        case 0x00065:
        case 0x00035:
        case 0x02082:
        case 0x00027:
        case 0x00061:
        case 0x01e6d:
        case 0x02084:
        case 0x00070:
        case 0x00055:
        case 0x02085:
        case 0x01e2b:
        case 0x00160:
        case 0x00032:
        case 0x00041:
        case 0x00044:
        case 0x0006b:
        case 0x00077:
        case 0x00054:
        case 0x01e2a:
        case 0x00058:
        case 0x0006f:
        case 0x00069:
        case 0x00038:
        case 0x00051:
        case 0x00033:
        case 0x00045:
        case 0x02087:
        case 0x02089:
        case 0x00042:
        case 0x0006c:
        case 0x00071:
        case 0x00030:
        case 0x00066:
        case 0x0004b:
        case 0x0014b:
        case 0x00047:
        case 0x02088:
        case 0x00067:
        case 0x00039:
        case 0x00049:
        case 0x0014a:
        case 0x00034:
        case 0x00079:
        case 0x02086:
        case 0x00053:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%xur/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
xur_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00074:
          append("t");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00053:
          append("S");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys xur_g_keys[] =
  {
    { "S,"	, "\xe1\xb9\xa2" },
    { "sz"	, "\xc5\xa1" },
    { "T,"	, "\xe1\xb9\xac" },
    { "'"	, "\xca\xbe" },
    { "SZ"	, "\xc5\xa0" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "t,"	, "\xe1\xb9\xad" },
    { "t"	, "t" },
    { "6"	, "6" },
    { "z"	, "z" },
    { "u"	, "u" },
    { "H"	, "H" },
    { "P"	, "P" },
    { "M"	, "M" },
    { "s"	, "s" },
    { "d"	, "d" },
    { "C"	, "C" },
    { "L"	, "L" },
    { "n"	, "n" },
    { "b"	, "b" },
    { "W"	, "W" },
    { "h"	, "h" },
    { "1"	, "1" },
    { "7"	, "7" },
    { "Y"	, "Y" },
    { "x"	, "x" },
    { "r"	, "r" },
    { "Z"	, "Z" },
    { "R"	, "R" },
    { "N"	, "N" },
    { "m"	, "m" },
    { "e"	, "e" },
    { "5"	, "5" },
    { "a"	, "a" },
    { "p"	, "p" },
    { "U"	, "U" },
    { "2"	, "2" },
    { "A"	, "A" },
    { "D"	, "D" },
    { "k"	, "k" },
    { "w"	, "w" },
    { "T"	, "T" },
    { "X"	, "X" },
    { "o"	, "o" },
    { "i"	, "i" },
    { "8"	, "8" },
    { "Q"	, "Q" },
    { "3"	, "3" },
    { "E"	, "E" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "q"	, "q" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "K"	, "K" },
    { "G"	, "G" },
    { "g"	, "g" },
    { "9"	, "9" },
    { "I"	, "I" },
    { "4"	, "4" },
    { "y"	, "y" },
    { "S"	, "S" },
    { NULL, NULL },
  };
struct charset xur_g =
  {
    "xur", c_xur , "g", m_graphemic,
    xur_g_keys,
    0,
    xur_g_val,
    xur_g_asc,
    0,
  };

static int
uga_a_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x01e6e:
        case 0x01e2a:
        case 0x00058:
        case 0x002bf:
        case 0x00054:
        case 0x001f4:
        case 0x00077:
        case 0x00045:
        case 0x00033:
        case 0x00051:
        case 0x00038:
        case 0x0006f:
        case 0x00069:
        case 0x02085:
        case 0x01e2b:
        case 0x00070:
        case 0x00055:
        case 0x00061:
        case 0x01e6d:
        case 0x02084:
        case 0x0006b:
        case 0x00044:
        case 0x00160:
        case 0x00032:
        case 0x00041:
        case 0x00034:
        case 0x0014a:
        case 0x00049:
        case 0x00039:
        case 0x02086:
        case 0x00053:
        case 0x00079:
        case 0x0006c:
        case 0x00042:
        case 0x02087:
        case 0x02089:
        case 0x00120:
        case 0x00067:
        case 0x00047:
        case 0x02088:
        case 0x00071:
        case 0x00030:
        case 0x00066:
        case 0x0004b:
        case 0x0014b:
        case 0x0004c:
        case 0x0006e:
        case 0x01e6f:
        case 0x00043:
        case 0x01e63:
        case 0x02081:
        case 0x01e25:
        case 0x02080:
        case 0x00075:
        case 0x0007a:
        case 0x00074:
        case 0x01e6c:
        case 0x00036:
        case 0x00121:
        case 0x00064:
        case 0x0004d:
        case 0x001f5:
        case 0x00073:
        case 0x02083:
        case 0x00048:
        case 0x00050:
        case 0x01e24:
        case 0x0005a:
        case 0x00052:
        case 0x00037:
        case 0x002be:
        case 0x00059:
        case 0x00078:
        case 0x00072:
        case 0x02093:
        case 0x00027:
        case 0x00035:
        case 0x02082:
        case 0x00065:
        case 0x0004e:
        case 0x0006d:
        case 0x00062:
        case 0x00161:
        case 0x01e62:
        case 0x00068:
        case 0x00057:
        case 0x00031:
        case 0x00000:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%uga/a",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
uga_a_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x01e6e:
          append("Ṯ");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x002bf:
          append("ʿ");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x001f4:
          append("Ǵ");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00120:
          append("Ġ");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x01e6f:
          append("ṯ");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x01e25:
          append("ḥ");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00121:
          append("ġ");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x001f5:
          append("ǵ");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x01e24:
          append("Ḥ");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x002be:
          append("ʾ");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00031:
          append("1");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys uga_a_keys[] =
  {
    { "S,"	, "\xe1\xb9\xa2" },
    { "sz"	, "\xc5\xa1" },
    { "T,"	, "\xe1\xb9\xac" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "SZ"	, "\xc5\xa0" },
    { "t,"	, "\xe1\xb9\xad" },
    { "X"	, "X" },
    { "T"	, "T" },
    { "w"	, "w" },
    { "E"	, "E" },
    { "3"	, "3" },
    { "Q"	, "Q" },
    { "8"	, "8" },
    { "o"	, "o" },
    { "i"	, "i" },
    { "p"	, "p" },
    { "U"	, "U" },
    { "a"	, "a" },
    { "k"	, "k" },
    { "D"	, "D" },
    { "2"	, "2" },
    { "A"	, "A" },
    { "4"	, "4" },
    { "I"	, "I" },
    { "9"	, "9" },
    { "S"	, "S" },
    { "y"	, "y" },
    { "l"	, "l" },
    { "B"	, "B" },
    { "g"	, "g" },
    { "G"	, "G" },
    { "q"	, "q" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "K"	, "K" },
    { "L"	, "L" },
    { "n"	, "n" },
    { "C"	, "C" },
    { "u"	, "u" },
    { "z"	, "z" },
    { "t"	, "t" },
    { "6"	, "6" },
    { "d"	, "d" },
    { "M"	, "M" },
    { "s"	, "s" },
    { "H"	, "H" },
    { "P"	, "P" },
    { "Z"	, "Z" },
    { "R"	, "R" },
    { "7"	, "7" },
    { "Y"	, "Y" },
    { "x"	, "x" },
    { "r"	, "r" },
    { "'"	, "'" },
    { "5"	, "5" },
    { "e"	, "e" },
    { "N"	, "N" },
    { "m"	, "m" },
    { "b"	, "b" },
    { "h"	, "h" },
    { "W"	, "W" },
    { "1"	, "1" },
    { NULL, NULL },
  };
struct charset uga_a =
  {
    "uga", c_uga , "a", m_alphabetic,
    uga_a_keys,
    0,
    uga_a_val,
    uga_a_asc,
    0,
  };

static int
uga_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x001f4:
        case 0x00077:
        case 0x00054:
        case 0x002bf:
        case 0x00058:
        case 0x01e2a:
        case 0x00069:
        case 0x0006f:
        case 0x00038:
        case 0x00033:
        case 0x00051:
        case 0x00045:
        case 0x02084:
        case 0x01e6d:
        case 0x00061:
        case 0x00055:
        case 0x00070:
        case 0x01e2b:
        case 0x02085:
        case 0x00041:
        case 0x00032:
        case 0x00160:
        case 0x00044:
        case 0x0006b:
        case 0x00039:
        case 0x00049:
        case 0x0014a:
        case 0x00034:
        case 0x00079:
        case 0x00053:
        case 0x02086:
        case 0x02089:
        case 0x02087:
        case 0x00042:
        case 0x0006c:
        case 0x0004b:
        case 0x0014b:
        case 0x00066:
        case 0x00030:
        case 0x00071:
        case 0x02088:
        case 0x00047:
        case 0x00067:
        case 0x01e63:
        case 0x00043:
        case 0x0006e:
        case 0x0004c:
        case 0x02081:
        case 0x00036:
        case 0x00074:
        case 0x01e6c:
        case 0x0007a:
        case 0x00075:
        case 0x02080:
        case 0x01e25:
        case 0x00050:
        case 0x00048:
        case 0x02083:
        case 0x00073:
        case 0x001f5:
        case 0x0004d:
        case 0x00064:
        case 0x02093:
        case 0x00072:
        case 0x00059:
        case 0x00078:
        case 0x002be:
        case 0x00037:
        case 0x00052:
        case 0x0005a:
        case 0x01e24:
        case 0x0006d:
        case 0x0004e:
        case 0x00065:
        case 0x02082:
        case 0x00035:
        case 0x00027:
        case 0x00161:
        case 0x00062:
        case 0x00000:
        case 0x00031:
        case 0x00068:
        case 0x00057:
        case 0x01e62:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%uga/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
uga_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x001f4:
          append("Ǵ");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x002bf:
          append("ʿ");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x01e25:
          append("ḥ");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x001f5:
          append("ǵ");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x002be:
          append("ʾ");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x01e24:
          append("Ḥ");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x01e62:
          append("S,");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys uga_g_keys[] =
  {
    { "t,"	, "\xe1\xb9\xad" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "SZ"	, "\xc5\xa0" },
    { "T,"	, "\xe1\xb9\xac" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "w"	, "w" },
    { "T"	, "T" },
    { "X"	, "X" },
    { "i"	, "i" },
    { "o"	, "o" },
    { "8"	, "8" },
    { "3"	, "3" },
    { "Q"	, "Q" },
    { "E"	, "E" },
    { "a"	, "a" },
    { "U"	, "U" },
    { "p"	, "p" },
    { "A"	, "A" },
    { "2"	, "2" },
    { "D"	, "D" },
    { "k"	, "k" },
    { "9"	, "9" },
    { "I"	, "I" },
    { "4"	, "4" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "K"	, "K" },
    { "f"	, "f" },
    { "0"	, "0" },
    { "q"	, "q" },
    { "G"	, "G" },
    { "g"	, "g" },
    { "C"	, "C" },
    { "n"	, "n" },
    { "L"	, "L" },
    { "6"	, "6" },
    { "t"	, "t" },
    { "z"	, "z" },
    { "u"	, "u" },
    { "P"	, "P" },
    { "H"	, "H" },
    { "s"	, "s" },
    { "M"	, "M" },
    { "d"	, "d" },
    { "r"	, "r" },
    { "Y"	, "Y" },
    { "x"	, "x" },
    { "7"	, "7" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "m"	, "m" },
    { "N"	, "N" },
    { "e"	, "e" },
    { "5"	, "5" },
    { "'"	, "'" },
    { "b"	, "b" },
    { "1"	, "1" },
    { "h"	, "h" },
    { "W"	, "W" },
    { NULL, NULL },
  };
struct charset uga_g =
  {
    "uga", c_uga , "g", m_graphemic,
    uga_g_keys,
    0,
    uga_g_val,
    uga_g_asc,
    0,
  };

static int
qse_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x0012b:
        case 0x0003b:
        case 0x0002c:
        case 0x0002e:
        case 0x02081:
        case 0x000c1:
        case 0x00112:
        case 0x00040:
        case 0x0004c:
        case 0x000fa:
        case 0x0006e:
        case 0x00043:
        case 0x01e63:
        case 0x000b0:
        case 0x0016a:
        case 0x0004f:
        case 0x00064:
        case 0x00048:
        case 0x00050:
        case 0x0004d:
        case 0x00101:
        case 0x000ee:
        case 0x00073:
        case 0x02083:
        case 0x000c8:
        case 0x00075:
        case 0x02080:
        case 0x0003e:
        case 0x000ca:
        case 0x00063:
        case 0x00036:
        case 0x01e6c:
        case 0x00074:
        case 0x0015b:
        case 0x0007a:
        case 0x00035:
        case 0x02082:
        case 0x00027:
        case 0x00100:
        case 0x0004e:
        case 0x0006d:
        case 0x0012a:
        case 0x0015a:
        case 0x000e8:
        case 0x00065:
        case 0x0005b:
        case 0x0016b:
        case 0x000cd:
        case 0x00052:
        case 0x0005a:
        case 0x0002a:
        case 0x02093:
        case 0x000ed:
        case 0x002be:
        case 0x000c0:
        case 0x00037:
        case 0x00046:
        case 0x00072:
        case 0x00059:
        case 0x00078:
        case 0x00057:
        case 0x0002d:
        case 0x00068:
        case 0x01e62:
        case 0x00031:
        case 0x000ce:
        case 0x000b7:
        case 0x00000:
        case 0x000da:
        case 0x0006a:
        case 0x00161:
        case 0x000ea:
        case 0x00062:
        case 0x0003a:
        case 0x000fb:
        case 0x000db:
        case 0x00033:
        case 0x00051:
        case 0x000ec:
        case 0x0003c:
        case 0x00045:
        case 0x0006f:
        case 0x00069:
        case 0x00038:
        case 0x01e2a:
        case 0x00058:
        case 0x00077:
        case 0x00054:
        case 0x0006b:
        case 0x000e0:
        case 0x00041:
        case 0x000e9:
        case 0x00032:
        case 0x00160:
        case 0x000cc:
        case 0x00044:
        case 0x01e2b:
        case 0x02085:
        case 0x0005d:
        case 0x01e6d:
        case 0x00061:
        case 0x02084:
        case 0x00070:
        case 0x00076:
        case 0x00055:
        case 0x00079:
        case 0x02086:
        case 0x00053:
        case 0x0007c:
        case 0x00034:
        case 0x0014a:
        case 0x000d9:
        case 0x00039:
        case 0x00049:
        case 0x000e2:
        case 0x00056:
        case 0x02088:
        case 0x00047:
        case 0x00067:
        case 0x0004a:
        case 0x0003f:
        case 0x00071:
        case 0x0014b:
        case 0x0004b:
        case 0x00066:
        case 0x00030:
        case 0x00113:
        case 0x00042:
        case 0x000c9:
        case 0x000c2:
        case 0x0006c:
        case 0x000e1:
        case 0x000f9:
        case 0x02089:
        case 0x02087:
        case 0x00060:
          break;
        default:
          mesg_verr(NULL, "Character %s (hex %x) not allowed in %%qse/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qse_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x0012b:
          append("=i");
          break;
        case 0x0003b:
          append(";");
          break;
        case 0x0002c:
          append(",");
          break;
        case 0x0002e:
          append(".");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x00112:
          append("=E");
          break;
        case 0x00040:
          append("@");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x000b0:
          append("°");
          break;
        case 0x0016a:
          append("=U");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00101:
          append("=a");
          break;
        case 0x000ee:
          append("^i");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x0003e:
          append(">");
          break;
        case 0x000ca:
          append("^E");
          break;
        case 0x00063:
          append("c");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x00100:
          append("=A");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0012a:
          append("=I");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0005b:
          append("[");
          break;
        case 0x0016b:
          append("=u");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x0002a:
          append("*");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x00046:
          append("F");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x000ce:
          append("^I");
          break;
        case 0x000b7:
          append("·");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x0006a:
          append("j");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x000ea:
          append("^e");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0003a:
          append(":");
          break;
        case 0x000fb:
          append("^u");
          break;
        case 0x000db:
          append("^U");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x0003c:
          append("<");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x0005d:
          append("]");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00076:
          append("v");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00053:
          append("S");
          break;
        case 0x0007c:
          append("|");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x000e2:
          append("^a");
          break;
        case 0x00056:
          append("V");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x0003f:
          append("?");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00113:
          append("=e");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x000c2:
          append("^A");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00060:
          append("`");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qse_n_keys[] =
  {
    { "s,"	, "\xe1\xb9\xa3" },
    { "SZ"	, "\xc5\xa0" },
    { "=e"	, "\xc4\x93" },
    { "^E"	, "\xc3\x8a" },
    { "'"	, "\xca\xbe" },
    { "=a"	, "\xc4\x81" },
    { "^a"	, "\xc3\xa2" },
    { "t,"	, "\xe1\xb9\xad" },
    { "^e"	, "\xc3\xaa" },
    { "=E"	, "\xc4\x92" },
    { "s'"	, "\xc5\x9b" },
    { "^A"	, "\xc3\x82" },
    { "sz"	, "\xc5\xa1" },
    { "=U"	, "\xc5\xaa" },
    { "^i"	, "\xc3\xae" },
    { "=I"	, "\xc4\xaa" },
    { "=u"	, "\xc5\xab" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "S'"	, "\xc5\x9a" },
    { "^u"	, "\xc3\xbb" },
    { "^U"	, "\xc3\x9b" },
    { "=i"	, "\xc4\xab" },
    { "T,"	, "\xe1\xb9\xac" },
    { "^I"	, "\xc3\x8e" },
    { "j"	, "\xc5\x8b" },
    { "=A"	, "\xc4\x80" },
    { ";"	, ";" },
    { ","	, "," },
    { "L"	, "L" },
    { "n"	, "n" },
    { "C"	, "C" },
    { "O"	, "O" },
    { "d"	, "d" },
    { "H"	, "H" },
    { "P"	, "P" },
    { "M"	, "M" },
    { "s"	, "s" },
    { "u"	, "u" },
    { ">"	, ">" },
    { "c"	, "c" },
    { "6"	, "6" },
    { "t"	, "t" },
    { "z"	, "z" },
    { "5"	, "5" },
    { "N"	, "N" },
    { "m"	, "m" },
    { "e"	, "e" },
    { "["	, "[" },
    { "R"	, "R" },
    { "Z"	, "Z" },
    { "*"	, "*" },
    { "7"	, "7" },
    { "F"	, "F" },
    { "r"	, "r" },
    { "Y"	, "Y" },
    { "x"	, "x" },
    { "W"	, "W" },
    { "-"	, "-" },
    { "h"	, "h" },
    { "1"	, "1" },
    { "b"	, "b" },
    { ":"	, ":" },
    { "3"	, "3" },
    { "Q"	, "Q" },
    { "<"	, "<" },
    { "E"	, "E" },
    { "o"	, "o" },
    { "i"	, "i" },
    { "8"	, "8" },
    { "X"	, "X" },
    { "w"	, "w" },
    { "T"	, "T" },
    { "k"	, "k" },
    { "A"	, "A" },
    { "2"	, "2" },
    { "D"	, "D" },
    { "]"	, "]" },
    { "a"	, "a" },
    { "p"	, "p" },
    { "v"	, "v" },
    { "U"	, "U" },
    { "y"	, "y" },
    { "S"	, "S" },
    { "|"	, "|" },
    { "4"	, "4" },
    { "9"	, "9" },
    { "I"	, "I" },
    { "V"	, "V" },
    { "G"	, "G" },
    { "g"	, "g" },
    { "J"	, "J" },
    { "?"	, "?" },
    { "q"	, "q" },
    { "K"	, "K" },
    { "f"	, "f" },
    { "0"	, "0" },
    { "B"	, "B" },
    { "l"	, "l" },
    { "`"	, "`" },
    { NULL, NULL },
  };
struct charset qse_n =
  {
    "qse", c_qse , "n", m_normalized,
    qse_n_keys,
    0,
    qse_n_val,
    qse_n_asc,
    0,
  };


struct charset *cset_index[c_count][modes_top];

static void
set_charset(struct charset *cp)
{
  cset_index[cp->lcode][cp->mcode] = cp;
}

struct charset *
get_charset(enum langcode lcode, enum t_modes mcode)
{
  return cset_index[lcode][mcode];
}

static void
free_charset(struct charset *cp)
{
  if (cp->asc)
    cp->asc(NULL,0);
  if (cp->u2u)
    cp->u2u(NULL,0);
  if (cp->to_uni)
    inctrie_free(cp->to_uni);
}

void
charsets_init(void)
{
  set_charset(&akk_g);
  set_charset(&akk_n);
  set_charset(&arc_a);
  set_charset(&egy_a);
  set_charset(&egy_g);
  set_charset(&elx_g);
  set_charset(&grc_a);
  set_charset(&hit_g);
  set_charset(&peo_g);
  set_charset(&peo_n);
  set_charset(&qca_g);
  set_charset(&qcu_g);
  set_charset(&qcu_n);
  set_charset(&qeb_g);
  set_charset(&qeb_n);
  set_charset(&qka_g);
  set_charset(&qpc_g);
  set_charset(&qpe_g);
  set_charset(&qpn_g);
  set_charset(&sux_g);
  set_charset(&sux_n);
  set_charset(&xhu_g);
  set_charset(&xur_g);
  set_charset(&uga_a);
  set_charset(&uga_g);
  set_charset(&qse_n);
    subdig_init();
}

void
charsets_term(void)
{
  free_charset(&akk_g);
  free_charset(&akk_n);
  free_charset(&arc_a);
  free_charset(&egy_a);
  free_charset(&egy_g);
  free_charset(&elx_g);
  free_charset(&grc_a);
  free_charset(&hit_g);
  free_charset(&peo_g);
  free_charset(&peo_n);
  free_charset(&qca_g);
  free_charset(&qcu_g);
  free_charset(&qcu_n);
  free_charset(&qeb_g);
  free_charset(&qeb_n);
  free_charset(&qka_g);
  free_charset(&qpc_g);
  free_charset(&qpe_g);
  free_charset(&qpn_g);
  free_charset(&sux_g);
  free_charset(&sux_n);
  free_charset(&xhu_g);
  free_charset(&xur_g);
  free_charset(&uga_a);
  free_charset(&uga_g);
  free_charset(&qse_n);
    subdig_term();
}

void
chartrie_init(struct charset*cp)
{
  int i;
  cp->to_uni = inctrie_create();
  for (i = 0; cp->keys[i].key; ++i)
    inctrie_insert(cp->to_uni,cp->keys[i].key,cp->keys[i].val,0);
}

void
chartrie_not(const char *str, const char *sofar,struct charset *cp,
	     const char *f, size_t l)
{
    if (!isspace(*sofar) && !chartrie_suppress_errors)
	mesg_verr(mesg_mloc(f,l),"%s: character '%c' (hex %x) not allowed in %s/%s",sofar,*sofar,*sofar,cp->lang,cp->mode);
}

void
charset_init_lang(struct lang_context *lp)
{
  enum t_modes i;
  for (i = 0; i < modes_top; ++i)
    {
      struct charset *cp = get_charset(lp->core->code,i);
      if (cp)
	{
	  if (cp->keys)
	    chartrie_init(cp);
	  lp->cset = cp;
	}
      else
        {
	    /* FIXME: we can't issue a diagnostic here, but we should
                when a lang/mode is actually used */
	  /*cp = get_default_charset();*/
	}
    }
}

