#ifndef CUN_H_
#define CUN_H_
struct perfnt
{
  const char *name;
  const char *fnt;
};
extern struct perfnt *perfnt (register const char *str, register size_t len);

#endif/*CUN_H_*/
