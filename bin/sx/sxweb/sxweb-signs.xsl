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

<xsl:include href="lex-sign-lookup.xsl"/>
<!--xpd.xsl is included by g2-gdl-HTML.xsl-->
<xsl:include href="g2-gdl-HTML.xsl"/>
<xsl:include href="sxweb-util.xsl"/>

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<!--### Set all the asl-* config variables from config.xml -->

<xsl:variable name="sl-config-xml" select="concat($oraccbuilds,'/xml/',/*/@project,'/signlist/config.xml')"/>

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

<!--### Iteration over sx.xml -->

<xsl:template match="sl:signlist">
  <xsl:message>sxweb-signs.xsl processing <xsl:value-of select="count(sl:letter/sl:sign)"/> signs in <xsl:value-of select="@project"/>:
&#x9;asl-stats=<xsl:value-of select="$asl-stats"/>
&#x9;asl-periods=<xsl:value-of select="$asl-periods"/>
&#x9;asl-insts=<xsl:value-of select="$asl-insts"/>
&#x9;asl-suxword=<xsl:value-of select="$asl-suxword"/>
&#x9;asl-snippets=<xsl:value-of select="$asl-snippets"/>
&#x9;asl-lexdata=<xsl:value-of select="$asl-lexdata"
/></xsl:message>
  <xsl:apply-templates select="sl:letter/sl:sign"/>
</xsl:template>

<!--### Iteration over each sign -->

<xsl:template match="sl:sign">
  <ex:document href="{concat('signlist/00web/',@xml:id,'.xml')}"
	       method="xml" encoding="utf-8"
	       indent="yes">
  <esp:page>
    <xsl:call-template name="sws-esp-header"/>
    <html>
      <head/>
      <body>
	  <xsl:call-template name="sws-navbar"/>
	  <div class="asl-sign">
	    <div class="asl-sign-sign">
	      <xsl:call-template name="sws-form-jumps"/>
	      <!--<xsl:call-template name="sws-sections"/>?-->
	      <xsl:call-template name="sws-sign-or-form"/>
	    </div>
	    <xsl:if test="count(sl:form)>0">
	      <div class="asl-forms">
		<xsl:for-each select="sl:form">
		  <div class="asl-sign-form"
		       id="form{count(preceding-sibling::sl:form)}">
		    <xsl:call-template name="sws-form-h"/>
		    <xsl:call-template name="sws-sign-or-form"/>
		  </div>
		</xsl:for-each>
	      </div>
	    </xsl:if>
	  </div>
	  <xsl:call-template name="sws-cite-url"/>
      </body>
    </html>
  </esp:page>
  </ex:document>
</xsl:template>

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

<xsl:template name="sws-form-jumps">
  <xsl:if test="count(sl:form) > 0">
    <div class="asl-bkmk-forms">
      <p>
	<xsl:text>Jump to variant form: </xsl:text>
	<xsl:for-each select="sl:form">
	  <esp:link bookmark="form{count(preceding-sibling::sl:form)}">
	    <xsl:value-of select="@n"/>
	    <xsl:if test="sl:images/sl:i[@loc]">
	      <xsl:text>&#xa0;=&#xa0;</xsl:text>
	      <xsl:variable name="base" select="'../../../pctc'"/>
	      <xsl:for-each select="sl:images/sl:i[@loc][1]">
		<xsl:variable name="ref" select="@ref"/>
		<xsl:variable name="header" select="/*/sl:iheader[@xml:id=$ref]"/>
		<esp:image height="40px" file="{$base}/{$header/@path}/{@loc}"
			   description="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
	      </xsl:for-each>
	    </xsl:if>
	  </esp:link>
	  <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
	</xsl:for-each>
      </p>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-cite-url">
  <div class="asl-cite-url">
    <hr/>
    <p>Sign ID <xsl:value-of select="@xml:id"/>;
    Citation URL http://oracc.org/<xsl:value-of select="/*/@project"/>/<xsl:value-of select="@xml:id"/>
    </p>
    <hr/>
  </div>
</xsl:template>

<!--### Controller for sign/form div creation -->

<xsl:template name="sws-sign-or-form">
  <div class="asl-sf-body">
    <!--<xsl:call-template name="sws-stats"/>-->
    <xsl:call-template name="sws-meta"/>
    <xsl:call-template name="sws-values"/>
    <xsl:call-template name="sws-images"/>
    <xsl:call-template name="sws-snippets"/>
    <xsl:call-template name="sws-instances"/>
    <xsl:call-template name="sws-lexdata"/>
    <xsl:call-template name="sws-suxword"/>
  </div>
</xsl:template>

<!--### Subroutines for sign-or-form -->

<xsl:template name="sws-meta">
  <xsl:if test="sl:uage|sl:ucun|sl:aka|sl:list|sl:sys|@compoundonly|sl:cpds">
    <div class="asl-meta">
      <xsl:call-template name="sws-unicode"/>
      <xsl:call-template name="sws-akas"/>
      <xsl:call-template name="sws-lists"/>
      <xsl:call-template name="sws-systems"/>
      <xsl:call-template name="sws-compounds"/>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-unicode">
  <p style="font-size: 150%">
    <xsl:choose>
      <xsl:when test="sl:ucun">
	<xsl:value-of select="sl:ucun"/>
      </xsl:when>
      <xsl:when test="sl:useq">
	<xsl:value-of select="sl:useq"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:for-each select=".//sl:name[1]//*">
	  <xsl:choose>
	    <xsl:when test="@g:utf8"><xsl:value-of select="@g:utf8"/></xsl:when>
	    <xsl:otherwise/>
	  </xsl:choose>
	</xsl:for-each>
      </xsl:otherwise>
    </xsl:choose>
  </p>
</xsl:template>

<xsl:template name="sws-akas">
  <xsl:if test="sl:aka">
    <p>
      <span class="sl-ihead">AKA</span>
      <span class="sl-ibody">
	<xsl:for-each select="sl:aka">
	  <xsl:value-of select="@n"/>
	  <xsl:if test="not(position()=last())">
	    <xsl:text>; </xsl:text>
	  </xsl:if>
	</xsl:for-each>
      </span>
    </p>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-lists">
  <xsl:if test="sl:list">
    <p>
      <span class="sl-ihead">LISTS</span>
      <span class="sl-ibody">
	<xsl:for-each select="sl:list">
	  <xsl:value-of select="@n"/>
	  <xsl:if test="not(position()=last())">
	    <xsl:text>; </xsl:text>
	  </xsl:if>
	</xsl:for-each>
      </span>
    </p>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-systems">
  <xsl:if test="sl:sys">
    <p>
      <span class="sl-ihead">SYS</span>
      <xsl:for-each select="sl:sys">
	<span class="sl-sys-name">
	  <xsl:value-of select="@name"/>
	  <xsl:if test="@subname">
	    <span class="sl-sys-subname"><xsl:value-of select="concat(':',@subname)"/></span>
	  </xsl:if>
	  <xsl:text>:&#xa0;</xsl:text>
	</span>
	<span class="sl-sys-token">
	  <xsl:value-of select="@token"/>
	  <xsl:if test="string-length(text())>0">
	    <xsl:text>&#x2192;</xsl:text>
	    <xsl:value-of select="text()"/>
	  </xsl:if>
	</span>
	<xsl:if test="not(position()=last())">
	  <xsl:text>; </xsl:text>
	</xsl:if>
      </xsl:for-each>
    </p>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-compounds">
  <xsl:choose>
    <xsl:when test="@compoundonly = 'yes'">
      <xsl:choose>
	<xsl:when test="string-length(@cpd-refs)>0">
	  <xsl:variable name="s">
	    <xsl:if test="contains(@cpd-refs, ' ')">
	      <xsl:text>s</xsl:text>
	    </xsl:if>
	  </xsl:variable>
	  <p>Occurs in the following compound<xsl:value-of select="$s"/>:
	  <xsl:for-each select="id(@cpd-refs)">
	    <xsl:text> </xsl:text>
	    <esp:link page="{ancestor-or-self::sl:sign[1]/@xml:id}">
	      <xsl:apply-templates select=".//sl:name[1]"/>
	      <xsl:if test="sl:images/sl:i[@loc]">
		<xsl:text> = </xsl:text>
		<xsl:for-each select="sl:images/sl:i[@loc][1]">
		  <xsl:call-template name="esp-sign-image"/>
		</xsl:for-each>
	      </xsl:if>
	    </esp:link>
	  </xsl:for-each>
	  <xsl:text>.</xsl:text>
	  </p>
	</xsl:when>
	<xsl:otherwise>
	  <p>(Only in compounds but no compounds containing this sign were found in this signlist.)</p>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:if test="sl:cpds/sl:memb">
	<div class="asl-compounds">
	  <p>
	    <xsl:text>See also </xsl:text>
	    <xsl:for-each select="id(sl:cpds/sl:memb/@oids)">
	      <esp:link page="{@xml:id}">
		<xsl:choose>
		  <xsl:when test="sl:ucun">
		    <xsl:value-of select="sl:ucun"/>
		    <xsl:text> (</xsl:text>
		    <xsl:value-of select="@n"/>
		    <xsl:text>)</xsl:text>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:value-of select="@n"/>
		  </xsl:otherwise>
		</xsl:choose>
	      </esp:link>
	      <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
	    </xsl:for-each>
	    <xsl:text>.</xsl:text>
	  </p>
	</div>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!--### Images and snippets -->

<xsl:template name="sws-images">
  <xsl:if test="sl:images/sl:i[@loc]">
    <div class="asl-images">
      <xsl:for-each select="sl:images/sl:i[@loc]">
	<xsl:variable name="ref" select="@ref"/>
	<xsl:variable name="header" select="/*/sl:iheader[@xml:id=$ref]"/>
	<table class="itable">
	  <tr><td><span class="im-label"><xsl:value-of select="$header/@label"/>:</span></td></tr>
	  <tr><td>
	    <esp:image width="100%" file="{$base}/{$header/@path}/{@loc}"
		       description="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
	  </td></tr>
	</table>
      </xsl:for-each>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-snippets">
  <xsl:if test="asl-snippets='yes'">
    <xsl:variable name="oid" select="@xml:id"/>
    <xsl:for-each select="document('/ogsl/downloads/snippets.xml')">
      <xsl:for-each select="id($oid)">
	<div class="asl-snippets">
	  <p>See the <esp:link url="/ogsl/{$oid}.html">snippets page for this sign</esp:link>.</p>
	</div>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-instances">
  <xsl:if test="$asl-insts = 'yes'">
    <xsl:variable name="oid" select="@xml:id"/>
    <xsl:for-each select="document('provides-instances.xml',/)">
      <xsl:for-each select="id($oid)">
	<div class="asl-insts">
	  <p>See the <esp:link url="/{$project}/inst/{$oid}.html">instances page for this sign</esp:link>.</p>
	</div>
      </xsl:for-each>
    </xsl:for-each>     
  </xsl:if>
</xsl:template>

<xsl:template name="sws-lexdata">
  <xsl:if test="$asl-lexdata = 'yes'">
    <xsl:call-template name="lex-sign"/>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-suxword">
  <xsl:if test="$asl-suxword = 'yes'">
  </xsl:if>
</xsl:template>

<!-- ### Values -->

<xsl:template name="sws-values">
  <xsl:if test="count(sl:v)>0">
    <div class="asl-values">
      <p>
	<span class="asl-values-h">Values: </span>
	<xsl:for-each select="sl:v">
	  <xsl:choose>
	    <xsl:when test="@deprecated='yes'">
	      <span class="v-drop"><xsl:value-of select="@n"/></span>
	    </xsl:when>
	    <xsl:when test="@uncertain='yes'">
	      <span class="v-query"><xsl:value-of select="@n"/></span>
	    </xsl:when>
	    <xsl:otherwise>
	      <span class="v-ok"><xsl:value-of select="@n"/></span>
	    </xsl:otherwise>
	  </xsl:choose>
	  <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
	</xsl:for-each>
	<xsl:text>.</xsl:text>
      </p>
    </div>
  </xsl:if>
</xsl:template>

<!-- Trap unhandled tags -->

<xsl:template mode="rest"
	      match="is:land|sl:aka|sl:v|sl:sort
		     |sl:uphase|sl:utf8|sl:uname|sl:ucun
		     |sl:list|sl:name|sl:pname|sl:inote|sl:form|sl:unote|sl:note
		     |sl:qs|sl:inherited|sl:uage|sl:sys|sl:smap|sl:images"/>

<xsl:template mode="rest" match="*">
  <xsl:message>sxweb-signs.xsl: tag <xsl:value-of select="local-name(.)"/> not handled</xsl:message>
</xsl:template>

</xsl:transform>
