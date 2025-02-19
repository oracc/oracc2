<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns="http://www.w3.org/1999/xhtml"
	       xmlns:esp="http://oracc.org/ns/esp/1.0">

  <xsl:template match="csltab">
    <csldata>
      <xsl:apply-templates select="data/*"/>
    </csldata>
  </xsl:template>

  <xsl:template match="o">
    <table k="{@k}" class="csltab">
      <thead>
	<tr>
	  <th/>
	  <th colspan="3">Uruk</th>
	  <th colspan="3">Uqair</th>
	  <th colspan="3">JN</th>
	  <th colspan="3">Umma</th>
	  <th colspan="3">Misc</th>
	  <!--<th>Total</th>-->
	</tr>
	<tr>
	  <th/>
	  <th>Pub</th><th>Unp</th><th>All</th>
	  <th>Pub</th><th>Unp</th><th>All</th>
	  <th>Pub</th><th>Unp</th><th>All</th>
	  <th>Pub</th><th>Unp</th><th>All</th>
	  <th>Pub</th><th>Unp</th><th>All</th>
	</tr>
      </thead>
      <tbody c="{@c}" t="{@t}">
	<tr>
	  <xsl:if test="c1[1][@c]">
	    <xsl:attribute name="c"><xsl:value-of select="c1[1]/@c"/></xsl:attribute>
	    <xsl:attribute name="t"><xsl:value-of select="c1[1]/@t"/></xsl:attribute>
	  </xsl:if>
	  <th>IV</th>
	  <xsl:for-each select="c1[1]">
	    <xsl:call-template name="data"/>
	  </xsl:for-each>
	</tr>
	<tr>
	  <xsl:if test="c1[2][@c]">
	    <xsl:attribute name="c"><xsl:value-of select="c1[2]/@c"/></xsl:attribute>
	    <xsl:attribute name="t"><xsl:value-of select="c1[2]/@t"/></xsl:attribute>
	  </xsl:if>
	  <th>III</th>
	  <xsl:for-each select="c1[2]">
	    <xsl:call-template name="data"/>
	  </xsl:for-each>
	</tr>
      </tbody>
    </table>
  </xsl:template>

  <xsl:template name="data">
    <xsl:for-each select="c2">
      <xsl:apply-templates select="c3"/>
      <xsl:apply-templates select="."/>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="c2|c3">
    <xsl:choose>
      <xsl:when test="@c">
	<td t="{@t}"><xsl:value-of select="@c"/></td>
      </xsl:when>
      <xsl:otherwise>
	<td/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <xsl:template match="text()"/>
  
</xsl:transform>
