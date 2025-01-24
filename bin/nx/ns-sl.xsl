<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:nss="http://oracc.org/ns/numsys/1.0"
	       xmlns:sl="http://oracc.org/ns/sl/1.0"
	       >

  <xsl:output method="xml" encoding="UTF-8"/>

  <xsl:param name="sl" select="'/home/oracc/osl/02xml/sl.xml'"/>

  <xsl:key name="v" match="sl:v" use="@n"/>
  
  <xsl:template match="nss:step[@type='sign']">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates mode="sl"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="nss:inst" mode="sl">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:call-template name="add-sl-attr"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="*" mode="sl">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
  
  <xsl:template match="*|text()">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template name="add-sl-attr">
    <xsl:variable name="v" select="@n"/>
    <xsl:for-each select="document($sl)">
      <xsl:variable name="slv" select="key('v',$v)"/>
      <xsl:choose>
	<xsl:when test="count($slv)=1">
	  <xsl:copy-of select="$slv/@key"/>
	  <xsl:for-each select="$slv/../sl:ucun">
	    <xsl:copy-of select="@*"/>
	    <xsl:if test="$v='1(šar₂)' or $v='1(šargal)'">
	      <xsl:attribute name="feat"><xsl:text>ss02</xsl:text></xsl:attribute>
	    </xsl:if>
	    <xsl:attribute name="ucun"><xsl:value-of select="."/></xsl:attribute>
	  </xsl:for-each>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:message><xsl:value-of select="$v"
	  /> has <xsl:value-of select="count($slv)"/> nodes</xsl:message>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:template>
  
</xsl:transform>
