<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:g="http://oracc.org/ns/gdl/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:is="http://oracc.org/ns/is/1.0"
    exclude-result-prefixes="sl dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<!-- always invoke with -stringparam project PROJECT -->
<xsl:param name="project"/>
<xsl:param name="hproject" select="translate($project,'/','-')"/>

<!--xpd.xsl is included by g2-gdl-HTML.xsl-->
<xsl:include href="g2-gdl-HTML.xsl"/>
<xsl:include href="sxweb-util.xsl"/>

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<!--### Set all the asl-* config variables from config.xml -->

<!--Inlined data-->

<!--statistics for occurrences; these are embedded in the headers for each sign/form/value name-->
<xsl:variable name="asl-stats">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'asl-stats'"/>
    <xsl:with-param name="default" select="'no'"/>
  </xsl:call-template>
</xsl:variable>

<!--period stats default=yes when stats=yes; asl-periods=no can be used to switch periods para off-->
<xsl:variable name="asl-periods">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'asl-periods'"/>
    <xsl:with-param name="default" select="$asl-stats"/>
  </xsl:call-template>
</xsl:variable>

<!--Linked or xincluded pages-->

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
    <xsl:with-param name="option" select="'asl-insts'"/>
    <xsl:with-param name="default" select="$insts-default"/>
  </xsl:call-template>
</xsl:variable>

<!--lexdata asl-lexdata=link = link to lexdata; asl-lexdata=xinc = xinclude lexdata-->
<xsl:variable name="asl-lexdata">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'asl-lexdata'"/>
    <xsl:with-param name="default" select="'no'"/>
  </xsl:call-template>
</xsl:variable>

<!--snippets asl-snippets=link = link to snippets; asl-snippets=xinc = xinclude snippets-->
<xsl:variable name="asl-snippets">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'asl-snippets'"/>
    <xsl:with-param name="default" select="'no'"/>
  </xsl:call-template>
</xsl:variable>

<!--words asl-words=link = link to words; asl-words=xinc = xinclude words-->
<xsl:variable name="asl-words">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'asl-words'"/>
    <xsl:with-param name="default" select="'no'"/>
  </xsl:call-template>
</xsl:variable>

<!--### Iteration over sx.xml -->

<xsl:template match="sl:signlist">
  <xsl:message>sxweb-signs.xsl processing <xsl:value-of select="count(sl:letter/sl:sign)"/> signs:
&#x9;asl-stats=<xsl:value-of select="$asl-stats"/>
&#x9;asl-periods=<xsl:value-of select="$asl-periods"/>
&#x9;asl-insts=<xsl:value-of select="$asl-insts"/>
&#x9;asl-words=<xsl:value-of select="$asl-words"/>
&#x9;asl-snippets=<xsl:value-of select="$asl-snippets"/>
&#x9;asl-lexdata=<xsl:value-of select="$asl-lexdata"
/></xsl:message>
<wrapper>
  <xsl:apply-templates select="sl:letter/sl:sign[position()&lt;10]"/>
</wrapper>
</xsl:template>

<!--### Iteration over each sign -->

<xsl:template match="sl:sign">
  <!--
    <ex:document href="{concat('signlist/00web/',@xml:id,'.xml')}"
    method="xml" encoding="utf-8"
    indent="yes">
  -->
  <esp:page>
    <xsl:call-template name="sws-esp-header"/>
    <html>
      <head/>
      <body>
	  <xsl:call-template name="sws-navbar"/>
	  <div class="asl-sign">
	    <div class="asl-sign-sign">
	      <xsl:call-template name="sws-sign-or-form"/>
	    </div>
	    <xsl:if test="count(sl:form)>0">
	      <div class="asl-forms">
		<xsl:for-each select="sl:form">
		  <div class="asl-sign-form">
		    <xsl:call-template name="sws-form-h"/>
		    <xsl:call-template name="sws-sign-or-form"/>
		  </div>
		</xsl:for-each>
	      </div>
	    </xsl:if>
	</div>
      </body>
    </html>
  </esp:page>
  <!--</ex:document>-->
</xsl:template>

<!--### Controller for sign/form div creation -->

<xsl:template name="sws-sign-or-form">
</xsl:template>

<!--### Subroutines for each part of sign-or-form page -->

<xsl:template name="sws-esp-header">
  <xsl:variable name="nopipes" select="translate(@n,'|','')"/>
  <esp:name><xsl:value-of select="$nopipes"/></esp:name>
  <!-- esp:title becomes the top-level h1 -->
  <esp:title>
    <xsl:call-template name="sws-sign-or-form-h">
      <xsl:with-param name="nopipes" select="$nopipes"/>
    </xsl:call-template>
  </esp:title>
  <esp:url><xsl:value-of select="@xml:id"/></esp:url>
</xsl:template>

<xsl:template name="sws-sign-or-form-h">
  <xsl:param name="nopipes"/>
  <xsl:value-of select="$nopipes"/>
  <xsl:if test="not(sl:smap) and not(sl:uage='0')">
    <xsl:text> </xsl:text>
    <span class="asl-oid"><xsl:value-of select="concat('[',@xml:id,']')"/></span>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-form-h">
  <h2 class="asl-form-h">
    <xsl:call-template name="sws-sign-or-form-h">
      <xsl:with-param name="nopipes" select="translate(@n,'|','')"/>
    </xsl:call-template>
  </h2>
</xsl:template>

<xsl:template name="sws-navbar">
  <xsl:variable name="prev" select="preceding::sl:sign[1]"/>
  <xsl:variable name="next" select="following::sl:sign[1]"/>
  <xsl:if test="$prev|$next">
    <div class="navbar">
      <xsl:choose>
	<xsl:when test="$prev and $next">
	  <p style="text-align-last: justify">
	    <xsl:text>«</xsl:text>
	    <esp:link page="{$prev/@xml:id}"><xsl:value-of select="$prev/@n"/></esp:link>
	    <xsl:text>«</xsl:text>
	    <xsl:text> </xsl:text>
	    <xsl:text>»</xsl:text>
	    <esp:link page="{$next/@xml:id}"><xsl:value-of select="$next/@n"/></esp:link>
	    <xsl:text>»</xsl:text>
	  </p>
	</xsl:when>
	<xsl:when test="$prev">
	  <p>
	    <xsl:text>«</xsl:text>
	    <esp:link page="{$prev/@xml:id}"><xsl:value-of select="$prev/@n"/></esp:link>
	    <xsl:text>«</xsl:text>
	  </p>
	</xsl:when>
	<xsl:otherwise>
	  <p style="text-align-last: right">
	    <xsl:text>»</xsl:text>
	    <esp:link page="{$next/@xml:id}"><xsl:value-of select="$next/@n"/></esp:link>
	    <xsl:text>»</xsl:text>
	  </p>
	</xsl:otherwise>
      </xsl:choose>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-value-h">
  <h2 class="asl-value-h"><xsl:value-of select="@n"/></h2>
</xsl:template>


<xsl:template name="sws-unicode">
  <xsl:value-of select=".//sl:uname[1]"/>
</xsl:template>

</xsl:transform>
