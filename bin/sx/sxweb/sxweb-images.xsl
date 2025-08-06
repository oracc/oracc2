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
    xmlns:ofp="http://oracc.org/ns/ofp/1.0"
    xmlns:is="http://oracc.org/ns/is/1.0"
    xmlns:param="http://oracc.org/ns/esp-param/1.0" 
    exclude-result-prefixes="sl dc xh esp param"
    extension-element-prefixes="ex"
    version="1.0">

<!--### Images and snippets -->

<xsl:key name="oid" match="ofp:sign" use="@oid"/>

<!--emit all the images for a sign or form; this always sets the
     context to an sl:sign or the defining sl:form using id(@oid) if
     necessary-->
<xsl:template name="sws-asl-cun-img">
  <xsl:param name="esp-mode" select="true()"/>
  <xsl:if test="sl:ucun|sl:images/sl:i[@loc]|sl:sys[@token='img-page']">
    <div class="asl-cun-img">
      <span class="sl-ihead">CUNEIFORM</span>
      <xsl:call-template name="sws-cuneiform">
	<xsl:with-param name="esp-mode" select="$esp-mode"/>
      </xsl:call-template>
      <xsl:call-template name="sws-image-bar">
	<xsl:with-param name="esp-mode" select="$esp-mode"/>
      </xsl:call-template>
      <xsl:if test="self::sl:sign and sl:sys[@token='img-page']">
	<table class="itable">
	  <tr>
	    <td><esp:link url="/pcsl/cdli-gh/{@xml:id}.html">CDLI-gh original:</esp:link></td>
	  </tr>
	  <tr><td>
	    <object data="/pcsl/cdli-gh/{@xml:id}.html" height="100px" width="200px"/>
	  </td></tr>
	</table>
      </xsl:if>
    </div>
  </xsl:if>
</xsl:template>

<!-- Provide a single reference image of the sign from the signlist's font.
     
     If asl-salt='yes' also display any SALT the character may have in the signlist's font.
  -->
<xsl:template name="sws-cuneiform">
  <xsl:param name="esp-mode"/>
  <div class="asl-cuneiform">
    <xsl:choose>
      <xsl:when test="sl:glyf">
	<xsl:call-template name="sws-glyfs"/>
      </xsl:when>
      <xsl:otherwise>
	<table class="itable">
	  <tr>
	    <td><span class="im-label">&#xa0;</span></td>
	  </tr>
	  <tr>
	    <td>
	      <p class="asl-ucun">
		<xsl:choose>
		  <xsl:when test="sl:ucun">
		    <xsl:call-template name="sxweb-ucun"/>
		    <xsl:if test="not($asl-salt='no')">
		      <xsl:call-template name="sws-cun-salt"/>
		    </xsl:if>
		  </xsl:when>
		  <xsl:when test="sl:useq">
		    <span class="{$asl-script}">
		      <xsl:value-of select="sl:useq"/>
		    </span>
		    <xsl:if test="not($asl-salt='no')">
		      <xsl:call-template name="sws-cun-salt"/>
		    </xsl:if>
		  </xsl:when>	      
		  <xsl:otherwise>
		    <span class="{$asl-script}">
		      <xsl:for-each select=".//sl:name[1]//*">
			<xsl:choose>
			  <xsl:when test="@g:utf8"><xsl:value-of select="@g:utf8"/></xsl:when>
			  <xsl:otherwise/>
			</xsl:choose>
		      </xsl:for-each>
		    </span>
		  </xsl:otherwise>
		</xsl:choose>
	      </p>
	    </td>
	  </tr>
	</table>
      </xsl:otherwise>
    </xsl:choose>
  </div>
</xsl:template>

<xsl:template name="sws-cun-salt">
  <xsl:variable name="data" select="concat($oracc,'/lib/data/',$asl-salt,'.ofp')"/>
  <xsl:variable name="oid">
    <xsl:call-template name="sws-sign-or-form-oid"/>
  </xsl:variable>
  <!--<xsl:message>sws-cun-salt data=<xsl:value-of select="$data"/> oid=<xsl:value-of select="$oid"/></xsl:message>-->
  <xsl:for-each select="document($data)">
    <xsl:for-each select="key('oid',$oid)/ofp:salts/*">
      <!--<xsl:message>sws-cun-salt found salt=<xsl:value-of select="."/></xsl:message>-->
      <span class="asl-salt {/*/@css} ofs-{/*/@mag} salt{.}"><xsl:value-of select="ancestor::ofp:sign/@utf8"/></span>
      <wbr/>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>

<xsl:template name="sws-glyfs">
  <table class="glyfs">
    <tr>
      <xsl:for-each select="sl:glyf">
	<td>
	  <div class="glyf vbox">
	    <div class="glyf-ucun"><span class="ofs-pc ofs-200"><xsl:value-of select="@ucun"/></span></div>
	    <div class="glyf-uhex"><xsl:value-of select="@uhex"/></div>
	    <div class="glyf-cvnn"><xsl:value-of select="@cvnn"/></div>
	  </div>
	</td>
      </xsl:for-each>
    </tr>
  </table>
</xsl:template>

<!-- Display the reference image for each entry in sl:images/@refs -->
<xsl:template name="sws-image-bar">
  <xsl:param name="esp-mode"/>
  <!--<xsl:message>sws-images: esp-mode=<xsl:value-of select="$esp-mode"/></xsl:message>-->
  <xsl:if test="sl:images/@refs">
    <div class="asl-images">
      <xsl:for-each select="id(@refs)">
	<xsl:for-each select="document(@data)">
	  <xsl:for-each select="id($oid)">
	    <xsl:choose>
	      <xsl:when test="@utf8">
		<xsl:variable name="u8" select="@utf8"/>
		<span class="{/*/@css} {/*/@mag}">
		  <xsl:value-of select="@utf8"/>
		</span>
	      </xsl:when>
	      <xsl:when test="$esp-mode">
		<esp:image height="50px" url="/{/*/@path}/{$image}"
			   description="{/*/@label} image of {ancestor::ofp:sign/@n}"/>
	      </xsl:when>
	      <xsl:otherwise>
		<img height="50px" src="/{/*/@path}/{$image}"
		     alt="{/*/@label} image of {ancestor::ofp:sign/@n}"/>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:for-each>
	</xsl:for-each>
      </xsl:for-each>
      <xsl:for-each select="sl:images/sl:i[@loc]">
	<xsl:variable name="ref" select="@ref"/>
	<xsl:variable name="header" select="/*/sl:iheader[@xml:id=$ref]"/>
	<xsl:variable name="css" select="$header/@css"/>
	<table class="itable">
	  <tr>
	    <td>
	      <xsl:choose>
		<xsl:when test="string-length($header/@page)>0">
		  <esp:link url="{$header/@page}" >
		    <xsl:call-template name="sws-image-label">
		      <xsl:with-param name="header" select="$header"/>
		    </xsl:call-template>
		  </esp:link>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:call-template name="sws-image-label">
		    <xsl:with-param name="header" select="$header"/>
		  </xsl:call-template>
		</xsl:otherwise>
	      </xsl:choose>
	    </td>
	  </tr>
	  <tr><td>
	    <xsl:choose>
	      <xsl:when test="string-length($css)>0">
		<p class="asl-ucun">
		  <span class="{$css}">
		    <xsl:value-of select="../preceding-sibling::sl:ucun"/>
		  </span>
		</p>
	      </xsl:when>
	      <xsl:when test="$esp-mode">
		<esp:image height="50px" url="/{$header/@proj}/{$header/@path}/{@loc}"
			   description="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
	      </xsl:when>
	      <xsl:otherwise>
		<img height="50px" src="/{$header/@proj}/{$header/@path}/{@loc}"
		     alt="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
	      </xsl:otherwise>
	    </xsl:choose>
	  </td></tr>
	</table>
      </xsl:for-each>
    </div>
  </xsl:if>
</xsl:template>

<!-- Display reference image and SALTs for each entry in sl:images/@refs -->
<xsl:template name="sws-image-all">
  <xsl:param name="oid"/>
  <xsl:param name="withfeat"/>
  <xsl:param name="esp-mode"/>
  <xsl:for-each select="id(@refs)">
    <xsl:for-each select="document(@data)">
      <xsl:for-each select="id($oid)">
	<xsl:choose>
	  <xsl:when test="@utf8">
	    <xsl:variable name="u8" select="@utf8"/>
	    <span class="{/*/@css} {/*/@mag}">
	      <xsl:value-of select="@utf8"/>
	    </span>
	    <xsl:if test="$withfeat">
	      <xsl:choose>
		<xsl:when test="ofp:lists/*/ofp:salts">
		  <xsl:for-each select="ofp:list/*/ofp:salts">
		    <xsl:value-of select="../@l"/>
		    <xsl:for-each select="*">
		      <span class="{/*/@css} ofs-{/*/@mag} salt{.}"><xsl:value-of select="$u8"/></span>
		    </xsl:for-each>
		  </xsl:for-each>
		</xsl:when>
		<xsl:when test="ofp:salts">
		  <xsl:for-each select="ofp:salts/*">
		    <span class="{/*/@css} ofs-{/*/@mag} salt{.}"><xsl:value-of select="$u8"/></span>
		  </xsl:for-each>
		</xsl:when>
	      </xsl:choose>
	    </xsl:if>
	  </xsl:when>
	  <xsl:when test="@image">
	  </xsl:when>
	</xsl:choose>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>

<xsl:template name="sws-image-label">
  <xsl:param name="header"/>
  <span class="im-label">
    <xsl:if test="string-length($header/@title)">
      <xsl:attribute name="title"><xsl:value-of select="$header/@title"/></xsl:attribute>
    </xsl:if>
    <xsl:choose>
      <xsl:when test="@n">
	<xsl:value-of select="@n"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$header/@label"/>
      </xsl:otherwise>
    </xsl:choose>
  <xsl:text>:</xsl:text></span>
</xsl:template>

<!-- Provide one image of a sign within the jump-link for the form -->
<xsl:template name="sws-form-jump-image">
  <xsl:if test="sl:images/sl:i[@loc]">
    <xsl:text>&#xa0;=&#xa0;</xsl:text>
    <xsl:for-each select="sl:images/sl:i[@loc][1]">
      <xsl:variable name="ref" select="@ref"/>
      <xsl:variable name="css" select="/*/sl:iheader[@xml:id=$ref]/@css"/>
      <xsl:choose>
	<xsl:when test="string-length($css)>0">
	  <span class="{$css}"><xsl:value-of select="../preceding-sibling::sl:ucun"/></span>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:variable name="header" select="/*/sl:iheader[@xml:id=$ref]"/>
	  <esp:image height="40px" url="/{$header/@proj}/{$header/@path}/{@loc}"
		     description="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:if>
  </xsl:template>

<!-- Provide the = <SIGN> for a compound link -->
<xsl:template name="sws-compounds-link-img">
  <xsl:param name="esp-mode"/>
  <xsl:if test="sl:images/sl:i[@loc]">
    <xsl:text> = </xsl:text>
    <xsl:for-each select="sl:images/sl:i[@loc][1]">
      <xsl:call-template name="esp-sign-image">
	<xsl:with-param name="esp-mode" select="$esp-mode"/>
      </xsl:call-template>
    </xsl:for-each>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-snippets">
  <xsl:if test="asl-snippets='yes'">
    <xsl:variable name="oid" select="@xml:id"/>
    <xsl:for-each select="document('/osl/downloads/snippets.xml')">
      <xsl:for-each select="id($oid)">
	<div class="asl-snippets">
	  <p>See the <esp:link url="/osl/{$oid}.html">snippets page for this sign</esp:link>.</p>
	</div>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-sign-or-form-oid">
  <xsl:choose>
    <xsl:when test="@oid"><xsl:value-of select="@oid"/></xsl:when>
    <xsl:otherwise><xsl:value-of select="@xml:id"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:transform>
