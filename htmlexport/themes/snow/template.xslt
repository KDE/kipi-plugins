<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE stylesheet [
<!ENTITY raquo "&#187;">
]>

<xsl:transform version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:exsl="http://exslt.org/common"
	extension-element-prefixes="exsl">

<xsl:template name="imagePage">
	<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
		<title><xsl:value-of select="title"/></title>
		<link rel="stylesheet" type="text/css" href="../snow/style.css"/>
	</head>
	<body id="imagePage">
	<h1>
		<div id="previous">
			<xsl:choose>
				<xsl:when test="position() &gt; 1">
					<a href="{preceding-sibling::image[position()=1]/full/@fileName}.html">
						<img src="../snow/previous.png" alt="&lt;" title="{$i18nPrevious}" />
					</a>
				</xsl:when>
				<xsl:otherwise>
					<img src="../snow/previous_disabled.png" alt="&lt;" title="{$i18nPrevious}" />
				</xsl:otherwise>
			</xsl:choose>
		</div>
		<div id="next">
			<xsl:choose>
				<xsl:when test="position() &lt; last()">
					<a href="{following-sibling::image[position()=1]/full/@fileName}.html">
						<img src="../snow/next.png" alt="&gt;" title="{$i18nNext}" />
					</a>
				</xsl:when>
				<xsl:otherwise>
					<img src="../snow/next_disabled.png" alt="&gt;" title="{$i18nNext}" />
				</xsl:otherwise>
			</xsl:choose>
		</div>
		<div id="caption">
			<xsl:choose>
				<xsl:when test="count(/collections/collection) &gt; 1">
					<a href="../index.html"><xsl:value-of select="$i18nCollectionList"/></a>
					&raquo;
					<a href="../{../fileName}.html"><xsl:value-of select="../name"/></a>
				</xsl:when>
				<xsl:otherwise>
					<a href="../index.html"><xsl:value-of select="../name"/></a>
				</xsl:otherwise>
			</xsl:choose>

			&raquo; <xsl:value-of select="title"/>
			(<xsl:value-of select="position()"/>/<xsl:value-of select="last()"/>)
		</div>
	</h1>

	<div id="content">
		<img src="{full/@fileName}" width="{full/@width}" height="{full/@height}" />
		<p>
		<xsl:value-of select="description"/>
		</p>
	</div>
	</body>
	</html>
</xsl:template>


<xsl:template name="collectionPage">
	<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
		<title><xsl:value-of select="name"/></title>
		<link rel="stylesheet" type="text/css" href="snow/style.css"/>
	</head>
	<body id="collectionPage">
	<h1>
		<xsl:if test="count(/collections/collection) &gt; 1">
			<a href="index.html"><xsl:value-of select="$i18nCollectionList"/></a>
			&raquo;
		</xsl:if>
		<xsl:value-of select="name"/>
	</h1>
	<div id="content">
		<ul>
			<xsl:variable name="folder" select='fileName'/>
			<xsl:for-each select="image">
				<li>
					<a href='{$folder}/{full/@fileName}.html'>
						<img src="{$folder}/{thumbnail/@fileName}" width="{thumbnail/@width}" height="{thumbnail/@height}" />
					</a>
					<br/>
					<a href='{$folder}/{full/@fileName}.html'>
						<xsl:value-of select="title"/>
					</a>
				</li>
				<exsl:document href='{$folder}/{full/@fileName}.html'>
					<xsl:call-template name="imagePage"/>
				</exsl:document>
			</xsl:for-each>
		</ul>
	</div> <!-- /content -->
	</body>
	</html>
</xsl:template>


<xsl:template name="collectionListPage">
	<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
		<title><xsl:value-of select="$i18nCollectionList"/></title>
		<link rel="stylesheet" type="text/css" href="snow/style.css"/>
	</head>
	<body>
	<h1><xsl:value-of select="$i18nCollectionList"/></h1>
	<div id="content">
		<ul>
			<xsl:for-each select="collections/collection">
				<li>
					<a href="{fileName}.html">
						<xsl:value-of select="name"/>
					</a>
				</li>
				<exsl:document href="{fileName}.html">
					<xsl:call-template name="collectionPage"/>
				</exsl:document>
			</xsl:for-each>
		</ul>
	</div> <!-- /content -->
	</body>
	</html>
</xsl:template>


<xsl:template match="/">
	<xsl:choose>
		<xsl:when test="count(collections/collection) &gt; 1">
			<xsl:call-template name="collectionListPage"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:for-each select="collections/collection">
				<xsl:call-template name="collectionPage"/>
			</xsl:for-each>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>


</xsl:transform>
