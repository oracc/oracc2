<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <!-- Read a .ttx file and iterate over ll.xml to add la2-liga
       entries for each <l> node in ll.xml -->
  
  <xsl:output method="xml" encoding="UTF-8" indent="yes"/>

  <xsl:variable name="la2TTGlyph" select="//TTGlyph[@name='u121F2']"/>
  
  <xsl:variable name="la2mtx" select="//mtx[@name='u121F2']"/>
  
  <xsl:template match="GlyphOrder">
    <xsl:copy>
      <xsl:apply-templates/>
      <xsl:variable name="id" select="*[last()]/@id"/>
      <xsl:for-each select="document('ll.xml')/*/*">
	<GlyphID id="{$id+count(preceding-sibling::*)+1}" name="u121F2_{.}.liga"/>
      </xsl:for-each>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="glyf">
    <xsl:copy>
      <xsl:apply-templates/>
      <xsl:variable name="id" select="*[last()]/@id"/>
      <xsl:for-each select="document('ll.xml')/*/*">
	<TTGlyph name="u121F2_{.}.liga">
	  <xsl:for-each select="$la2TTGlyph">
	    <xsl:copy-of select="@xMin|@yMin|@xMax|@yMax"/>
	  </xsl:for-each>
	  <component glyphName="u121F2" x="0" y="0" flags="0x1000"/>
	  <component glyphName="{.}" x="240" y="-40" scale="0.75" flags="0x1000"/>
	  <!--<GlyphID id="{$id+count(preceding-sibling::*)+1}" name="u121F2_{.}.liga"/>-->
	</TTGlyph>
      </xsl:for-each>      
    </xsl:copy>
  </xsl:template>

  <xsl:template match="hmtx">
    <xsl:copy>
      <xsl:apply-templates/>
      <xsl:for-each select="document('ll.xml')/*/*">
	<mtx name="u121F2_{.}.liga">
	  <xsl:for-each select="$la2mtx">
	    <xsl:copy-of select="@width|@lsb"/>
	  </xsl:for-each>
	</mtx>
      </xsl:for-each>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="*|text()|comment()">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:transform>
