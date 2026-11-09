#ifndef CURRMEM_H_
#define CURRMEM_H_
#ifdef linux
#define currmem currmem_linux
extern void currmem_linux(FILE *fp);
#else
#define currmem currmem_macos
extern void currmem_macos(FILE *fp);
#endif
#endif/*CURRMEM_H_*/
