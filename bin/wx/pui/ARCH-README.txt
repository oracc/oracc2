P3 Architecture
===============

This README is a planning document for revision/documentation of the
P3 architecture.  Once implemented this file will be replaced by a
publically visible version.

Top Level Structure
===================

At the top level, a P3 page has three parts: a banner; a search form;
and a display area.

Banner
------

The banner is an HTML div with id p3Banner and contains the project
name and some project-related navigation buttons.

Search
------

The search is an HTML div with id p3Banner and contains the search
form and inputs related to elements of the display.

Display
-------

The display is an HTML div with id p3Display and contains a navigation
bar, the current content and an optional outline-menu.

Display Structure
=================

Navigation Bar
--------------

The navigation bar is a div with id p3NavBar and hosts the permutation
selector for an outline if there is one and the prev/next arrors and
page selector controls.

Outline
-------

The outline is a div with id p3Outline.

The first child is an optional div with id p3MetaPerm which provides a
dropdown with an option for each of the possible permutations of the
sort metadata for the outline.

The last child is an div with id p3Menu.  The menu contains a ul with
an li for each possible sort metadata heading.

Content
-------

The content is a div with id p3Content.

The first child is an optional div with id p3ContentNav which provides	
controls for navigating through an instance set (IS).

The last child is an required div with id p3ContentDiv which hosts the
content.  Content may be an individual item or a list of items.

Records and Instances
---------------------

The new P3 definition distinguishes two broad categories of
displayable data: records and instances.  The most obvious record is
an entry in a catalogue.  Glossary and signlist articles are also
records in this approach.  Text editions are also a form of record.

Records may contain links to other records, to pages which give
further data on the record (i.e., effectively sub-records), and to
instances or instances sets.

Subrecords should contain links to the parent record.

An instance is an occurrence of a textual data item--grapheme, word,
phrase, etc.--in a text.

Lists
-----

P3 works with two kinds of lists: record lists and instance sets.

A RECORD LIST may be a catalogue list with tabular display of selected
fields, a list of glossary articles by glossary-summary, or a list of
signs by sign-summary. An RL is by definition a list of record
IDs--either PQX in the case of catalogues, or OIDs in the case of
glossaries and signlists. An RL may be created by a search of a
catalogue, glossary, or signlist, may be precompiled for glossary and
signlist records, or may be given in a list file originating from a
project's 00lib/lists directory.

An INSTANCE SET is by definition a list of line, word, or grapheme
IDs.  An IS may be created by a search of a txt or lem index, may be
precompiled for glossary and signlist instances, or may be given in a
list file originating from a project's 00lib/lists directory.
