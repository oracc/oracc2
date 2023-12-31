<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:o="http://oracc.org/ns/oracc/1.0"
		version="1.0">
  <xsl:template match="/">
    <provides>
      <xsl:for-each select="*/*[string-length(@o:id)>0]">
	<oid>
	  <xsl:attribute name="xml:id">
	    <xsl:value-of select="@o:id"/>
	  </xsl:attribute>
	</oid>
      </xsl:for-each>
    </provides>
  </xsl:template>
</xsl:stylesheet>
