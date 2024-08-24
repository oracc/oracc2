nx -- number tool

nx is a tool to support the Oracc Numbers project, num
(http://oracc.org/num)

num covers both numbers and counting systems and measures and
measuring systems

num consists of several parts

 ns -- number systems

 nu -- number units

 ni -- number instance

 ng -- number grapheme

 nw -- number words

 nd -- number determiners

 nc -- number commodities

 no -- number other

 ne -- number environments
 
 np -- number parsing

 nq -- number quantity

 nx -- number conversion

 nr -- number rendering

 nz -- number stoppers


ns -- number system

A number system consists of a name,an optional base unit and
conversion ratio, optional ne constraints, and a nu specification.

The syntax of an ns is:

 '@' NAME BASE? CONV? ne* nu ni* '.'

Whitespace is obligatory after any of NAME, BASE, CONV, ne.

Whitespace is obligatory between nu and ni sequences.

Whitespace is not allowed between '@' and NAME.

Whitespace is optional after nu or the last ni.

NAME is a segmented string with each segment separated by a hyphen
('-').  By convention, the first segment is a count or measure type
and the remaining segments are qualifiers time/place/genre/extension.

The first NAME segment may have one or more GDL modifiers, e.g.,
@c or @f, to associate the system with the relevant number styles.

If a NAME has more than one segment, at least four segments must be
given--empty segments are indicated with '*', e.g.:

 @drycap-*-ur-*

The fifth segment, extension, is optional.

Control lists for the segment values may be defined in a future
version of the num data.

BASE is a single token which must occur as a STEP in the nu.

CONV is a modern metric unit which is the equivalent of 1*BASE. The
syntax of CONV is an integer or floating point number followed by
either 'u' to indicate a numeric equivalent or one of the following
metric unit abbreviations:

   cm ha g kg l m mm



nu -- number units

An nu is a sequence of one or more steps in a system. A step is a unit
and by definition a step-name is either a grapheme that can occur
within parentheses in an ATF number grapheme or a word for a unit.
Unit-words are always written with the CF (Citation-Form) in nu, e.g.,
sila is written sila rather than sila₃.  A GW (Guide-Word) may be
added for disambiguation if necessary, e.g., gun[weight].

Steps are connected by multipliers (mult). A mult is the number of
units on the right-hand step (rhs) that make up the unit on the
left-hand step (lhs). Steps in an nu progress from smallest on the
right to largest on the left, mimicking the sequence of graphemes on
the tablet.

The syntax of an nu is:

    STEP := STEP_NAME

    MULT := [0-9]+ | 'A' | 'B' | 'D'

    multstep := MULT '*' STEP

    nu := multstep ('=' multstep)*

A simple example would be this subsequence of sexagesimal numbers:

   n * ŋeš₂ = 6 * u = 10 * diš

The left-terminal multiplier is 'n' meaning that the numeric
multiplier is unbounded from the point of view of validation.

For unit-words that are preceded by an independent number sequence the
sequence is indicated by one of the uppercase codes 'A' (AŠ-system),
'B' (bisexagesimal system), or 'D' (DIŠ-system) before the multiplier
('B' is experimental and may be withdrawn):

  0A gur = 5 * bariga = 6 * ban₂ = D10 * sila₃ = D60 * giŋ₄

Whitespace is insignificant, so the above example could also be
written as n*geš₂=6*u=D10*diš.

A future implementation may support alternates in steps, e.g.,

 1(ŋešmin) = 2(ŋeš₂) | 12(u)



ni -- number instances

An ni is count-unit pair, where the unit represents a step.  For
number graphemes such as 1(aš), the ni consists of 1 and aš.  For unit
sequences such as 1(u) 1(diš) sila₃, the ni consists of 11 and
sila--note that the number has been normalized to decimal and the
grapheme form of sila₃ has been normalized to the CF sila.

Within an ns, a grapheme ni is defined with the following syntax:

 GRAPHEME>COUNT*STEP

This form can be used to normalize variant graphemic forms to valid ni:

 1(aš)=1*aš
 9(diš)~v=9*diš

A unit-word ni is defined with the following syntax:

 AŠ|DIŠ+WORD>STEP

This data is used bidirectionally by nx: if a sequence could be either
an AŠ-number or a DIŠ-number and is followed by a unit-word that
occurs in an ni definition the number-system is disambiguated
accordingly.  Type-example:

 AŠ+gur=gur

A generic set of ni is predefined in nx.  These are overridden by ni
that occur in an ns if that ns is in effect in the parsing context.



ne -- number environments

num processing occurs in an environment which may consist of zero or
more contexts and constraints.

Environment context may include any or all of the following:

     p -- project, i.e., an Oracc project name

     t -- text, i.e., a P/Q/X ID; a project/text pair may be given in
       	  the common PROJECT:PQXID notation

     n -- name, i.e., the display name for a PQXID

     l -- label, i.e., the value of the computed label for a line with
       	  whitespace sequences each replaced by a single underscore
       	  character, e.g., 'o i 1' in the form $l=o_i_1. No provision
       	  is made to pass the line or word ID as part of the context
       	  because they can be attached to the signatures in a token
       	  list.

     f -- ATF file locator in the form file:line (e.g., $f=sale.atf:1)
          or line (e.g., $f=2); if the filename begins with '/' or
          './' it is a full path to the file; otherwise the path must
          be constructed from the project and is assumed to be in
          '00atf'.

Environmental constraints have several types:

     m -- metadata such as time (mt), place (mp), genre (mg), language (ml)
     
     s -- system such as system name (s) unit (su); area (sa); dry
       	  capacity (sd); liquid capacity (sl) weight (sw). For some
       	  systems cuneiform metrology uses special signs to qualify
       	  the system explicitly, e.g., gur and gan₂

     c -- commodities, i.e., the head noun of the description of what
          is being counted or measured

     j -- juxtapositions, i.e., two or more ns which may be juxtaposed
       	  without any intervening commodities as happens with
       	  area::seed ratios

     r -- recipient, i.e., DN, PN, TN in offering or ration lists

An ne may be included in number definition or prefixed to a
token list. The syntax of an ne is:

      '$' (c|f|j|l|m|mt|mp|mg|ml|n|p|r|s|t) '=' "STRING"
    | '$' (su|sa|sd|sl|sw)



np -- number parsing

Parsing is carried out with nx which loads the nd data and uses it to
parse token lists.

A token list consists of zero or more ne followed by one or more nf, no or
nz tokens.

Both nf and no tokens take the form of Oracc signatures.

The signatures may be partial, consisting of just a grapheme, or they
may contain select additional information such as a CF[GW]POS
lemmatization, or they may be the complete signatures produced by ATF
parsing.

A signature may also have a word ID appended to it with the special
joiner '~~'.

A token list is parsed to identify any subsequences of graphemes that
conform to nd data.  A successful parse of a subsequence results in a
nq, a number quantity. A token list can contain more than one nq
because nx can be used to process anything from discrete numbers on a
command line to entire texts.



ng -- number grapheme

An ng is one of the tokens that can be part of an nu sequence and consists of
numeric graphemes such as 1(aš).



nw -- number words

An nw is a number word, another token that can be part of an nu sequence.



nd -- number determiners

Number determiners, nd, are tokens that indicate a metrological system
but are not part of the unit sequence. Examples include gan₂ and gur.



nc -- number commodities

Commodities are both number stoppers and part of the number
environment.  They normally come after a number, but may come before a
number determiner as in 1(aš) še gur.



no -- number other

An no is a token that can not be part of an nu sequence.



nz -- number stopper

An nz is a token that unambiguously concludes the end of a number
sequence and consists of the period character ('.').  When parsing ATF
the comma (field-delimiter) causes an nz to be inserted into the token
stream.  Where juxtaposed sequences are difficult to parse with
precision this can be used to ensure that nx finds the proper boundary.



nq -- number quantity

An nq consists of a parsed number and the set of ne which was matched
during the parse.  All environment context ne that are present are
assumed matched, i.e., they are automatically included in the nq.

Any ne which were not matched are omitted from the nq. If the token
list contained a commodity that will be included in the nq.

An nq may be serialized as an nq token signature or in the xlink form used
for the linkbases which are part of lemmatized text in Oracc.



nx -- number conversion

If BASE and CONV are both set in the ns, nx performs conversion of
ancient to modern values and included the result in the serialized
output.



nr -- number rendering

nx will produce simple step diagrams in xhtml and other formats; cases
where multiple options exist for a step will be handled in a future
version, possibly using SVG.


