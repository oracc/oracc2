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
    xmlns:param="http://oracc.org/ns/esp-param/1.0" 
    exclude-result-prefixes="sl dc xh esp param"
    extension-element-prefixes="ex"
    version="1.0">

<xsl:strip-space elements="*"/>

<xsl:template name="sws-sel-page">
  <xsl:param name="body-id" select="'SelPage'"/>
  <xsl:param name="file"/>
  <xsl:param name="type"/>
  <xsl:param name="title"/>
  <xsl:param name="nodes"/>
  <xsl:param name="parameters"/>
  <xsl:param name="parent"/>
  <xsl:param name="back" select="false()"/>
  <xsl:variable name="basename">    
    <xsl:choose>
      <xsl:when test="$file"><xsl:value-of select="$file"/></xsl:when>
      <xsl:otherwise><xsl:value-of select="concat(@xml:id,'-',$type)"/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:if test="count($nodes)>0">
    <ex:document href="{concat('signlist/01bld/selpages/',$basename,'.html')}"
		 method="xml" encoding="utf-8" omit-xml-declaration="yes"
		 indent="no" doctype-system="html">
      <html>
	<head>
	  <xsl:call-template name="esp2-head-content">
	    <xsl:with-param name="parameters" select="$parameters"/>
	    <xsl:with-param name="project" select="$project"/>
	  </xsl:call-template>
	  <!--
	      <meta charset="UTF-8"/>
	      <title><xsl:value-of select="$title"/></title>
	      <link media="screen,projection" href="{concat('/',/*/@project,'/signlist/css/projesp.css')}" type="text/css" rel="stylesheet" />
	  -->
	</head>
	<body id="{$body-id}" class="selpage">
	  <xsl:call-template name="esp2-banner-div">
	    <xsl:with-param name="parameters" select="$parameters"/>
	    <xsl:with-param name="project" select="$project"/>
	    <xsl:with-param name="current-page" select="ancestor-or-self::sl:sign"/>
	    <xsl:with-param name="nomenu" select="true()"/>
	    <xsl:with-param name="top-index-link" select="concat('/',$project,'/signlist')"/>
	  </xsl:call-template>
	  <xsl:choose>
	    <xsl:when test="$back = false()">
	      <h2><a href="/{$project}/signlist/{$parent/@xml:id}"
		     ><xsl:value-of select="$title"/></a></h2>
	    </xsl:when>
	    <xsl:otherwise>
	      <h2><a href="javascript://" onclick="window.history.back()"
		     ><xsl:value-of select="$title"/></a></h2>	      
	    </xsl:otherwise>
	  </xsl:choose>
	  <table class="selpage">
	    <xsl:choose>
	      <xsl:when test="$type='h'">
		<xsl:for-each select="$nodes">
		  <xsl:sort select="@vsort" data-type="number"/>
		  <xsl:sort select="@ssort" data-type="number"/>
		  <tr>
		    <td><xsl:value-of select="@v"/></td>
		    <xsl:for-each select="id(@oid)">
		      <xsl:call-template name="sws-sel-summary"/>
		    </xsl:for-each>
		  </tr>
		</xsl:for-each>
	      </xsl:when>
	      <xsl:when test="$type='numset'">
		<xsl:for-each select="$nodes">
		  <tr>
		    <td><xsl:value-of select="@n"/></td>
		    <xsl:for-each select="id(@ref)">
		      <xsl:call-template name="sws-sel-summary"/>
		    </xsl:for-each>
		  </tr>
		</xsl:for-each>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:for-each select="$nodes">
		  <xsl:sort select="@sort" data-type="number"/>
		  <tr>
		    <xsl:call-template name="sws-sel-summary"/>
		  </tr>
		</xsl:for-each>
	      </xsl:otherwise>
	    </xsl:choose>
	  </table>
	</body>
      </html>
    </ex:document>
  </xsl:if>
</xsl:template>

<!-- the context node must be sl:sign or sl:form here -->
<xsl:template name="sws-sel-summary">
  <!--<xsl:message>name=<xsl:value-of select="local-name()"/></xsl:message>-->
  <td>
    <xsl:choose>
      <xsl:when test="self::sl:form">
	<!--get ancestor sl:letter and sl:sign IDs for pathname-->
	<a href="{concat('/',/*/@project,'/signlist/',../../@xml:id,'/',../@xml:id,'/index.html')}">
	  <span class="snames"><xsl:value-of select="@n"/></span>
	</a>
      </xsl:when>
      <xsl:otherwise>
	<a href="{concat('/',/*/@project,'/signlist/',../@xml:id,'/',@xml:id,'/index.html')}">
	  <span class="snames"><xsl:value-of select="ancestor-or-self::sl:sign/@n"/></span>
	</a>
      </xsl:otherwise>
    </xsl:choose>
  </td>
  <td class="noto"><xsl:value-of select="sl:ucun"/></td>
  <td>
    <xsl:for-each select="ancestor-or-self::sl:sign/sl:v">
      <xsl:value-of select="@n"/>
      <xsl:if test="not(position() = last())"><xsl:text> </xsl:text></xsl:if>
    </xsl:for-each>
  </td>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
