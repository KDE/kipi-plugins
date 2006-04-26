<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE stylesheet [
<!ENTITY raquo "&#187;">
]>

<xsl:transform version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:exsl="http://exslt.org/common"
	extension-element-prefixes="exsl">

<xsl:template name="thumbnailLink">
	<xsl:param name="text"/>
	<xsl:param name="folder" select="'.'"/>
	<a class="imageLink" href="{$folder}/{full/@fileName}.html">
		<img src="{$folder}/{thumbnail/@fileName}" width="{thumbnail/@width}" height="{thumbnail/@height}" />
		<br/>
		<xsl:value-of select="$text"/>
	</a>
</xsl:template>

<xsl:template name="imagePage">
	<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
		<title><xsl:value-of select="title"/></title>
		<link rel="stylesheet" type="text/css" href="../matrix/style.css"/>
	</head>
	<body id="imagePage">
	<h1>
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
		<div>
			<div id="previous">
				<xsl:choose>
					<xsl:when test="position() &gt; 1">
						<xsl:for-each select="preceding-sibling::image[position()=1]">
							<xsl:call-template name="thumbnailLink">
								<xsl:with-param name="text">Previous</xsl:with-param>
							</xsl:call-template>
						</xsl:for-each>
					</xsl:when>
					<xsl:otherwise>
						<div class="endOfCollection">
							Previous
						</div>
					</xsl:otherwise>
				</xsl:choose>
			</div>
			
			<div id="next">
				<xsl:choose>
					<xsl:when test="position() &lt; last()">
						<xsl:for-each select="following-sibling::image[position()=1]">
							<xsl:call-template name="thumbnailLink">
								<xsl:with-param name="text">Next</xsl:with-param>
							</xsl:call-template>
						</xsl:for-each>
					</xsl:when>
					<xsl:otherwise>
						<div class="endOfCollection">
							Next
						</div>
					</xsl:otherwise>
				</xsl:choose>
			</div>
			
			<div id="full">
				<img src="{full/@fileName}" width="{full/@width}" height="{full/@height}" />
				<p>
				<xsl:value-of select="description"/>
				</p>
			</div>
			<br style="clear:both"/>
		</div>
		
	</div>
	</body>
	</html>
</xsl:template>


<xsl:template name="collectionPage">
	<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
		<title><xsl:value-of select="name"/></title>
		<link rel="stylesheet" type="text/css" href="matrix/style.css"/>
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
					<xsl:call-template name="thumbnailLink">
						<xsl:with-param name="text"><xsl:value-of select="title"/></xsl:with-param>
						<xsl:with-param name="folder"><xsl:value-of select="$folder"/></xsl:with-param>
					</xsl:call-template>
				</li>
				<exsl:document href='{$folder}/{full/@fileName}.html'>
					<xsl:call-template name="imagePage"/>
				</exsl:document>
			</xsl:for-each>
		</ul>
		<br style="clear:both"/>
	</div> <!-- /content -->
	</body>
	</html>
</xsl:template>


<xsl:template name="collectionListPage">
	<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
		<title><xsl:value-of select="$i18nCollectionList"/></title>
		<link rel="stylesheet" type="text/css" href="matrix/style.css"/>
	</head>
	<body>
	<h1><xsl:value-of select="$i18nCollectionList"/></h1>
	<div id="content">
		<ul>
			<xsl:for-each select="collections/collection">
				<li>
					<a href="{fileName}.html">
						<!-- Use first image as collection image -->
						<img src="{fileName}/{image[1]/thumbnail/@fileName}"
							width="{image[1]/thumbnail/@width}"
							height="{image[1]/thumbnail/@height}" />
						<br />
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
