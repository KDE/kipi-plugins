/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a tool to export GPS data to KML file.
 *
 * Copyright (C) 2006-2007 by Stephane Pontier <shadow dot walker at free dot fr>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KIPIKMLEXPORTKMLEXPORT_H
#define KIPIKMLEXPORTKMLEXPORT_H

// Qt includes

#include <QColor>
#include <QDir>
#include <QDomDocument>

// Local includes

#include "kmlgpsdataparser.h"

class QImage;

namespace KIPI 
{
    class Interface;
}

namespace KIPIPlugins 
{
    class BatchProgressDialog;
}

namespace KIPIGPSSyncPlugin 
{

/**
Exporter to KML

	@author KIPI dev. team
*/
class kmlExport
{

public:

    kmlExport(KIPI::Interface* interface);
    ~kmlExport();

    bool createDir(QDir dir);

    /*! generate the kml element for pictures with tumbnails
     *  @param interface the kipi interface
     *  @param KUrl the URL of the picture
     *  @param kmlAlbum the album used
     */
    void generateImagesthumb(KIPI::Interface* interface, const KUrl&, QDomElement &kmlAlbum);

    /*! Produce a web-friendly file name
     *  otherwise, while google earth works fine, maps.google.com may not find pictures and thumbnail
     *  thank htmlexport
     *  @param the filename
     *  @return the webifyed filename
     */
    QString webifyFileName(const QString &fileName);

    /*! Generate a square thumbnail from @fullImage of @size x @size pixels
     *  @param fullImage the original image
     *  @param size the size of the thumbnail
     *  @return the thumbnail
     */
    QImage generateSquareThumbnail(const QImage& fullImage, int size);

    /*! Generate a square thumbnail from @fullImage of @size x @size pixels
     *  with a white border
     *  @param fullImage the original image
     *  @param size the size of the thumbnail
     *  @return the thumbnail
     */
    QImage generateBorderedThumbnail(const QImage& fullImage, int size);
    void   addTrack(QDomElement &kmlAlbum);
    void   generate();
    int    getConfig();

public:

    bool             m_localTarget;
    bool             m_optimize_googlemap;
    bool             m_GPXtracks;

    int              m_iconSize;
    int              m_googlemapSize;
    int              m_size;
    int              m_altitudeMode;
    int              m_TimeZone;
    int              m_LineWidth;
    int              m_GPXOpacity;
    int              m_GPXAltitudeMode;

    /** directory used in kmldocument structure */
    QString          m_imageDir;
    QString          m_GPXFile;
    QString          m_UrlDestDir;

    /** temporary directory where everything will be created */
    QString          m_tempDestDir;

    /** directory selected by user*/
    QString          m_baseDestDir;

    QString          m_imgdir;
    QString          m_KMLFileName;

    QColor           m_GPXColor;

    KIPI::Interface *m_interface;

private:

    /*! the root document, used to create all QDomElements */
    QDomDocument                     *kmlDocument;

    /*! the GPS parsed data */
    KMLGPSDataParser                  m_gpxParser;

    KIPIPlugins::BatchProgressDialog *m_progressDialog;

private:

    void logInfo(const QString& msg);
    void logError(const QString& msg);
    void logWarning(const QString& msg);

    /*!
     *  \fn KIPIKMLExport::kmlExport::addKmlElement(QDomElement target, QString tag)
     *  Add a new element
     *  @param target the parent element to which add the element
     *  @param tag the new element name
     *  @return the New element
     */
    QDomElement addKmlElement(QDomElement &target, QString tag)
    {
        QDomElement kmlElement = kmlDocument->createElement( tag );
        target.appendChild( kmlElement );
        return kmlElement;
    }

    /*!
     *  \fn KIPIKMLExport::kmlExport::addKmlTextElement(QDomElement target, QString tag, QString text)
     *  Add a new element with a text
     *  @param target the parent element to which add the element
     *  @param tag the new element name
     *  @param text the text content of the new element
     *  @return the New element
     */
    QDomElement addKmlTextElement(QDomElement &target, QString tag, QString text)
    {
        QDomElement kmlElement  = kmlDocument->createElement( tag );
        target.appendChild( kmlElement );
        QDomText kmlTextElement = kmlDocument->createTextNode( text );
        kmlElement.appendChild( kmlTextElement );
        return kmlElement;
    }

    /*!
     *  \fn KIPIKMLExport::kmlExport::addKmlHtmlElement(QDomElement target, QString tag, QString text)
     *  Add a new element with html content (html entities are escaped and text is wrapped in a CDATA section)
     *  @param target the parent element to which add the element
     *  @param tag the new element name
     *  @param text the HTML content of the new element
     *  @return the New element
     */
    QDomElement addKmlHtmlElement(QDomElement &target, QString tag, QString text)
    {
        QDomElement kmlElement  = kmlDocument->createElement( tag );
        target.appendChild( kmlElement );
        QDomText kmlTextElement = kmlDocument->createCDATASection( text );
        kmlElement.appendChild( kmlTextElement );
        return kmlElement;
    }
};

} // namespace KIPIGPSSyncPlugin

#endif // KIPIKMLEXPORTKMLEXPORT_H
