#ifndef CDLI2XTF_H_
#define CDLI2XTF_H_
typedef struct xtfhead
{
  char *P;
  char *equals;
  char *lang;
  struct xtfline *lines;
  struct xtfline *last;
} xtfhead;

typedef struct xtfline
{
  char *text;
  struct xtfline *next;
} xtfline;

#endif/*CDLI2XTF_H_*/
