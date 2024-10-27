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

<xsl:include href="g2-gdl-HTML.xsl"/>
  
<xsl:param name="overview-images" select="'font'"/>

<!--### Set all the asl-* config variables from config.xml -->
<xsl:variable name="q">'</xsl:variable>
<xsl:variable name="sl-config-xml" select="concat($oraccbuilds,'/xml/',$project,'/signlist/config.xml')"/>

<!--instances asl-insts=link = link to instances; asl-insts=xinc =
     xinclude instances. Instances default to xinc when stats is yes.-->
<xsl:variable name="asl-insts">
  <xsl:variable name="insts-default">
    <xsl:choose>
      <xsl:when test="$asl-stats='yes'">xinc</xsl:when>
      <xsl:otherwise>no</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="config-xml" select="$sl-config-xml"/>
    <xsl:with-param name="option" select="'asl-insts'"/>
    <xsl:with-param name="default" select="$insts-default"/>
  </xsl:call-template>
</xsl:variable>

<!--lexdata asl-lexdata=link = link to lexdata; asl-lexdata=xinc = xinclude lexdata-->
<xsl:variable name="asl-lexdata">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="config-xml" select="$sl-config-xml"/>
    <xsl:with-param name="option" select="'asl-lexdata'"/>
    <xsl:with-param name="default" select="'no'"/>
  </xsl:call-template>
</xsl:variable>

<!--snippets asl-snippets=link = link to snippets; asl-snippets=xinc = xinclude snippets-->
<xsl:variable name="asl-snippets">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="config-xml" select="$sl-config-xml"/>
    <xsl:with-param name="option" select="'asl-snippets'"/>
    <xsl:with-param name="default" select="'no'"/>
  </xsl:call-template>
</xsl:variable>

<!--suxword asl-suxword=link = link to words; asl-suxword=xinc = xinclude words-->
<xsl:variable name="asl-suxword">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="config-xml" select="$sl-config-xml"/>
    <xsl:with-param name="option" select="'asl-suxword'"/>
    <xsl:with-param name="default" select="'no'"/>
  </xsl:call-template>
</xsl:variable>

<!--statistics for occurrences; these are embedded in the headers for each sign/form/value name-->
<xsl:variable name="asl-stats">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="config-xml" select="$sl-config-xml"/>
    <xsl:with-param name="option" select="'asl-stats'"/>
    <xsl:with-param name="default" select="'no'"/>
  </xsl:call-template>
</xsl:variable>

<!--period stats default=yes when stats=yes; asl-periods=no can be used to switch periods para off-->
<xsl:variable name="asl-periods">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="config-xml" select="$sl-config-xml"/>
    <xsl:with-param name="option" select="'asl-periods'"/>
    <xsl:with-param name="default" select="$asl-stats"/>
  </xsl:call-template>
</xsl:variable>

<xsl:variable name="asl-script">
  <xsl:message>sl-config-xml in asl-script set = <xsl:value-of select="$sl-config-xml"/></xsl:message>
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="config-xml" select="$sl-config-xml"/>
    <xsl:with-param name="option" select="'asl-script'"/>
    <xsl:with-param name="default" select="'ofs-noto'"/>
  </xsl:call-template>
</xsl:variable>

<xsl:variable name="font">
  <xsl:choose>
    <xsl:when test="string-length($asl-script)>0">
      <xsl:message>font=<xsl:value-of select="$asl-script"/></xsl:message>
      <xsl:value-of select="$asl-script"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>osl</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:variable>

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
	      <p class="sl-td-sign {$font}"><xsl:call-template name="sxweb-ucun"/></p>
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
	      <p class="sl-td-sign {$font}"><xsl:call-template name="sxweb-ucun"/></p>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:otherwise>
      </xsl:choose>
    </esp:link>
  </td>
</xsl:template>

<xsl:template name="sxweb-ucun">
  <xsl:variable name="hex" select="sl:ucun/@hex"/>
  <xsl:variable name="salt" select="/*/sl:scripts/*[@n=$asl-script]/*[@code=$hex]/@salt"/>
  <xsl:if test="$hex='U+12324'">
    <xsl:message>sxweb-ucun asl-script=<xsl:value-of select="$asl-script"
    />; <xsl:value-of select="$hex"/> has salt=<xsl:value-of select="$salt"/></xsl:message>
  </xsl:if>
  <xsl:choose>
    <xsl:when test="string-length($salt)>0">
      <span class="salt{$salt}"><xsl:value-of select="sl:ucun"/></span>
    </xsl:when>
    <xsl:otherwise><xsl:value-of select="sl:ucun"/></xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:transform>
