		atfdatax : tool for working with .atf files

By default atfdatax works on 00atf/*.atf

Two main functionalities: creating atfdata.tab and editing sets of .atf files.

To create atfdata.tab (done by build process):

   atfdatax >[TABFILE]

To suppress output except for errors:

   atfdatax -n

=== DUPLICATES ===

To show duplicates:

   atfdatax -d

To remove duplicates

   atfdatax -S

This will leave the files to keep in atfdata.d and the split off,
removed bits, atf_split.d.

To move the files for keeping:

   mv atfdata.d/* 00atf

=== CDLI NUMBERS THAT HAVE MOVED ===

To fix CDLI numbers that have moved, first use
o2/msc/prj/cdli/cdli-dups.sh to create a file dups-ISODATE.tsv. Then:

   atfdatax -f dups-ISODATE.tsv

This will write a fixed set of atfs to atfd_fix.d; to check them do

   diff 00atf atfd_fix.d

To move the fixed files:

   mv atfd_fix.d/* 00atf

=== RETIRED ATFS ===

Texts that have been retired by CDLI may or may not redirect to
another number. The best approach is to go through the moved numbers
process first--which may end up creating new duplicates in the corpus.
Once that is done, any numbers that are reported as not being in the
CDLI catalogue are probably retired, and can be dropped using the same
mechanism as duplicate removal, but providing a list of the texts to
drop.  A drop list may contain more than one column; only the first
column is used.

Use the -D option and give the list of drops as the argument; this
automatically sets dups_mode and split_dups:

  atfdatax -D drop-20260613.lst

The dropped texts will be in atfd_split.d and the versions to keep and
use will be in atfdata.d
