#ifndef LEMMER_H_
#define LEMMER_H_
extern int verbose;

struct numtab { const char *name; int gur; };

extern struct numtab *numword(register const char *str, register size_t len);

#endif/*LEMMER_H_*/
