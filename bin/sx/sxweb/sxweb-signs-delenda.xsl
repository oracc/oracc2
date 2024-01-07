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

<xsl:template name="icount">
  <xsl:if test="@icount">
    <xsl:variable name="s">
      <xsl:if test="not(@icount = '1')">
	<xsl:text>s</xsl:text>
      </xsl:if>
    </xsl:variable>
    <p><esp:link url="/{$project}/{@xml:id}.tis"><xsl:value-of select="@icount"/>
    instance<xsl:value-of select="$s"/>
    attested in project <xsl:value-of select="$project"/>.</esp:link></p>
  </xsl:if>
</xsl:template>

<xsl:template name="compute-iframe-height">
  <xsl:param name="c"/>
  <xsl:variable name="total" select="$c/c/r[1]/c[2]"/>
  <xsl:variable name="nsubh" select="count($c/c/r[position()>1]/c[2][not(text()='0')])"/>
  <xsl:variable name="ncite">
    <xsl:choose>
      <xsl:when test="$total > 30">
	<!-- we are in selective mode; up to 5 instances from each period are shown -->
	<xsl:variable name="V">
	  <xsl:choose>
	    <xsl:when test="$c/c/r[2]/c[2] > 4">
	      <xsl:text>5</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$c/c/r[2]/c[2]"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:variable name="IV">
	  <xsl:choose>
	    <xsl:when test="$c/c/r[3]/c[2] > 4">
	      <xsl:text>5</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$c/c/r[3]/c[2]"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:variable name="III">
	  <xsl:choose>
	    <xsl:when test="$c/c/r[4]/c[2] > 4">
	      <xsl:text>5</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$c/c/r[4]/c[2]"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:variable name="I">
	  <xsl:choose>
	    <xsl:when test="$c/c/r[5]/c[2] > 4">
	      <xsl:text>5</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$c/c/r[5]/c[2]"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:value-of select="$V+$IV+$III+$I"/>
      </xsl:when>
      <xsl:otherwise>
	<!-- we are in inclusive mode; all the instances are shown -->
	<xsl:value-of select="$c/c/r[2]/c[2]+$c/c/r[3]/c[2]+$c/c/r[4]/c[2]+$c/c/r[5]/c[2]"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="subtot" select="$nsubh + $ncite"/>
  <!--<xsl:message><xsl:value-of select="$c/@xml:id"/> subtot = <xsl:value-of select="$subtot"/></xsl:message>-->
  <xsl:choose>
    <xsl:when test="$total > 30">
      <xsl:value-of select="40*($subtot+1)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="40*$subtot"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="sign-or-form">
     <xsl:choose>
	<xsl:otherwise>
	  <xsl:if test="/*/@signlist = 'pcsl'">
	    <xsl:variable name="o" select="@xml:id"/>
	    <!--retrieve the count node without changing context -->
	    <xsl:variable name="dot" select="."/>
	    <xsl:for-each select="document('sl-corpus-counts.xml',/)">
	      <xsl:variable name="c" select="id($o)"/>
	      <xsl:for-each select="$dot">
		<xsl:choose>
		  <xsl:when test="count($c/*)=0 or $c/c/r[1]/c[2]='0'">
		    <xsl:choose>
		      <xsl:when test="sl:images/sl:i[@loc]">
			<table width="95%">
			  <tr>
			    <td width="5%" valign="top" class="ii-signs">
			      <xsl:call-template name="add-images"/>
			    </td>
			    <td width="93%" class="ii-insts">
			      <xsl:call-template name="add-unattested"/>
			    </td>
			  </tr>
			</table>
		      </xsl:when>
		      <xsl:otherwise>
			<xsl:call-template name="add-unattested"/>
		      </xsl:otherwise>
		    </xsl:choose>
		  </xsl:when>
		  <xsl:otherwise>
		    <div class="image-insts">
		      <xsl:if test="sl:uage='0'">
			<p>
			  <xsl:text>(</xsl:text>
			  <xsl:value-of select="@n"/>
			  <xsl:text> has uage=0</xsl:text>
			  <xsl:text>)</xsl:text>
			</p>
		      </xsl:if>
		      <table width="95%">
			<tr>
			  <td width="5%" valign="top" class="ii-signs">
			    <xsl:choose>
			      <xsl:when test="sl:images/sl:i[@loc]">
				<xsl:call-template name="add-images"/>
			      </xsl:when>
			      <xsl:otherwise>
				<xsl:text>&#xa0;</xsl:text>
			      </xsl:otherwise>
			    </xsl:choose>
			  </td>
			  <td width="93%" class="ii-insts">
			    <xsl:variable name="height">
			      <xsl:call-template name="compute-iframe-height">
				<xsl:with-param name="c" select="$c"/>
			      </xsl:call-template>
			    </xsl:variable>
			    <iframe width="100%" height="{$height}" src="/pctc/signlist/inst/{$o}.html"/>
			  </td>
			</tr>
		      </table>
		    </div>
		  </xsl:otherwise>
		</xsl:choose>
	      </xsl:for-each>
	    </xsl:for-each>
	  </xsl:if>
	</xsl:otherwise>
      </xsl:choose>

    <xsl:if test="not(/*/@signlist = 'pcsl')">
      <xsl:call-template name="lex-sign"/>
    </xsl:if>

    <xsl:apply-templates select="sl:note"/>
    <xsl:call-template name="unicode-info"/>
    <xsl:apply-templates mode="rest"/>    
    <xsl:if test="count(sl:glo)>0">
      <div id="glodata">
	<h2 class="{$project}-glo">Glossary Attestations</h2>
	<xsl:for-each select="sl:v">
	  <xsl:if test="sl:glo">
	    <h2 class="psl-e"><span class="psl-ahead"><xsl:value-of select="@n"/></span></h2>
	    <xsl:apply-templates select="sl:glo"/>
	  </xsl:if>
	</xsl:for-each>
      </div>
    </xsl:if>
</xsl:template>

<xsl:template name="add-unattested">
  <p class="sl-unattested">
    <xsl:text>(</xsl:text>
    <xsl:value-of select="@n"/>
    <xsl:choose>
      <xsl:when test="count(sl:form)=0">
	<xsl:text> has no attestations in corpus [uage=</xsl:text>
	<xsl:value-of select="sl:uage"/>
	<xsl:text>]</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text> has uage </xsl:text>
	<xsl:value-of select="sl:uage"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>)</xsl:text>
  </p>
</xsl:template>

<xsl:template match="sl:glo">
  <xsl:for-each select="*">
    <h3 class="{$project}-glo">
      <xsl:choose>
	<xsl:when test="@type='s'"><xsl:text>Independent</xsl:text></xsl:when>
	<xsl:when test="@type='i'"><xsl:text>Initial</xsl:text></xsl:when>
	<xsl:when test="@type='m'"><xsl:text>Medial</xsl:text></xsl:when>
	<xsl:when test="@type='f'"><xsl:text>Final</xsl:text></xsl:when>
	<xsl:otherwise><xsl:message>sl:glo with unknown @type <xsl:value-of select="@type"/></xsl:message></xsl:otherwise>
      </xsl:choose>
    </h3>
    <table class="psl">
      <xsl:apply-templates/>
    </table>
  </xsl:for-each>
</xsl:template>

<xsl:template match="sl:glo-inst">
  <tr><td class="psl-eleft"><xsl:apply-templates/></td
  ><td class="psl-eright"
  ><esp:link hide-print="yes" url="/{$project}/cbd/sux/{@ref}.html"><xsl:value-of select="@cfgw"/></esp:link
  ><xsl:if test="$with-stats='yes'"><xsl:value-of select="concat(' (',@icount,'× / ',@ipct,'%)')"/></xsl:if>
  </td
  ></tr>
</xsl:template>


<xsl:template match="sl:listdef|sl:sysdef|sl:images|sl:compoundonly|sl:iheader"/>

<xsl:template match="sl:note">
  <p class="{$project}-note"><xsl:apply-templates/></p>
</xsl:template>

<xsl:template match="sl:inote"/>

<xsl:template mode="rest" match="sl:lit">
  <p class="{$project}-lit"><xsl:apply-templates/></p>
</xsl:template>


</xsl:transform>
