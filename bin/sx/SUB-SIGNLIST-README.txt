				SUB-SIGNLISTS

Oracc can create sign lists which are a subset of one of the system
sign lists, OSL or PCSL; we call these sub-signlists or subsls.

A subsl is derived from a corpus: the corpus may be an entire project,
i.e., 01bld/lists/xtfindex.lst, or it may be defined as a list of
qualified PQX numbers:

	gudea:Q000377
	gudea:Q000881
	gudea:Q000882
	gudea:Q000883

The first step is to run tokx on the list of inputs to generate
subsl.tok.  For subsls the tok lines of interest are the grapheme
token lines starting with 'g<TAB>':

	P       
	Y       xtf
	F       /home/oracc/bld/gudea/Q000/Q000377/Q000377.xmd  0       0
	T       gudea   Q000377 The building of Ninŋirsu's temple (Gudea, cylinders A and B)
	K       period  000049/Lagaš II
	K       sphase  000049/Lagaš II
	L       11200   Q000377.3       A i 1
	W       Q000377.3.1
	g       o0000565..ud    @gudea%sl:v/ud=UD--ud#wn%sux:#u%:#a1#a1#u0#+k-------    ud
	W       Q000377.3.2
	g       o0000099..an    @gudea%sl:v/an=AN--an#wn%sux:#u%:#a1#a1#u0#.k-------    an
	W       Q000377.3.3
	g       o0000279..ki    @gudea%sl:v/ki=KI--ki#wn%sux:#u%:#i1#i1#u0#.k-------    ki-a

In the 'g' line, column two is a gkey: a triple consisting of
SIGNOID.FORMOID.VALUE.

A pipeline in tok2tis.sh processes subsl.tok and produces several
outputs.  It gathers references into instance lists and stores them in
a dbi index, 02pub/tok; it writes a virtual ID file mapping gkeys to
the instance set IDs; and it outputs statistics data suitable for
inclusion in the signlist.  The stats data (01tmp/subsl.kis) has the
following format:

	t       o       22398   %
	t000304 o0001378..      3       0.0134
	t000647 o0000820..      3       0.0134
	t001232 o0002331.o0002330.      4       50
	t001047 o0001069.o0001066.      1       0
	t001019 o0000477..šeŋ₆  2       0.833
	t000888 o0000497..ŋidru 12      17.6

The first line is informational for other processors and must be
discarded for processing into the subsl.

The subsl is generated from column two of the .kis file, i.e., the
gkey; sx uses the key list to select which signs/forms/values are
output.

Mergers are handled in the subsl-generation phase by reading a merge
table, e.g., 00lib/gudea.mrg:

	AŠ₂ ZIZ₂
	BAD TIL
	DUR₂ KU
	...

The left entry is the head of the mergers--remaining signs are
considered 'mergers with' the head.

During this phase @merge entries are inserted in the subsl.  The
@merge entries are subsequently reprocessed when the subsl is later
subjected to the normal signlist processing.

A typical sx invocation to create a subsl looks like this:

	sx -K01tmp/csl.key -Pgudea

The -P (project-config) argument tells sx to load configuration
options from the gudea project config.xml; this enables subsl
generation options such as domain, merge tables, and fonts, in the
project config.

Once the subsl has been created, it is processed in the same manner as
the system sign lists: stats and lemmata can be added, and various
derivative lists may be created.

***TODO: HOW ARE MERGED INSTANCE SETS HANDLED?***
