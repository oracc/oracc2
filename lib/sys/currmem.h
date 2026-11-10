#ifndef CURRMEM_H_
#define CURRMEM_H_
#ifdef linux
#define currmem currmem_linux
#define currbytes (uint64_t)0
extern void currmem_linux(FILE *fp);
#else
#define currmem currmem_macos
#define currbytes currbytes_macos
extern void currmem_macos(FILE *fp);
uint64_t currbytes_macos(void);
#endif
#endif/*CURRMEM_H_*/
