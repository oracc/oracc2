Cuneify Implementation Notes

These notes document the new implementation of Cuneify, the cfy executable.

ATF
===

In ATF %-codes can be used to control cuneify at anything from the
text to grapheme level.

A cfy %-code has the same format as a class key, i.e., it is a string
of the form, but with 'cfy' as the first element:

 cfy-FONT-FEAT-MAG-SCRIPT-SL

FONT is a single token associated with an Oracc cuneiform font, e.g., lak.

FEAT is an arbitrary list of OpenType Features, separated by
commas. The list may not contain hyphens, e.g., ss01 or ss02,cv02

MAG is a number-only percentage magnification, e.g., 150.

SCRIPT is an Oracc script type: early, middle, late [this will likely
be adjusted over time with more experience across the whole of
cuneiform].

SL is a sign list to use for links out from cuneified signs. It should
correspond to an Oracc project name, with '.' available as a shorthand
for the project within which the cuneified data lives.

The asterisk '*' can be used to inherit the member from the current
class data.

 %cfy-lak-ss02-150-early-.

This can be used to switch font sizes for glosses:

 %cfy-oobf-*-70-middle-osl

Font switching can be made easier by defining %-numbers in the project
configuration or the ATF protocols at the start of a text.  %-numbers
0..9 are reserved for Oracc system use, so these %-number switches
must be %10 or above:

 #cfy: %10 = %cfy-oobf-*-70-middle-osl
 #cfy: %11 = %cfy-oobf-ss01-70-middle-osl

Or in the config file:

 <option name="cfy-switch" value="%10=%cfy-oobf-*-70-middle-osl"/>

Two shorthands are predefined: %- means 10% smaller; %+ means 10%
larger.  Multiple '-' or '+' increase each by 10%: %-- means 20%
smaller; %+++ means 30% larger'.  This is implemented relative to the
current magnification, so if the current mag is 150 %--- means
(150-30)% = 120%.

The ATF processor tracks the current class data and attaches it to the
XTF at key points.  It is always attached on the root node, i.e.,
transliteration, composite, or score, and to each line.  When it
changes within a line it is also attached to the next node that has a
@utf8 attribute.  The switch %% terminates an inline %-cfy switch and
restores the previous class data.

This approach means that Cuneify does not have to manage a state stack
of class data.

Ligatures
=========

Cuneify loads ligature tables associated with fonts and ensures that
characters that belong to ligatures are emitted within a single node
to ensure that the ligatures are recognized.

To break up ligatures use the following in ATF or in the sub table:

	\- = ZWS
	\  = ZWS

To request a discretionary ligature use

	+  = ZWJ

***should every sign list sequence be a ligature in the font?


Formatting
==========

Cuneify provides some basic support for emulating the format
characteristics of cuneiform manuscripts, but it is not intended as a
general purpose solution for producing facsimiles.

