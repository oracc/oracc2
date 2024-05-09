Calling Sequence
****************

Restful Interface
=================

P4 is a restful interface to Oracc. All URLs are handled by the URL resolver, wx.

WX
==

wx parses the URLs and when the URL resolves to a file it returns the
file directly.

On error, wx returns a P4 error page.

If the URL contains a 'srch=' component, wx calls the query-engine,
wx, passing it the following CGI options:

    web=1 proj=PROJECT [glos=GLOSSARY] srch=SEARCH [debug]

The 'glos=' component is empty unless the search comes from a glossary
view. The 'debug' component is passed through from the query string
when present.

The URL only contains a 'srch=' component when the user has activated
the search bar in the pager; when paging the results of a search, the
original contents of the search bar are stored in data-qury and passed
down by wx in the 'qury=' component.

When the URL does not contain a 'srch=' component, wx calls the pager,
px, directly.  The possible CGI arguments to a px call are documented
on the P4 URL page.

QX
==

qx creates a P4 temporary directory and runs a search, leaving the
results in the file 'list' in the tmpdir--it is the equivalent of the
list creation that is done by px based on instance lists etc.

The mkdtemp basename template is s.XXXXXX; the tmpdir is created as
ORACC_BUILDS/www/is.d/PROJECT[/GLOS]/s.XXXXXX where [/GLOS] is only
present if the search originates from a glossary view.

qx also leaves its state data in the file tmpdir/sdata.

On error, qx returns a P4 error page. When the 'debug' CGI component
is given, qx deposits debug info in tmpdir/debug.

On success, qx calls the pager, px, with the following CGI options:

   web=1 s.d=TMPDIR [debug]

PX
==

px creates the view of a subset of a list.

On error, px returns a P4 error page. If the CGI option 'debug' is
present, a P4 debug page is returned.

On success, px returns a fully formed pager page with the state
encoded in 'data-XXXX' attributes which are mostly on top level div of
the pager proper, which has the ID #p4Pager.  Some data-XXXX
attributes are placed deeper in the content, where, for example, they
provide the ID of an item that will be displayed when a link is
activated.
