<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:ce="http://oracc.org/ns/ce/1.0"
		xmlns:xtr="http://oracc.org/ns/xtr/1.0"
		xmlns:syn="http://oracc.org/ns/syntax/1.0"
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:xh="http://www.w3.org/1999/xhtml"
		exclude-result-prefixes="ce syn xh xtr"
		>

<xsl:include href="html-standard.xsl"/>
<xsl:include href="g2-gdl-HTML.xsl"/>

<xsl:param name="fragment" select="'yes'"/>
<xsl:param name="item-offset"/>
<xsl:param name="project"/>

<xsl:output method="xml" encoding="utf-8" omit-xml-declaration="yes"/>

<xsl:variable name="lq"><xsl:text>'</xsl:text></xsl:variable>

<xsl:template match="ce:ce">
  <xsl:choose>
    <xsl:when test="$fragment = 'yes'">
      <xsl:call-template name="call-back"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="make-html">
	<xsl:with-param name="project" select="$project"/>
	<xsl:with-param name="webtype" select="'p2-p1'"/>
	<xsl:with-param name="p2" select="'yes'"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="call-back">
  <xsl:choose>
    <xsl:when test="@cetype='kwic'">
      <table class="kwic">
	<xsl:apply-templates mode="kwic"/>
      </table>
    </xsl:when>
    <xsl:when test="@cetype='xmd'">
      <table class="xmd">
	<xsl:apply-templates mode="xmd"/>
      </table>
    </xsl:when>
    <xsl:when test="@cetype='tra'">
      <div class="tra">
	<xsl:apply-templates mode="tra"/>
      </div>
    </xsl:when>
    <xsl:when test="@cetype='summ'">
      <div class="summ">
	<xsl:apply-templates mode="summ"/>
      </div>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="ce:l"/>

<xsl:template mode="xmd" match="ce:heading">
  <tr class="ce-heading">
    <td class="h2 border-top heading" colspan="{/*/@ncols}">
      <xsl:apply-templates/>
    </td>
  </tr>
</xsl:template>

<xsl:template mode="tra" match="ce:heading">
  <h2 class="ce-heading h2 border-top heading"><xsl:apply-templates/></h2>
</xsl:template>

<xsl:template mode="tra" match="ce:data">
  <xsl:apply-templates mode="tra"/>
</xsl:template>

<xsl:template mode="tra" match="xh:p">
  <p class="refline {@class}">
    <xsl:apply-templates mode="tra"/>
  </p>
  <p class="label">
    <xsl:call-template name="make-label-href">
      <xsl:with-param name="label" select="@xtr:se_label"/>
    </xsl:call-template>
  </p>
</xsl:template>

<xsl:template mode="tra" match="xh:innerp">
  <xsl:apply-templates mode="tra" select="*|text()"/>
</xsl:template>

<xsl:template mode="tra" match="*">
  <xsl:element name="{local-name(.)}">
    <xsl:copy-of select="@class"/>
    <xsl:apply-templates mode="tra"/>
  </xsl:element>
</xsl:template>

<xsl:template mode="tra" match="text()">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template mode="xmd" match="ce:data">
  <xsl:copy-of select="*"/>
</xsl:template>

<xsl:template mode="kwic" match="ce:heading">
  <tr class="ce-heading">
    <td class="h2 border-top heading" colspan="3">
      <xsl:apply-templates/>
    </td>
  </tr>
</xsl:template>

<xsl:template mode="kwic" match="ce:data">
  <tr>
    <xsl:apply-templates mode="kwic" select="ce:kwic1|ce:kwic2|ce:kwic3"/>
  </tr>
  <tr>
    <td class="kwic-label" colspan="3">
      <xsl:apply-templates select="ce:label"/>
    </td>
  </tr>
</xsl:template>

<xsl:template mode="kwic" match="ce:kwic1|ce:kwic2|ce:kwic3">
  <td class="{local-name(.)}">
    <xsl:apply-templates/>
  </td>
</xsl:template>

<xsl:template mode="summ" match="ce:data">
  <xsl:apply-templates mode="summ"/>
</xsl:template>

<!-- A feature of summary output is that the HTML tags are not in a default
     xhtml namespace ... -->
<xsl:template mode="summ" match="a">
  <xsl:variable name="item">
    <xsl:call-template name="calc-item"/>
  </xsl:variable>
  <a href="javascript:p3item('cbd',{$item})">
    <xsl:apply-templates mode="summ"/>
  </a>
</xsl:template>

<xsl:template mode="summ" match="*">
<!--  <xsl:message><xsl:value-of select="local-name(.)"/></xsl:message> -->
  <xsl:element name="{local-name(.)}">
    <xsl:copy-of select="@class"/>
    <xsl:apply-templates mode="summ"/>
  </xsl:element>
</xsl:template>

<xsl:template match="ce:data">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="ce:heading">
  <h2 class="ce-heading h2 border-top heading"><xsl:apply-templates/></h2>
</xsl:template>

<xsl:template match="ce:content">
  <p class="ce-result">
    <xsl:apply-templates/>
  </p>
</xsl:template>

<xsl:template match="ce:label">
  <p class="ce-label">
    <xsl:call-template name="make-label-href"/>
  </p>
</xsl:template>

<xsl:template name="make-label-href">
  <xsl:param name="label" select="''"/>
  <xsl:variable name="item">
    <xsl:call-template name="calc-item"/>
  </xsl:variable>
  <xsl:variable name="ce-data" select="ancestor::ce:data"/>
  <a href="javascript:p3item('xtf',{$item})"> <!-- think about how to switch transonly on automatically for ITEM mode inside TRA search -->
    <xsl:text>(</xsl:text>
    <xsl:choose>
      <xsl:when test="string-length($label)=0">
	<xsl:apply-templates/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$label"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>)</xsl:text>
  </a>
</xsl:template>

<xsl:template name="calc-item">
  <xsl:value-of select="number($item-offset) + 1 + count(ancestor::ce:data/preceding-sibling::ce:data)"/>
</xsl:template>

</xsl:stylesheet>
