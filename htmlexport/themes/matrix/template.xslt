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
	<a class="imageLink" href="{full/@fileName}.html">
		<img src="{thumbnail/@fileName}" width="{thumbnail/@width}" height="{thumbnail/@height}" />
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
			<a href="../index.html">Collection List</a>
			&raquo; <a href="index.html"><xsl:value-of select="../name"/></a>
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
		<link rel="stylesheet" type="text/css" href="../matrix/style.css"/>
	</head>
	<body id="collectionPage">
	<h1>
		<a href="../index.html">Collection List</a> &raquo; <xsl:value-of select="name"/>
	</h1>
	<div id="content">
		<ul>
			<xsl:for-each select="image">
				<li>
					<xsl:call-template name="thumbnailLink">
						<xsl:with-param name="text"><xsl:value-of select="title"/></xsl:with-param>
					</xsl:call-template>
				</li>
				<exsl:document href='{full/@fileName}.html'>
					<xsl:call-template name="imagePage"/>
				</exsl:document>
			</xsl:for-each>
		</ul>
		<br style="clear:both"/>
	</div> <!-- /content -->
	</body>
	</html>
</xsl:template>


<xsl:template match="/">
	<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
		<title>Collection List</title>
		<link rel="stylesheet" type="text/css" href="matrix/style.css"/>
	</head>
	<body>
	<h1>Collection List</h1>
	<div id="content">
		<ul>
			<xsl:for-each select="collections/collection">
				<li>
					<a href="{fileName}/index.html">
						<xsl:value-of select="name"/>
					</a>
				</li>
				<exsl:document href="{fileName}/index.html">
					<xsl:call-template name="collectionPage"/>
				</exsl:document>
			</xsl:for-each>
		</ul>
	</div> <!-- /content -->
	</body>
	</html>
</xsl:template>

</xsl:transform>
