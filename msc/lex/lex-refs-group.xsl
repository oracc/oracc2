<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:key name="refgroups" match="lex:data"
	 use="concat(../@xml:id,':',@id_text)"/>

<xsl:template match="lex:group[@xml:id]">
  <xsl:variable name="data"
		select="lex:data[
			generate-id(.)
		 	=
			generate-id(key('refgroups',concat(../@xml:id,':',@id_text)))
			]"/>
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:if test="count($data)>0">
      <xsl:apply-templates mode="refs" select="$data"/>
    </xsl:if>
  </xsl:copy>
</xsl:template>

<xsl:template match="lex:data" mode="refs">
  <xsl:variable name="p" select="@project"/>
  <lex:group type="refs" value="{@id_text}">
<!--    <xsl:message>project = <xsl:value-of select="*[1]/@project"/></xsl:message>-->
    <xsl:copy-of select="@project"/>
    <xsl:copy-of select="@n"/>
    <xsl:attribute name="xis">
      <xsl:for-each select="key('refgroups', concat(../@xml:id,':',@id_text))/*/lex:word">
	<xsl:value-of select="concat($p,':',@wref)"/>
	<xsl:if test="not(position()=last())">
	  <xsl:text>+</xsl:text>
	</xsl:if>
      </xsl:for-each>
    </xsl:attribute>
    <xsl:for-each select="key('refgroups', concat(../@xml:id,':',@id_text))">
      <xsl:copy>
	<xsl:copy-of select="@xml:id|@label|@sref|@wref|@word|@base|@pos|@line"/>
	<xsl:copy-of select="*"/>
      </xsl:copy>
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
