<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:g="http://oracc.org/ns/gdl/1.0"
    exclude-result-prefixes="sl dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<xsl:param name="title"/>
<xsl:param name="index"/>

<xsl:include href="sxweb-util.xsl"/>

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="counts">
  <esp:page
      xmlns="http://www.w3.org/1999/xhtml"
      >
    <esp:name><xsl:value-of select="concat('Only in ',$title)"/></esp:name>
    <esp:title><xsl:value-of select="concat('Only in ',$title)"/></esp:title>
    <html>
      <head/>
      <body>
	<table>
	  <xsl:for-each select="o/c/r[c[1]=$index]">
	    <xsl:if test="not(c[2]='0') and count(../r[c[2]='0'])=3">
	      <xsl:variable name="xid" select="ancestor::o/@xml:id"/>
	      <xsl:for-each select="document('sl.xml',/)">
		<xsl:variable name="n" select="id($xid)"/>
		<tr>
		  <td><xsl:value-of select="$xid"/></td>
		  <td><xsl:value-of select="$n/@n"/></td>
		  <td><xsl:value-of select="$n/sl:uage"/></td>
		  <td>
		    <xsl:for-each select="$n/sl:images/sl:i[@loc][1]">
		      <xsl:call-template name="esp-sign-image"/>
		    </xsl:for-each>
		  </td>
		</tr>
	      </xsl:for-each>
	    </xsl:if>
	  </xsl:for-each>
	</table>
      </body>
    </html>
  </esp:page>
</xsl:template>

</xsl:transform>
