#include <oraccsys.h>

/**Use this instead of uni2utf
 */
unsigned char *
utf8ify(unsigned int c)
{
  static unsigned char buf[5];
  unsigned char *b = buf;
  memset(buf,'\0',5);
  if (c<0x80) *b++=c;
  else if (c<0x800) *b++=192+c/64, *b++=128+c%64;
  else if (c-0xd800u<0x800) goto error;
  else if (c<0x10000) *b++=224+c/4096, *b++=128+c/64%64, *b++=128+c%64;
  else if (c<0x110000) *b++=240+c/262144, *b++=128+c/4096%64, *b++=128+c/64%64, *b++=128+c%64;
  *b = '\0';
  return buf;
 error:
  return NULL;
}
