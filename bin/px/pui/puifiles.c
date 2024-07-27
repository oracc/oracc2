/* C code produced by gperf version 3.0.3 */
/* Command-line: /Library/Developer/CommandLineTools/usr/bin/gperf -G -W puifiles_wl -N puifiles -tT puifiles.g  */
/* Computed positions: -k'5,7' */

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

#line 1 "puifiles.g"

#include <string.h>
#include "pui.h"

#define TOTAL_KEYWORDS 27
#define MIN_WORD_LENGTH 10
#define MAX_WORD_LENGTH 15
#define MIN_HASH_VALUE 10
#define MAX_HASH_VALUE 43
/* maximum key range = 34, duplicates = 0 */

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
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44,  0, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44,  0,  5,  5,
       0, 15, 25, 15, 44, 10, 44,  5, 15, 30,
       0,  5, 15, 44,  0,  5,  0, 20, 44,  0,
       0, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
      44, 44, 44, 44, 44, 44
    };
  return len + asso_values[(unsigned char)str[6]] + asso_values[(unsigned char)str[4]];
}

static struct puifilestab puifiles_wl[] =
  {
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""},
#line 15 "puifiles.g"
    {"p4head.xml", &p4head},
#line 8 "puifiles.g"
    {"p4error.xml", &p4error},
#line 27 "puifiles.g"
    {"p4nowhat.xml", &p4nowhat},
#line 28 "puifiles.g"
    {"p4whatnot.xml", &p4whatnot},
#line 9 "puifiles.g"
    {"p4controls.xml", &p4controls},
#line 26 "puifiles.g"
    {"p4back.xml", &p4back},
#line 32 "puifiles.g"
    {"p4oracc.xml", &p4oracc},
#line 12 "puifiles.g"
    {"p4search.xml", &p4search},
#line 33 "puifiles.g"
    {"p4article.xml", &p4article},
#line 16 "puifiles.g"
    {"p4dropdown.xml", &p4dropdown},
#line 13 "puifiles.g"
    {"p4searchbar.xml", &p4searchbar},
#line 11 "puifiles.g"
    {"p4icons.xml", &p4icons},
#line 23 "puifiles.g"
    {"p4nosrch.xml", &p4nosrch},
#line 22 "puifiles.g"
    {"p4unknown.xml", &p4unknown},
    {""},
#line 24 "puifiles.g"
    {"p4full.xml", &p4full},
#line 7 "puifiles.g"
    {"p4pager.xml", &p4pager},
#line 31 "puifiles.g"
    {"p4nolang.xml", &p4nolang},
#line 20 "puifiles.g"
    {"p4itemxtf.xml", &p4itemxtf},
    {""},
#line 25 "puifiles.g"
    {"p4plus.xml", &p4plus},
#line 17 "puifiles.g"
    {"p4debug.xml", &p4debug},
#line 10 "puifiles.g"
    {"p4footer.xml", &p4footer},
    {""},
#line 18 "puifiles.g"
    {"p4debugdiv.xml", &p4debugdiv},
#line 29 "puifiles.g"
    {"p4noproject.xml", &p4noproject},
#line 21 "puifiles.g"
    {"p4noxtf.xml", &p4noxtf},
#line 30 "puifiles.g"
    {"p4noitem.xml", &p4noitem},
    {""}, {""},
#line 14 "puifiles.g"
    {"p4html.xml", &p4html},
    {""}, {""},
#line 19 "puifiles.g"
    {"p4itemglo.xml", &p4itemglo}
  };

struct puifilestab *
puifiles (str, len)
     register const char *str;
     register size_t len;
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = puifiles_wl[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &puifiles_wl[key];
        }
    }
  return 0;
}
