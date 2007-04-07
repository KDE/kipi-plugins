/***************************************************************************
 *   Copyright (C) 2006-2007 by Stéphane Pontier                           *
 *   shadow.walker@free.fr                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef KIPIKMLEXPORTKMLEXPORT_H
#define KIPIKMLEXPORTKMLEXPORT_H

// LibKIPI includes
#include <libkipi/plugin.h>

// KIPI
#include <libkipi/batchprogressdialog.h>
#include <libkipi/imageinfo.h>
#include <libkipi/interface.h>

// Qt
#include <qdir.h>
#include <qdom.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qimage.h>
#include <kdebug.h>
#include <qpainter.h>
#include <qregexp.h>

// locale include
#include "kmlgpsdataparser.h"


namespace KIPIGPSSyncPlugin {

/**
Exporter to KML

	@author KIPI dev. team
*/
class kmlExport{
public:
    kmlExport(KIPI::Interface* interface);

    ~kmlExport();

    bool createDir(QDir dir);
    /*! generate the kml element for pictures with tumbnails
     *  @param interface the kipi interface
     *  @param KURL the URL of the picture
     *  @param kmlAlbum the album used
     */
    void generateImagesthumb(KIPI::Interface* interface, const KURL&, QDomElement &kmlAlbum);
    /*! Produce a web-friendly file name
     *  otherwise, while google earth works fine, maps.google.com may not find pictures and thumbnail
     *  thank htmlexport
     *  @param the filename
     *  @return the webifyed filename
     */
    QString webifyFileName(QString fileName);
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
    void addTrack(QDomElement &kmlAlbum);
    void generate();
    int getConfig();

    /** temporary directory where everything will be created */
    QString m_tempDestDir;
    /** directory selected by user*/
    QString m_baseDestDir;
    QString m_imgdir;
    QString m_KMLFileName;
    KIPI::Interface* m_interface;

    /** directory used in kmldocument structure */
    QString m_imageDir;

    bool m_localTarget;
    int m_iconSize;
    int m_googlemapSize;
    bool m_optimize_googlemap;
    int m_size;
    QString m_UrlDestDir;
    int m_altitudeMode;
    bool m_GPXtracks;
    QString m_GPXFile;
    int m_TimeZone;
    int m_LineWidth;
    QColor m_GPXColor;
    int m_GPXOpacity;
    int m_GPXAltitudeMode;

private:
    KIPI::BatchProgressDialog* m_progressDialog;
    /*! the root document, used to create all QDomElements */
    QDomDocument *kmlDocument;
    /*! the GPS parsed data */
    KMLGPSDataParser m_gpxParser;


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
        QDomElement kmlElement = kmlDocument->createElement( tag );
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
        QDomElement kmlElement = kmlDocument->createElement( tag );
        target.appendChild( kmlElement );
        QDomText kmlTextElement = kmlDocument->createCDATASection( text );
        kmlElement.appendChild( kmlTextElement );
        return kmlElement;
    }
};

}

#endif
