<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:output method="text" encoding="UTF-8"/>

  <xsl:template match="/">
    <xsl:apply-templates select=".//LigatureSet"/>
    <xsl:apply-templates select=".//map"/>
    <xsl:apply-templates select=".//GlyphID[contains(@name,'.')]"/>
    <xsl:apply-templates select=".//TTGlyph[contains(@name,'.liga')]"/>
  </xsl:template>
  
  <xsl:template match="LigatureSet">
    <xsl:for-each select="*">
      <xsl:value-of select="@glyph"/>
      <xsl:text> &lt;- </xsl:text>
      <xsl:value-of select="translate(concat(../@glyph,',',@components), ',', '_')"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="map">
    <xsl:value-of select="@code"/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:template>
  
  <xsl:template match="map">
    <xsl:value-of select="@code"/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="GlyphID">
    <xsl:if test="not(contains(@name, '.liga')) or contains(@name, '.liga.')">
      <xsl:value-of select="@name"/>
      <xsl:text>&#xa;</xsl:text>      
    </xsl:if>
  </xsl:template>
  
  <xsl:template match="TTGlyph">
    <xsl:if test="component">
      <xsl:value-of select="@name"/>
      <xsl:text> -> </xsl:text>
      <xsl:value-of select="component/@glyphName"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
  </xsl:template>
  
  <xsl:template match="text()"/>
  
</xsl:transform>
