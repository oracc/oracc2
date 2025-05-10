<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:h="http://www.w3.org/1999/xhtml"
	       xmlns:esp="http://oracc.org/ns/esp/1.0"
	       xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
	       xmlns:tex="http://oracc.org/ns/tex/1.0"
	       >
  
  <xsl:param name="class-arg" select="',sqopq,sqseq,sqinv,sqchr,not,'"/>
  
  <xsl:output method="text" encoding="UTF-8"/>

  <xsl:strip-space elements="*"/>
  
  <xsl:template match="h:html">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="h:a[@href]">
    <xsl:value-of select="concat('\Ha{',@href,'}{')"/>
    <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
  </xsl:template>
  
  <xsl:template match="h:body">
    <xsl:choose>
      <xsl:when test="@class">
	<xsl:text>\bgroup</xsl:text>
	<xsl:call-template name="class"/>
	<xsl:apply-templates/>
	<xsl:text>\egroup&#xa;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="h:blockquote">
    <xsl:text>\begin{quote}</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#xa;\end{quote}&#xa;</xsl:text>
  </xsl:template>
  
  <xsl:template match="h:code|h:em">
    <xsl:value-of select="concat('\',local-name(.),'{')"/>
    <xsl:call-template name="class"/>
    <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="h:div">
    <xsl:choose>
      <xsl:when test="contains(@class,'names')">
	<xsl:call-template name="sl-names"/>
      </xsl:when>
      <xsl:when test="contains(@class,'cdligh') and count(*)=0">
	<xsl:text>\slnocdli</xsl:text>
      </xsl:when>
      <xsl:when test="contains(@class,'vbox')">
	<xsl:choose>
	  <xsl:when test="ancestor::*[contains(@class,'codechart')]">
	    <xsl:text>\vbox to\ccht{\vfil</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>\vbox{\vfil</xsl:text>
	  </xsl:otherwise>
	</xsl:choose>
	<xsl:call-template name="class">
	  <xsl:with-param name="nono" select="'vbox'"/>
	</xsl:call-template>
	<xsl:apply-templates mode="hbox"/>
	<xsl:text>\vfil}%&#xa;</xsl:text>
      </xsl:when>
      <xsl:when test="@class">
	<xsl:text>\bgroup</xsl:text>
	<xsl:call-template name="class"/>
	<xsl:apply-templates/>
	<xsl:text>\egroup&#xa;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="h:div" mode="hbox">
    <xsl:if test="string-length(.)>0">
      <xsl:choose>
	<xsl:when test="h:span[@class='cc-chr']">
	  <xsl:text>\vbox to\cccht{\vfil</xsl:text>
	  <xsl:text>\hbox to\cccwd{\hfil</xsl:text>
	  <xsl:call-template name="class"/>
	  <xsl:apply-templates/>
	  <xsl:text>\hfil}%&#xa;</xsl:text>
	  <xsl:text>\vfil}%&#xa;</xsl:text>
	</xsl:when>
	<xsl:when test="h:span[@class='cc-uni']">
	  <xsl:text>\hbox to\cccwd{\hfil</xsl:text>
	  <xsl:call-template name="class"/>
	  <xsl:apply-templates/>
	  <xsl:text>\hfil}%&#xa;</xsl:text>
	</xsl:when>
	<xsl:when test="@class='sl-c-c' or @class='fchr'">
	  <xsl:if test="preceding-sibling::*[@class='fchr']">
	    <xsl:text>\kern3pt</xsl:text>
	  </xsl:if>
	  <xsl:text>\hbox to\slcwd{\hss</xsl:text>
	  <xsl:call-template name="class"/>
	  <xsl:apply-templates/>
	  <xsl:text>\hss}%&#xa;</xsl:text>
	</xsl:when>
	<xsl:when test="@class='sl-c-u' or @class='fhex'">
	  <xsl:text>\kern-1pt</xsl:text>
	  <xsl:text>\hbox to\slcwd{\hfil</xsl:text>
	  <xsl:text>\slcufont </xsl:text>
	  <xsl:apply-templates mode="nosqb"/>
	  <xsl:text>\hfil}%&#xa;</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>\hbox{</xsl:text>
	  <xsl:call-template name="class"/>
	  <xsl:apply-templates/>
	  <xsl:text>}%&#xa;</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:template>

  <xsl:template match="h:dl">
    <xsl:text>\Hd</xsl:text>
    <xsl:text>l</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>\Henddl&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="h:dd">
    <xsl:call-template name="class"/>
    <xsl:apply-templates/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="h:dt">
    <xsl:text>\Hdt</xsl:text>
    <xsl:call-template name="class"/>
    <xsl:apply-templates/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template mode="hbox" match="h:img">
    <xsl:text>\hbox to\cdliwd{\hss</xsl:text>
    <xsl:value-of select="concat('\includegraphics{propgh/',@data-row,'.png}{',../@data-sf,'}')"/>
    <xsl:text>\hss}</xsl:text>
  </xsl:template>

  <xsl:template match="h:li">
    <xsl:text>\Hli</xsl:text>
    <xsl:call-template name="class"/>
    <xsl:apply-templates/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="h:ol">
    <xsl:text>\Ho</xsl:text>
    <xsl:text>l</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>\Hendol&#xa;</xsl:text>
  </xsl:template>
  
  <xsl:template match="h:h1">
    <xsl:text>\Hh</xsl:text>
    <xsl:call-template name="class"/>
    <xsl:apply-templates/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="h:h2">
    <xsl:text>\Hhh</xsl:text>
    <xsl:call-template name="class"/>
    <xsl:apply-templates/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="h:h3">
    <xsl:text>\Hhhh</xsl:text>
    <xsl:call-template name="class"/>
    <xsl:apply-templates/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="h:h4">
    <xsl:text>\Hhhhh</xsl:text>
    <xsl:call-template name="class"/>
    <xsl:apply-templates/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="h:h5">
    <xsl:text>\Hhhhhh</xsl:text>
    <xsl:call-template name="class"/>
    <xsl:apply-templates/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="h:p">
    <xsl:choose>
      <xsl:when test="ancestor::h:table">
	<xsl:apply-templates/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>&#xa;\par </xsl:text>
	<xsl:apply-templates/>
	<xsl:text>&#xa;&#xa;</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="h:pre">
    <xsl:text>\begin{verbatim}</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#xa;@end{verbatim}&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="h:span">
    <xsl:text>\bgroup</xsl:text>
    <xsl:call-template name="class"/>
    <xsl:apply-templates/>
    <xsl:call-template name="class-close"/>
    <xsl:text>\egroup{}</xsl:text>
  </xsl:template>

  <xsl:template match="h:sub">
    <xsl:text>\Hsub{</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="h:table[contains(@class,'codechart-list')]">
    <xsl:text>\beginchartlist&#xa;</xsl:text>
    <xsl:for-each select="h:tbody/h:tr">
      <xsl:text>\cclrow{</xsl:text>
      <xsl:value-of select="h:td[1]"/>
      <xsl:text>}{</xsl:text>
      <xsl:value-of select="h:td[2]"/>
      <xsl:text>}{</xsl:text>
      <xsl:value-of select="h:td[3]"/>
      <xsl:text>}&#xa;</xsl:text>
    </xsl:for-each>
    <xsl:text>\endchartlist&#xa;</xsl:text>
  </xsl:template>

  <!-- The column 1 that we are dropping is th not td so args are td[1] .. td[8] -->
  <xsl:template match="h:table[contains(@class,'sltab')]">
    <xsl:message>sltab</xsl:message>
    <xsl:text>\beginpcsltab&#xa;</xsl:text>
    <xsl:for-each select="h:tbody/h:tr">
      <xsl:if test="../@data-aka">
	<xsl:text>\akatoks{</xsl:text>
	<xsl:call-template name="textmap">
	  <xsl:with-param name="t" select="../@data-aka"/>
	</xsl:call-template>
	<xsl:text>}</xsl:text>
      </xsl:if>
      <xsl:if test="../@data-cdiff">
	<xsl:text>\cdifftoks{</xsl:text>
	<xsl:call-template name="textmap">
	  <xsl:with-param name="t" select="../@data-cdiff"/>
	</xsl:call-template>
	<xsl:text>}</xsl:text>
      </xsl:if>
      <xsl:text>\pcslrow{</xsl:text>
      <xsl:apply-templates select="h:td[1]" mode="sltab"/>
      <xsl:text>}{</xsl:text>
      <xsl:apply-templates select="h:td[2]" mode="sltab"/>
      <xsl:text>}{</xsl:text>
      <xsl:apply-templates select="h:td[3]" mode="sltab"/>
      <xsl:text>}{</xsl:text>
      <xsl:apply-templates select="h:td[4]" mode="slglyf"/>
      <xsl:text>}{</xsl:text>
      <xsl:apply-templates select="h:td[5]" mode="slglyf"/>
      <xsl:text>}{</xsl:text>
      <xsl:apply-templates select="h:td[6]" mode="slglyf"/>
      <xsl:text>}{</xsl:text>
      <xsl:apply-templates select="h:td[7]" mode="slglyf"/>
      <xsl:text>}{</xsl:text>
      <xsl:apply-templates select="h:td[8]" mode="slglyf"/>
      <xsl:text>}{.75}&#xa;</xsl:text>
    </xsl:for-each>
    <xsl:text>\endpcsltab&#xa;</xsl:text>
  </xsl:template>

  <xsl:template mode="slglyf" match="h:td">
    <xsl:choose>
      <xsl:when test="count(*)>0">
	<xsl:apply-templates mode="sltab" select="."/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>\slnoglyf</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template mode="sltab" match="h:td">
    <xsl:call-template name="class"/>
    <xsl:choose>
      <xsl:when test="count(h:div[contains(@class,'vbox')])>1">
	<xsl:text>\vbox{</xsl:text>
	<xsl:apply-templates/>
	<xsl:text>}</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:call-template name="class-close"/>
  </xsl:template>
  
  <xsl:template match="h:table">
    <xsl:text>\bigskip&#xa;</xsl:text>
    <xsl:choose>
      <xsl:when test="contains(@class,'pretty') or contains(@class,'borders') ">
	<xsl:text>\centerline{\vbox{\offinterlineskip\tabskip0pt&#xa;</xsl:text>
	<xsl:apply-templates mode="halign" select=".">
	  <xsl:with-param name="ruled" select="'yes'"/>
	</xsl:apply-templates>
	<xsl:text>}}</xsl:text>
      </xsl:when>
      <xsl:when test="contains(@class,'codechart')">
	<xsl:text>\centerline{\codechart\vbox{\offinterlineskip\tabskip0pt&#xa;</xsl:text>
	<xsl:apply-templates mode="halign" select=".">
	  <xsl:with-param name="ruled" select="'yes'"/>
	  <xsl:with-param name="rulerules" select="'cc'"/>
	</xsl:apply-templates>
	<xsl:text>}}</xsl:text>	
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates mode="halign" select="."/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="h:ul">
    <xsl:text>\Hul</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>\Hendul&#xa;</xsl:text>
  </xsl:template>

  <!-- tables using \halign -->

  <xsl:template mode="halign" match="h:table">
    <xsl:param name="ruled" select="'no'"/>
    <xsl:param name="rulerules" select="'none'"/>
    <xsl:param name="csname"/>
    <xsl:variable name="p-row">
      <xsl:choose>
	<xsl:when test="@tex:preamble-row">
	  <xsl:value-of select="@tex:preamble-row"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>1</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <!--<xsl:message>tex:preamble-row $p-row=<xsl:value-of select="$p-row"/></xsl:message>-->
    <xsl:text>\halign{%&#xa;</xsl:text>
    <xsl:choose>
      <xsl:when test="$ruled='yes'">
	<xsl:call-template name="halign-preamble">
	  <xsl:with-param name="preamble-row" select="$p-row"/>
	  <xsl:with-param name="ruled" select="$ruled"/>
	  <xsl:with-param name="rulerules" select="$rulerules"/>
	</xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="halign-preamble">
	  <xsl:with-param name="rulerules" select="$rulerules"/>
	</xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates mode="halign">
      <xsl:with-param name="ruled" select="$ruled"/>
      <xsl:with-param name="rulerules" select="$rulerules"/>
    </xsl:apply-templates>
    <xsl:text>}&#xa;</xsl:text>
  </xsl:template>

  <xsl:template mode="halign" match="h:thead">
    <xsl:param name="ruled"/>
    <xsl:param name="rulerules" select="'none'"/>
    <xsl:apply-templates mode="halign">
      <xsl:with-param name="ruled" select="$ruled"/>
      <xsl:with-param name="rulerules" select="$rulerules"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template mode="halign" match="h:tbody">
    <xsl:param name="ruled"/>
    <xsl:param name="rulerules" select="'none'"/>
    <xsl:apply-templates mode="halign">
      <xsl:with-param name="ruled" select="$ruled"/>
      <xsl:with-param name="rulerules" select="$rulerules"/>
    </xsl:apply-templates>
    <xsl:choose>
      <xsl:when test="$rulerules='sltab'">
	<xsl:text>\sltabrule</xsl:text>
      </xsl:when>
      <xsl:when test="contains(ancestor::h:table/@class,'tbodyrules')">
	<xsl:text>\tbodyrule</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>\tbodybreak</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template mode="halign" match="h:tr">
    <xsl:param name="ruled"/>
    <xsl:param name="rulerules" select="'none'"/>
    <xsl:for-each select="*">
      <xsl:choose>
	<xsl:when test="$rulerules='cc' and position()=1"/><!--omit the left numbers in cc-->
	<xsl:when test="$rulerules='sltab' and position()=1"/><!--omit the left numbers in cc-->
	<xsl:otherwise>
	  <xsl:if test="self::h:td and $rulerules='cc' and position()=2">
	    <xsl:text>\cclnum{</xsl:text>
	    <xsl:value-of select="preceding-sibling::*[1]/text()"/>
	    <xsl:text>}</xsl:text>
	  </xsl:if>
	  <xsl:apply-templates mode="halign" select=".">
	    <xsl:with-param name="rulerules" select="$rulerules"/>
	  </xsl:apply-templates>
	  <xsl:choose>
	    <xsl:when test="position()=last()">
	      <xsl:if test="$ruled='yes'">
		<xsl:text>&amp;</xsl:text>
	      </xsl:if>
	      <xsl:if test="$rulerules='cc' and ancestor::h:thead">
		<xsl:text>\omit</xsl:text>
	      </xsl:if>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text>&amp;</xsl:text>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
    <xsl:text>\cr</xsl:text>
    <xsl:if test="$ruled='yes'">
      <xsl:choose>
	<xsl:when test="ancestor::h:thead">
	  <xsl:text>\theadrule</xsl:text>
	</xsl:when>
	<xsl:when test="$rulerules='cc' and position()=last()">
	  <xsl:text>\theadrule</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>\tablerule</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:if>
    <xsl:text>&#xa;</xsl:text>
  </xsl:template>  
  
  <xsl:template mode="halign" match="h:th">
    <xsl:param name="rulerules" select="'none'"/>
    <xsl:choose>
      <xsl:when test="ancestor::h:tbody">
	<xsl:call-template name="class">
	  <xsl:with-param name="class" select="concat(../@class,' ',@class)"/>
	</xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
	<xsl:if test="$rulerules='cc'">
	  <xsl:text>\omit</xsl:text>
	  <xsl:if test="ancestor::h:thead and count(preceding-sibling::h:th)=1">
	    <xsl:text>\vrule width1pt</xsl:text>
	  </xsl:if>
	  <xsl:text>\trhook</xsl:text>
	</xsl:if>
	<xsl:call-template name="class"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="$rulerules='cc'">
	<xsl:text>\cctnum{</xsl:text>
	<xsl:apply-templates/>
	<xsl:text>}</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>\thead{}</xsl:text>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="ancestor::h:tbody">
	<xsl:call-template name="class-close">
	  <xsl:with-param name="class" select="concat(../@class,' ',@class)"/>
	</xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="class-close"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template mode="halign" match="h:td">
    <xsl:param name="rulerules" select="'none'"/>
    <xsl:call-template name="class"/>
    <xsl:choose>
      <xsl:when test="count(h:div[contains(@class,'vbox')])>1">
	<xsl:text>\vbox{</xsl:text>
	<xsl:apply-templates/>
	<xsl:text>}</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:call-template name="class-close"/>
  </xsl:template>
    
  <xsl:template name="halign-preamble">
    <xsl:param name="preamble-row" select="1"/>
    <xsl:param name="ruled"/>
    <xsl:param name="rulerules"/>
    <xsl:message>halign-preamble rulerules=<xsl:value-of select="$rulerules"/></xsl:message>
    <xsl:for-each select=".//h:tr[position()=$preamble-row]">
      <xsl:if test="position()=1">
	<xsl:for-each select="*">
	  <xsl:choose>
	    <xsl:when test="$rulerules='cc' and position()=1"/><!--omit the left numbers in cc-->
	    <xsl:when test="$rulerules='sltab' and position()=1"/><!--omit the left numbers in cc-->
	    <xsl:otherwise>
	      <xsl:text>\Hstrut</xsl:text>
	      <xsl:if test="$ruled='yes'">
		<xsl:choose>
		  <xsl:when test="$rulerules='cc'">
		    <xsl:choose>
		      <xsl:when test="position()=2"><!--position()=1 is still left number-->
			<xsl:text>\tvrulex\hskip0pt</xsl:text>
		      </xsl:when>
		      <xsl:otherwise>
			<xsl:text>\tvrule\hskip0pt</xsl:text>
		      </xsl:otherwise>
		    </xsl:choose>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:text>\tvrule\hskip2pt</xsl:text>
		  </xsl:otherwise>
		</xsl:choose>
	      </xsl:if>
	      <xsl:text>$\vcenter{\hbox{#}}$</xsl:text>
	      <xsl:choose>
		<xsl:when test="position()=last()">
		  <xsl:text>%&#xa;&amp;</xsl:text>
		  <xsl:if test="$ruled='yes'">
		    <xsl:text>\tvrule</xsl:text>
		  </xsl:if>
		  <xsl:text>#\tabskip0pt\cr</xsl:text>
		  <xsl:if test="$ruled='yes' and not($rulerules='cc')">
		    <xsl:text>\tablerule</xsl:text>
		  </xsl:if>
		  <xsl:text>&#xa;</xsl:text>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:text>\hfil</xsl:text>
		  <xsl:if test="not($rulerules='cc') and not($rulerules='sltab')">
		    <xsl:text>\quad</xsl:text>
		  </xsl:if>
		  <xsl:choose>
		    <xsl:when test="$rulerules='cc' or $rulerules='sltab'">
		      <xsl:text>\tabskip0pt%&#xa;&amp;</xsl:text>
		    </xsl:when>
		    <xsl:otherwise>
		      <xsl:text>\tabskip3pt plus1pt%&#xa;&amp;</xsl:text>
		    </xsl:otherwise>
		  </xsl:choose>
		</xsl:otherwise>
	      </xsl:choose>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:for-each>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="h:br">
    <xsl:text>\newline{}</xsl:text>
  </xsl:template>

  <xsl:template mode="nosqb" match="text()">
    <xsl:value-of select="translate(.,'[]','')"/>
  </xsl:template>

  <!-- this deletes '_' -->
  <xsl:template name="textmap">
    <xsl:param name="t"/>
    <xsl:value-of select="translate($t,'&amp;~%#_', '&#xfe60;&#x223c;&#x2052;&#xfe5f;')"/>
  </xsl:template>

  <xsl:template match="text()">
    <xsl:call-template name="textmap">
      <xsl:with-param name="t" select="."/>
    </xsl:call-template>
  </xsl:template>

  <!-- Ignored HTML tags -->
  <xsl:template match="h:head
		       |h:a[@name]
		       |h:div[@id='Menu' or @id='Breadcrumb' or @id='BackToTop'
		       or starts-with(@id,'Footer') or @id='URL' or @id='EndContentSpace'
		       or @id='StyleSheetWarning' or @id='Header']
		       |h:span[@class='externallinktext']"/>
  
  <xsl:template match="h:*">
    <xsl:message>Unhandled HTML tag <xsl:value-of select="local-name(.)"/></xsl:message>
  </xsl:template>

  <xsl:template match="h2t-wrapper">
    <xsl:apply-templates/>
  </xsl:template>
  
  <xsl:template match="*">
    <xsl:message>Unnamespaced tag <xsl:value-of select="local-name(.)"/></xsl:message>
  </xsl:template>
  
<!--  <xsl:template match="text()"/> -->
  
  <!-- HTML FUNCTIONS -->

  <xsl:template name="class">
    <xsl:param name="class" select="@class"/>
    <xsl:param name="nono" select="''"/>
    <xsl:choose>
      <xsl:when test="not(contains($class, ' '))">
	<xsl:choose>
	  <xsl:when test="string-length($class)>0">
	    <xsl:variable name="nhclass" select="translate($class,'-', '')"/>
	    <xsl:choose>
	      <xsl:when test="contains($class-arg, concat(',',$nhclass,','))">
		<xsl:value-of select="concat('\', $nhclass,'{')"/>
	      </xsl:when>
	      <xsl:when test="not(contains($nono,$class))">
		<xsl:value-of select="concat('\', $nhclass)"/>
		<xsl:text>{}</xsl:text>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:text>{}</xsl:text>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>{}</xsl:text>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="class">
	  <xsl:with-param name="class" select="substring-before($class,' ')"/>
	  <xsl:with-param name="nono" select="$nono"/>
	</xsl:call-template>
	<xsl:call-template name="class">
	  <xsl:with-param name="class" select="substring-after($class,' ')"/>
	  <xsl:with-param name="nono" select="$nono"/>
	</xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <xsl:template name="class-close">
    <xsl:param name="class" select="@class"/>
    <xsl:choose>
      <xsl:when test="not(contains($class, ' '))">
	<xsl:if test="string-length($class)>0">
	  <xsl:variable name="nhclass" select="translate($class,'-', '')"/>
	  <xsl:if test="contains($class-arg, concat(',',$nhclass,','))">
	    <xsl:text>}</xsl:text>
	  </xsl:if>
	</xsl:if>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="class-close">
	  <xsl:with-param name="class" select="substring-before($class,' ')"/>
	</xsl:call-template>
	<xsl:call-template name="class-close">
	  <xsl:with-param name="class" select="substring-after($class,' ')"/>
	</xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="sl-names">
    <xsl:text>\slnames{</xsl:text>
    <xsl:apply-templates select="*[@class='sname']"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="*[@class='uname']"/>
    <xsl:text>}{</xsl:text>
    <xsl:apply-templates select="*[@class='stags']/@data-hrt"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="*[@class='uname']/@data-pc25g"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="*[@class='uname']/@data-pc25h"/>
    <!--<xsl:apply-templates mode="nosqb" select="*[@class='rhex']"/>-->
    <xsl:text>}{</xsl:text>
    <xsl:apply-templates select="*[@class='zatu']"/>
    <xsl:text>}{</xsl:text>
    <!--<xsl:value-of select="*[@class='dist']"/>-->
    <xsl:call-template name="sl-dist"/>
    <xsl:text>}{</xsl:text>
    <xsl:value-of select="*[@class='notes']"/>
    <xsl:text>}&#xa;</xsl:text>
  </xsl:template>

  <xsl:template name="sl-dist">
    <xsl:choose>
      <xsl:when test="count(*[@class='dist'])>0">
	<xsl:for-each select="*[@class='dist']">
	  <xsl:text>\sldistt{</xsl:text>
	  <xsl:value-of select="@data-distt"/>
	  <xsl:text>}\sldistiv{</xsl:text>
	  <xsl:value-of select="@data-dist4"/>
	  <xsl:text>}\sldistiii{</xsl:text>
	  <xsl:value-of select="@data-dist3"/>
	  <xsl:text>}\sldist</xsl:text>
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>\slnodist</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template mode="hbox" match="*">
    <xsl:message>Tag <xsl:value-of select="local-name(.)"/> not handled in mode=hbox</xsl:message>
  </xsl:template>
  
  <!-- ESP -->
  
  <xsl:template match="esp:titlex">
    <xsl:text>\chapter{</xsl:text>
    <xsl:value-of select="."/>
    <xsl:text>}&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="esp:title|esp:name|esp:url"/>

  <xsl:template match="esp:*">
    <xsl:message>Unhandled ESP tag <xsl:value-of select="local-name(.)"/></xsl:message>
  </xsl:template>

  <!-- ESP-STRUCT -->
  
  <xsl:template match="struct:page">
    <xsl:apply-templates/>
  </xsl:template>
  
  <xsl:template match="struct:*">
    <xsl:message>Unhandled ESP-STRUCT tag <xsl:value-of select="local-name(.)"/></xsl:message>
  </xsl:template>

</xsl:transform>
