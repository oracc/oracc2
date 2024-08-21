nx -- number tool

nx is a tool to support the Oracc Numbers project, num
(http://oracc.org/num)

num covers both numbers and counting systems and measures and
measuring systems

num consists of several parts

 ns -- number systems

 nu -- number units:

 ne -- number environments
 
 np -- number parsing

 nq -- number quantity

 nc -- number conversion

 nr -- number rendering



ns -- number system

A number system consists of a name, optional ne constraints, and a nu.

The syntax of an ns is:

 '@' NAME BASE? CONV? ne* nu '.'

Whitespace is obligatory after any of NAME, BASE, CONV, ne.

Whitespace is not allowed between '@' and NAME.

Whitespace is optional after nu.

NAME is a segmented string with each segment separated by a hyphen
('-').  By convention, the first segment is a count or measure type
and the remaining segments are qualifiers time/place/genre/extension.

If a NAME has more than one segment, at least four segments must be
given--empty segments are indicated with '*', e.g.:

 @drycap-*-ur-*

The fifth segment, extension, is optional.

Control lists for the segment values may be defined in a future
version of the num data.

BASE is a single token which must occur as a STEP in the nu.

CONV is a modern metric unit which is the equivalent of 1*BASE. The
syntax of CONV is an integer or floating point number followed by one
of the following metric unit abbreviations:

   cm ha g kg l m mm



nu -- number units

An nu is a sequence of one or more steps in a system. A step is a unit
and. Steps are connected by multipliers (mult). A mult is the number
of units on the right-hand step (rhs) that make up the unit on the
left-hand step (lhs). Steps in an nu progress from smallest on the
right to largest on the left, mimicking the sequence of graphemes on
the tablet.

The syntax of an nu is:

    STEP := NUMBER_GRAPHEME

    MULT := [0-9]+ | 'n'

    multstep := MULT '*' STEP

    nu := multstep ('=' multstep)*

A simple example would be this subsequence of sexagesimal numbers:

   n * ŋeš₂ = 6 * u = 10 * diš

The right-terminal multiplier is 'n' meaning that the numeric
multiplier is unbounded from the point of view of validation.

Whitespace is insignificant, so the above example could also be
written as n*geš₂=6*u=10*diš.



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

An ne may be included in number definition or prefixed to a
token list. The syntax of an ne is:

      '$' "c|f|j|l|m|mt|mp|mg|ml|n|p|s|t" '=' VALUE
    | '$' "su|sa|sd|sl|sw"



np -- number parsing

Parsing is carried out with nx which loads the nd data and uses it to
parse token lists.

A token list consists of zero or more ne followed by one or more
grapheme or word signatures.  The signatures may be partial,
consisting of just a grapheme, or they may contain select additional
information such as a CF[GW]POS lemmatization, or they may be the
complete signatures produced by ATF parsing.

A signature may also have a word ID appended to it with the special
joiner '~~'.

A token list is parsed to identify any subsequences of
graphemes that conform to nd data.  A successful parse of a
subsequence results in a nq, a number quantity. A token list can
contain more than one nq because nx can be used to process discrete
numbers or entire texts.



nq -- number quantity

An nq consists of a parsed number and the set of ne which was matched
during the parse.  All environment context ne that are present are
assumed matched, i.e., they are automatically included in the nq.

Any ne which were not matched are omitted from the nq. If the token
list contained a commodity that will be included in the nq.

An nq may be serialized as an nq token signature or in the xlink form used
for the linkbases which are part of lemmatized text in Oracc.



nc -- number conversion

If BASE and CONV are both set in the ns, nx performs conversion of
ancient to modern values and included the result in the serialized
output.



nr -- number rendering

nx aspires to support automated generation of step diagrams from nd
data, probably by generating SVG.


