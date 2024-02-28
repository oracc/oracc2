ISP (Instance Set Pager) ARCHITECTURE
=====================================

Introduction
------------

The ISP is a successor to elements of P3 and the XIS resolver and
generalizes these previous components as well as completing the
URL-based API of the XIS resolver.

Background
----------

Oracc's token architecture consists of three parts:

 	1. Token extraction and expansion (tok)
	2. Token Instance Set (tis) compilation
	3. The Instance Set Pager subsystem (isp)

ISP works both with tis outputs and with any other list of Oracc list
of text, line, word, or grapheme IDs--collectively we call this suite
of IDs content IDs, or CIDs.

In the following, we say 'LIST' when we mean a name that identifies a
list of CIDs--this list could be a temporary list identified by a
tmpdir such as p3.fzCvhE, an XIS name such as akk.r00001, a TIS name
such as t2df223, or the name of a project-build or user-specified list
such as 'sansatf' or 'emar'.  Collectively we call this suite of IDs
list IDs, or LSIDs.

ISP Parameters
--------------

ISP operates with the following context parameters:

 PROJECT -- the project owning the LSIDS

 LIST -- an LSID providing a set of CIDs

 ZOOM -- a slice of the LIST selected by sorting and grouping
      	 according to the project's use of metadata; the default
      	 metadata fields are time/place/genre
 
 PAGE -- a page of the current ZOOM slice that is sent for display

 SIZE -- the number of instances per page

 MODE -- the UI mode for the display; MAXI is the traditional P3
      	 display; MINI is the reduced version utilized in instance
      	 popups

 UNIT -- the unit type for rendering instances -- UNIT, LINE, KWIC
 
 LANG -- the language for translations accompanying instances

 AAPI -- the A tag API: REST=construct URLs for server resolution;
      	 FILE=construct PATHS for client-based static file reading

ISP Sequence of Actions
-----------------------

ISP does its work in the following sequence of steps:

 1. LA--List acquisition
 2. LS--List sorting and grouping
 3. ZS--Zoom slicing
 4. PE--Page extraction
 5. WM--Word mapping
 6. CE--Context extraction
 7. OG--Output generation

ISP Toolchain
-------------

ISP is implemented in a collection of library routines with a related
suite of front-end programs meaning that processing can be done as a
typical filter or by building the libraries in to client programs.

