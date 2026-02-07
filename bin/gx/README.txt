20260124 Notes on rebooting work on gx

* The cbd 2.0 schema page https://oracc.museum.upenn.edu/ns/cbd/2.0/
  is regarded as abortive work for the time being

* The target data format is describe in
  https://build-oracc.museum.upenn.edu/doc/help/glossaries/cbd2/index.html

* The rnc schema referenced at the and of cbd2/index.html, cbd-gx.rnc,
  is the new starting point for the reboot and is now in o2/lib/rnc/cbd.rnc

* At one point I was flirting with the idea of using lib/cat with gx
  but this is now considered abortive; instead gx will proceed using
  the native cbd.l/cbd.y in lib/cbd

* gx's XML output will be reimplemented using lib/joxer so that JSON
  output will also be built in; the JSON output must conform closely
  to the exising Perl-based JSON output for glossaries because it is
  used by some non-Oracc projects: UPDATE: JSON output is built-in but
  uses a separate joxer stream from XML output to ensure the JSON is
  identical with previous JSON output.

* Processing works in multiple phases:

  - (possibly construct a CBD from the corpus sigs)

  - (possibly build token data from corpus)

  - load the corpus-sigs CBD if any

  - load the 00lib/*.glo if any

  - merge 00lib/*.glo into corpus-sigs CBD as necessary

  - validate xrefs, parts, and rels and cross-link them to the entries

  - add a @form for any entry in @bases that is not in any @form

  - once all merging and field augmentation has been done, run cbd_kis to add stats

  - cbd_kis creates similar data whether or not there is a .kis file

    * each entry has an array of Field arrays, one for each field data
      type, with period also being treated as a field

    *
  
