#ifndef _BLOCK_H
#define _BLOCK_H

extern int div_level;
extern unsigned char line_label_buf[];

extern const char * const roman[];

extern unsigned char **parse_block(unsigned char *lines);
extern struct node* scan_comment(unsigned char **lines,int *nlines,int badcolon);
extern unsigned char *scan_comment_sub(unsigned char **lines, int *nlinesp, int badcolon);
extern unsigned char **skip_rest(unsigned char **lines);
extern unsigned char *hash_hash_file;
extern unsigned int hash_hash_line;
extern int hash_hash_get_file(unsigned char *t);
extern int hash_hash_get_line(unsigned char *t);

#endif /*_BLOCK_H*/
