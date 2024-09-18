<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:g="http://oracc.org/ns/gdl/1.0"
    exclude-result-prefixes="sl dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<xsl:param name="overview-images" select="'font'"/>
  
<xsl:template name="esp-sign-image">
  <xsl:param name="esp-mode" select="true()"/>
  <xsl:variable name="ref" select="@ref"/>
  <xsl:variable name="header" select="/*/sl:iheader[@xml:id=$ref]"/>
  <xsl:choose>
    <xsl:when test="$esp-mode">
      <esp:image class="middle" height="50px" url="/{$header/@proj}/{$header/@path}/{@loc}"
		 description="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
    </xsl:when>
    <xsl:otherwise>
      <img height="50px" align="middle" src="/{$header/@proj}/{$header/@path}/{@loc}"
	   alt="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="esp-sign-thumb">
  <xsl:variable name="ref" select="@ref"/>
  <xsl:variable name="header" select="/*/sl:iheader[@xml:id=$ref]"/>
  <xsl:choose>
    <xsl:when test="string-length($header/@thumb)>0">
      <esp:image class="middle" url="/{$header/@proj}/{$header/@thumb}/{@loc}"
		 description="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
    </xsl:when>
    <xsl:otherwise>
      <esp:image class="middle" height="30px" url="/{$header/@proj}/{$header/@path}/{@loc}"
		 description="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="mcol" match="sl:sign">
  <!--<xsl:message>sl:sign <xsl:value-of select="@n"
      /> moid=<xsl:value-of select="@moid"/></xsl:message>-->
  <xsl:variable name="img" select="(.//sl:images/sl:i[@loc])[1]"/>
  <td>
    <xsl:attribute name="class">
      <xsl:choose>
	<xsl:when test="$overview-images='font' or count($img)>0 or .//sl:uage='4'">
	  <xsl:text>name-sign</xsl:text>
	</xsl:when>
	<xsl:otherwise><xsl:text>name-nosign</xsl:text></xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <esp:link>
      <xsl:attribute name="page">
	<xsl:choose>
	  <xsl:when test="@moid">
	    <!--<xsl:message>sl:sign page link using @moid <xsl:value-of select="@moid"/></xsl:message>-->
	    <xsl:value-of select="@moid"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="@xml:id"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:attribute>
      <xsl:choose>
	<xsl:when test=".//sl:uage='4'">
	  <p class="sl-td-name"><xsl:value-of select="@n"/></p>
	  <p class="sl-td-seq"><xsl:text>(Sequence)</xsl:text></p>
	</xsl:when>
	<xsl:otherwise>
	  <p class="sl-td-name"><xsl:value-of select="@n"/></p>
	  <xsl:choose>
	    <xsl:when test="$overview-images='font'">
	      <p class="sl-td-sign noto"><xsl:value-of select="sl:ucun"/></p>
	    </xsl:when>
	    <xsl:when test="count($img)>0">
	      <xsl:for-each select="$img">
		<p class="sl-td-sign">
		  <xsl:call-template name="esp-sign-thumb">
		    <xsl:with-param name="height" select="'40px'"/>
		  </xsl:call-template>
		</p>
	      </xsl:for-each>
	    </xsl:when>
	    <xsl:otherwise>
	      <p class="sl-td-sign noto"><xsl:value-of select="sl:ucun"/></p>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:otherwise>
      </xsl:choose>
    </esp:link>
  </td>
</xsl:template>

</xsl:transform>
