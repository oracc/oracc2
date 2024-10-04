/* C code produced by gperf version 3.0.3 */
/* Command-line: /Library/Developer/CommandLineTools/usr/bin/gperf -G -W cbdrws_wl -N cbdrws -tT cbdrws.g  */
/* Computed positions: -k'1-2' */

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

#line 1 "cbdrws.g"

#include "cbdyacc.h"
#include "cbd.h"

#define TOTAL_KEYWORDS 15
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 3
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 26
/* maximum key range = 25, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, size_t len)
{
  static unsigned char asso_values[] =
    {
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27,  5,  0, 14,  5,  3,
      10,  0, 27, 27, 27, 27, 27,  4, 10,  0,
      27, 27, 27, 14, 27,  0, 27, 27, 27,  5,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27
    };
  return len + asso_values[(unsigned char)str[1]] + asso_values[(unsigned char)str[0]];
}

static struct cbdrws cbdrws_wl[] =
  {
    {""}, {""},
#line 16 "cbdrws.g"
    {"OB", "akk-x-oldbab"},
#line 10 "cbdrws.g"
    {"UGN","sux-x-udganu"},
    {""},
#line 7 "cbdrws.g"
    {"EG", "sux"},
#line 18 "cbdrws.g"
    {"MB", "akk-x-midbab"},
#line 15 "cbdrws.g"
    {"OA", "akk-x-oldass"},
    {""}, {""},
#line 9 "cbdrws.g"
    {"ED", "sux-x-eardyn"},
#line 17 "cbdrws.g"
    {"MA", "akk-x-midass"},
#line 20 "cbdrws.g"
    {"NB", "akk-x-neobab"},
#line 12 "cbdrws.g"
    {"UNO","sux-x-unorth"},
    {""}, {""},
#line 21 "cbdrws.g"
    {"SB", "akk-x-stdbab"},
#line 19 "cbdrws.g"
    {"NA", "akk-x-neoass"},
    {""},
#line 8 "cbdrws.g"
    {"ES", "sux-x-emesal"},
    {""},
#line 14 "cbdrws.g"
    {"CA", "akk-x-conakk"},
#line 11 "cbdrws.g"
    {"SYL","sux-x-syllab"},
    {""}, {""}, {""},
#line 13 "cbdrws.g"
    {"CF", "akk"}
  };

struct cbdrws *
cbdrws (register const char *str, size_t len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = cbdrws_wl[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &cbdrws_wl[key];
        }
    }
  return 0;
}
