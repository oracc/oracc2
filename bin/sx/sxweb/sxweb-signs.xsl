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

<!-- always invoke with -stringparam project PROJECT -->
<xsl:param name="project"/>
<xsl:param name="hproject" select="translate($project,'/','-')"/>
<xsl:param name="projesp"/>

<xsl:include href="lex-sign-lookup.xsl"/>
<!--xpd.xsl is included by g2-gdl-HTML.xsl-->
<xsl:include href="g2-gdl-HTML.xsl"/>
<xsl:include href="sxweb-util.xsl"/>
<xsl:include href="esp2-head.xsl"/>

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:key name="lemuniq" match="sl:lemma" use="concat(@n,'::',@base)"/>

<!--### Set all the asl-* config variables from config.xml -->
<xsl:variable name="q">'</xsl:variable>
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
  <xsl:if test="not(@moid)">
    <ex:document href="{concat('signlist/00web/',@xml:id,'.xml')}"
		 method="xml" encoding="utf-8"
		 indent="no">
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
    <xsl:call-template name="sws-selection-pages"/>
  </xsl:if>
</xsl:template>

<!--
    All code that uses the esp2-head and banner must do so via this
    routine because it sets $parameters.
    
    This is called for each sl:sign and on the first sl:sign we emit homophone selpages
   -->
<xsl:template name="sws-selection-pages">
  <xsl:variable name="parameters-xml" select="concat($projesp, '/signlist/00web/00config/parameters.xml')"/>
  <!--<xsl:message>sws-sel-page: projesp=<xsl:value-of select="$projesp"/>;
      parameters.xml=<xsl:value-of select="$parameters-xml"/></xsl:message>-->
  <xsl:variable name="parameters"
		select="document($parameters-xml)/param:parameters"/>
  <!--
      <xsl:message>sws-sel-page called with type=<xsl:value-of select="$type"
      />; title=<xsl:value-of select="$title"/>; nodecount=<xsl:value-of select="count($nodes)"/></xsl:message>
  -->
  <xsl:call-template name="sws-sel-page">
    <xsl:with-param name="parameters" select="$parameters"/>
    <xsl:with-param name="type" select="'cmemb'"/>
    <xsl:with-param name="title" select="concat(@n,  ' in compounds')"/>
    <xsl:with-param name="nodes" select="id(sl:cpds/sl:memb/@oids)"/>
  </xsl:call-template>
  <xsl:call-template name="sws-lem-page">
    <xsl:with-param name="parameters" select="$parameters"/>
  </xsl:call-template>
  <xsl:call-template name="sws-lex-page">
    <xsl:with-param name="parameters" select="$parameters"/>
  </xsl:call-template>
  <xsl:if test="count(preceding-sibling::sl:sign)=0 and count(../preceding-sibling::sl:letter)=0">
    <xsl:message>Emitting homophone selpages</xsl:message>
    <xsl:apply-templates select="/*/sl:homophones/sl:base">
      <xsl:with-param name="parameters" select="$parameters"/>
    </xsl:apply-templates>
  </xsl:if>
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
    <!--<span class="asl-oid"><xsl:value-of select="concat('[',@xml:id,']')"/></span>-->
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
  <!--The [not(@moid)] predicate means that merged signs do not appear in the navbar-->
  <xsl:variable name="prev" select="preceding::sl:sign[not(@moid)][1]"/>
  <xsl:variable name="next" select="following::sl:sign[not(@moid)][1]"/>
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
	      <xsl:for-each select="sl:images/sl:i[@loc][1]">
		<xsl:variable name="ref" select="@ref"/>
		<xsl:variable name="header" select="/*/sl:iheader[@xml:id=$ref]"/>
		<esp:image height="40px" url="/{$header/@proj}/{$header/@path}/{@loc}"
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
    <xsl:if test="not(/*/@project='ogsl') and not(/*/@project='pctc')">
      <esp:link url="/ogsl/signlist/{ancestor-or-self::sl:sign/@xml:id}">See full OGSL page</esp:link>
    </xsl:if>
    <xsl:if test="/*/@project='ogsl' or /*/@project='pctc'">
      Citation URL http://oracc.org/<xsl:value-of select="/*/@project"/>/<xsl:value-of select="@xml:id"/>
    </xsl:if>
    </p>
    <hr/>
  </div>
</xsl:template>

<!--### Controller for sign/form div creation -->

<xsl:template name="sws-sign-or-form">
  <div class="asl-sf-body">
      <xsl:if test="@merge">
	<p class="sl-merge"><xsl:text>(Includes merging of </xsl:text
	><xsl:value-of select="id(@merge)/@n"/><xsl:text>)</xsl:text></p>
      </xsl:if>
    <xsl:if test="sl:ucun|sl:images/sl:i[@loc]">
      <div class="asl-cun-img">
	<span class="sl-ihead">SIGNS</span>
	<xsl:call-template name="sws-cuneiform"/>
	<xsl:call-template name="sws-images"/>
      </div>
    </xsl:if>
    <xsl:call-template name="sws-stats"/>
    <xsl:call-template name="sws-values"/>
    <xsl:call-template name="sws-meta"/>
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
      <xsl:call-template name="sws-lists"/>
      <!--<xsl:call-template name="sws-images"/>-->
      <xsl:call-template name="sws-akas"/>
      <xsl:call-template name="sws-systems"/>
      <xsl:call-template name="sws-compounds"/>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-cuneiform">
  <div class="asl-cuneiform">
    <table class="itable">
      <tr>
	<td><span class="im-label">&#xa0;</span></td>
      </tr>
      <tr>
	<td>
	  <p class="asl-ucun">
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
	</td>
      </tr>
    </table>
  </div>
</xsl:template>

<xsl:template name="sws-unicode">
  <div class="asl-unicode">
    <p>
      <span class="sl-ihead">UNICODE</span>
      <span class="sl-ibody">
	<xsl:choose>
	  <xsl:when test="sl:ucun">
	    <xsl:value-of select="sl:ucun/@hex"/>
	    <xsl:if test="sl:uname">
	      <xsl:text> = </xsl:text><span class="snames"><xsl:value-of select="sl:uname"/><xsl:text>
	      (</xsl:text><xsl:value-of select="sl:uage"/><xsl:text>)</xsl:text></span>
	    </xsl:if>
	  </xsl:when>
	  <xsl:when test="sl:useq">
	    <xsl:value-of select="sl:useq"/>
	  </xsl:when>
	  <xsl:otherwise/>
	</xsl:choose>
      </span>
    </p>
  </div>
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
    <p class="sl-hang">
      <span class="sl-ihead-h">LISTS</span>
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

<xsl:template name="sws-stats">
  <xsl:if test="@iref">
    <p class="sl-hang"><span class="sl-ihead-h"><xsl:text>INSTANCES</xsl:text></span><esp:link
    url="javascript:distprof2({concat($q,$project,$q,',',$q,'tok',$q,',',$q,@iref,$q)})"
    notarget="yes"><xsl:text>see </xsl:text><xsl:value-of select="@icnt"/>
    <xsl:text> occurrences.</xsl:text></esp:link></p>
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

<xsl:template name="sws-sl-cpds-memb">
  <!--<xsl:message>sws-sl-cpds-cmemb oids=<xsl:value-of select="sl:cpds/sl:memb/@oids"/></xsl:message>-->
  <xsl:if test="string-length(sl:cpds/sl:memb/@oids)>0">
    <xsl:choose>
      <xsl:when test="string-length(sl:cpds/sl:memb/@oids) &lt; 100">
	<xsl:for-each select="id(sl:cpds/sl:memb/@oids)">
	  <esp:link page="{id(@xml:id)/ancestor-or-self::sl:sign/@xml:id}">
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
      </xsl:when>
      <xsl:otherwise>
	<!--<xsl:message
	    ><xsl:value-of select="string-length(sl:cpds/sl:memb/@oids)"
	    /><xsl:text>&#x09;</xsl:text><xsl:value-of select="sl:cpds/sl:memb/@oids"/></xsl:message>-->
	<esp:link notarget="yes" url="{concat('/', /*/@project, '/signlist/selpages/', @xml:id, '-cmemb.html')}"
		  >see list of <xsl:value-of select="ceiling(string-length(sl:cpds/sl:memb/@oids) div 9)"/> compounds</esp:link>
      </xsl:otherwise>
    </xsl:choose>
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
      <xsl:if test="sl:cpds/sl:memb|id(@merge)/sl:cpds/sl:memb">
	<div class="asl-compounds">
	  <p class="sl-hang">
	    <span class="sl-ihead-h">COMPOUNDS</span>
	    <xsl:call-template name="sws-sl-cpds-memb"/>
	    <xsl:if test="id(@merge/sl:cpds/sl:memb)">
	      <xsl:if test="sl:cpds/sl:memb">;; </xsl:if>
	    </xsl:if>
	    <xsl:for-each select="id(@merge)">
	      <xsl:if test="not(position()=1)">
		<xsl:if test="sl:cpds/sl:memb">;; </xsl:if>
	      </xsl:if>
	      <xsl:call-template name="sws-sl-cpds-memb"/>
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
	  <tr>
	    <td><span class="im-label">
	      <xsl:choose>
		<xsl:when test="@n">
		  <xsl:value-of select="@n"/>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:value-of select="$header/@label"/>
		</xsl:otherwise>
	      </xsl:choose>
	      <xsl:text>:</xsl:text></span>
	    </td>
	  </tr>
	  <tr><td>
	    <esp:image height="50px" url="/{$header/@proj}/{$header/@path}/{@loc}"
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
    <xsl:variable name="lex-ok">
      <xsl:call-template name="lex-provides-sign"/>
    </xsl:variable>
    <xsl:if test="$lex-ok='y'">
	<p class="sl-hang"
	   ><span class="sl-ihead-h">LEXICAL</span
	   ><esp:link
	   notarget="yes"
	   url="{concat('/',/*/@project,'/signlist/selpages/',ancestor-or-self::sl:sign/@xml:id,'-lex.html')}"
	   >See lexical data for values of <xsl:value-of
	   select="@n"/> sign</esp:link></p>
      </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-lemmas-by-pos-sub"
  	      ><xsl:param name="nodes"
  	      /><xsl:param name="sorf"
	      /><xsl:variable name="url-base"><xsl:choose
	      ><xsl:when test="/*/@lemma-http"><xsl:value-of select="/*/@lemma-http"/></xsl:when
	      ><xsl:otherwise><xsl:value-of select="concat('/',/*/@project)"/></xsl:otherwise></xsl:choose
	      ></xsl:variable><xsl:for-each
	      select="$nodes[not(ancestor::sl:form) or $sorf='form']"
	      ><esp:link notarget="yes" url="{$url-base}/{@oid}"
			      ><span class="asl-lem-base"><xsl:value-of select="@base"/></span><xsl:text> = </xsl:text
			      ><xsl:value-of select="@n"
			      /><xsl:text> </xsl:text
			      ><span class="asl-lem-cnt"><xsl:text>(</xsl:text><xsl:value-of select="@bcnt"/><xsl:text>×)</xsl:text
			      ></span></esp:link
			      ><xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if
			      ></xsl:for-each></xsl:template>

<!--
    	   ><xsl:for-each select="$nodes"
	   ><esp:link notarget="yes" url="/{/*/@project}/{@oid}"
	   ><xsl:value-of select="@base"/><xsl:text> = </xsl:text
	   ><xsl:value-of select="@n"/></esp:link
	   ><xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if
	   ></xsl:for-each
 -->

<xsl:template name="sws-lemmas-by-pos">
  <xsl:param name="nodes"/>
  <xsl:param name="type"/>
  <xsl:param name="sorf"/>
  <xsl:param name="group-label" select="''"/>
  <xsl:if test="count($nodes)>0">
    <xsl:choose>
      <xsl:when test="string-length($group-label) = 0">
	<p class="sl-hang"
	   ><span class="sl-ihead-h"><xsl:value-of select="$type"/></span
	   ><xsl:call-template name="sws-lemmas-by-pos-sub"
	   ><xsl:with-param name="nodes" select="$nodes"
	   /><xsl:with-param name="sorf" select="$sorf"
	   /></xsl:call-template></p>
      </xsl:when>
      <xsl:otherwise>
	<p class="sl-hang"
	   ><span class="sl-ihead-h-r"><xsl:value-of select="concat('[',$group-label,']&#xa0;&#xa0;')"/></span
	   ><xsl:call-template name="sws-lemmas-by-pos-sub"
	   ><xsl:with-param name="nodes" select="$nodes"
	   /></xsl:call-template></p>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-lemmas-via-merge-absolute">
  <xsl:for-each select="id(@merge)">
    <xsl:call-template name="sws-lemmas-by-pos">
      <xsl:with-param name="nodes" select=".//sl:lemmas/*[@gpos='a']"/>
      <xsl:with-param name="type" select="''"/>
      <xsl:with-param name="group-label" select="@n"/>
    </xsl:call-template>
  </xsl:for-each>
</xsl:template>

<!--
<xsl:template name="sws-lemmas-via-merge-combined">
  <xsl:for-each select="id(@merge)">
    <xsl:call-template name="sws-lemmas-by-pos">
      <xsl:with-param name="nodes" select="sl:v/sl:lemmas/*[not(@gpos='a') and not(@gpos='A')]"/>
      <xsl:with-param name="type" select="'Combined'"/>
      <xsl:with-param name="group-label" select="@n"/>
    </xsl:call-template>
  </xsl:for-each>
</xsl:template>

<xsl:template name="sws-lemmas-via-merge-compound">
  <xsl:for-each select="id(@merge)">
    <xsl:call-template name="sws-lemmas-by-pos">
      <xsl:with-param name="nodes" select="sl:v/sl:lemmas/*[@gpos='A']"/>
      <xsl:with-param name="type" select="'Compound'"/>
      <xsl:with-param name="group-label" select="@n"/>
    </xsl:call-template>
  </xsl:for-each>
</xsl:template>
-->

<xsl:template name="sws-suxword">
  <xsl:if test="$asl-suxword = 'yes'">
    <xsl:variable name="lnodes" select=".//sl:lemmas/sl:lemma|id(@merge)//sl:lemmas/sl:lemma"/>
    <xsl:if test="count($lnodes)>0">
      <div class="sl-lemmas">
	<xsl:call-template name="sws-lemmas-by-pos">
	  <xsl:with-param name="nodes" select=".//sl:lemmas/*[@gpos='a']"/>
	  <xsl:with-param name="type" select="'SUMERIAN'"/>
	  <xsl:with-param name="sorf" select="local-name(.)"/>
	</xsl:call-template>
	<xsl:call-template name="sws-lemmas-via-merge-absolute"/>
	<!--
	    <xsl:call-template name="sws-lemmas-by-pos">
	    <xsl:with-param name="nodes" select="sl:v/sl:lemmas/*[not(@gpos='a') and not(@gpos='A')]"/>
	    <xsl:with-param name="type" select="'Combined'"/>
	    </xsl:call-template>
	    <xsl:call-template name="sws-lemmas-via-merge-combined"/>
	    <xsl:call-template name="sws-lemmas-by-pos">
	    <xsl:with-param name="nodes" select="sl:v/sl:lemmas/*[@gpos='A']"/>
	    <xsl:with-param name="type" select="'Compounds'"/>
	    </xsl:call-template>
	    <xsl:call-template name="sws-lemmas-via-merge-compound"/>
	-->
	<xsl:if test="count($lnodes[not(@gpos='a')])>0">
	  <p><span class="sl-ihead">&#xa0;</span>
	  <esp:link notarget="yes" url="{concat('/',/*/@project,'/signlist/selpages/',ancestor-or-self::sl:sign/@xml:id,'-sux.html')}"
		    >See all information on Sumerian words written with the <xsl:value-of
		    select="@n"/> sign</esp:link>
	  </p>
	</xsl:if>
      </div>
    </xsl:if>
  </xsl:if>
</xsl:template>

<!-- ### Values -->

<xsl:template name="sws-values-sub">
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
    <xsl:if test="@icnt">
      <span class="asl-lem-cnt">&#xa0;<xsl:text>(</xsl:text><xsl:value-of select="@icnt"/><xsl:text>×)</xsl:text></span>
    </xsl:if>
    <xsl:if test="not(position()=last())"><span class="homophone-n"/><wbr/></xsl:if>
  </xsl:for-each>
  <xsl:text>.</xsl:text>
</xsl:template>

<xsl:template name="sws-values">
  <xsl:if test="count(sl:v|id(@merge)/sl:v)>0">
    <div class="asl-values">
      <xsl:if test="count(sl:v)>0"
	><p class="sl-hang">
	  <span class="sl-ihead-h"><xsl:text>VALUES</xsl:text></span>
	  <xsl:call-template name="sws-values-sub"/>
	</p>
      </xsl:if>
      <xsl:if test="@merge">
	<xsl:for-each select="id(@merge)">
	  <xsl:if test="count(sl:v)>0"
	    ><p class="sl-hang"
		><span class="sl-ihead-h-r"
		><xsl:value-of select="concat('[',@n,']&#xa0;&#xa0;')"/></span
		><xsl:call-template name="sws-values-sub"
	    /></p
	  ></xsl:if>
	</xsl:for-each>
      </xsl:if>
      <!--<xsl:variable name="hnodes" select="id(sl:v/@hid)[@count&gt;1]"/>-->
      <xsl:variable name="all-v" select="sl:v|id(@merge)/sl:v"/>
      <xsl:variable name="all-h" select="id($all-v/@hid)"/>		    
      <xsl:variable name="hnodes" select="$all-h[@count>1]"/>
      <!--<xsl:message>all-h=<xsl:value-of select="count($all-h)"/>; hnodes=<xsl:value-of select="count($hnodes)"/></xsl:message>-->
      <xsl:if test="count($hnodes)>0">
	<p class="sl-hang"
	    ><span class="sl-ihead-h">HOMOPHONES</span
	    ><xsl:for-each select="$hnodes"
	    ><xsl:sort select="@sort" data-type="number"/><esp:link notarget="yes"
	    url="{concat('/',/*/@project,'/signlist/selpages/',@xml:id,'.html')}"><span class="homophone-n"><xsl:value-of select="@n"/></span></esp:link
	    ><xsl:if test="not(position()=last())"><wbr/></xsl:if
	    ></xsl:for-each
	    ></p
	    ></xsl:if>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-sel-page">
  <xsl:param name="file"/>
  <xsl:param name="type"/>
  <xsl:param name="title"/>
  <xsl:param name="nodes"/>
  <xsl:param name="parameters"/>
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
	<body class="selpage">
	  <xsl:call-template name="esp2-banner-div">
	    <xsl:with-param name="parameters" select="$parameters"/>
	    <xsl:with-param name="project" select="$project"/>
	    <xsl:with-param name="current-page" select="ancestor-or-self::sl:sign"/>
	    <xsl:with-param name="nomenu" select="true()"/>
	  </xsl:call-template>
	  <h2><xsl:value-of select="$title"/></h2>
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
  <td>
    <xsl:choose>
      <xsl:when test="self::sl:form">
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
  <td><xsl:value-of select="sl:ucun"/></td>
  <td>
    <xsl:for-each select="ancestor-or-self::sl:sign/sl:v">
      <xsl:value-of select="@n"/>
      <xsl:if test="not(position() = last())"><xsl:text> </xsl:text></xsl:if>
    </xsl:for-each>
  </td>
</xsl:template>

<xsl:template match="sl:base">
  <xsl:param name="parameters"/>
  <xsl:call-template name="sws-sel-page">
    <xsl:with-param name="parameters" select="$parameters"/>
    <xsl:with-param name="file" select="@xml:id"/>
    <xsl:with-param name="type" select="'h'"/>
    <xsl:with-param name="title" select="concat(@n,  ' homophones')"/>
    <xsl:with-param name="nodes" select="sl:h"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="sws-lem-page-by-pos">
  <xsl:param name="nodes"/>
  <xsl:param name="type"/>
  <xsl:param name="group-label" select="''"/>
  <xsl:variable name="url-base">
    <xsl:choose>
      <xsl:when test="/*/@lemma-http">
      <xsl:value-of select="/*/@lemma-http"/></xsl:when>
      <xsl:otherwise><xsl:value-of select="concat('/', /*/@project)"/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:if test="count($nodes)>0">
    <tr class="lemsel-h3"><td colspan="3"><xsl:value-of select="$type"></xsl:value-of></td></tr>
    <xsl:for-each select="$nodes">
      <xsl:sort select="@sort" data-type="number"/>
      <xsl:variable name="k" select="concat(@n,'::',@base)"/>
      <xsl:if test="generate-id(.)=generate-id(key('lemuniq',$k)[1])">
	<tr class="lemlem">
	  <td class="asl-sel-l-base"><xsl:value-of select="@base"/></td>
	  <td class="asl-sel-l-link"><a href="{$url-base}/{@oid}"><xsl:value-of select="@n"/></a></td>
	  <td class="asl-sel-l-count"><xsl:value-of select="@bcnt"/><xsl:text>×</xsl:text></td>
	</tr>
      </xsl:if>
    </xsl:for-each>
  </xsl:if>
</xsl:template>

<xsl:template name="sws-lem-page">
  <xsl:param name="parameters"/>  
  <ex:document href="{concat('signlist/01bld/selpages/',@xml:id,'-sux.html')}"
	       method="xml" encoding="utf-8" omit-xml-declaration="yes"
	       indent="yes" doctype-system="html">
    <html>
      <head>
	<xsl:call-template name="esp2-head-content">
	  <xsl:with-param name="parameters" select="$parameters"/>
	  <xsl:with-param name="project" select="$project"/>
	</xsl:call-template>
      </head>
      <body class="selpage">
	<xsl:call-template name="esp2-banner-div">
	  <xsl:with-param name="parameters" select="$parameters"/>
	  <xsl:with-param name="project" select="$project"/>
	  <xsl:with-param name="current-page" select="ancestor-or-self::sl:sign"/>
	  <xsl:with-param name="nomenu" select="true()"/>
	</xsl:call-template>
	<h2>Sumerian Words written with <xsl:value-of select="@n"/></h2>
	<table class="lemsel">
	  <xsl:variable name="all-lem" select=".//sl:lemmas|id(@merge)//sl:lemmas"/>
	  <xsl:variable name="sf-lem" select="$all-lem[not(../self::sl:v)]"/>
	  <xsl:if test="count($sf-lem)>0">
	    <tbody>
	      <xsl:for-each select="$sf-lem">
		<xsl:variable name="p" select="(ancestor::sl:form|ancestor::sl:sign)[1]"/>
		<tr class="lemsel-h2">
		  <td colspan="3">
		    <xsl:text>For sign </xsl:text>
		    <span class="snames"><xsl:value-of select="$p/@n"/></span>
		  </td>
		</tr>
		<xsl:call-template name="sws-lem-page-by-pos">
		  <xsl:with-param name="nodes" select="sl:lemma[@gpos='a']"/>
		  <xsl:with-param name="type" select="'Independent'"/>
		</xsl:call-template>
		<xsl:call-template name="sws-lem-page-by-pos">
		  <xsl:with-param name="nodes" select="sl:lemma[not(@gpos='a') and not(@gpos='A')]"/>
		  <xsl:with-param name="type" select="'Combined'"/>
		</xsl:call-template>
		<xsl:call-template name="sws-lem-page-by-pos">
		  <xsl:with-param name="nodes" select="sl:lemma[@gpos='A']"/>
		  <xsl:with-param name="type" select="'Compounds'"/>
		</xsl:call-template>
	      </xsl:for-each>
	    </tbody>
	  </xsl:if>
	  <xsl:variable name="v" select=".//sl:v|id(@merge)//sl:v"/>
	  <xsl:for-each select="$v">
	    <xsl:sort select="@sort" data-type="number"/>
	    <xsl:if test="sl:lemmas">
	      <tbody>
		<tr class="lemsel-h2">
		  <td colspan="3">
		    <xsl:text>For value </xsl:text>
		    <xsl:value-of select="@n"/>
		    <xsl:if test="../@moid|../self::sl:form"><xsl:value-of select="concat(' [',../@n,']')"/></xsl:if>
		  </td>
		</tr>
		<xsl:call-template name="sws-lem-page-by-pos">
		  <xsl:with-param name="nodes" select="sl:lemmas/*[@gpos='a']"/>
		  <xsl:with-param name="type" select="'Independent'"/>
		</xsl:call-template>
		<xsl:call-template name="sws-lem-page-by-pos">
		  <xsl:with-param name="nodes" select="sl:lemmas/*[not(@gpos='a') and not(@gpos='A')]"/>
		  <xsl:with-param name="type" select="'Combined'"/>
		</xsl:call-template>
		<xsl:call-template name="sws-lem-page-by-pos">
		  <xsl:with-param name="nodes" select="sl:lemmas/*[@gpos='A']"/>
		  <xsl:with-param name="type" select="'Compounds'"/>
		</xsl:call-template>
	      </tbody>
	    </xsl:if>
	  </xsl:for-each>
	</table>
      </body>
    </html>
  </ex:document>
</xsl:template>

<xsl:template name="sws-lex-page">
  <xsl:param name="parameters"/>
  <xsl:if test="$asl-lexdata = 'yes'">
    <xsl:variable name="lex-ok">
      <xsl:call-template name="lex-provides-sign"/>
    </xsl:variable>
    <xsl:if test="$lex-ok='y'">
      <ex:document href="{concat('signlist/01bld/selpages/',@xml:id,'-lex.html')}"
		   method="xml" encoding="utf-8" omit-xml-declaration="yes"
		   indent="yes" doctype-system="html">
	<html>
	  <head>
	    <xsl:call-template name="esp2-head-content">
	      <xsl:with-param name="parameters" select="$parameters"/>
	      <xsl:with-param name="project" select="$project"/>
	    </xsl:call-template>
	  </head>
	  <body class="selpage">
	    <xsl:call-template name="esp2-banner-div">
	      <xsl:with-param name="parameters" select="$parameters"/>
	      <xsl:with-param name="project" select="$project"/>
	      <xsl:with-param name="current-page" select="ancestor-or-self::sl:sign"/>
	      <xsl:with-param name="nomenu" select="true()"/>
	    </xsl:call-template>
	    <xsl:call-template name="lex-sign-data"/>
	  </body>
	</html>
      </ex:document>
    </xsl:if>
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
