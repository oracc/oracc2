<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:output method="text" encoding="UTF-8"/>

  <!-- Generate a .oin file from AFDKO ttx output -->

  <xsl:template match="/">
    <!--List ligature components in the form NAME <- SUBS-->
    <xsl:for-each select="*/*/LookupList//LigatureSet">
      <xsl:for-each select="Ligature">
	<xsl:value-of select="@glyph"/>
	<xsl:text> &lt;- </xsl:text>
	<xsl:value-of select="concat(../@glyph,'_',translate(@components,',','_'))"/>
	<xsl:text>&#xa;</xsl:text>
      </xsl:for-each>
    </xsl:for-each>
    <xsl:for-each select="*/cmap/cmap_format_12[@platformID='0']">
      <xsl:for-each select="map">
	<xsl:value-of select="concat(@code,'&#x9;',@name,'&#xa;')"/>
      </xsl:for-each>
    </xsl:for-each>
    <!--
	<xsl:for-each select="*/*/LookupList//Substitution">
	<xsl:value-of select="concat(@out,'&#xa;')"/>
	</xsl:for-each>
      -->
    <xsl:for-each select="*/*/LookupList//Alternate">
      <xsl:value-of select="concat(@glyph,'&#xa;')"/>
    </xsl:for-each>
  </xsl:template>
</xsl:transform>
