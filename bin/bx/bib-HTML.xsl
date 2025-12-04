<?xml version='1.0'?>

<!-- Implement a mostly Chicago Manual of Style type A reference list
     for Oracc bib; input is the XML version of .bib files generated
     by bibx.

     Series formatting doesn't conform; instead series name and
     volume in parentheses are given with no `no.' or `vol.'

     This is a modified version of an old TEI biblStruct CMS A formatter
     from Oracc I.
 -->

<!DOCTYPE xsl:stylesheet [
<!ENTITY comma "<xsl:text xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>,</xsl:text>">
<!ENTITY dot "<xsl:text xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>.</xsl:text>">
<!ENTITY nbsp "<xsl:text xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>&#xa0;</xsl:text>">
<!ENTITY newline "<xsl:text xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>&#xa;</xsl:text>">
<!ENTITY space "<xsl:text xmlns:xsl='http://www.w3.org/1999/XSL/Transform'> </xsl:text>">
]>
<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:h="http://www.w3.org/1999/xhtml"
  xmlns:b="http://oracc.org/ns/bib/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="map-jrnser.xsl"/>

<xsl:variable name="lq" select="'&#x201c;'"/>
<xsl:variable name="rq" select="'&#x201d;'"/>

<xsl:param name="outmode" select="''"/>
<xsl:param name="project" select="''"/>
<xsl:param name="title"/>
<xsl:strip-space elements="*"/>
<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:variable name="ti">
    <xsl:choose>
      <xsl:when test="string-length($title) > 0">
        <xsl:value-of select="$title"/>
      </xsl:when>
      <xsl:when test="string-length(/b:title|/*/b:title) > 0">
        <xsl:value-of select="/b:title|/*/b:title"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>Bibliography</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="$outmode='div'">
      <xsl:call-template name="bib-reflist"/>
    </xsl:when>
    <xsl:otherwise>
      <html>
	<head>
	  <link rel="stylesheet" type="text/css" 
		href="/css/bib.css"/>
	  <title><xsl:value-of select="$ti"/></title>
	</head>
	<body>
	  <xsl:call-template name="bib-reflist"/>
	</body>
      </html>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="bib-reflist">
  <div class="bib-reflist">
    <xsl:apply-templates select="*/b:entry"/>
  </div>
</xsl:template>

<xsl:template match="b:entry">
  <xsl:choose>
    <xsl:when test="b:journal='NABU'">
      <xsl:call-template name="format-nabu"/>
    </xsl:when>
    <xsl:when test="@type='article' or @type='incollection'">
      <xsl:call-template name="format-article"/>
    </xsl:when>
    <xsl:when test="@type='book'">
      <xsl:call-template name="format-book"/>
    </xsl:when>
    <xsl:when test="@type='booklet'">
      <xsl:call-template name="format-booklet"/>
    </xsl:when>
    <xsl:when test="@type='collection'">
      <xsl:call-template name="format-collection"/>
    </xsl:when>
    <xsl:when test="@type='dataset'">
      <xsl:call-template name="format-dataset"/>
    </xsl:when>
    <xsl:when test="@type='inbook'">
      <xsl:call-template name="format-inbook"/>
    </xsl:when>
    <xsl:when test="@type='inproceedings'">
      <xsl:call-template name="format-inproceedings"/>
    </xsl:when>
    <xsl:when test="@type='mastersthesis'">
      <xsl:call-template name="format-mastersthesis"/>
    </xsl:when>
    <xsl:when test="@type='online'">
      <xsl:call-template name="format-online"/>
    </xsl:when>
    <xsl:when test="@type='phdthesis'">
      <xsl:call-template name="format-phdthesis"/>
    </xsl:when>
    <xsl:when test="@type='proceedings'">
      <xsl:call-template name="format-proceedings"/>
    </xsl:when>
    <xsl:when test="@type='report'">
      <xsl:call-template name="format-report"/>
    </xsl:when>
    <xsl:when test="@type='software'">
      <xsl:call-template name="format-software"/>
    </xsl:when>
    <xsl:when test="@type='techreport'">
      <xsl:call-template name="format-techreport"/>
    </xsl:when>
    <xsl:when test="@type='unpublished'">
      <xsl:call-template name="format-unpublished"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>bib-HTML.xsl: unhandled entry type `<xsl:value-of select="@type"/></xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="format-booklet">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-collection">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-dataset">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-inbook">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-inproceedings">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-mastersthesis">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-online">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-phdthesis">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-proceedings">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-report">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-software">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-techreport">
  <xsl:call-template name="format-generic"/>
</xsl:template>
<xsl:template name="format-unpublished">
  <xsl:call-template name="format-generic"/>
</xsl:template>

<xsl:template name="format-generic">
  <p class="generic" id="{@xml:id}" data-key="{@key}">
    <xsl:call-template name="fmt-init-names"/>
    &space;
    <xsl:call-template name="fmt-title"/>
    &dot;
  </p>
</xsl:template>

<xsl:template name="format-nabu">
  <p class="article nabu" id="{@xml:id}" data-key="{@key}">
    <xsl:call-template name="fmt-init-names"/>
    &space;
    <xsl:call-template name="fmt-title"/>
    &space;
    <xsl:call-template name="fmt-journal-name"/>
    &space;
    <span class="bib-jrn-issue">
      <xsl:value-of select="b:year"/>
      <xsl:text>/</xsl:text>
      <xsl:value-of select="b:number"/>
    </span>
    &space;
    <xsl:variable name="pp">
      <xsl:choose>
	<xsl:when test="contains(b:pages, '-')"><xsl:text>pp.</xsl:text></xsl:when>
	<xsl:otherwise><xsl:text>p.</xsl:text></xsl:otherwise>
      </xsl:choose>
      &nbsp;
    </xsl:variable>
    <span class="bib-jrn-pages"><xsl:value-of select="concat('[',$pp,b:pages,']')"/></span>
    &dot;
  </p>
</xsl:template>

<xsl:template name="format-article">
  <p class="article" id="{@xml:id}" data-key="{@key}">

  <xsl:call-template name="fmt-init-names"/>
  &newline;

  <xsl:value-of select="$lq"/>
    <xsl:value-of select="normalize-space(b:title)"/>
  <xsl:call-template name="maybe-add-period">
    <xsl:with-param name="prev-str" select="b:title"/>
  </xsl:call-template>
  <xsl:value-of select="$rq"/>
  <xsl:text> </xsl:text>
  &newline;

  <xsl:choose>
    <xsl:when test="string-length(b:volume)=0">
      <xsl:text>Pp. </xsl:text>
      <xsl:value-of select="b:pages"/>
      <xsl:text> in </xsl:text>
      <xsl:call-template name="fmt-bookinfo"/>
    </xsl:when>
    <xsl:otherwise>
      <span class="journal">
        <xsl:call-template name="map-jrnser">
          <xsl:with-param name="js" select="b:booktitle"/>
        </xsl:call-template>
      </span>
      <xsl:text> </xsl:text>
      <xsl:if test="b:volume">
        <xsl:value-of select="b:volume"/>
      </xsl:if>
      <xsl:if test="b:date">
        <xsl:text> (</xsl:text>
        <xsl:value-of select="b:date"/>
        <xsl:text>)</xsl:text>
      </xsl:if>
      <xsl:if test="b:pages">
        <xsl:text>: </xsl:text>
        <xsl:value-of select="b:pages"/>
      </xsl:if>
      <xsl:text>.</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  </p>
</xsl:template>

<xsl:template name="format-book">
  <p class="book" id="{@xml:id}" data-key="{@key}">

    <xsl:choose>
      <xsl:when test="@same-auth = 't'">
	<xsl:text>---------. </xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="fmt-author"/>
      </xsl:otherwise>
    </xsl:choose>
    &newline;
    
    <xsl:call-template name="fmt-bookinfo"/>
  </p>
</xsl:template>

<xsl:template name="fmt-bookinfo">
  <xsl:variable name="norm-boo">
    <xsl:choose>
      <xsl:when test="string-length(b:booktitle)>0">
	<xsl:value-of select="normalize-space(b:booktitle)"/>
      </xsl:when>
      <xsl:otherwise><xsl:value-of select="normalize-space(b:title)"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <span class="booktitle">
    <xsl:value-of select="$norm-boo"/>
    <xsl:call-template name="maybe-add-period">
      <xsl:with-param name="prev-str" select="$norm-boo"/>
    </xsl:call-template>
  </span>

  <xsl:if test="b:editor and b:author">
    <xsl:text> </xsl:text>
    &newline;
    <xsl:call-template name="fmt-names">
      <xsl:with-param name="names" select="b:editor"/>
      <xsl:with-param name="flags" select="1"/>
    </xsl:call-template>
  </xsl:if>

  <xsl:if test="b:series">
    <xsl:text> </xsl:text>
    &newline;
    <xsl:text>(</xsl:text>
    <xsl:call-template name="map-jrnser">
      <xsl:with-param name="js" select="b:series"/>
    </xsl:call-template>
    <xsl:text> </xsl:text>
    <xsl:value-of select="b:volume"/>
    <!--
    <xsl:if test="$sn2">
      <xsl:text> = </xsl:text>
      <xsl:call-template name="map-jrnser">
        <xsl:with-param name="js" select="$sn2"/>
      </xsl:call-template>
      <xsl:text> </xsl:text>
      <xsl:value-of select="$sv2"/>
      </xsl:if>
      -->
    <xsl:text>). </xsl:text>
  </xsl:if>

  <xsl:call-template name="trap-diss">
    <xsl:with-param name="strs" select="b:note"/>
  </xsl:call-template>

  <xsl:if test="b:publisher or b:location or b:date">
    <xsl:text> </xsl:text>
    &newline;
    <xsl:value-of select="b:location"/>
    <xsl:if test="b:publisher">
      <xsl:if test="b:location">
        <xsl:text>: </xsl:text>
      </xsl:if>
      <xsl:value-of select="b:publisher"/>
    </xsl:if>
    <xsl:if test="b:date">
      <xsl:if test="b:publisher or b:location">
        <xsl:text>, </xsl:text>
      </xsl:if>
      <xsl:value-of select="b:date"/>
    </xsl:if>
    <xsl:text>. </xsl:text>
  </xsl:if>

</xsl:template>

<xsl:template name="fmt-author">
  <xsl:choose>
    <xsl:when test="count(b:author) > 0">
      <xsl:call-template name="fmt-names">
        <xsl:with-param name="names" select="b:author"/>
      </xsl:call-template>
      <xsl:call-template name="maybe-add-period">
        <xsl:with-param name="prev-str" select="b:author[last()]"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="count(b:editor) > 0">
      <xsl:call-template name="fmt-names">
        <xsl:with-param name="names" select="b:editor"/>
        <xsl:with-param name="flags" select="1"/>
      </xsl:call-template>
<!--
      <xsl:call-template name="maybe-add-period">
        <xsl:with-param name="prev-str" select="b:editor[last()]"/>
      </xsl:call-template>
 -->
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="fmt-names">
        <xsl:with-param name="names" select="b:publisher"/>
        <xsl:with-param name="flags" select="2"/>
      </xsl:call-template>
      <xsl:call-template name="maybe-add-period">
        <xsl:with-param name="prev-str" select="b:publisher[last()]"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="fmt-init-names">
  <xsl:variable name="sames">
    <xsl:choose>
      <xsl:when test="@sameauth='1'"><xsl:text> sames</xsl:text></xsl:when>
      <xsl:otherwise><xsl:text/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="b:author">
      <xsl:call-template name="fmt-names">
	<xsl:with-param name="names" select="b:author"/>
	<xsl:with-param name="sames" select="$sames"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="b:editor">
      <xsl:call-template name="fmt-names">
	<xsl:with-param name="names" select="b:editor"/>
	<xsl:with-param name="sames" select="$sames"/>
        <xsl:with-param name="flags" select="1"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="b:publisher">
      <xsl:call-template name="fmt-names">
	<xsl:with-param name="names" select="b:publisher"/>
	<xsl:with-param name="sames" select="$sames"/>
        <xsl:with-param name="flags" select="2"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="fmt-names">
	<xsl:with-param name="names" select="document(/)/*/xsl:template[name='anon']"/>
	<xsl:with-param name="sames" select="$sames"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>

  <xsl:call-template name="maybe-add-period">
    <xsl:with-param name="prev-str" select="b:author[last()]"/>
  </xsl:call-template>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template name="anon"><b:name last="Anonymous"/></xsl:template>
  
<xsl:template name="fmt-journal-name">
  <span class="bib-journal"><xsl:value-of select="b:journal"/></span>
</xsl:template>

<xsl:template name="fmt-names">
  <xsl:param name="names"/>
  <xsl:param name="sames"/>
  <xsl:param name="flags"/>

  <span class="bib-names{$sames}">
    <xsl:for-each select="$names/b:name">
      <xsl:choose>
	<xsl:when test="position() > 1 and position() = last()">
          <xsl:text> and </xsl:text>
	</xsl:when>
	<xsl:when test="position() > 1">
          <xsl:text>, </xsl:text>
	</xsl:when>
      </xsl:choose>
      <xsl:value-of select="@last"/>
      <xsl:if test="string-length(@rest)>0">
	&comma;&space;
	<xsl:value-of select="@rest"/>
      </xsl:if>
    </xsl:for-each>
    <xsl:if test="$flags = 1">
      <xsl:text>, ed</xsl:text>
      <xsl:if test="count($names/b:name) > 1">
	<xsl:text>s</xsl:text>
      </xsl:if>
      <xsl:text>. </xsl:text>
    </xsl:if>
  </span>
</xsl:template>

<xsl:template name="fmt-title">
  <span class="bib-title">
    <xsl:value-of select="b:title"/>
    <xsl:if test="b:subtitle">
      <xsl:text>: </xsl:text>
      <span class="bib-subtitle"><xsl:value-of select="b:subtitle"/></span>
    </xsl:if>
  </span>
</xsl:template>

<xsl:template name="maybe-add-period">
  <xsl:param name="prev-str"/>
  <xsl:variable name="str" select="normalize-space($prev-str)"/>
  <xsl:if test="translate(substring($str,
                                    string-length($str)),
                          ',.:;?!',
                          '')">
    <xsl:text>.</xsl:text>
  </xsl:if>
</xsl:template>

<!-- 
 Try to spot Ph.D./M.A./D.Phil. dissertations/theses in a plain text
 string.  Dump the concatenation of the string nodes if no diss is found.
 -->
<xsl:template name="trap-diss">
  <xsl:param name="strs"/>
  <xsl:variable name="onestr">
    <xsl:for-each select="$strs">
      <xsl:if test="not(@type='abbrev') and not(@type='keys')">
        <xsl:value-of select="."/>
        <xsl:call-template name="maybe-add-period">
          <xsl:with-param name="prev-str" select="."/>
        </xsl:call-template>
        <xsl:text> </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="norm-str" select="translate(translate($onestr,
						            '.-',
							    ''),
						  'PHDMADISSLTE',
					   	  'phdmadisslte')"/>
  <xsl:if test="contains($norm-str,'diss') 
             or contains($norm-str,'thesis')">
    <xsl:text> </xsl:text>
    <xsl:choose>
      <xsl:when test="contains($norm-str,'phd')
                   or contains($norm-str,'dphil')">
        <xsl:text>Ph.D. dissertation. </xsl:text>
      </xsl:when>
      <xsl:when test="contains($norm-str,'ma')">
        <xsl:text>M.A. thesis. </xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$onestr"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

<xsl:template match="text()"/>

<!-- BUGS: [held over from the TEI version]

  series handling may be inadequate; need to support more than two?

  how about a journal volume that is also in a series?

  -->

</xsl:stylesheet>
