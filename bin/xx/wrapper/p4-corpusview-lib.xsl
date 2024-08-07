<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:note="http://oracc.org/ns/note/1.0"
  xmlns:xmd="http://oracc.org/ns/xmd/1.0"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xst="http://oracc.org/ns/syntax-tree/1.0"
  exclude-result-prefixes="note xmd xst xtr xh">

<!--  xmlns="http://www.w3.org/1999/xhtml" -->

<xsl:output method="xml" encoding="utf-8" indent="yes" omit-xml-declaration="yes"/>

<xsl:include href="xpd.xsl"/>
<xsl:include href="xtr-label.xsl"/>

<xsl:include href="xst-HTML.xsl"/>

<xsl:key name="xtr" match="xh:p" use="@xtr:unit"/>
<xsl:key name="xtr-h" match="xh:h1|xh:h2|xh:h3|xh:h4" use="@xtr:hdr-ref"/>
<xsl:key name="xst" match="xst:unit" use="@ref"/>
<xsl:key name="tr-id" match="xh:p" use="@xtr:ref|@xtr:sref"/>

<xsl:param name="frag-id"/>
<xsl:param name="line-id"/>
<xsl:param name="seqfile"/>
<xsl:param name="trans" select="'en'"/>
<xsl:param name="transonly" select="'false'"/>

<xsl:param name="oracc" select="'/home/oracc'"/>

<xsl:variable name="outer-id" select="/*/@xml:id"/>

<xsl:variable name="config-xml-file" 
	      select="concat($oracc,'/xml/',$project,'/','config.xml')"/>

<xsl:param name="external-url-host">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="config-xml" select="$config-xml-file"/>
    <xsl:with-param name="option" select="'external-url-host'"/>
  </xsl:call-template>
</xsl:param>

<xsl:template name="corpusview-project-pqid">
  <xsl:param name="project"/>
  <xsl:param name="host"/>
  <xsl:param name="pqid"/>
  <xsl:param name="txhdir"/>
  
  <xsl:variable 
      name="basedir" 
      select="concat($oracc,'/bld/',$project,'/',substring($pqid,1,4),'/',$pqid,'/')"/>

  <xsl:variable name="basename" select="concat($basedir,$pqid)"/>

  <xsl:variable name="txh-name">
    <xsl:choose>
      <xsl:when test="string-length($txhdir) > 0">
	<xsl:value-of select="concat($txhdir,'/',$pqid,'.txh')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="concat($basename,'.txh')"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <!--
  <xsl:message>corpusview:basename: <xsl:value-of select="$basename"/></xsl:message>
  <xsl:message>corpusview:txhdir: <xsl:value-of select="$txhdir"/></xsl:message>
  <xsl:message>corpusview:txh-name: <xsl:value-of select="$txh-name"/></xsl:message>
  -->
  
  <xsl:variable name="xmd" select="document(concat($basename,'.xmd'))/xmd:xmd"/>
  <xsl:variable name="txh" select="document($txh-name,/)"/>
  <xsl:variable name="uxh" select="document(concat($basename,'.uxh'),/)"/>
  <xsl:variable name="xst" select="document(concat($basename,'.xst'),/)"/>
  <xsl:variable name="xtr" select="document(concat($basename,'_project-',$trans,'.xtr'))"/>

  <xsl:variable name="name">
    <xsl:choose>
      <xsl:when test="string-length($xmd/xmd:cat/xmd:c_name)>0">
	<xsl:value-of select="$xmd/xmd:cat/xmd:c_name"/>
      </xsl:when>
      <xsl:when test="string-length($xmd/xmd:cat/xmd:xname)>0">
	<xsl:value-of select="$xmd/xmd:cat/xmd:xname"/>
      </xsl:when>
      <xsl:when test="string-length($xmd/xmd:cat/xmd:name)>0">
	<xsl:value-of select="$xmd/xmd:cat/xmd:name"/>
      </xsl:when>
      <xsl:when test="string-length($xmd/xmd:cat/xmd:designation)>0">
	<xsl:value-of select="$xmd/xmd:cat/xmd:designation"/>
      </xsl:when>
      <xsl:when test="string-length($xmd/xmd:cat/xmd:museum_no)>0">
	<xsl:value-of select="$xmd/xmd:cat/xmd:museum_no"/>
      </xsl:when>
      <xsl:when test="string-length($xmd/xmd:cat/xmd:accession_no)>0">
	<xsl:value-of select="$xmd/xmd:cat/xmd:accession_no"/>
      </xsl:when>
      <xsl:when test="string-length($xmd/xmd:cat/xmd:excavation_no)>0">
	<xsl:value-of select="$xmd/xmd:cat/xmd:excavation_no"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$xmd/xmd:cat/*"/> <!-- horrid, but what else to do?-->
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="$xmd/xmd:cat/xmd:title">
      <xsl:value-of select="concat('. ',$xmd/xmd:cat/xmd:title)"/>
    </xsl:if>
  </xsl:variable>

  <div class="text" dc:title="{$project} {$pqid}">
    <xsl:comment><xsl:value-of select="concat('config-xml-file = ',$config-xml-file)"/></xsl:comment>
    <xsl:choose>
      <xsl:when test="count($uxh) > 0 and $uxh/xh:p/xh:span">
	<xsl:call-template name="unitary-view">
	  <xsl:with-param name="config-xml-file" select="$config-xml-file"/>
	  <xsl:with-param name="host" select="host"/>
	  <xsl:with-param name="pqid" select="$pqid"/>
	  <xsl:with-param name="xmd" select="$xmd"/>
	  <xsl:with-param name="txh" select="$txh"/>
	  <xsl:with-param name="uxh" select="$uxh"/>
	  <xsl:with-param name="xtr" select="$xtr"/>
	  <xsl:with-param name="xst" select="$xst"/>
	  <xsl:with-param name="name" select="$name"/>
	</xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="linear-view">
	  <xsl:with-param name="config-xml-file" select="$config-xml-file"/>
	  <xsl:with-param name="host" select="$host"/>
	  <xsl:with-param name="pqid" select="$pqid"/>
	  <xsl:with-param name="xmd" select="$xmd"/>
	  <xsl:with-param name="txh" select="$txh"/>
	  <xsl:with-param name="uxh" select="$uxh"/>
	  <xsl:with-param name="xtr" select="$xtr"/>
	  <xsl:with-param name="xst" select="$xst"/>
	  <xsl:with-param name="name" select="$name"/>
	  <xsl:with-param name="basename" select="$basename"/>
	</xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="$xmd/xmd:cat/xmd:credits">
      <hr class="credits"/>
      <p><xsl:apply-templates select="$xmd/xmd:cat/xmd:credits"/></p>
    </xsl:if>
  </div>
</xsl:template>

<xsl:template name="unitary-view">
  <xsl:param name="config-xml-file"/>
  <xsl:param name="host"/>
  <xsl:param name="xmd"/>
  <xsl:param name="txh"/>
  <xsl:param name="uxh"/>
  <xsl:param name="xst"/>
  <xsl:param name="name"/>
  <xsl:param name="basename"/>
  <xsl:variable name="cat" select="/*"/>
<!--  <xsl:call-template name="navigator"/> -->
  <xsl:for-each select="$uxh/*/*">
    <xsl:variable name="u" select="@ref"/>
    <div class="unit">
      <h2 class="p3h2 border-top heading">
	<xsl:value-of select="concat($name,' unit ', substring-after($u,'.U'))"/>
      </h2>
      <table>
	<tr>
	  <td class="tlt">
	    <xsl:choose>
	      <xsl:when test="string-length(.) > 0">
		<xsl:copy-of select="."/>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:text> </xsl:text>
	      </xsl:otherwise>
	    </xsl:choose>
	  </td>
	  <td class="syn">
	    <xsl:call-template name="gen-xst">
	      <xsl:with-param name="u" select="$u"/>
	    </xsl:call-template>
	  </td>
	</tr>
      </table>
      <xsl:call-template name="gen-xtr">
	<xsl:with-param name="u" select="$u"/>
	<xsl:with-param name="xtr" select="$xtr"/>
      </xsl:call-template>
    </div>
  </xsl:for-each>
  <h2 class="endview">End of text</h2>
</xsl:template>

<xsl:template name="gen-xst">
  <xsl:param name="u"/>
  <xsl:for-each select="$xst">
    <xsl:apply-templates select="key('xst',$u)">
      <xsl:with-param name="clean" select="true()"/>
    </xsl:apply-templates>
  </xsl:for-each>
</xsl:template>

<xsl:template name="gen-xtr">
  <xsl:param name="u"/>
  <xsl:param name="xtr"/>
  <xsl:for-each select="$xtr">
    <xsl:for-each select="key('xtr',$u)">
<!--      <h3 class="view">TRANSLATION</h3> -->
      <div class="trans">
	<xsl:element name="{local-name()}">
	  <xsl:copy-of select="@class"/>
	  <xsl:apply-templates/>
	</xsl:element>
      </div>
      <xsl:if test="following-sibling::*[1][local-name() = 'div']">
	<hr class="notesep"/>
	<xsl:apply-templates select="following-sibling::*[1]"/>
      </xsl:if>
    </xsl:for-each>  
  </xsl:for-each>
</xsl:template>

<xsl:template name="linear-view">
  <xsl:param name="config-xml-file"/>
  <xsl:param name="pqid"/>
  <xsl:param name="host"/> 
  <xsl:param name="xmd"/>
  <xsl:param name="txh"/>
  <xsl:param name="uxh"/>
  <xsl:param name="xtr"/>
  <xsl:param name="xst"/>
  <xsl:param name="name"/>
  <xsl:variable name="ext-host">
    <xsl:choose>
      <xsl:when test="string-length($external-url-host)>0">
	<xsl:value-of select="$external-url-host"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>cdli.ucla.edu</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="starts-with($pqid,'P')">
      <h1 class="p3h2 border-top heading">
	<a href="http://{$ext-host}/{$pqid}" target="_blank">
	  <xsl:value-of select="$name"/>
	  <xsl:if test="not($name=$xmd/xmd:cat/xmd:primary_publication)
			and string-length($xmd/xmd:cat/xmd:primary_publication)>0"
		  >
	    <xsl:text> (</xsl:text>
	    <xsl:value-of select="$xmd/xmd:cat/xmd:primary_publication"/>
	    <xsl:text>)</xsl:text>
	  </xsl:if>
	  <xsl:if test="string-length($xmd/xmd:cat/xmd:subgenre) > 0">
	    <xsl:text> [</xsl:text>
	    <xsl:value-of select="$xmd/xmd:cat/xmd:subgenre"/>
	    <xsl:text>]</xsl:text>
	  </xsl:if>
	</a>
	<xsl:call-template name="via"/>
      </h1>
    </xsl:when>
    <xsl:otherwise>
      <h1 class="p3h2 heading border-top">
	<xsl:value-of select="$name"/>
	<xsl:if test="not($name=$xmd/xmd:cat/xmd:primary_publication)
		      and string-length($xmd/xmd:cat/xmd:primary_publication)>0"
		>
	  <xsl:text> (</xsl:text>
	  <xsl:value-of select="$xmd/xmd:cat/xmd:primary_publication"/>
	  <xsl:text>)</xsl:text>
	</xsl:if>

	<xsl:if test="string-length($host) > 0 
		      and not($project=$host)">
	  <xsl:value-of select="concat(' &lt;from ',$project,'>')"/>
	</xsl:if>
      </h1>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="do-tlit">
    <xsl:with-param name="pqid" select="$pqid"/>
    <xsl:with-param name="xmd" select="$xmd"/>
    <xsl:with-param name="txh" select="$txh"/>
    <xsl:with-param name="xtr" select="$xtr"/>
    <xsl:with-param name="config-xml-file" select="$config-xml-file"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="preview-view">
  <xsl:param name="pqid"/>
  <xsl:param name="name"/>
  <xsl:param name="txh"/>
  <xsl:param name="xtr"/>
  <xsl:param name="config-xml-file"/>
  <h1 class="p3h2 heading border-top">
    <a href="http://cdli.ucla.edu/{$pqid}" target="_blank">
      Preview of <xsl:value-of select="$pqid"/> = <xsl:value-of select="$name"/>
    </a>
    <xsl:call-template name="via"/>
  </h1>
   <xsl:choose>
    <xsl:when test="count($txh) > 0">
      <xsl:apply-templates select="$txh/*">
	<xsl:with-param name="txh" select="$txh"/>
	<xsl:with-param name="xtr" select="$xtr"/>
  	<xsl:with-param name="config-xml-file" select="$config-xml-file"/>
    </xsl:apply-templates>
    </xsl:when>
    <xsl:otherwise>
      <p>(No transliterationX available)</p>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="do-tlat">
  <xsl:param name="xmd"/>
  <xsl:param name="xtr"/>
  <xsl:param name="config-xml-file"/>

  <xsl:apply-templates select="$xtr/*">
    <xsl:with-param name="config-xml-file" select="$config-xml-file"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="xh:div">
  <xsl:param name="xmd"/>
  <xsl:param name="txh"/>
  <xsl:param name="xtr"/>
  <xsl:param name="config-xml-file"/>

  <xsl:apply-templates>
    <xsl:with-param name="xmd" select="$xmd"/>
    <xsl:with-param name="txh" select="$txh"/>
    <xsl:with-param name="xtr" select="$xtr"/>
    <xsl:with-param name="config-xml-file" select="$config-xml-file"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template name="do-tlit">
  <xsl:param name="pqid"/>
  <xsl:param name="xmd"/>
  <xsl:param name="txh"/>
  <xsl:param name="xtr"/>
  <xsl:param name="config-xml-file"/>

  <xsl:choose>
    <xsl:when test="count($txh) > 0">
      <xsl:apply-templates select="$txh/*">
	<xsl:with-param name="xmd" select="$xmd"/>
	<xsl:with-param name="xtr" select="$xtr"/>
	<xsl:with-param name="txh" select="$txh"/>
	<xsl:with-param name="config-xml-file" select="$config-xml-file"/>
      </xsl:apply-templates>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="pager-fallback-cdli">
	<xsl:call-template name="xpd-option">
	  <xsl:with-param name="config-xml" select="$config-xml-file"/>
	  <xsl:with-param name="option" select="'pager-fallback-cdli'"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:choose>
	<xsl:when test="$pager-fallback-cdli='yes' and starts-with($pqid,'P')">
	  <xsl:variable name="cdli-url" select="concat('https://cdli.ucla.edu/',$pqid)"/>
	  <h3>No transliteration available: you can try the CDLI page
	  <a href="{$cdli-url}"><xsl:value-of select="$cdli-url"/></a></h3>
	  <!--<p>bdtns_id = <xsl:value-of select="$xmd/xmd:cat/xmd:bdtns_id"/></p>-->
	  <xsl:if test="string-length($xmd/xmd:cat/xmd:bdtns_id)>0">
	    <xsl:variable name="bdtns-url">
	      <xsl:value-of select="concat('http://bdtns.filol.csic.es/',$xmd/xmd:cat/xmd:bdtns_id)"/>
	    </xsl:variable>
	    <!--<p><xsl:value-of select="$bdtns-url"/></p>-->
	    <h3>There is also a BDTNS record available at <a href="{$bdtns-url}"><xsl:value-of select="$bdtns-url"/></a></h3>
	    <!--<iframe width="90%" height="500" src="{$bdtns-url}">&#xa0;</iframe>-->
	  </xsl:if>
	</xsl:when>
	<xsl:otherwise>
	  <p>(No transliteration available)</p>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xh:tr[@class='hforce']">
  <xsl:param name="txh"/>
  <xsl:param name="xtr"/>
  <xsl:param name="tlit-cols"/>

  <xsl:variable name="id" select="@xml:id"/>
  <xsl:variable name="tlit-h" select="."/>

  <!-- emit any translation headings that are supposed to precede this line -->
  <xsl:for-each select="$xtr">
    <xsl:for-each select="key('xtr-h',$id)">
      <xsl:if test="not(position()=last())">
	<tr class="xtr-h xtr">
	  <td colspan="{1+$tlit-cols}"> </td>
	  <td colspan="{$xtr/*/@xtr:cols}" class="{concat('xtr-',local-name(.))}">
	    <xsl:choose>
	      <xsl:when test="string-length(.) > 0">
		<xsl:apply-templates/>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:text> </xsl:text>
	      </xsl:otherwise>
	    </xsl:choose>
	  </td>
	</tr>
      </xsl:if>
    </xsl:for-each>
  </xsl:for-each>
 
  <!-- this code needs revising to check that the cases where there is no translation
       or no matching translation-heading work correctly -->
  <xsl:choose>
    <xsl:when test="$txh and $txh/xh:table[@class='score']|$txh/*/xh:table[@class='score']">
      <xsl:copy-of select="."/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:for-each select="$xtr">
	<xsl:for-each select="key('xtr-h',$id)">
	  <xsl:if test="position()=last()">
	    <tr class="hforce">
	      <td colspan="{1+$tlit-cols}">
		<xsl:for-each select="$tlit-h/xh:td">
		  <xsl:apply-templates>
		    <xsl:with-param name="xtr" select="$xtr"/>
		    <xsl:with-param name="txh" select="$txh"/>
		  </xsl:apply-templates>
		</xsl:for-each>
	      </td>
	      <!--  colspan="{$xtr/*/@xtr:cols}" -->
	      <td class="{concat('xtr-',local-name(.))}">
		<xsl:choose>
		  <xsl:when test="string-length(.) > 0">
		    <xsl:apply-templates/>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:text> </xsl:text>
		  </xsl:otherwise>
		</xsl:choose>
	      </td>
	    </tr>
	  </xsl:if>
	</xsl:for-each>
      </xsl:for-each>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xh:tr[not(@class='hforce')]">
  <xsl:param name="txh"/>
  <xsl:param name="xtr"/>
  <xsl:param name="tlit-cols"/>
  <xsl:param name="config-xml-file"/>

  <xsl:variable name="id" select="@xml:id"/>
  <xsl:variable name="select">
    <xsl:if test="$id=$line-id"><xsl:message>hiliting <xsl:value-of select="$line-id"/></xsl:message><xsl:text> selected</xsl:text></xsl:if>
  </xsl:variable>
  <xsl:variable name="class" select="concat(@class, $select)"/>

  <!-- emit any translation headings that are supposed to precede this line -->
  <xsl:for-each select="$xtr">
    <xsl:for-each select="key('xtr-h',$id)">
      <tr class="xtr-h">
	<td colspan="{1+$tlit-cols}"> </td>
	<td colspan="{$xtr/*/@xtr:cols}" class="{concat('xtr-',local-name(.))}">
	  <xsl:choose>
	    <xsl:when test="string-length(.) > 0">
	      <xsl:apply-templates/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text> </xsl:text>
	    </xsl:otherwise>
	  </xsl:choose>
	</td>
      </tr>
    </xsl:for-each>
  </xsl:for-each>

  <xsl:variable name="labeled">
    <xsl:choose>
      <xsl:when test="$xtr/xtr:translation">
	<xsl:if test="$xtr/xtr:translation/@xtr:type = 'labeled'">
	  <xsl:text>labeled</xsl:text>
	</xsl:if>
      </xsl:when>
      <xsl:otherwise>
	<xsl:if test="$xtr/@xtr:type = 'labeled'">
	  <xsl:text>labeled</xsl:text>
	</xsl:if>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <!-- determine how many translation TR paras correspond to this line -->
  <xsl:variable name="xtr-count">
    <xsl:for-each select="$xtr">
      <xsl:value-of select="count(key('tr-id',$id))"/>
    </xsl:for-each>
  </xsl:variable>

  <!-- this is where we copy the <tr> element and use the first of the TR paras -->
  <xsl:copy>
    <xsl:attribute name="id">
      <xsl:value-of select="@xml:id"/>
    </xsl:attribute>
    <xsl:attribute name="class">
      <xsl:value-of select="$class"/>
    </xsl:attribute>

    <!-- this copies the TD elements of the transliteration -->
    <xsl:apply-templates select="*[position() &lt; last()]">
      <xsl:with-param name="hilite" select="$select"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="*[last()]" mode="maybe-with-note-marker">
      <xsl:with-param name="hilite" select="$select"/>
    </xsl:apply-templates>

    <xsl:choose>
      <xsl:when test="$xtr-count > 0">
	<xsl:for-each select="$xtr">
	  <xsl:for-each select="key('tr-id',$id)[1]">
	    <xsl:call-template name="render-xtr">
	      <xsl:with-param name="config-xml-file" select="$config-xml-file"/>
	      <xsl:with-param name="labeled" select="$labeled"/>
	    </xsl:call-template>
	  </xsl:for-each>
	</xsl:for-each>
      </xsl:when>
<!--
      <xsl:when test="count($itr/*) > 0">
	<xsl:for-each select="$itr">
	  <xsl:for-each select="key('tr-id',$id)">
	    <td class="t1"><xsl:apply-templates select="."/></td>
	  </xsl:for-each>
	</xsl:for-each>
      </xsl:when>
 -->
    </xsl:choose>
  </xsl:copy>

  <!-- Now process the rest of the TR paras with empty transliteration cells
       opposite them -->
  <xsl:if test="$xtr-count > 1">
    <xsl:for-each select="$xtr">
      <xsl:for-each select="key('tr-id',$id)[position()>1]">
	<tr>
	  <td colspan="2"> </td> <!--FIXME: should sum any colspans in TXH's tr also-->
	  <xsl:call-template name="render-xtr">
	    <xsl:with-param name="labeled" select="$labeled"/>
	  </xsl:call-template>
	</tr>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:if>

</xsl:template>

<xsl:template name="render-xtr">
  <xsl:param name="labeled"/>
  <xsl:param name="config-xml-file"/>
  <xsl:variable name="select-this">
    <xsl:choose>
      <xsl:when test="@xml:id=$line-id"> <!-- can be span or p -->
	<xsl:text>selected</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="xh:span[@class='cell']">
      <xsl:variable name="xid" select="@xml:id"/>
      <xsl:for-each select="xh:span[1]">
	<xsl:variable name="class">
	  <xsl:choose>
	    <xsl:when test="string-length($select-this)>0">
	      <xsl:value-of select="concat('t1',' selected')"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text>t1</xsl:text>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<td class="{$class} xtr">
	  <xsl:if test="@xtr:span">
	    <xsl:variable name="cspan">
	      <xsl:choose>
		<xsl:when test="../@xtr:spanall">
		  <xsl:value-of select="ancestor::xtr:translation/@xtr:cols"/>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:value-of select="@xtr:span"/>
		</xsl:otherwise>
	      </xsl:choose>
	    </xsl:variable>
	    <xsl:attribute name="colspan">
	      <xsl:value-of select="$cspan"/>
	    </xsl:attribute>
	  </xsl:if>
	  <p class="tr">
	    <xsl:copy-of select="ancestor::xtr:translation/@dir"/>
	    <xsl:apply-templates/>
	  </p>
	</td>
      </xsl:for-each>
      <xsl:for-each select="xh:span[position()>1]">
	<td class="t2 xtr">
	  <xsl:if test="@xtr:span">
	    <xsl:attribute name="colspan">
	      <xsl:value-of select="@xtr:span"/>
	    </xsl:attribute>
	  </xsl:if>
	  <p class="tr"><xsl:apply-templates/></p>
	</td>
      </xsl:for-each>
    </xsl:when>
    <xsl:otherwise>
      <td class="t1 xtr">
	<xsl:if test="@xtr:rows">
	  <xsl:variable name="rows" select="@xtr:rows"/>
	  <xsl:if test="not($rows = 'NaN') and $rows > 0">
	    <xsl:choose>
	      <xsl:when test="following-sibling::xh:p[1][@xtr:overlap='1']">
		<xsl:if test="$rows > 1">
		  <xsl:attribute name="rowspan">
		    <xsl:value-of select="number($rows)-1"/>
		  </xsl:attribute>
		</xsl:if>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:attribute name="rowspan">
		  <xsl:value-of select="$rows"/>
		</xsl:attribute>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:if>
	</xsl:if>
	<xsl:if test="@class='dollar'">
	  <xsl:attribute name="colspan">
	    <xsl:value-of select="ancestor::xtr:translation/@xtr:cols"/>
	  </xsl:attribute>
	</xsl:if>
<!--
	<xsl:message>Processing xh:p with xml:id=<xsl:value-of
	select="@xml:id"/> when line-id=<xsl:value-of 
	select="$line-id"/></xsl:message>
 -->
	<p>
	  <xsl:choose>
	    <xsl:when test="@xml:id=$line-id">
	      <xsl:attribute name="class">tr selected</xsl:attribute>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:attribute name="class">tr</xsl:attribute>
	    </xsl:otherwise>
	  </xsl:choose>
	  <span>
	    <xsl:attribute name="class">
	      <xsl:choose>
		<xsl:when test="@class='dollar'">
		  <xsl:text>xtr-dollar</xsl:text>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:text>xtr-label</xsl:text>
		</xsl:otherwise>
	      </xsl:choose>
	    </xsl:attribute>
	    <xsl:call-template name="xtr-label">
	      <xsl:with-param name="config-xml" select="$config-xml-file"/>
	    </xsl:call-template>
	  </span>
	  <a name="a.{@xml:id}"><xsl:text> </xsl:text></a>
	  <xsl:choose>
	    <xsl:when test="@class='dollar'">
	      <xsl:choose>
		<xsl:when test="text()='(SPACER)'"/>
		<xsl:when test="text()='ruling'">
		  <hr/>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:copy-of select="."/>
		</xsl:otherwise>
	      </xsl:choose>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:apply-templates/>
	    </xsl:otherwise>
	  </xsl:choose>
	</p>
      </td>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xh:table[@class='transliteration' or @class='composite']">
  <xsl:param name="xtr"/>
  <xsl:param name="txh"/>
  <xsl:param name="xmd"/>

  <xsl:choose>
    <xsl:when test="not(xh:table)">
      <xsl:variable name="tlit-cols">
	<xsl:choose>
	  <xsl:when test="$txh/*/*/@cols">
	    <xsl:value-of select="number($txh/*/*/@cols)"/>
	  </xsl:when>
	  <xsl:when test="$txh/*/@cols">
	    <xsl:value-of select="number($txh/*/@cols)"/>
	  </xsl:when>
	  <xsl:when test="$txh/@cols">
	    <xsl:value-of select="number($txh/@cols)"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="number('2')"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:variable>

      <xsl:variable name="tlat-cols">
	<xsl:choose>
	  <xsl:when test="$xtr and $xtr/* and $xtr/*/@xtr:cols">
	    <xsl:value-of select="$xtr/*/@xtr:cols"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="number('0')"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:variable>

      <xsl:variable name="ncols" select="$tlit-cols+$tlat-cols"/>

      <xsl:variable name="p">
	<xsl:choose>
	  <xsl:when test="$ncols>10"/>
	  <xsl:otherwise>
	    <xsl:value-of select="concat(' p',string($ncols))"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:variable>

      <xsl:element name="{local-name()}">
	<xsl:copy-of select="@class"/>
	<!-- this is a template line -->
	<tr class="{concat('l',$p)}">
	  <td class="lnum"><xsl:text> </xsl:text></td>
	  <td class="tlit"><xsl:text> </xsl:text></td>
	  <xsl:for-each select="document('')/*/*[position()&lt;$ncols]">
	    <td><xsl:text> </xsl:text></td>
	  </xsl:for-each>
	</tr>
	<xsl:if test="$xmd/*/xmd:cat/xmd:ch_name"> <!--hack to do SAA headers-->
	  <tr>
	    <td class="textnum" colspan="2">
	      <xsl:value-of select="concat($xmd/*/xmd:cat/xmd:mus_no,'&#xa0;&#xa0;&#xa0;',
				    $xmd/*/xmd:cat/xmd:script)"/>
	    </td>
	    <td class="textnum">
	      <xsl:value-of select="$xmd/*/xmd:cat/xmd:pub_no"/>
	    </td>
	  </tr>
	</xsl:if>
	<xsl:apply-templates>
	  <xsl:with-param name="xtr" select="$xtr"/>
	  <xsl:with-param name="txh" select="$txh"/>
	  <xsl:with-param name="tlit-cols" select="$tlit-cols"/>
	</xsl:apply-templates>
      </xsl:element>

      <!-- n.b.: note IDs are xml:id in the .xtf files, but are mapped to HTML id attr in the .txh files that
	   this xsl script operates on -->
      <div class="p3notes">
	<xsl:for-each select="following-sibling::xh:div[@class='p3notes']/xh:p[@class='note']">
	  <p class="note" onmouseout="hideNote(event,'{@id}')" onclick="hideNote(event,'{@id}')" id="{@id}">
	    <span class="notemark"><xsl:value-of select="@mark"/></span>
	    <xsl:apply-templates mode="notediv"/>
	  </p>
	</xsl:for-each>
	<!-- but they are xml:id in the .xtr files :( [sort this out, Steve] -->
	<xsl:if test="$xtr and $xtr/*">
	  <xsl:for-each select="$xtr/*/xh:div[@class='note']">
	    <p class="note" id="{@xml:id}">
	      <xsl:attribute 
		  name="onmouseout">hideNote(event,'<xsl:value-of select="@xml:id"/>')</xsl:attribute>
	      <xsl:attribute 
		  name="onclick">hideNote(event,'<xsl:value-of select="@xml:id"/>')</xsl:attribute>
	      <xsl:apply-templates mode="notediv"/>
	    </p>
	  </xsl:for-each>
	</xsl:if>
	<p/>
      </div>
    </xsl:when>
    <xsl:otherwise>
      <!--This can't happen if the .txh file has been produced using the proper XSL script-->
      <p>Incompatible .txh version in <xsl:value-of select="$project"/>:<xsl:value-of select="$outer-id"/>.</p>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xh:div[@class='p3notes']"/>

<xsl:template match="xh:innerp[@class='tr-comment']"/>

<xsl:template match="xh:innerp">
  <xsl:apply-templates/>
  <xsl:if test="following-sibling::xh:innerp">
    <br/><span class="indent"/>
  </xsl:if>
</xsl:template>

<!-- This is the note mark in the translation para of XTR data -->
<xsl:template match="xh:span[@class='notelink']">
  <a href="javascript://" 
     onmouseover="showNote(event,'{@note:ref}', 1)" 
     onmouseout="hideNote(event,'{@note:ref}')">
    <xsl:copy>
      <xsl:copy-of select="@class"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </a>
</xsl:template>

<xsl:template match="xh:span[@class='w']">
  <xsl:element name="{local-name()}">
    <xsl:copy-of select="@*[not(name()='xml:id')]"/>
    <xsl:choose>
      <xsl:when test="substring(@xml:id,string-length(@xml:id)-1) = '.0'">
	<a name="{concat('a.',substring-before(@xml:id,'.0'))}">
	  <xsl:choose>
	    <xsl:when test="string-length(.) > 0">
	      <xsl:apply-templates/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text> </xsl:text>
	    </xsl:otherwise>
	  </xsl:choose>
	</a>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:element>
</xsl:template>

<xsl:template mode="maybe-with-note-marker" match="xh:td">
  <xsl:param name="hilite" select="''"/>
  <xsl:variable name="marker">
    <xsl:if test="../following-sibling::*[1][@class='tnote'][not(@mark)]">
      <xsl:value-of select="1+count(../preceding-sibling::*[@class='note'])"/>
    </xsl:if>
  </xsl:variable>
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:choose>
      <xsl:when test="xh:p"> <!-- we assume only 1 p within <td> -->
	<xsl:for-each select="xh:p">
	  <xsl:copy>
	    <xsl:copy-of select="@*"/>
	    <xsl:choose>
	      <xsl:when test="string-length(.) > 0">
		<xsl:apply-templates/>
		<xsl:if test="string-length($marker) > 0">
		  <a href="javascript://" onmouseover="showNote(event,'{concat($prefix,'note',$marker)}', 0)">
		    <span class="marker"><xsl:value-of select="$marker"/></span>
		  </a>
		</xsl:if>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:text>&#xa0;</xsl:text>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:copy>
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates/>
	<xsl:if test="string-length($marker) > 0">
	  <a href="javascript://" onmouseover="showNote(event,'{concat('tnote',$marker)}', 0)">
	    <span class="marker"><xsl:value-of select="$marker"/></span>
	  </a>
	</xsl:if>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:p[@class='note']"/>

<xsl:template match="xh:*">
  <xsl:param name="xtr"/>
  <xsl:param name="hilite" select="''"/>
  <xsl:param name="config-xml-file"/>
  <xsl:variable name="c" select="concat(@class,' ',$hilite)"/>
  <xsl:element name="{local-name()}">
    <xsl:attribute name="class"><xsl:value-of select="$c"/></xsl:attribute>
    <xsl:copy-of select="@*[not(local-name()='class')]"/>
    <xsl:if test="self::xh:p and $transonly='true'">
      <span class="xtr-label">
	<xsl:call-template name="xtr-label">
	  <xsl:with-param name="config-xml" select="$config-xml-file"/>
	</xsl:call-template>
      </span>
    </xsl:if>
    <xsl:apply-templates>
      <xsl:with-param name="xtr" select="$xtr"/>
    </xsl:apply-templates>
  </xsl:element>
</xsl:template>

<xsl:template mode="notediv" match="xh:p">
  <xsl:apply-templates mode="notediv"/>
</xsl:template>

<xsl:template mode="notediv" match="xh:span[@class='w']">
  <xsl:apply-templates mode="notediv"/>
<!--  <xsl:if test="not(position()=last())"> -->
  <xsl:if test="following-sibling::*[self::xh:span[@class='w']]">
    <xsl:text> </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template mode="notediv" match="xh:*">
  <xsl:param name="xtr"/>
  <xsl:element name="{local-name()}">
    <xsl:copy-of select="@*[not(local-name(.)='id')]"/>
    <xsl:apply-templates mode="notediv">
      <xsl:with-param name="xtr" select="$xtr"/>
    </xsl:apply-templates>
  </xsl:element>
</xsl:template>

<xsl:template mode="notediv" match="text()">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template name="via">
  <span class="via">[via <xsl:value-of select="$project"/>]</span>
</xsl:template>

</xsl:stylesheet>
