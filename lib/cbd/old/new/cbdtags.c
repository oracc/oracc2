/* C code produced by gperf version 3.0.3 */
/* Command-line: /Library/Developer/CommandLineTools/usr/bin/gperf -N cbdtags -tT cbdtags.g  */
/* Computed positions: -k'2-3' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "cbdtags.g"

#include <string.h>
#include "cbd.h"
#include "cbd.tab.h"

#define TOTAL_KEYWORDS 29
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 9
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 49
/* maximum key range = 47, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register size_t len;
{
  static unsigned char asso_values[] =
    {
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 25,
      50, 50, 50, 50, 50, 50, 50, 20, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50,  5,  3, 20,  0,
      50, 15, 11, 25,  0,  0,  1, 50,  0, 10,
      28, 10,  5, 20, 15, 15,  0, 20, 15, 20,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50
    };
  return len + asso_values[(unsigned char)str[2]+1] + asso_values[(unsigned char)str[1]];
}

struct cbdtag *
cbdtags (str, len)
     register const char *str;
     register size_t len;
{
  static struct cbdtag wordlist[] =
    {
      {""}, {""}, {""},
#line 18 "cbdtags.g"
      {"bib",   BIB, 0, 0},
#line 30 "cbdtags.g"
      {"disc",  EDISC, 0, 0},
#line 33 "cbdtags.g"
      {"discl", EDISCL, 0, 0},
#line 15 "cbdtags.g"
      {"alias", ALIAS, 0, 0},
      {""},
#line 8 "cbdtags.g"
      {"bases", BASES, 0, 0},
#line 22 "cbdtags.g"
      {"phon",  PHON, 0, 0},
#line 28 "cbdtags.g"
      {"pl_id",    PL_ID, 0, 0},
#line 29 "cbdtags.g"
      {"pl_uid",   PL_UID, 0, 0},
      {""},
#line 27 "cbdtags.g"
      {"pl_coord", PL_COORD, 0, 0},
#line 31 "cbdtags.g"
      {"file",  FILE, 0, 0},
#line 16 "cbdtags.g"
      {"allow", ALLOW, 0, 0},
#line 24 "cbdtags.g"
      {"stems", STEMS, 0, 0},
      {""},
#line 20 "cbdtags.g"
      {"oid",   OID, 0, 0},
#line 23 "cbdtags.g"
      {"root",  ROOT, 0, 0},
#line 11 "cbdtags.g"
      {"isslp", ISSLP, 0, 0},
      {""}, {""},
#line 13 "cbdtags.g"
      {"parts", PARTS, 0, 0},
#line 25 "cbdtags.g"
      {"prop",  PROP, 0, 0},
#line 21 "cbdtags.g"
      {"collo", COLLO, 0, 0},
      {""}, {""},
#line 26 "cbdtags.g"
      {"proplist", PROPS, 0, 0},
#line 9 "cbdtags.g"
      {"form",  FORM, 0, 0},
#line 10 "cbdtags.g"
      {"sense", SENSE, 0, 0},
      {""}, {""},
#line 34 "cbdtags.g"
      {"gwl",   GWL, 0, 0},
#line 17 "cbdtags.g"
      {"note",  NOTE, 0, 0},
#line 36 "cbdtags.g"
      {"notel", NOTEL, 0, 0},
      {""}, {""},
#line 12 "cbdtags.g"
      {"inote", INOTE, 0, 0},
#line 14 "cbdtags.g"
      {"bff",   BFF, 0, 0},
#line 19 "cbdtags.g"
      {"equiv", EQUIV, 0, 0},
      {""}, {""}, {""},
#line 32 "cbdtags.g"
      {"translang", TRANSLANG, 0, 0},
      {""}, {""}, {""}, {""},
#line 35 "cbdtags.g"
      {"i18n",  I18N, 0, 0}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
