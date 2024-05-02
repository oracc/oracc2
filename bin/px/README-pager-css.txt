P4 Structural CSS
=================

The selection of which panes to display and which controls are active
is determined by class attributes on the top level P4 div, #p4Pager.

There are two data-source states:

 list -- data comes from a static list of some kind
 srch -- data comes from search results

There are three data-source types:

 dcat -- context-catalogue: data is from catalogue
 dtxt -- context-text: data is from text editions
 dtra -- context-translation: data is from a srch in the translation index
 dglo -- context-glossary: data is from a glossary

Each data-source type can be showing either a record list or an
item:

		record				item

cat		rcat: metadata summary table	itxt: text edition
txt		rtxt: tlit reference+labels	itxt: text edition
tra		rtra: tlat reference+labels	itxt: text edition
glo		rglo: article summary list	iart: glossary article

px places one each of list/srch, dcat/dtxt/dglo, and
rcat/rtxt/rtra/rglo/itxt/iart in p4Pager.class and the P4 CSS modulates the
control display and frame layout accordingly.

Page/Item Toggling
==================

When toggling to item from page the item is set from the control.
When toggling to page from item the current item is passed back to the
server with as bkmk= rather than item=. This allows the pager to set
up the zoom/page context appropriately but display a page rather than
an item.


