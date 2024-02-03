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
 * levels. Multiple levels, when present, are joined with periods
 * ('.').
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
 * has the input as the key and the output as the value.  The metadata
 * for a script-type may include font name and selector information as
 * well as keys that will select the script-type.
 * 
 */

/* CONFIGURATION 
 *
 * cuneify configuration is carried out by loading one or more config
 * files; if a later config file has data for an script-type/input
 * that is also in an earlier config file, the data from the late file
 * overrides the earlier data.
 */
