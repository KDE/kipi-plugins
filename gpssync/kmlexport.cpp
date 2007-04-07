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

// KDE includes.

#include <kstandarddirs.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kio/job.h>
#include <klocale.h>
#include <libkexiv2/kexiv2.h>

// Local includes.

#include "kmlexport.h"

namespace KIPIGPSSyncPlugin {

kmlExport::kmlExport(KIPI::Interface* interface)
{
    m_interface = interface;
    QWidget* parent=KApplication::kApplication()->mainWidget();
    m_progressDialog=new KIPI::BatchProgressDialog(parent, i18n("Generating KML file..."));
}


kmlExport::~kmlExport()
{
    delete m_progressDialog;
}

/*!
	\fn kmlExport::createDir(QDir dir)
 */
bool kmlExport::createDir(QDir dir) {
    if (dir.exists()) return true;

    QDir parent=dir;
    parent.cdUp();
    bool ok=createDir(parent);
    if (!ok) {
        logError(i18n("Could not create '%1").arg(parent.path()));
        return false;
    }
    return parent.mkdir(dir.dirName());
}


/*!
    \fn kmlExport::webifyFileName(QString fileName)
 */
QString kmlExport::webifyFileName(QString fileName) {
    fileName=fileName.lower();

    // Remove potentially troublesome chars
    fileName=fileName.replace(QRegExp("[^-0-9a-z]+"), "_");

    return fileName;
}


/*!
    \fn kmlExport::generateSquareThumbnail(const QImage& fullImage, int size)
 */
QImage kmlExport::generateSquareThumbnail(const QImage& fullImage, int size) {
    QImage image = fullImage.smoothScale(size, size, QImage::ScaleMax);

    if (image.width() == size && image.height() == size) {
        return image;
    }
    QPixmap croppedPix(size, size);
    QPainter painter(&croppedPix);

    int sx=0, sy=0;
    if (image.width()>size) {
        sx=(image.width() - size)/2;
    } else {
        sy=(image.height() - size)/2;
    }
    painter.drawImage(0, 0, image, sx, sy, size, size);
    painter.end();

    return croppedPix.convertToImage();
}

/*!
    \fn kmlExport::generateBorderedThumbnail(const QImage& fullImage, int size)
 */
QImage kmlExport::generateBorderedThumbnail(const QImage& fullImage, int size) {
    int image_border = 3;

    // getting an image minus the border 
    QImage image = fullImage.smoothScale(size -(2*image_border), size - (2*image_border) , QImage::ScaleMax);


    QPixmap croppedPix(image.width() + (2*image_border), image.height() + (2*image_border));
    QPainter painter(&croppedPix);

    QColor BrushColor(255,255,255);
    painter.fillRect(0,0,image.width() + (2*image_border),image.height() + (2*image_border),BrushColor);
    /*! @todo add a corner to the thumbnail and a hotspot to the kml element */

    painter.drawImage(image_border, image_border, image );
    painter.end();

    return croppedPix.convertToImage();
}

/*!
\fn kmlExport::generateImagesthumb(KIPI::Interface* interface, const KURL& imageURL, QDomElement &kmlAlbum )
 */
void kmlExport::generateImagesthumb(KIPI::Interface* interface, const KURL& imageURL, QDomElement &kmlAlbum ) {
    KIPI::Interface* mInterface=interface;
    KIPI::ImageInfo info=mInterface->info(imageURL);

    // Load image
    QString path=imageURL.path();
    QFile imageFile(path);
    if (!imageFile.open(IO_ReadOnly)) {
        logWarning(i18n("Could not read image '%1'").arg(path));
        return;
    }

    QString imageFormat = QImageIO::imageFormat(&imageFile);
    if (imageFormat.isEmpty()) {
        logWarning(i18n("Format of image '%1' is unknown").arg(path));
        return;
    }
    imageFile.close();
    imageFile.open(IO_ReadOnly);

    QByteArray imageData = imageFile.readAll();
    QImage image;
    if (!image.loadFromData(imageData) ) {
        logWarning(i18n("Error loading image '%1'").arg(path));
        return;
    }

    // Process images
    /** FIXME depending the soft used, angle could return a good value (digikam) or a value of 0 (gwenview)
        * and, in some case the picture is not rotated as it should be.
        */
    if ( info.angle() != 0 ) {
        QWMatrix matrix;
        matrix.rotate( info.angle() );
        image = image.xForm( matrix );
    }
    image = image.smoothScale(m_size, m_size, QImage::ScaleMax);

    QImage icon;
    if (m_optimize_googlemap) {
        icon = generateSquareThumbnail(image,m_googlemapSize);
    } else {
    //	icon = image.smoothScale(m_iconSize, m_iconSize, QImage::ScaleMax);
        icon = generateBorderedThumbnail(image, m_iconSize);
    }


    // Save images
    /** @todo remove the extension of the file
        * it's appear with digikam but not with gwenview
        * which already seems to strip the extention
        */
    QString baseFileName = webifyFileName(info.title());
    //	baseFileName = mUniqueNameHelper.makeNameUnique(baseFileName);
    QString fullFileName;
    fullFileName = baseFileName + "." + imageFormat.lower();
    QString destPath = m_tempDestDir + m_imageDir + fullFileName;
    if (!image.save(destPath, imageFormat.ascii(), 85)) {
        // if not able to save the image, it's pointless to create a placemark
        logWarning(i18n("Could not save image '%1' to '%2'").arg(path).arg(destPath));
    } else {
        //logInfo(i18n("Creation of picture '%1'").arg(fullFileName));
        KExiv2Iface::KExiv2 exiv2Iface;
        exiv2Iface.load(imageURL.path());
        double alt, lat, lng;
        exiv2Iface.getGPSInfo(alt, lat, lng);
        QDomElement kmlPlacemark = addKmlElement(kmlAlbum, "Placemark");
        addKmlTextElement(kmlPlacemark,"name",fullFileName);
        // location and altitude
        QDomElement kmlGeometry = addKmlElement(kmlPlacemark, "Point");
        if (alt) {
            addKmlTextElement(kmlGeometry, "coordinates", QString("%1,%2,%3").arg(lng).arg(lat).arg(alt));
        } else {
            addKmlTextElement(kmlGeometry, "coordinates", QString("%1,%2").arg(lng).arg(lat));
        }
        if (m_altitudeMode == 2 ) {
            addKmlTextElement(kmlGeometry, "altitudeMode", "absolute");
        } else if (m_altitudeMode == 1 ) {
            addKmlTextElement(kmlGeometry, "altitudeMode", "relativeToGround");
        } else {
            addKmlTextElement(kmlGeometry, "altitudeMode", "clampToGround");
        }
        addKmlTextElement(kmlGeometry, "extrude", "1");
       // we try to load exif value if any otherwise, try the application db
        /** we need to take the DateTimeOriginal
            * if we refer to http://www.exif.org/Exif2-2.PDF
            * (standard)DateTime: is The date and time of image creation. In this standard it is the date and time the file was changed
            * DateTimeOriginal: The date and time when the original image data was generated. 
            *                   For a DSC the date and time the picture was taken are recorded.
            * DateTimeDigitized: The date and time when the image was stored as digital data.
            * So for:
            * - a DSC: the right time is the DateTimeDigitized wich is also DateTimeOriginal
            *          if the picture has been modified the (standard)DateTime should change.
            * - a scanned picture, the right time is the DateTimeOriginal which should also be the the DateTime
            *          the (standard)DateTime should be the same except if the picture is modified
            * - a panorama created from several pictures, the right time is the DateTimeOriginal (average of DateTimeOriginal actually)
            *          The (standard)DateTime is the creation date of the panorama.
            * it's seems the time to take into acccount is the DateTimeOriginal.
            * but the exiv2Iface.getImageDateTime() return the (standard)DateTime first
            * libkexiv2 seems to take Original dateTime first so it shoul be alright now.
            */
        QDateTime datetime = exiv2Iface.getImageDateTime();
        if (datetime.isValid()) {
            QDomElement kmlTimeStamp = addKmlElement(kmlPlacemark, "TimeStamp");
            addKmlTextElement(kmlTimeStamp, "when", datetime.toString("yyyy-MM-ddThh:mm:ssZ"));
        } else if ( mInterface->hasFeature(KIPI::ImagesHasTime)){
            QDomElement kmlTimeStamp = addKmlElement(kmlGeometry, "TimeStamp");
            addKmlTextElement(kmlTimeStamp, "when", (info.time()).toString("yyyy-MM-ddThh:mm:ssZ"));
        }
        QString my_description;
        if (m_optimize_googlemap) {
            my_description = "<img src=\"" + m_UrlDestDir + m_imageDir + fullFileName + "\">";
        } else {
            my_description = "<img src=\"" + m_imageDir + fullFileName + "\">";
        }
        if ( m_interface->hasFeature( KIPI::ImagesHasComments ) ) {
            my_description += "<br/>" + info.description() ;
        }
        addKmlTextElement(kmlPlacemark, "description", my_description);
        logInfo(i18n("Creation of placemark '%1'").arg(fullFileName));

        // Save icon
        QString iconFileName = "thumb_" + baseFileName + "." + imageFormat.lower();
        QString destPath = m_tempDestDir + m_imageDir + iconFileName;
        if (!icon.save(destPath, imageFormat.ascii(),85)) {
            logWarning(i18n("Could not save icon for image '%1' to '%2'").arg(path).arg(destPath));
        } else {
            //logInfo(i18n("Creation of icon '%1'").arg(iconFileName));
            // style et icon
            QDomElement kmlStyle = addKmlElement(kmlPlacemark, "Style");
            QDomElement kmlIconStyle = addKmlElement(kmlStyle, "IconStyle");
            QDomElement kmlIcon = addKmlElement(kmlIconStyle, "Icon");
            if (m_optimize_googlemap) {
                addKmlTextElement(kmlIcon, "href", m_UrlDestDir + m_imageDir + iconFileName);
            } else {
                addKmlTextElement(kmlIcon, "href", m_imageDir + iconFileName);
            }
            QDomElement kmlBallonStyle = addKmlElement(kmlStyle, "BalloonStyle");
            addKmlTextElement(kmlBallonStyle, "text", "$[description]");
        }
    }
}

/*!
\fn kmlExport::addTrack(QDomElement &kmlAlbum)
 */
/**
 * 
 */
void kmlExport::addTrack(QDomElement &kmlAlbum)
{
    if( m_GPXFile.isEmpty() ) {
        logWarning(i18n("No GPX file Chosen!"));
        return;
    }

    m_gpxParser.clear();
    bool ret = m_gpxParser.loadGPXFile(m_GPXFile);

    if (!ret)
    {
        logError(i18n("Cannot parse %1 GPX file!")
                 .arg(m_GPXFile));
        return;
    }

    if (m_gpxParser.numPoints() <= 0)
    {
        logError(i18n("The %1 GPX file do not have a date-time track to use!")
                 .arg(m_GPXFile));
        return;
    }

    // style of points and track
    QDomElement kmlTrackStyle = addKmlElement(kmlAlbum, "Style");
    kmlTrackStyle.setAttribute("id","track");
    QDomElement kmlIconStyle = addKmlElement(kmlTrackStyle, "IconStyle");
    QDomElement kmlIcon = addKmlElement(kmlIconStyle, "Icon");
    //! FIXME is there a way to be sure of the location of the icon?
    addKmlTextElement(kmlIcon, "href", "http://maps.google.com/mapfiles/kml/pal4/icon60.png");
    // linetrack style
    QDomElement kmlLineTrackStyle = addKmlElement(kmlAlbum, "Style");
    kmlLineTrackStyle.setAttribute("id","linetrack");
    QDomElement kmlLineStyle = addKmlElement(kmlLineTrackStyle, "LineStyle");
    // the KML color is not #RRGGBB but AABBGGRR
    QString KMLColorValue = QString("%1%2%3%4")
        .arg((int)m_GPXOpacity*256/100, 2, 16)
        .arg((&m_GPXColor)->blue(), 2, 16)
        .arg((&m_GPXColor)->green(), 2, 16)
        .arg((&m_GPXColor)->red(), 2, 16);
    addKmlTextElement(kmlLineStyle, "color", KMLColorValue);
    addKmlTextElement(kmlLineStyle, "width", QString("%1").arg(m_LineWidth) );

    m_gpxParser.CreateTrack(kmlAlbum, *kmlDocument, m_TimeZone - 12, m_GPXAltitudeMode);

}

/*!
    \fn kmlExport::generate()
 */
/**
 * 
 */
void kmlExport::generate()
{
    //! @todo faire un test ici avant d'aller plus loin
    createDir(m_tempDestDir + m_imageDir);

    m_progressDialog->show();
    KIPI::ImageCollection selection = m_interface->currentSelection();
    KIPI::ImageCollection album = m_interface->currentAlbum();
    // create the document, and it's root
    kmlDocument = new QDomDocument("");
    QDomImplementation impl;
    QDomProcessingInstruction instr = kmlDocument->createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    kmlDocument->appendChild(instr);
    QDomElement kmlRoot = kmlDocument->createElementNS( "http://earth.google.com/kml/2.1","kml");
    kmlDocument->appendChild( kmlRoot );

    QDomElement kmlAlbum = addKmlElement( kmlRoot, "Document");
    QDomElement kmlName= addKmlTextElement( kmlAlbum, "name", album.name());
    QDomElement kmlDescription = addKmlHtmlElement( kmlAlbum, "description", "Created with kmlexport <a href=\"http://www.kipi-plugins.org/\">kipi-plugin</a>");


    KURL::List images = selection.images();
    int pos = 1;
    int count = images.count();
    for( KURL::List::Iterator selIt = images.begin(); selIt != images.end(); ++selIt, ++pos) {
        KExiv2Iface::KExiv2 exiv2Iface;
        KIPI::ImageInfo info = m_interface->info( *selIt );
        // exiv2 load from url not image
        KURL url = *selIt;
        exiv2Iface.load(url.path());
        double alt, lat, lng;
        bool hasGPSInfo = exiv2Iface.getGPSInfo(alt, lat, lng);
        if ( hasGPSInfo ) {
            // generation de l'image et de l'icone
            generateImagesthumb(m_interface,url,kmlAlbum);
        } else {
            logWarning(i18n("No position info for:'%1'").arg(info.title()));
        }
        m_progressDialog->setProgress(pos, count);
        qApp->processEvents();
    }

    if (m_GPXtracks) {
        addTrack(kmlAlbum);
    }


    /** @todo change to kml or kmz if compressed */
    QFile file( m_tempDestDir + m_KMLFileName + ".kml");
    /** @todo handle file opening problems */
    file.open( IO_WriteOnly );
    QTextStream stream( &file ); // we will serialize the data into the file
    stream << kmlDocument->toString();
    file.close();

    delete kmlDocument;

    KIO::moveAs(m_tempDestDir,m_baseDestDir,false);
    logInfo(i18n("Move to final directory"));
    m_progressDialog->close();
}

/*!
    \fn kmlExport::getConfig()
 */
int kmlExport::getConfig()
{
    KConfig config("kipirc");
    config.setGroup("KMLExport Settings");

    m_localTarget			= config.readBoolEntry("localTarget");
    m_optimize_googlemap	= config.readBoolEntry("optimize_googlemap");
    m_iconSize				= config.readNumEntry("iconSize");
    //	googlemapSize		= config.readNumEntry("googlemapSize");
    m_size					= config.readNumEntry("size");
    // UrlDestDir have to have the trailing /
    m_baseDestDir			= config.readEntry("baseDestDir");
    m_UrlDestDir			= config.readEntry("UrlDestDir");
    m_altitudeMode          = config.readNumEntry("Altitude Mode", 0);
    m_KMLFileName			= config.readEntry("KMLFileName");
    m_GPXtracks				= config.readBoolEntry("UseGPXTracks");
    m_GPXFile				= config.readEntry("GPXFile");
    m_TimeZone              = config.readNumEntry("Time Zone", 12);
    m_LineWidth             = config.readNumEntry("Line Width", 4);
    m_GPXColor              = config.readEntry("Track Color", "#17eeee" );
    m_GPXOpacity            = config.readNumEntry("Track Opacity", 64 );
    m_GPXAltitudeMode       = config.readNumEntry("GPX Altitude Mode", 0);

    KStandardDirs dir;
    m_tempDestDir = dir.saveLocation("tmp", "kipi-kmlrexportplugin-" + QString::number(getpid()) + "/");

    m_imageDir = "images/";
    m_googlemapSize = 32;
    return 1;
}

void kmlExport::logInfo(const QString& msg) {
    m_progressDialog->addedAction(msg, KIPI::ProgressMessage);
}

void kmlExport::logError(const QString& msg) {
    m_progressDialog->addedAction(msg, KIPI::ErrorMessage);
}

void kmlExport::logWarning(const QString& msg) {
    m_progressDialog->addedAction(msg, KIPI::WarningMessage);
    //	mWarnings=true;
}


} //namespace
