#ifndef NOTE_H_
#define NOTE_H_

enum note_status
{
  NOTE_REGISTERED,
  NOTE_REFERENCED
};

typedef struct note
{
  const unsigned char *tag; 	/* the tag in a text, e.g., 1 in
				   ^1^--this is optional */
  const unsigned char *mark; 	/* the note number in the text or run;
				   this is what is output as the note
				   number */
  int automark; 		/* this is the note number's index in the text */
  const unsigned char *text;
  const char *xid;		/* xml:id */
  enum note_status status;	/* registered and or referenced */
} Note;

const char *note_create_id(void);

extern void note_initialize(void);
extern void note_initialize_line(void);
extern void note_initialize_text(void);
extern void note_number_by_run(void);
extern int note_parse_tlit(struct node *parent, int current_level, unsigned char **lines);
extern const unsigned char *note_register_tag(const unsigned char *tag, struct node *parent);
extern void note_register_note(const unsigned char *mark, struct node *node);

#endif/*NOTE_H_*/
