<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" 
		xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
		xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0"
		xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
		xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"
		xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0"
		xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0"
		xmlns:xlink="http://www.w3.org/1999/xlink"
		xmlns:str="http://exslt.org/strings"
		xmlns:oracc="http://oracc.org/ns/oracc/1.0"
		extension-element-prefixes="str"
		>

<xsl:template name="cat-tabs">
  <xi:include xmlns:xi="http://www.w3.org/2001/XInclude" 
	      href="/home/oracc/rinap/00lib/tabset.xml"/>
</xsl:template>

<xsl:output method="text"/>

<xsl:variable name="letters" select="'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<xsl:template match="office:document-content">
  <xsl:apply-templates select="office:body"/>
</xsl:template>

<xsl:template match="office:body">
  <xsl:text>\macrofile{rinap}
</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>\bye</xsl:text>
</xsl:template>

<xsl:template match="office:text">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text:h">
  <xsl:if test="not(starts-with(.,'CATALOG'))">
    <xsl:choose>
      <xsl:when test="@text:style-name='Heading_20_1'">
	<xsl:text>\chapter </xsl:text>
      </xsl:when>
      <xsl:when test="@text:style-name='Heading_20_2'">
	<xsl:text>\heading </xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:message>Unhandled heading <xsl:value-of select="@text:style-name"/></xsl:message>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates/>
    <xsl:text>

</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="text:section">
  <xsl:choose>
    <!-- Note that the first text:p is the hidden one with the title, 
	 so count=2 means only one biblio entry -->
    <xsl:when test="@text:name='bibliography' and count(*) = 2">
      <xsl:text>\begin{bibliographySingleton}%</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>\end{bibliographySingleton}
</xsl:text>
    </xsl:when>
<!--
    <xsl:when test="@text:name='bibliography' and count(*) = 3">
      <xsl:text>\begin{bibliographyDoubleton}%</xsl:text>
      <xsl:apply-templates mode="bibDoubleton" select="text:p[2]"/>
      <xsl:text>\bibliographyDoubletonSwitch%</xsl:text>
      <xsl:apply-templates mode="bibDoubleton" select="text:p[3]"/>
      <xsl:text>\end{bibliographyDoubleton}
</xsl:text>
    </xsl:when>
 -->
    <xsl:otherwise>
      <xsl:text>\begin{</xsl:text>
      <xsl:value-of select="@text:name"/>
      <xsl:text>}%
</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>\end{</xsl:text>
    <xsl:value-of select="@text:name"/>
<xsl:text>}

</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="bibDoubleton" match="text:p">
  <xsl:text>\vtop\bgroup
</xsl:text>
   <xsl:apply-templates select="."/>
   <xsl:text>\egroup</xsl:text>
</xsl:template>

<xsl:template match="text:p">
  <xsl:choose>
    <xsl:when test="starts-with(@text:style-name,'hidden-')">
      <xsl:value-of 
	  select="concat('\',
	                 substring-after(@text:style-name,'hidden-'),
			 '{',text:hidden-text[1]/@text:string-value,'}%&#xa;'
			 )"/>
    </xsl:when>
    <xsl:when test="@text:style-name='introduction.p1'">
      <xsl:text>\introductionFirstPar </xsl:text>
      <xsl:apply-templates/>
      <xsl:text>

</xsl:text>
    </xsl:when>
    <xsl:when test="@text:style-name='introduction.p'">
      <xsl:text>\introductionPar </xsl:text>
      <xsl:apply-templates/>
      <xsl:text>

</xsl:text>
    </xsl:when>
    <xsl:when test="@text:style-name='introductionb.p1'">
      <xsl:text>\introductionbFirstPar </xsl:text>
      <xsl:apply-templates/>
      <xsl:text>

</xsl:text>
    </xsl:when>
    <xsl:when test="@text:style-name='introductionb.p'">
      <xsl:text>\introductionbPar </xsl:text>
      <xsl:apply-templates/>
      <xsl:text>

</xsl:text>
    </xsl:when>
    <xsl:when test="@text:style-name='discussion.p1'">
      <xsl:text>\discussionFirstPar </xsl:text>
      <xsl:apply-templates/>
      <xsl:text>

</xsl:text>
    </xsl:when>
    <xsl:when test="@text:style-name='discussion.p'">
      <xsl:text>\discussionPar </xsl:text>
      <xsl:apply-templates/>
      <xsl:text>

</xsl:text>
    </xsl:when>
    <xsl:when test="@text:style-name='bibliography.p1' 
		    or @text:style-name='bibliography.p'">
      <xsl:variable name="bibyear" select="text()[1]"/>
      <xsl:text>\bibPar</xsl:text>
      <xsl:choose>
	<xsl:when test="string-length($bibyear) > 7">
          <xsl:text>\bibyearB{</xsl:text>
	  <xsl:value-of select="substring-before($bibyear,'–')"/>
	  <xsl:text>–\newline\quad</xsl:text>
	  <xsl:value-of select="substring-after($bibyear,'–')"/>
	  <xsl:apply-templates select="(*|text())[position()>1]"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>\bibyear{</xsl:text>
	  <xsl:apply-templates/>
	</xsl:otherwise>
      </xsl:choose>
      <xsl:text>}\bibEnd
</xsl:text>
    </xsl:when>
    <xsl:when test="@text:style-name='CatHeader' 
		    or @text:style-name='CatBody'">
      <xsl:value-of select="concat('\',@text:style-name,' ')"/>
      <xsl:if test="(text()|*)[1][self::text:line-break]">
	<xsl:text>\Catstrut</xsl:text>
      </xsl:if>
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:when test="@text:style-name='Footnote'">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>text:p[@<xsl:value-of select="@text:style-name"/>] not handled</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="text:tab">
  <xsl:if test="ancestor::text:section[@text:name='bibliography']">
    <xsl:text>}%&amp;%
  \bibinfo{</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="table:table">
  <xsl:choose>
    <xsl:when test="ancestor::text:section[@text:name='catalog' or @text:name='cataloguncertain' or @text:name='cattable']">
      <xsl:call-template name="cat-table"/>
    </xsl:when>
    <xsl:when test="ancestor::text:section[@text:name='edition']">
      <xsl:call-template name="text-table"/>
    </xsl:when>
    <xsl:when test="ancestor::text:section[@text:name='score']">
      <xsl:call-template name="score-table"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>table:table[@<xsl:value-of 
        select="ancestor::text:section/@text:name"/>] not handled (X1)</xsl:message>
    </xsl:otherwise>
  </xsl:choose>      
</xsl:template>

<xsl:template name="cat-signature">
<!--
  <xsl:for-each select="table:table-rows/table:table-row[1]/table:table-cell">
    <xsl:value-of select="substring(.,1,1)"/>
  </xsl:for-each>
 -->
   <xsl:value-of select="table:table-rows/table:table-row[1]/@oracc:signature"/>
</xsl:template>

<xsl:template name="cat-table">
  <xsl:variable name="cat-sig">
    <xsl:call-template name="cat-signature"/>
  </xsl:variable>
  <xsl:message><xsl:value-of 
		select="ancestor::text:section/preceding-sibling::text:h[@text:outline-level='1'][1]/text()"
		/> uses signature <xsl:value-of select="$cat-sig"/></xsl:message>
  <xsl:variable name="tabs-node" 
		select="document('')/*/*[@name='cat-tabs']/*/*[@signature=$cat-sig]"/>
  <xsl:choose>
    <xsl:when test="count($tabs-node) > 0">
      <xsl:variable name="tabs-total" select="sum($tabs-node/*)"/>
      <!--  <xsl:message>tabs-total = <xsl:value-of select="format-number($tabs-total,'#.###')"/></xsl:message>-->
      <xsl:if test="$tabs-total>6.5">
	<xsl:message>Catalog table for text <xsl:value-of 
	select="ancestor::office:text//text:h[1]/text()"/> is too wide at <xsl:value-of
	select="format-number($tabs-total,'#.###')"/></xsl:message>
      </xsl:if>
      <xsl:text>\catbodyformat{</xsl:text>
      <xsl:value-of select="format-number($tabs-total, '#.###in')"/>
      <xsl:text>}
      \settabs\+
</xsl:text>
      <xsl:for-each select="$tabs-node/*">
	<xsl:value-of select="concat('\hbox to',format-number(text(),'#.##'),'in{\hfil}')"/>
	<xsl:choose>
	  <xsl:when test="not(position()=last())">
	    <xsl:text>&amp;%
</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>\cr
</xsl:text>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:for-each>
      <xsl:for-each select="table:table-rows/table:table-row[1]">
	<xsl:text>\+</xsl:text>
	<xsl:for-each select="table:table-cell">
	  <xsl:text>\vtop{</xsl:text>
	  <xsl:apply-templates/>
	  <xsl:text>}</xsl:text>
	  <xsl:if test="not(position()=last())">
	    <xsl:text>&amp;%
</xsl:text>
	  </xsl:if>
	</xsl:for-each>
	<xsl:text>\cr
</xsl:text>
      </xsl:for-each>
      <xsl:text>\catheaderskip
</xsl:text>
      <xsl:for-each select="table:table-rows/table:table-row[position()>1]">
	<xsl:text>\CatVbox\bgroup\+</xsl:text>
	<xsl:for-each select="table:table-cell">
	  <xsl:text>\vtop{</xsl:text>
	  <xsl:variable name="index" select="position()"/>
	  <xsl:value-of select="concat('\hsize',format-number($tabs-node/*[$index],'#.##in'))"/>
	  <xsl:apply-templates/>
	  <xsl:text>}</xsl:text>
	  <xsl:if test="not(position()=last())">
	    <xsl:text>&amp;%
</xsl:text>
	  </xsl:if>
	</xsl:for-each>
<xsl:text>\cr\egroup
</xsl:text>
      </xsl:for-each>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>tabset has no entry for signature <xsl:value-of select="$cat-sig"/></xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="score-table">
  <xsl:text>\FLtlitsanstlat
</xsl:text>
  <xsl:apply-templates mode="score-table"/>
</xsl:template>

<xsl:template mode="score-table" match="table:table-row-group">
  <xsl:call-template name="score-table-rows"/>
  <xsl:if test="not(position()=last())">
      <xsl:text>\FLblockskip
</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template mode="score-table" match="table:table-row">
  <xsl:call-template name="score-table-one-row"/>
</xsl:template>

<xsl:template name="score-table-rows">
  <xsl:for-each select="table:table-row">
    <xsl:call-template name="score-table-one-row"/>
  </xsl:for-each>
</xsl:template>

<xsl:template name="score-table-one-row">
  <xsl:choose>
    <xsl:when test="table:table-cell/text:p[@text:style-name='Horizontal_20_Line']">
      <xsl:text>\facingrulings
</xsl:text>
    </xsl:when>
    <xsl:when test="*[2][self::table:covered-table-cell]">
      <xsl:choose>
	<xsl:when test="*[3]/*">
	  <xsl:text>\facingblock{</xsl:text>
	  <xsl:text>}{\FLforceleft </xsl:text>
	  <xsl:apply-templates mode="text-table" select="*[1]"/>
	  <xsl:text>}{</xsl:text>
	  <xsl:apply-templates mode="text-table" select="*[3]"/>
	  <xsl:text>}{</xsl:text>	
	  <xsl:call-template name="dump-notes"/>
	  <xsl:text>}
</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>\facingmoretlit{}{\FLforceleft </xsl:text>
	  <xsl:if test="@table:style-name='nobreak'">
	    <xsl:text>\FLsuppressbreak </xsl:text>
	  </xsl:if>
	  <xsl:apply-templates mode="text-table" select="*[1]"/>
	  <xsl:text>}{</xsl:text>
	  <xsl:call-template name="dump-notes"/>
	  <xsl:text>}
</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="*[3][self::table:covered-table-cell]">
      <xsl:choose>
	<xsl:when test="*[2]/*">
	  <xsl:text>\facingmoretlit{</xsl:text>
	  <xsl:apply-templates mode="text-table" select="*[1]"/>
	  <xsl:text>}{</xsl:text>
	  <xsl:apply-templates mode="text-table" select="*[2]"/>
	  <xsl:text>}{</xsl:text>
	  <xsl:call-template name="dump-notes"/>
	  <xsl:text>}
</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>\facingmoretlit{}{}{</xsl:text>
	  <xsl:text>}{</xsl:text>
	  <xsl:call-template name="dump-notes"/>
	  <xsl:text>}
</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>\facingblock{</xsl:text>
      <xsl:apply-templates mode="text-table" select="*[1]"/>
      <xsl:text>}{</xsl:text>
      <xsl:apply-templates mode="text-table" select="*[2]"/>
      <xsl:text>}{</xsl:text>
      <xsl:apply-templates mode="text-table" select="*[3]"/>
      <xsl:text>}{</xsl:text>	
      <xsl:call-template name="dump-notes"/>
      <xsl:text>}
</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>\facingflush
</xsl:text>
</xsl:template>


<xsl:template name="text-table">
  <xsl:choose>
    <xsl:when test="*/*/@table:style-name='xtf_tlat_cell'">
      <xsl:text>\FLtlitwithtlat
</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>\FLtlitsanstlat
</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:for-each select="table:table-row">
    <xsl:if test="count(*) &lt; 3">
      <xsl:message><xsl:value-of select="preceding::text:h[1]"/>: column count &lt; 3</xsl:message>
    </xsl:if>
    <xsl:choose>
      <xsl:when test="table:table-cell/text:p[@text:style-name='Horizontal_20_Line']">
	<xsl:text>\facingrulings
</xsl:text>
      </xsl:when>
      <xsl:when test="*[2][self::table:covered-table-cell]">
	<xsl:choose>
	  <xsl:when test="*[3]/*">
	    <xsl:text>\facingblock{</xsl:text>
	    <xsl:text>}{\FLforceleft </xsl:text>
	    <xsl:apply-templates mode="text-table" select="*[1]"/>
	    <xsl:text>}{</xsl:text>
	    <xsl:apply-templates mode="text-table" select="*[3]"/>
	    <xsl:text>}{</xsl:text>	
	    <xsl:call-template name="dump-notes"/>
	    <xsl:text>}
</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>\facingmoretlit{}{\FLforceleft </xsl:text>
	    <xsl:if test="@table:style-name='nobreak'">
	      <xsl:text>\FLsuppressbreak </xsl:text>
	    </xsl:if>
	    <xsl:apply-templates mode="text-table" select="*[1]"/>
	    <xsl:text>}{</xsl:text>
	    <xsl:call-template name="dump-notes"/>
	    <xsl:text>}
</xsl:text>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:when test="*[3][self::table:covered-table-cell]">
	<xsl:choose>
	  <xsl:when test="*[2]/*">
	    <xsl:text>\facingmoretlit{</xsl:text>
	    <xsl:apply-templates mode="text-table" select="*[1]"/>
	    <xsl:text>}{</xsl:text>
	    <xsl:apply-templates mode="text-table" select="*[2]"/>
	    <xsl:text>}{</xsl:text>
	    <xsl:call-template name="dump-notes"/>
	    <xsl:text>}
</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>\facingmoretlit{}{}{</xsl:text>
	    <xsl:text>}{</xsl:text>
	    <xsl:call-template name="dump-notes"/>
	    <xsl:text>}
</xsl:text>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>\facingblock{</xsl:text>
	<xsl:apply-templates mode="text-table" select="*[1]"/>
	<xsl:text>}{</xsl:text>
	<xsl:apply-templates mode="text-table" select="*[2]"/>
	<xsl:text>}{</xsl:text>
	<xsl:apply-templates mode="text-table" select="*[3]"/>
	<xsl:text>}{</xsl:text>	
	<xsl:call-template name="dump-notes"/>
	<xsl:text>}
</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
  <xsl:text>\facingflush
</xsl:text>
</xsl:template>

<xsl:template mode="text-table" match="table:table-cell">
  <xsl:apply-templates mode="text-table"/>
</xsl:template>

<xsl:template mode="text-table" match="table:covered-table-cell"/>

<xsl:template mode="text-table" match="text:span">
  <xsl:choose>
    <xsl:when test="@text:style-name">
      <xsl:call-template name="X-style"/>
      <xsl:apply-templates/>
      <xsl:text>}</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>      
</xsl:template>

<xsl:template mode="text-table" match="text:p">
  <xsl:choose>
    <xsl:when test="@text:style-name='xtf_tlit_hdr'">
      <xsl:text>\FLheading </xsl:text>
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:when test="@text:style-name='xtf_tlat_hdr'">
      <xsl:text>\FLheading </xsl:text>
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="text:span">
  <xsl:choose>
    <xsl:when test="@text:style-name='w'">
      <xsl:text>\|</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>\|</xsl:text>
    </xsl:when>
    <xsl:when test="@text:style-name='xtr_label'">
      <xsl:text>{\tlatnumspacing </xsl:text>
      <xsl:apply-templates/>
      <xsl:text>}</xsl:text>
    </xsl:when>
    <xsl:when test="@text:style-name">
      <!-- Note that X-style emits its own { before or after the CS
	   but that we handle the } because it follows apply-templates -->
      <xsl:call-template name="X-style"/>
      <xsl:apply-templates/>
      <xsl:text>}</xsl:text>
    </xsl:when>
    <xsl:when test="ancestor::table:table-cell[@table:style-name='xtf_tlat_cell']">
      <!--<xsl:text>\|</xsl:text>-->
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="text:note"/>

<xsl:template name="dump-notes">
  <xsl:apply-templates select=".//text:note" mode="do-notes"/>
</xsl:template>

<xsl:template mode="do-notes" match="text:note">
  <xsl:text>\rinapnote{</xsl:text>
  <xsl:apply-templates select="text:note-body"/>
  <xsl:text>}</xsl:text>
</xsl:template>

<xsl:template match="text:note-body">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text:line-break">
  <xsl:text>\newline{}</xsl:text>
</xsl:template>

<xsl:template match="text:s">
  <xsl:text>{ }</xsl:text>
</xsl:template>

<xsl:template match="text:page-break">
  <xsl:choose>
    <xsl:when test="@oracc:odd='yes'">
      <xsl:text>
\newoddpage

</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>
\newpage

</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="draw:frame">
  <xsl:choose>
    <xsl:when test="@draw:style-name='here'">
      <xsl:message>draw-frame here</xsl:message>
    </xsl:when>
    <xsl:when test="@draw:style-name='mid'
		    or @draw:style-name='top'
		    or @draw:style-name='page'">
      <xsl:message>draw-frame insert</xsl:message>
      <xsl:value-of select="concat('\',@draw:style-name,'insert&#xa;')"/>
      <xsl:apply-templates mode="insert"/>
      <xsl:text>\endinsert&#xa;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>\vbox{</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>}</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*">
  <xsl:message><xsl:value-of select="name(.)"/> not handled</xsl:message>
</xsl:template>

<xsl:template name="X-style">
  <xsl:choose>
    <xsl:when test="@text:style-name = 'flags' 
		    or @text:style-name = 'sup' or @text:style-name = 'sub'
		    or @text:style-name = 'notelabel'">
      <xsl:value-of select="concat('\',@text:style-name,'{')"/>
    </xsl:when>
    <xsl:when test="@text:style-name">
      <xsl:text>{</xsl:text>
      <xsl:choose>
	<xsl:when test="string-length(translate(@text:style-name,$letters,''))=0">
	  <xsl:value-of select="concat('\',@text:style-name)"/>
	</xsl:when>
	<xsl:when test="@text:style-name='sac-sign'">
	  <xsl:text>\sac</xsl:text>
	</xsl:when>
	<xsl:when test="@text:style-name='sac-akk'">
	  <xsl:text>\sak</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="concat('\X{',@text:style-name,'}%&#xa;')"/>
	</xsl:otherwise>
      </xsl:choose>
      <xsl:call-template name="span-space"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>{ </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="span-space">
  <xsl:choose>
    <xsl:when test="substring(text(),1,1)=' '">
      <xsl:text>{}</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text> </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="insert" match="draw:text-box|draw:frame">
  <xsl:apply-templates mode="insert"/>
</xsl:template>

<xsl:template mode="insert" match="text:p">
  <xsl:choose>
    <xsl:when test="@text:style-name='Figure'">
      <xsl:variable name="caption-node" 
		    select=".//text:span[starts-with(@text:style-name,'caption')]"/>
      <xsl:text>\figure</xsl:text>
      <xsl:choose>
	<xsl:when test="contains($caption-node/@text:style-name, 'above')">
	  <xsl:text>0</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>1</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
      <xsl:text>{</xsl:text>
      <xsl:apply-templates mode="insert" select="$caption-node"/>
      <xsl:text>}{</xsl:text>
      <xsl:apply-templates mode="insert" select=".//draw:image"/>
      <xsl:text>}</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>Insertion has unknown type `<xsl:value-of select="@text:style-name"/>'</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="insert" match="draw:image">
<!--  <xsl:text>\includegraphics</xsl:text> -->
  <xsl:text>\XeTeXpicfile "</xsl:text>
  <xsl:value-of select="concat('pictures/',@xlink:href)"/>
  <xsl:text>"</xsl:text>
  <!-- at present image height/width are put on the parent draw:frame;
       I have to do more checking to see if this is correct (I picked
       it up from OpenOffice) -->
<!--
  <xsl:for-each select="..">
    <xsl:if test="@svg:height|@svg:width|draw:transform">
      <xsl:text>[</xsl:text>
      <xsl:choose>
	<xsl:when test="@svg:height">
	  <xsl:value-of select="concat('height=',@svg:height)"/>
	  <xsl:if test="@svg:width">
	    <xsl:value-of select="concat(',width=',@svg:width)"/>
	  </xsl:if>
	  <xsl:call-template name="draw-transform"/>
	</xsl:when>
	<xsl:when test="@svg:width">
	  <xsl:value-of select="concat('width=',@svg:width)"/>
	  <xsl:call-template name="draw-transform"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:call-template name="draw-transform"/>
	</xsl:otherwise>
      </xsl:choose>
      <xsl:text>]</xsl:text>
    </xsl:if>
  </xsl:for-each>
 -->
<!--
  <xsl:text>{</xsl:text>
  <xsl:value-of select="concat('pictures/',@xlink:href)"/>
  <xsl:text>}</xsl:text>
 -->
  <xsl:for-each select="..">
    <xsl:call-template name="draw-transform"/>
  </xsl:for-each>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template name="draw-transform">
  <xsl:if test="@draw:transform">
<!-- <xsl:text>,scale=</xsl:text> -->
    <xsl:text> scaled </xsl:text>
    <xsl:variable name="tstr">
      <xsl:choose>
	<xsl:when test="contains(@draw:transform,',')">
	  <xsl:value-of select="substring-before(@draw:transform,',')"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="substring-before(@draw:transform,')')"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:value-of select="substring-after($tstr,'(')"/>
  </xsl:if>
</xsl:template>

<xsl:template match="oracc:pageno">
  <xsl:value-of select="concat('\pageno',@value,'&#xa;')"/>
</xsl:template>

<xsl:template match="oracc:figno">
  <xsl:value-of select="concat('\figurecount',@value,'&#xa;')"/>
</xsl:template>

<xsl:template match="text()">
  <xsl:value-of select="translate(.,'&amp;','&#xefff;')"/>
</xsl:template>

</xsl:stylesheet>
