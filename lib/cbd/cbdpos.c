/* C code produced by gperf version 3.0.3 */
/* Command-line: /Library/Developer/CommandLineTools/usr/bin/gperf -G -W cbdpos_wl -N cbdpos -tT cbdpos.g  */
/* Computed positions: -k'1-3' */

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

#line 1 "cbdpos.g"

#include "cbdyacc.h"
#include "cbd.h"

#define TOTAL_KEYWORDS 57
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 3
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 163
/* maximum key range = 163, duplicates = 0 */

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
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164,   5, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164,  18,  11,   2,  21,  50,
        7,  13,   8,   3,   1,  52,  46,  62,   0,  10,
        5,  20,  40,  30,  60,  31,  27,  37,  57,  17,
       47,  42, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164,   5, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164,   0, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
      164, 164, 164, 164, 164, 164, 164
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]+1];
        break;
    }
  return hval;
}

static struct cbdpos cbdpos_wl[] =
  {
    {""},
#line 28 "cbdpos.g"
    {"M"},
#line 45 "cbdpos.g"
    {"MN"},
#line 41 "cbdpos.g"
    {"IN"},
#line 34 "cbdpos.g"
    {"BN"},
#line 40 "cbdpos.g"
    {"HN"},
#line 17 "cbdpos.g"
    {"O"},
#line 47 "cbdpos.g"
    {"ON"},
#line 12 "cbdpos.g"
    {"IP"},
#line 37 "cbdpos.g"
    {"EN"},
#line 39 "cbdpos.g"
    {"GN"},
#line 9 "cbdpos.g"
    {"N"},
#line 46 "cbdpos.g"
    {"NN"},
#line 33 "cbdpos.g"
    {"AN"},
#line 7 "cbdpos.g"
    {"AJ"},
#line 38 "cbdpos.g"
    {"FN"},
    {""},
#line 27 "cbdpos.g"
    {"NP"},
#line 58 "cbdpos.g"
    {"X"},
#line 59 "cbdpos.g"
    {"XN"},
#line 16 "cbdpos.g"
    {"MA"},
    {""},
#line 48 "cbdpos.g"
    {"PN"},
#line 35 "cbdpos.g"
    {"CN"},
#line 31 "cbdpos.g"
    {"XP"},
#line 14 "cbdpos.g"
    {"CNJ"},
    {""},
#line 13 "cbdpos.g"
    {"PP"},
#line 54 "cbdpos.g"
    {"U"},
#line 55 "cbdpos.g"
    {"UN"},
#line 50 "cbdpos.g"
    {"PNF"},
    {""},
#line 51 "cbdpos.g"
    {"RN"},
#line 53 "cbdpos.g"
    {"TN"},
#line 29 "cbdpos.g"
    {"MOD"},
    {""}, {""},
#line 19 "cbdpos.g"
    {"RP"},
#line 10 "cbdpos.g"
    {"V"},
#line 56 "cbdpos.g"
    {"VN"},
#line 8 "cbdpos.g"
    {"AV"},
    {""},
#line 49 "cbdpos.g"
    {"QN"},
#line 32 "cbdpos.g"
    {"NU"},
#line 61 "cbdpos.g"
    {"ZN"},
#line 63 "cbdpos.g"
    {"V/t"},
    {""},
#line 18 "cbdpos.g"
    {"QP"},
#line 43 "cbdpos.g"
    {"KN"},
#line 60 "cbdpos.g"
    {"YN"},
#line 62 "cbdpos.g"
    {"V/i"},
    {""},
#line 36 "cbdpos.g"
    {"DN"},
#line 15 "cbdpos.g"
    {"J"},
#line 42 "cbdpos.g"
    {"JN"},
    {""}, {""},
#line 11 "cbdpos.g"
    {"DP"},
#line 24 "cbdpos.g"
    {"PSP"},
#line 57 "cbdpos.g"
    {"WN"},
    {""}, {""},
#line 52 "cbdpos.g"
    {"SN"},
#line 22 "cbdpos.g"
    {"POS"},
#line 44 "cbdpos.g"
    {"LN"},
    {""}, {""}, {""},
#line 21 "cbdpos.g"
    {"PRP"},
    {""}, {""}, {""}, {""}, {""}, {""},
#line 26 "cbdpos.g"
    {"SBJ"},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 25 "cbdpos.g"
    {"PTC"},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""},
#line 23 "cbdpos.g"
    {"PRT"},
    {""}, {""}, {""}, {""}, {""},
#line 30 "cbdpos.g"
    {"REL"},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""},
#line 20 "cbdpos.g"
    {"DET"}
  };

struct cbdpos *
cbdpos (register const char *str, size_t len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = cbdpos_wl[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &cbdpos_wl[key];
        }
    }
  return 0;
}
