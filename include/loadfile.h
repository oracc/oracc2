#ifndef _LOADFILE_H
#define _LOADFILE_H

void loadfile_prog(const char *p);
unsigned char **loadfile_lines3(unsigned const char *fname, size_t *nlines, unsigned char **fmem);
unsigned char **loadfile_lines(unsigned const char *fname, size_t *nlines);
unsigned char *loadfile(unsigned const char *fname, size_t *nbytes);
unsigned char *loadstdin(size_t *nbytes);
unsigned char *loadoneline(FILE*fp, size_t *nbytes);

#endif/*_LOADFILE_H*/
