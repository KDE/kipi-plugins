/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-05-16
 * Description : a tool to export GPS data to KML file.
 *
 * Copyright (C) 2006-2007 by Stephane Pontier <shadow dot walker at free dot fr>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kmlexport.h"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// Qt includes

#include <QImageReader>
#include <QPainter>
#include <QRegExp>
#include <QTextStream>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kio/copyjob.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/plugin.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kpbatchprogressdialog.h"
#include "kpimageinfo.h"
#include "kpmetadata.h"

namespace KIPIKMLExportPlugin
{

KmlExport::KmlExport(Interface* const interface)
{
    m_localTarget        = true;
    m_optimize_googlemap = false;
    m_GPXtracks          = false;
    m_iconSize           = 33;
    m_googlemapSize      = 32;
    m_size               = 320;
    m_altitudeMode       = 0;
    m_TimeZone           = 12;
    m_LineWidth          = 4;
    m_GPXOpacity         = 64;
    m_GPXAltitudeMode    = 0;
    m_kmlDocument        = 0;
    m_interface          = interface;
    m_progressDialog     = new KPBatchProgressDialog(kapp->activeWindow(), i18n("Generating KML file..."));
}

KmlExport::~KmlExport()
{
    delete m_progressDialog;
}

/*!
    \fn KmlExport::createDir(QDir dir)
 */
bool KmlExport::createDir(const QDir& dir) const
{
    if (dir.exists()) return true;

    QDir parent = dir;
    parent.cdUp();
    bool ok     = createDir(parent);

    if (!ok)
    {
        logError(i18n("Could not create '%1'", parent.path()));
        return false;
    }

    return parent.mkdir(dir.dirName());
}

/*!
\fn KmlExport::webifyFileName(const QString &fileName)
 */
QString KmlExport::webifyFileName(const QString& fileName) const
{
    QString webFileName = fileName.toLower();

    // Remove potentially troublesome chars
    webFileName         = webFileName.replace(QRegExp("[^-0-9a-z]+"), "_");

    return webFileName;
}

/*!
    \fn KmlExport::generateSquareThumbnail(const QImage& fullImage, int size)
 */
QImage KmlExport::generateSquareThumbnail(const QImage& fullImage, int size) const
{
    QImage image = fullImage.scaled(size, size, Qt::KeepAspectRatioByExpanding);

    if (image.width() == size && image.height() == size)
    {
        return image;
    }

    QPixmap croppedPix(size, size);
    QPainter painter(&croppedPix);

    int sx=0, sy=0;

    if (image.width()>size)
    {
        sx = (image.width() - size)/2;
    }
    else
    {
        sy = (image.height() - size)/2;
    }

    painter.drawImage(0, 0, image, sx, sy, size, size);
    painter.end();

    return croppedPix.toImage();
}

/*!
    \fn KmlExport::generateBorderedThumbnail(const QImage& fullImage, int size)
 */
QImage KmlExport::generateBorderedThumbnail(const QImage& fullImage, int size) const
{
    int image_border = 3;

    // getting an image minus the border
    QImage image     = fullImage.scaled(size -(2*image_border), size - (2*image_border), Qt::KeepAspectRatioByExpanding);

    QPixmap croppedPix(image.width() + (2*image_border), image.height() + (2*image_border));
    QPainter painter(&croppedPix);

    QColor BrushColor(255,255,255);
    painter.fillRect(0,0,image.width() + (2*image_border),image.height() + (2*image_border),BrushColor);
    /*! @todo add a corner to the thumbnail and a hotspot to the kml element */

    painter.drawImage(image_border, image_border, image );
    painter.end();

    return croppedPix.toImage();
}

/*!
\fn KmlExport::generateImagesthumb(const KUrl& imageURL, QDomElement& kmlAlbum )
 */
void KmlExport::generateImagesthumb(const KUrl& imageURL, QDomElement& kmlAlbum )
{
    KPImageInfo info(imageURL);

    // Load image
    QString path = imageURL.path();
    QFile imageFile(path);

    if (!imageFile.open(QIODevice::ReadOnly))
    {
        logWarning(i18n("Could not read image '%1'",path));
        return;
    }

    QImageReader reader(&imageFile);
    QString imageFormat = reader.format();

    if (imageFormat.isEmpty())
    {
        logWarning(i18n("Format of image '%1' is unknown",path));
        return;
    }

    imageFile.close();
    imageFile.open(QIODevice::ReadOnly);

    QByteArray imageData = imageFile.readAll();
    QImage image;

    if (!image.loadFromData(imageData) )
    {
        logWarning(i18n("Error loading image '%1'",path));
        return;
    }

    // Process images

    if ( info.orientation() != KPMetadata::ORIENTATION_UNSPECIFIED )
    {
        QMatrix matrix = RotationMatrix::toMatrix(info.orientation());
        image          = image.transformed( matrix );
    }

    image = image.scaled(m_size, m_size, Qt::KeepAspectRatioByExpanding);
    QImage icon;

    if (m_optimize_googlemap)
    {
        icon = generateSquareThumbnail(image,m_googlemapSize);
    }
    else
    {
    //    icon = image.smoothScale(m_iconSize, m_iconSize, QImage::ScaleMax);
        icon = generateBorderedThumbnail(image, m_iconSize);
    }

    // Save images
    /** @todo remove the extension of the file
     * it's appear with digikam but not with gwenview
     * which already seems to strip the extension
     */
    QString baseFileName = webifyFileName(info.name());
    //baseFileName       = mUniqueNameHelper.makeNameUnique(baseFileName);
    QString fullFileName;
    fullFileName         = baseFileName + '.' + imageFormat.toLower();
    QString destPath     = m_tempDestDir + m_imageDir + fullFileName;

    if (!image.save(destPath, imageFormat.toAscii(), 85))
    {
        // if not able to save the image, it's pointless to create a placemark
        logWarning(i18n("Could not save image '%1' to '%2'",path,destPath));
    }
    else
    {
        //logInfo(i18n("Creation of picture '%1'").arg(fullFileName));

        double     alt, lat, lng;
        KPMetadata meta;

        if (info.hasGeolocationInfo())
        {
            lat = info.latitude();
            lng = info.longitude();
            alt = info.altitude();
        }
        else
        {
            meta.load(imageURL.path());
            meta.getGPSInfo(alt, lat, lng);
        }

        QDomElement kmlPlacemark = addKmlElement(kmlAlbum, "Placemark");
        addKmlTextElement(kmlPlacemark,"name",fullFileName);
        // location and altitude
        QDomElement kmlGeometry  = addKmlElement(kmlPlacemark, "Point");

        if (alt)
        {
            addKmlTextElement(kmlGeometry, "coordinates", QString("%1,%2,%3 ")
                .arg(lng, 0, 'f', 8)
                .arg(lat, 0, 'f', 8)
                .arg(alt, 0, 'f', 8));
        }
        else
        {
            addKmlTextElement(kmlGeometry, "coordinates", QString("%1,%2 ")
                .arg(lng, 0, 'f', 8)
                .arg(lat, 0, 'f', 8));
        }

        if (m_altitudeMode == 2 )
        {
            addKmlTextElement(kmlGeometry, "altitudeMode", "absolute");
        }
        else if (m_altitudeMode == 1 )
        {
            addKmlTextElement(kmlGeometry, "altitudeMode", "relativeToGround");
        }
        else
        {
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
          * - a DSC: the right time is the DateTimeDigitized which is also DateTimeOriginal
          *          if the picture has been modified the (standard)DateTime should change.
          * - a scanned picture, the right time is the DateTimeOriginal which should also be the DateTime
          *          the (standard)DateTime should be the same except if the picture is modified
          * - a panorama created from several pictures, the right time is the DateTimeOriginal (average of DateTimeOriginal actually)
          *          The (standard)DateTime is the creation date of the panorama.
          * it's seems the time to take into acccount is the DateTimeOriginal.
          * but the KPMetadata::getImageDateTime() return the (standard)DateTime first
          * KPMetadata seems to take Original dateTime first so it shoul be alright now.
          */
        QDateTime datetime = meta.getImageDateTime();

        if (datetime.isValid())
        {
            QDomElement kmlTimeStamp = addKmlElement(kmlPlacemark, "TimeStamp");
            addKmlTextElement(kmlTimeStamp, "when", datetime.toString("yyyy-MM-ddThh:mm:ssZ"));
        }
        else if (m_interface->hasFeature(ImagesHasTime))
        {
            QDomElement kmlTimeStamp = addKmlElement(kmlPlacemark, "TimeStamp");
            addKmlTextElement(kmlTimeStamp, "when", (info.date()).toString("yyyy-MM-ddThh:mm:ssZ"));
        }

        QString my_description;

        if (m_optimize_googlemap)
        {
            my_description = "<img src=\"" + m_UrlDestDir + m_imageDir + fullFileName + "\">";
        }
        else
        {
            my_description = "<img src=\"" + m_imageDir + fullFileName + "\">";
        }

        if ( m_interface->hasFeature( ImagesHasComments ) )
        {
            my_description += "<br/>" + info.description() ;
        }

        addKmlTextElement(kmlPlacemark, "description", my_description);
        logInfo(i18n("Creation of placemark '%1'", fullFileName));

        // Save icon
        QString iconFileName = "thumb_" + baseFileName + '.' + imageFormat.toLower();
        QString destPath     = m_tempDestDir + m_imageDir + iconFileName;

        if (!icon.save(destPath, imageFormat.toAscii(), 85))
        {
            logWarning(i18n("Could not save icon for image '%1' to '%2'",path,destPath));
        }
        else
        {
            //logInfo(i18n("Creation of icon '%1'").arg(iconFileName));
            // style et icon
            QDomElement kmlStyle     = addKmlElement(kmlPlacemark, "Style");
            QDomElement kmlIconStyle = addKmlElement(kmlStyle,     "IconStyle");
            QDomElement kmlIcon      = addKmlElement(kmlIconStyle, "Icon");

            if (m_optimize_googlemap)
            {
                addKmlTextElement(kmlIcon, "href", m_UrlDestDir + m_imageDir + iconFileName);
            }
            else
            {
                addKmlTextElement(kmlIcon, "href", m_imageDir + iconFileName);
            }

            QDomElement kmlBallonStyle = addKmlElement(kmlStyle, "BalloonStyle");
            addKmlTextElement(kmlBallonStyle, "text", "$[description]");
        }
    }
}

/*!
\fn KmlExport::addTrack(QDomElement &kmlAlbum)
 */
void KmlExport::addTrack(QDomElement& kmlAlbum)
{
    if( m_GPXFile.isEmpty() )
    {
        logWarning(i18n("No GPX file chosen."));
        return;
    }

    m_gpxParser.clear();
    bool ret = m_gpxParser.loadGPXFile(m_GPXFile);

    if (!ret)
    {
        logError(i18n("Cannot parse %1 GPX file.",m_GPXFile));
        return;
    }

    if (m_gpxParser.numPoints() <= 0)
    {
        logError(i18n("The %1 GPX file do not have a date-time track to use.",
                      m_GPXFile));
        return;
    }

    // create a folder that will contain tracks and points
    QDomElement kmlFolder = addKmlElement(kmlAlbum, "Folder");
    addKmlTextElement(kmlFolder, "name", i18n("Tracks"));

    if (!m_optimize_googlemap)
    {
        // style of points and track
        QDomElement kmlTrackStyle = addKmlElement(kmlAlbum, "Style");
        kmlTrackStyle.setAttribute("id","track");
        QDomElement kmlIconStyle  = addKmlElement(kmlTrackStyle, "IconStyle");
        QDomElement kmlIcon       = addKmlElement(kmlIconStyle, "Icon");
        //! FIXME is there a way to be sure of the location of the icon?
        addKmlTextElement(kmlIcon, "href", "http://maps.google.com/mapfiles/kml/pal4/icon60.png");

        m_gpxParser.CreateTrackPoints(kmlFolder, *m_kmlDocument, m_TimeZone - 12, m_GPXAltitudeMode);
    }

    // linetrack style
    QDomElement kmlLineTrackStyle = addKmlElement(kmlAlbum, "Style");
    kmlLineTrackStyle.setAttribute("id","linetrack");
    QDomElement kmlLineStyle      = addKmlElement(kmlLineTrackStyle, "LineStyle");

    // the KML color is not #RRGGBB but AABBGGRR
    QString KMLColorValue = QString("%1%2%3%4")
        .arg((int)m_GPXOpacity*256/100, 2, 16)
        .arg((&m_GPXColor)->blue(), 2, 16)
        .arg((&m_GPXColor)->green(), 2, 16)
        .arg((&m_GPXColor)->red(), 2, 16);
    addKmlTextElement(kmlLineStyle, "color", KMLColorValue);
    addKmlTextElement(kmlLineStyle, "width", QString("%1").arg(m_LineWidth) );

    m_gpxParser.CreateTrackLine(kmlAlbum, *m_kmlDocument, m_GPXAltitudeMode);
}

/*!
    \fn KmlExport::generate()
 */
void KmlExport::generate()
{
    //! @todo perform a test here before continuing.
    createDir(QString(m_tempDestDir + m_imageDir));

    m_progressDialog->show();
    ImageCollection selection = m_interface->currentSelection();
    ImageCollection album     = m_interface->currentAlbum();

    // create the document, and it's root
    m_kmlDocument                   = new QDomDocument("");
    QDomImplementation impl;
    QDomProcessingInstruction instr = m_kmlDocument->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    m_kmlDocument->appendChild(instr);
    QDomElement kmlRoot             = m_kmlDocument->createElementNS("http://www.opengis.net/kml/2.2", "kml");
    m_kmlDocument->appendChild( kmlRoot );

    QDomElement kmlAlbum            = addKmlElement(kmlRoot, "Document");
    QDomElement kmlName             = addKmlTextElement(kmlAlbum, "name", album.name());
    QDomElement kmlDescription      = addKmlHtmlElement(kmlAlbum, "description",
                                                        "Created with kmlexport <a href=\"http://www.digikam.org/\">kipi-plugin</a>");

    if (m_GPXtracks)
    {
        addTrack(kmlAlbum);
    }

    KPMetadata meta;
    KUrl::List images = selection.images();
    int defectImage   = 0;
    int pos           = 1;
    int count         = images.count();
    KUrl::List::ConstIterator imagesEnd (images.constEnd());

    for( KUrl::List::ConstIterator selIt = images.constBegin(); selIt != imagesEnd; ++selIt, ++pos)
    {
        double alt, lat, lng;
        KUrl url        = *selIt;
        KPImageInfo info(url);
        bool hasGPSInfo = info.hasGeolocationInfo();

        if (hasGPSInfo)
        {
            lat = info.latitude();
            lng = info.longitude();
            alt = info.altitude();
        }
        else
        {
            meta.load(url.path());
            hasGPSInfo = meta.getGPSInfo(alt, lat, lng);
        }

        if ( hasGPSInfo )
        {
            // generation de l'image et de l'icone
            generateImagesthumb(url, kmlAlbum);
        }
        else
        {
            logWarning(i18n("No position data for '%1'", info.name()));
            defectImage++;
        }

        m_progressDialog->progressWidget()->setProgress(pos, count);
        kapp->processEvents();
    }

    if (defectImage)
    {
        /** @todo if defectImage==count there are no pictures exported, does is it worth to continue? */
        KMessageBox::information(kapp->activeWindow(),
                                 i18np("No position data for 1 picture",
                                       "No position data for %1 pictures", defectImage));
    }

    /** @todo change to kml or kmz if compressed */
    QFile file( m_tempDestDir + m_KMLFileName + ".kml");
    /** @todo handle file opening problems */
    file.open( QIODevice::WriteOnly );
    QTextStream stream( &file ); // we will serialize the data into the file
    stream << m_kmlDocument->toString();
    file.close();

    delete m_kmlDocument;
    m_kmlDocument = 0;

    KIO::moveAs(m_tempDestDir, m_baseDestDir, KIO::HideProgressInfo | KIO::Overwrite);
    logInfo(i18n("Move to final directory"));
    m_progressDialog->close();
}

/*!
    \fn KmlExport::getConfig()
 */
int KmlExport::getConfig()
{
    KConfig config("kipirc");
    KConfigGroup group   = config.group("KMLExport Settings");

    m_localTarget        = group.readEntry("localTarget", true);
    m_optimize_googlemap = group.readEntry("optimize_googlemap", false);
    m_iconSize           = group.readEntry("iconSize", 33);
    //    googlemapSize    = group.readNumEntry("googlemapSize");
    m_size               = group.readEntry("size", 320);

    // UrlDestDir have to have the trailing
    m_baseDestDir        = group.readEntry("baseDestDir", QString("/tmp/"));
    m_UrlDestDir         = group.readEntry("UrlDestDir", QString("http://www.example.com/"));
    m_KMLFileName        = group.readEntry("KMLFileName", QString("kmldocument"));
    m_altitudeMode       = group.readEntry("Altitude Mode", 0);

    m_GPXtracks          = group.readEntry("UseGPXTracks", false);
    m_GPXFile            = group.readEntry("GPXFile", QString());
    m_TimeZone           = group.readEntry("Time Zone", 12);
    m_LineWidth          = group.readEntry("Line Width", 4);
    m_GPXColor           = group.readEntry("Track Color", "#17eeee" );
    m_GPXOpacity         = group.readEntry("Track Opacity", 64 );
    m_GPXAltitudeMode    = group.readEntry("GPX Altitude Mode", 0);

    KStandardDirs dir;
    m_tempDestDir        = dir.saveLocation("tmp", "kipi-kmlrexportplugin-" + QString::number(getpid()) + '/');
    m_imageDir           = "images/";
    m_googlemapSize      = 32;
    return 1;
}

void KmlExport::logInfo(const QString& msg) const
{
    m_progressDialog->progressWidget()->addedAction(msg, ProgressMessage);
}

void KmlExport::logError(const QString& msg) const
{
    m_progressDialog->progressWidget()->addedAction(msg, ErrorMessage);
}

void KmlExport::logWarning(const QString& msg) const
{
    m_progressDialog->progressWidget()->addedAction(msg, WarningMessage);
    // mWarnings=true;
}

} // namespace KIPIKMLExportPlugin
