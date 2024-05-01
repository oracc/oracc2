o2/bin/xx/wrapper

The code here replaces the Perl modules ORACC/LIB/Wrapper.pm and
ORACC/LIB/Wrapper2.pm.

The core program, wrapper, is hard-linked to an array of program names
all of which apply an XSLT script to an XML input.  The wrapper sets
up the arguments, selects the correct script, applies it, and does the
appropriate output.

The wrapper handles applying the XSLT script to stdin, a named file, a
list of qualified PQX IDs, or one or more list files.

The basic invocation of libxslt is based on the tutorial at
http://xmlsoft.org/xslt/tutorial/libxslttutorial.html.

