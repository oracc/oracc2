#include <oraccsys.h>

/**cunx -- cuneify data in a configurable way to produce renderings
 *  that are sensitive to differences in glyph form; output data is
 *  not suitable for linguistic processing because it uses character
 *  substitution for rendering effects. 
 */

/* SCRIPT MODEL
 *
 * cuneify's script model is hierarchical; we use the term
 * 'script-type' to denote a specification of a script. A script-type
 * is only required to have one level but may have up to four
 * levels. Multiple levels, when present, are joined with hyphens
 * ('-').
 *
 * In ATF a script-type begins with a percent sign, '%'; 
 *
 * The top level is Sumero-Akkadian Cuneiform (SAC); if no script-type
 * has been specified this is the default script-type.  SAC is
 * normally omitted from script-type specifications.
 *
 * The second level is PHASE: a general term for a recognized period
 * of SAC development, e.g.:
 *
 * 	3M Third Millennium
 *	ED Early Dynastic
 *	AK Old Akkadian
 *	GU Gudea
 *	U3 Ur III
 *	OA Old Assyrian
 *	OB Old Babylonian
 *	NA Neo-Assyrian
 *	NB Neo-Babylonian
 *	
 * The third level is FORM: a subversion of a PHASE. FORM is
 * project-defined and although it will often be organized according
 * to time or place there is no restriction on the organizing
 * criteria.
 *
 * FORM names must be three-letters; SAC is a reserved word and may
 * not be used as a FORM name.  The first letter must be in uppercase;
 * remaining letters may be in any case and the names are
 * case-specific.
 *
 * The fourth level is HAND: the script as written by one or more
 * individuals in one ore more manuscripts.  A HAND may be given as a
 * number or a name; HAND names must be four letters long.
 *
 */

/* DATA STRUCTURES
 * 
 * cuneify uses a collection of hashes of metadata, inputs and
 * outputs, one hash for each loaded script-type.  A script-type hash
 * has the input as the key and the render as the value.  The metadata
 * for a script-type may include font name and selector information as
 * well as keys that will select the script-type.
 * 
 */

/* PROCESSING PHASES
 *
 * cuneify works directly on Unicode-encoded SAC; cuneify input that
 * is in transliteration is first converted to to Unicode SAC using
 * GDL routines.
 *
 * The resulting SAC sequence is then processed by the Ideographic
 * Variation Sequences library which uses metadata from the
 * environment--catalogue, project configuration, etc.--to add IVS
 * characters to indicate glyph variations that are
 * environment-specific.  For example, a common IVS decoration would
 * be to mark IM as being in the merged glyph form of NI2.
 *
 * The result with IVS may then optionally be processed again to
 * replace the IVS-decorated characters with a character or image that
 * corresponds to the IVS-selected glyph. The resultant data may no
 * longer be linguistically valid--the intent is to achieve a desired
 * display in the absence of suitable font support.
 *
 */

/* RENDERING
 *
 * cuneify rendering works two characters at a time, first checking to
 * see if there is an render specified for the pair--a common type of
 * pairing that has a render would be a character followed by an IVS.
 * Renders may be characters or images.
 *
 * If a render is an image it is placed on the output immediately.
 *
 * If a render is one or more characters they are placed at the front
 * of the input queue and reprocessed.
 *
 * When no render is found for the current pair, the first character
 * is checked; if there is a render for it that is either output, in
 * the case of an image, or placed at the front of the input queue, in
 * the case of character(s).
 *
 */

/* Ideographic Variation Sequences 
 *
 * IVS modifier characters range from e0100 to e01ef.  The proposed collection name is Oracc-SAC.
 *
 * Oracc-SAC defines IVSs for two reasons:
 *
 *    1. To support splits and mergers
 *    2. To support variant forms
 *
 * The following IVS uses are tentatively defined:
 *
 *    E0100 MERGER0
 *    E0101 MERGER1
 *    E0103 MERGER2
 *    E0103 MERGER3
 *    E0104 MERGER4
 *    E0105 MERGER5
 *    E0106 MERGER6
 *    E0107 MERGER7
 *    E0108 MERGER8
 *    E0109 MERGER9
 *
 *    E0110 SPLIT0
 *    E0111 SPLIT1
 *    E0113 SPLIT2
 *    E0113 SPLIT3
 *    E0114 SPLIT4
 *    E0115 SPLIT5
 *    E0116 SPLIT6
 *    E0117 SPLIT7
 *    E0118 SPLIT8
 *    E0119 SPLIT9
 *
 *    E0120 VFORM0
 *    E0121 VFORM1
 *    E0123 VFORM2
 *    E0123 VFORM3
 *    E0124 VFORM4
 *    E0125 VFORM5
 *    E0126 VFORM6
 *    E0127 VFORM7
 *    E0128 VFORM8
 *    E0129 VFORM9
 * 
 * The Oracc Ideographic Variation Database (OIVD) has the following fields:
 *
 * SIGN IVS GLYPH_NAME COMMENT
 *
 * The SIGN and IVS entries correspond to the first two fields in the
 * Unicode Ideographic Variation Database. The GLYPH_NAME is a
 * symbolic name for the glyph that can be used for the fourth field
 * of the IVD; the COMMENT explains what the glyph-form is.
 * 
 * Examples:
 * 

1214E E0100 Oracc-SAC IM_as_NI2 IM sign with form of NI2
1222C E0120 Oracc-SAC MU_KASKAL MU sign with KASKAL-style replacement for normal SHE-style component

 *    
 */


/* RENDER CONFIGURATION 
 *
 * cuneify render configuration tells cuneify what to output for a GLYPH_NAME:

IM_as_NI2	1224E
MU_KASKAL	mu_kaskal.png

 * future versions may support font-switches as part of the output.
 *
 */
