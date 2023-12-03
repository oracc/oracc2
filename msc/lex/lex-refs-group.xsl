<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:key name="refgroups" match="lex:data"
	 use="concat(../@xml:id,':',@id_text)"/>

<xsl:template match="lex:group[@xml:id]">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="refs"
   		         select="lex:data[generate-id(.)
		 	                  =generate-id(
					   key('refgroups',
					       concat(../@xml:id,':',@id_text)))
					   ]"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="lex:data" mode="refs">
  <xsl:variable name="p" select="@project"/>
  <lex:group type="refs" value="{@id_text}">
    <xsl:attribute name="xis">
      <xsl:for-each select="key('refgroups', concat(../@xml:id,':',@id_text))/*/lex:word">
	<xsl:value-of select="concat($p,':',@wref)"/>
	<xsl:if test="not(position()=last())">
	  <xsl:text>+</xsl:text>
	</xsl:if>
      </xsl:for-each>
    </xsl:attribute>
    <xsl:copy-of select="key('refgroups', concat(../@xml:id,':',@id_text))"/>
  </lex:group>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
