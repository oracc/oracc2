#ifndef GDLSTREAM_H_
#define GDLSTREAM_H_

/* Every word in GDL is part of a stream with some streams being
 * predefined and others able to be defined by users.  Streams are
 * annotated on g:w elements in XTF data and are separated into
 * separated trees in the XCL data.
 *
 * Alignment between streams is rudimentary: for intra-line streams
 * (e.g., document-level glosses), alignment is done via anchors to
 * the preceding and following host stream words.  For column-based
 * streams, alignment is to the sibling columns.  For line-based
 * streams, alignment is to the MTS line.  More precise alignment is
 * feasible using ATF's alignment groups, but that is for future
 * implementation.
 *
 * The basic stream is S1 (S0 is reserved for language-neutral
 * cuneiform), and is the stream for the language defined in #atf:
 * lang LANGUAGE. If there is no #atf: lang this is Sumerian.
 *
 * User definitions of streams have the form:
 *
 * #atf: stream SN <token> <shifts>
 *
 * or
 *
 * #atf: stream SN-N <token>
 *
 * Where <token> is a short, one-word, descriptive token; for SN
 * <token> will usually be a language code.
 *
 * The <shifts> element is a list of languages that select the stream,
 * reducing the need for explicit stream-shift codes.  For any
 * Sumerian stream, the default stream-shifts include all
 * register/writing-system variations of Sumerian, i.e., sux-x-emesal
 * and sux-x-udgaln.  This can be overridden by giving an explict
 * #atf: stream protocol.
 *
 * A language shift does not necessarily change the stream: if the
 * language is a logogram language (e.g., %s or %a in Hittite context)
 * the stream does not change because the logograms are part of the
 * same language context.
 *
 * In #atf: use bilingual the second language is S2; use bilingual
 * will in future support giving a sequence of languages which will
 * each be a stream in order to handle multilingual texts. S2 .. S5
 * are reserved for additional multilingual streams, e.g., %sux %akk
 * %xhu lex texts
 *
 * In #atf: use lexical each field is its own stream, with S1 and S2
 * being the primary term and primary translation fields.
 *
 * When {( ... )} and {{ ... }} are used there is always a stream
 * switch, either to a predefined stream or a user-defined one.
 *
 * {{ ... }}: if a language-switch comes after {{ switch to the
 * 		language stream;
 *            if no language switch the gloss is taken as a variant
 *		gloss, S6;
 *	      if /{{[0-9]+:/ switch to the numbered stream.
 *
 * {( ... )}: switch to SN-8 = stream-oriented gloss (requires
 * 	      language shift) or SN-9 = document-oriented gloss.
 *
 * Predefined streams: NEW::this is stream_lang
 *
 *  S0 language-neutral (cuneiform writing system)
 *  S1 language1
 *  S2 language2
 *  S3 language3
 *  S4 language4
 *  S5 language5
 *  S6 user-language1
 *  S7 user-language1
 *  S8 user-language1
 *  S9 user-language1
 *
 * S0 is intended for use in sign-list texts.
 *
 * NEW::this is stream_mode (or maybe stream_reln (relation) would be better?)
 * 
 * Streams also have substream tags, which consist of a hyphen plus
 * digit, e.g., S1-1.  The substream annotates or complements the main
 * stream in some way, often as a gloss:
 *
 *  SN-0 unmarked stream
 *  SN-1 variant gloss (by definition a bifurcation of the same stream);
 *       lex main column; lex equivalences implemented using other S-langs.
 *  SN-2 lex sign value (S0-2); or user-defined (SN-2 where N > 0)
 *  SN-3 lex sign pronunciation (S0-3) or user-defined
 *  SN-4 lex sign "G" (S0-4) or user defined
 *  SN-5 lex sign name (S0-5) or user defined
 *  SN-5 lex word/phrase (i.e., equivalent in same language as SN-1)
 *  SN-6 lex contained sign
 *  SN-7 reserved
 *  SN-8 stream-oriented gloss, e.g., he2-pi
 *  SN-9 document-oriented gloss, e.g., 10-marks
 *
 * Note that ATF also has a discourse chunk mechanism which is a layer
 * between the document and streams. A year-formula, for example, is a
 * discourse feature in the document, not a separate stream.
 *
 * Streams can be stored in integers as small as a char; for a
 * variable S, the main stream is given by S/10; the substream is
 * given by S%10.
 *
 * In XTF/XCL, streams are encoded in attributes as defined in
 * lib/rnc/stream.rnc: the stream tag is in s:str and the substream
 * tag in s:sub.  These are placed on g:w and analogous nodes in XTF
 * and in top-level xcl:c nodes in XCL, which now allows an array of
 * xcl:c as children of xcl:xcl.
 *
 */

enum gdlstream_e { GDL_S0 , GDL_S1 , GDL_S2 , GDL_S3 , GDL_S4 , GDL_S5 ,
		   GDL_S6 , GDL_S7 , GDL_S8 , GDL_S9 }; 

extern char strm_pop(void);
extern void strm_push(char s);
extern void strm_reset(void);
extern void strm_term(void);
const char *strm_str(unsigned char s);
const char *strm_sub(unsigned char s);

#endif/*GDLSTREAM_H_*/
