#ifndef OTF_DEFS_H_
#define OTF_DEFS_H_
#define xstrcmp(xa,xb) strcmp((ccp)(xa),(ccp)(xb))
#define xstrcpy(xa,xb) strcpy((char*)(xa),(ccp)(xb))
#define xxstrlen(xs) strlen((ccp)xs)
#define cc(xs) ((ccp)xs)
#define uc(xs) ((uccp)xs)
#define textid (atfp->pqx)
#endif/*OTF_DEFS_H*/
