OOD: Oracc Open Data
====================

OOD is a simple way of putting arbitrary XML-representable datasets
online in Oracc.

Each instance is an ood; each ood must be a project of its own because
the set up uses the 'cat' index as the index and 'home.xml' for basic
documentation.

At a minimum, an ood requires a 00lib directory with config.xml and
data.xml, and outlined.lst, and a 00web directory with home.xml and
attendant esp2 files.

By convention, the ood live in a project directory named 'ood', with a
subdirectory for each ood:

For the 'nimrud' project the basic ood layout is:

./ood:
niod

./ood/niod:
00lib	00web

./ood/niod/00lib:
config.xml	data.xml	outlined.lst

./ood/niod/00web:
00config	home.xml

./ood/niod/00web/00config:
parameters.xml	structure.xml

CONFIG.XML
==========

For an ood project the <type> field in config.xml must be 'ood'.

To switch off left menu:

 outline-default-leftmenu no
 outline-default-select no

DATA.XML
========

The file data.xml is designed to be easy to generate from a tabbed or structured file.

Data is wrapped in o:records with the ood namespace and project name:

<o:records xmlns:o="http://oracc.org/ns/ood/1.0" o:project="nimrud/ood/noid">
...
</o:records>

Each record is wrapped in o:record with an XML ID and two ood
administrative fields: o:id and o:n.

The field o:id gives an integer serial number for the record which is
permanent and unchanging; once an ood is made public the source data
should be managed in such a way that o:id cannot change.  For example,
deleted records could be marked as deleted rather than physically
deleted; they could be skipped when processed into data.xml but they
would be retained in the running count of records so the o:id remained
consistent.  New records should be processed at the end of the
data.xml input.

Note that the order in which records are processed into data.xml is
independent of the order in which they are displayed--that is managed
via Oracc's catalogue sort/display layer.

The field o:n gives a name to use as the record designation.

The remaining fields can be any legal XML NCTOKEN.

The first record in ood/niod/00lib/data.xml is:

<o:record xml:id="o0001">
    <o:id>0001</o:id>
    <o:n>Unnumbered tablet</o:n>
    <ND/>
    <ND2/>
    <Location/>
    <Quantity/>
    <Object>tablet</Object>
    <Material>clay</Material>
    <Provenance1>GP</Provenance1>
    <Provenance2>Room K</Provenance2>
    <Description>Tablet</Description>
    <Dimensions/>
    <Publication>CTN II no. 024; see on ND 480 for this unnumbered fragment</Publication>
    <Date/>
  </o:record>
  

CREATING DATA.XML
=================

If your data is in XML or TSV it is fairly easy to create data.xml.
Name your file with the appropriate extension and run:

 ood-data-dot-xml.sh [field-name]

A .tsv file must have the field-names in the first row, and the first
column must have the field name o:n and contain the name the record
should be known by.

A .xml file must have a field that can be used as the o:n field.  If
the data has a suitable field you can give the name of that field:

 ood-data-dot-xml.sh designation

Otherwise, you must create such a field and use the appropriate name.

One restriction on input data is that it may not have a field with a
local-name() of 'id' because that would conflict with the use of o:id.
The names are case-sensitive, so 'Id' or 'ID' are allowable.

The same applies to fields named 'n': a data set must rename any field
whose local-name() is 'n'--'N' would be allowable.


KEYDATA.XML
===========

Although this is strictly speaking optional, in practice it is much
better to use 00lib/keydata.xml to provide sortability of fields.

Once you have 00lib/data.xml you can run:

 ood-keydata-dot-xml.sh

To create a draft 00lib/keydata.xml and edit that as necessary.


CHANGES FOR Oracc II CX
=======================

For cx, ood must provide .tsv in 00cat (the indexed version) and
data.xml in 02pub (the displayed version).

