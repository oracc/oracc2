<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    exclude-result-prefixes="sl"
    version="1.0">

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:if test="not(@sort)">
      <xsl:if test="@xml:id">
	<xsl:variable name="xid" select="@xml:id"/>
	<xsl:for-each select="document('sl.xml',/)">
	  <xsl:variable name="n" select="id($xid)"/>
	  <xsl:if test="$n">
	    <xsl:copy-of select="$n/@sort"/>
	  </xsl:if>
	</xsl:for-each>
      </xsl:if>
    </xsl:if>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:transform>
