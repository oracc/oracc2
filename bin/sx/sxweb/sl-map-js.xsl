<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
    xmlns:xi="http://www.w3.org/2001/XInclude"
    exclude-result-prefixes="sl dc xh"
    version="1.0">

<xsl:output method="text" indent="yes" encoding="utf-8"/>
<xsl:variable name="dq">"</xsl:variable>
<xsl:template match="/">
  <xsl:text>const x={</xsl:text>
  <xsl:for-each select="(//sl:sign|//sl:v|//sl:list)[not(starts-with(@n,'|'))][not(contains(@n,'('))][not(contains(@n,$dq))]">
    <xsl:value-of select="concat($dq,@n,$dq,':',$dq,ancestor-or-self::sl:letter/@xml:id,'/',ancestor-or-self::sl:sign/@xml:id,$dq)"/>
    <xsl:if test="not(position()=last())">,&#xa;</xsl:if>
  </xsl:for-each>
  <xsl:text>};</xsl:text>
</xsl:template>

</xsl:transform>
