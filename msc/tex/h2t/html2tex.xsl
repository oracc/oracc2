<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:h="http://www.w3.org/1999/xhtml"
	       xmlns:esp="http://oracc.org/ns/esp/1.0"
	       xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
	       xmlns:tex="http://oracc.org/ns/tex/1.0"
	       >

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
  
  <xsl:template match="h:body|h:div">
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

  <xsl:template match="h:dl">
    <xsl:text>\Hd</xsl:text>
    <xsl:text>l</xsl:text>
    <!--
	<xsl:call-template name="ells">
	<xsl:with-param name="nells" select="count(ancestor-or-self::h:dl)"/>
	</xsl:call-template>
    -->
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

  <xsl:template match="h:img">
    <xsl:value-of select="concat('[[img:src=',@src,']]')"/>
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
    <!--
	<xsl:call-template name="ells">
	<xsl:with-param name="nells" select="count(ancestor-or-self::h:ol)"/>
	</xsl:call-template>
    -->
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
    <xsl:text>&#xa;\par </xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="h:pre">
    <xsl:text>\begin{verbatim}</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#xa;@end{verbatim}&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="h:span">
    <xsl:text>\bgroup</xsl:text>
    <xsl:call-template name="class"/>
    <xsl:text> </xsl:text>
    <xsl:apply-templates/>
    <xsl:text>\egroup </xsl:text>
  </xsl:template>

  <xsl:template match="h:sub">
    <xsl:text>\Hsub{</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
  </xsl:template>
  
  <xsl:template match="h:table">
    <xsl:text>\bigskip&#xa;</xsl:text>
    <xsl:choose>
      <xsl:when test="@class='pretty'">
	\centerline{\vbox{\offinterlineskip\tabskip0pt
	<xsl:apply-templates mode="halign" select=".">
	  <xsl:with-param name="ruled" select="'yes'"/>
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
    <!--
	<xsl:call-template name="ells">
	<xsl:with-param name="nells" select="count(ancestor-or-self::h:ul)"/>
	</xsl:call-template>
      -->
    <xsl:apply-templates/>
    <xsl:text>\Hendul&#xa;</xsl:text>
  </xsl:template>

  <!-- tables using \halign -->

  <xsl:template mode="halign" match="h:table">
    <xsl:param name="ruled" select="'no'"/>
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
    <xsl:text>\halign{</xsl:text>
    <xsl:choose>
      <xsl:when test="$ruled='yes'">
	<xsl:call-template name="halign-preamble">
	  <xsl:with-param name="preamble-row" select="$p-row"/>
	  <xsl:with-param name="ruled" select="$ruled"/>
	</xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="halign-preamble"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates mode="halign">
      <xsl:with-param name="ruled" select="$ruled"/>
    </xsl:apply-templates>
    <xsl:text>}&#xa;</xsl:text>
  </xsl:template>

  <xsl:template mode="halign" match="h:thead">
    <xsl:param name="ruled"/>
    <xsl:apply-templates mode="halign">
      <xsl:with-param name="ruled" select="$ruled"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template mode="halign" match="h:tbody">
    <xsl:param name="ruled"/>
    <xsl:apply-templates mode="halign">
      <xsl:with-param name="ruled" select="$ruled"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template mode="halign" match="h:tr">
    <xsl:param name="ruled"/>
    <xsl:for-each select="*">
      <xsl:apply-templates mode="halign" select="."/>
      <xsl:choose>
	<xsl:when test="position()=last()">
	  <xsl:text>&amp;\cr</xsl:text>
	  <xsl:if test="$ruled='yes'">
	    <xsl:choose>
	      <xsl:when test="ancestor::h:thead">
		<xsl:text>\theadrule</xsl:text>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:text>\tablerule</xsl:text>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:if>
	  <xsl:text>&#xa;</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>&amp;</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:template>  
  
  <xsl:template mode="halign" match="h:th">
    <xsl:call-template name="class"/>
    <xsl:text>\thead{}</xsl:text>
    <xsl:apply-templates/>
  </xsl:template>
  
  <xsl:template mode="halign" match="h:td">
    <xsl:call-template name="class"/>
    <xsl:apply-templates/>
  </xsl:template>
  
  <xsl:template name="halign-preamble">
    <xsl:param name="preamble-row" select="1"/>
    <xsl:param name="ruled"/>
    <xsl:for-each select=".//h:tr[position()=$preamble-row]">
      <xsl:if test="position()=1">
	<xsl:for-each select="*">
	  <xsl:text>\Hstrut</xsl:text>
	  <xsl:if test="$ruled='yes'">
	    <xsl:text>\vrule\hskip2pt</xsl:text>
	  </xsl:if>
	  <xsl:text>#</xsl:text>
	  <xsl:choose>
	    <xsl:when test="position()=last()">
	      <xsl:text>&amp;</xsl:text>
	      <xsl:if test="$ruled='yes'">
		<xsl:text>\vrule</xsl:text>
	      </xsl:if>
	      <xsl:text>#\tabskip0pt\cr</xsl:text>
	      <xsl:if test="$ruled='yes'">
		<xsl:text>\tablerule</xsl:text>
	      </xsl:if>
	      <xsl:text>&#xa;</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text>\hfil\quad\tabskip3pt plus1pt&amp;</xsl:text>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:for-each>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="text()">
    <xsl:value-of select="translate(.,'&amp;~%', '&#xfe60;&#x223c;&#x2052;')"/>
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

  <xsl:template match="*">
    <xsl:message>Unnamespaced tag <xsl:value-of select="local-name(.)"/></xsl:message>
  </xsl:template>

<!--  <xsl:template match="text()"/> -->
  
  <!-- HTML FUNCTIONS -->

  <xsl:template name="class">
    <xsl:if test="string-length(@class)>0">
      <xsl:value-of select="concat('\', @class)"/>
    </xsl:if>
    <xsl:text>{}</xsl:text>
  </xsl:template>

  <xsl:template name="ells">
    <xsl:param name="nells"/>
    <xsl:choose>
      <xsl:when test="$nells=1">
	<xsl:text>l</xsl:text>
      </xsl:when>
      <xsl:when test="$nells=2">
	<xsl:text>ll</xsl:text>
      </xsl:when>
      <xsl:when test="$nells=3">
	<xsl:text>lll</xsl:text>
      </xsl:when>
      <xsl:when test="$nells=4">
	<xsl:text>llll</xsl:text>
      </xsl:when>
      <xsl:when test="$nells=5">
	<xsl:text>lllll</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:message
	    >ells called asking for too many ells: nells=<xsl:value-of select="$nells"
	    /></xsl:message>
      </xsl:otherwise>
    </xsl:choose>
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
