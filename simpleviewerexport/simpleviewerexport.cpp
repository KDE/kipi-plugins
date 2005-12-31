/* ============================================================
 * File  : simpleviewerexport.cpp
 * Author: Joern Ahrens <joern.ahrens@kdemail.net>
 * Date  : 2005-12-19
 * Description :
 *
 * Copyright 2005 by Joern Ahrens
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// QT includes

#include <qtimer.h>
#include <qimage.h>
#include <qtextstream.h>
#include <qfile.h>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kio/netaccess.h>
#include <kfilemetainfo.h>

// KIPI includes

#include <libkipi/version.h>
#include <libkipi/batchprogressdialog.h>
#include <libkipi/imageinfo.h>

// Local includes

#include "simpleviewerexport.h"
#include "svedialog.h"

namespace KIPISimpleViewerExportPlugin
{
    
// maxium size of a simpleviewer thumbnail
const int maxThumbSize = 45;
    
void SimpleViewerExport::run(KIPI::Interface* interface, QObject *parent)
{
    SimpleViewerExport *plugin = new SimpleViewerExport(interface, parent);
    plugin->showDialog();
    plugin->startExport();
    
    delete plugin;
}
    
SimpleViewerExport::SimpleViewerExport(KIPI::Interface* interface, QObject *parent)
    : QObject(parent)
{
    m_interface = interface;
    m_configDlg = 0;
    m_canceled = true;    
}

SimpleViewerExport::~SimpleViewerExport()
{

}

void SimpleViewerExport::showDialog()
{
    m_canceled = true;
    
    if(!m_configDlg)
        m_configDlg = new SVEDialog(m_interface, kapp->activeWindow());
    
    if(m_configDlg->exec() == QDialog::Rejected)
        return;

    m_canceled = false;
}

void SimpleViewerExport::startExport()
{
    if(m_canceled)
        return;
    
    m_progressDlg = new KIPI::BatchProgressDialog(kapp->activeWindow(),
                                                  i18n("Simple Viewer Export"));
        
    connect(m_progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    m_progressDlg->show();

    // Estimate the number of actions for the KIPI progress dialog.
    m_progressDlg->addedAction(i18n("Estimate the number of actions to do..."), KIPI::StartingMessage);
    m_albumsList = m_configDlg->getSelectedAlbums();
    m_totalActions = 0;
    for( QValueList<KIPI::ImageCollection>::Iterator it = m_albumsList.begin() ;
         !m_canceled && (it != m_albumsList.end()) ; ++it )
    {
        m_totalActions += (*it).images().count();
    }
    
    m_progressDlg->setProgress(0, m_totalActions);

    slotProcess();
}

void SimpleViewerExport::slotCancel()
{
    m_canceled = true;
}

void SimpleViewerExport::slotProcess()
{
    m_progressDlg->addedAction(i18n("Initialising..."), KIPI::StartingMessage);
    if(!createExportDirectories())
    {
        m_progressDlg->addedAction(i18n("Failed to create export directories"),
                                   KIPI::ErrorMessage);
        return;
    }

    m_progressDlg->addedAction(i18n("Creating images and thumbnails..."), KIPI::StartingMessage);
    if(!exportImages())
    {
        m_progressDlg->addedAction(i18n("Failed to export the images"),
                                   KIPI::ErrorMessage);
        return;
    }
    m_progressDlg->addedAction(i18n("Finished creating images and thumbnails..."), KIPI::SuccessMessage);
}

bool SimpleViewerExport::createExportDirectories()
{
    KURL root = m_configDlg->exportURL();

    if(!KIO::NetAccess::mkdir(root, kapp->activeWindow()))
    {
        m_progressDlg->addedAction(i18n("Could not create folder '%1'").arg(root.url()),
                                   KIPI::ErrorMessage);
        return(false);
    }
    
    KURL thumbsDir = root;
    thumbsDir.addPath("/thumbs");
    if(!KIO::NetAccess::mkdir(thumbsDir, kapp->activeWindow()))
    {
        m_progressDlg->addedAction(i18n("Could not create folder '%1'").arg(thumbsDir.url()),
                                   KIPI::ErrorMessage);
        return(false);
    }

    KURL imagesDir = root;
    imagesDir.addPath("/images");
    if(!KIO::NetAccess::mkdir(imagesDir, kapp->activeWindow()))
    {
        m_progressDlg->addedAction(i18n("Could not create folder '%1'").arg(imagesDir.url()),
                                   KIPI::ErrorMessage);
        return(false);
    }
    
    return true;
}

bool SimpleViewerExport::exportImages()
{
    // TODO:
    KURL thumbsDir = m_configDlg->exportURL();
    thumbsDir.addPath("/thumbs");
    KURL imagesDir = m_configDlg->exportURL();
    imagesDir.addPath("/images");

    KURL xmlFile = m_configDlg->exportURL();
    xmlFile.addPath("/imageData.xml");
    QFile file( xmlFile.path() );

    file.open(IO_WriteOnly);
    QTextStream ts(&file);
    cfgCreateHeader(ts);

    int action = 0;
    int maxSize = m_configDlg->imagesExportSize();
    bool resizeImages = m_configDlg->resizeExportImages();
    for( QValueList<KIPI::ImageCollection>::Iterator it = m_albumsList.begin() ;
         !m_canceled && (it != m_albumsList.end()) ; ++it )
    {
        KURL::List images = (*it).images();
        for(KURL::List::Iterator it = images.begin();
            !m_canceled && (it != images.end()) ; ++it)
        {
            kapp->processEvents();

            KURL kurl = *it;
            m_progressDlg->addedAction(i18n("Processing %1").arg((*it).url()),
                                       KIPI::StartingMessage);
            QImage image;
            if(!image.load(kurl.path()))
            {
                m_progressDlg->addedAction(i18n("Could not open image '%1'").arg(kurl.path()),
                                           KIPI::WarningMessage);
                continue;
            }

            QImage thumbnail;
            if(!createThumbnail(image, thumbnail))
            {
                m_progressDlg->addedAction(i18n("Could not create thumbnail from '%1'").arg(kurl.path()),
                                           KIPI::WarningMessage);
                continue;
            }

            if(resizeImages && !resizeImage(image, maxSize, image))
            {
                m_progressDlg->addedAction(i18n("Could not resize image '%1'").arg(kurl.path()),
                                           KIPI::WarningMessage);
                continue;
            }

            // TODO:
            KURL thumbnailPath = thumbsDir;
            thumbnailPath.addPath(kurl.filename());
            kdDebug() << k_funcinfo << thumbnailPath.path() << endl;
            thumbnail.save(thumbnailPath.path(), "JPEG");

            KURL imagePath = imagesDir;
            imagePath.addPath(kurl.filename());
            kdDebug() << k_funcinfo << imagePath.path() << endl;
            image.save(imagePath.path(), "JPEG");

            cfgAddImage(ts, kurl);
            m_progressDlg->setProgress(++action, m_totalActions);
        }
        cfgCreateFooter(ts);
    }

    return true;
}

bool SimpleViewerExport::createThumbnail(const QImage &image, QImage &thumbnail)
{
    int w = image.width();
    int h = image.height();

    int maxSize;

    if(w > maxThumbSize || h > maxThumbSize)
    {
        if(w > h)
        {
            maxSize = (int)(double)(w * maxThumbSize) / h;
        }
        else
        {
            maxSize = (int)(double)(h * maxThumbSize) / w;
        }
    }
    
    maxSize = (maxSize < maxThumbSize) ? maxThumbSize : maxSize;
    
    return resizeImage(image, maxSize, thumbnail);
}

bool SimpleViewerExport::resizeImage(const QImage &image, int maxSize, QImage &resizedImage)
{
    int w = image.width();
    int h = image.height();

    if(w > maxSize || h > maxSize)
    {
        if(w > h)
        {
            h = (int)(double)(h * maxSize) / w;
            h = (h == 0) ? 1 : h;
            w = maxSize;
        }
        else
        {
            w = (int)(double)(w * maxSize) / h;
            w = (w == 0) ? 1 : w;
            h = maxSize;
        }
        resizedImage = image.smoothScale(w, h);
    }

    return true;
}

void SimpleViewerExport::cfgCreateHeader(QTextStream &ts)
{
    ts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    ts << "<SIMPLEVIEWER_DATA"
       << " maxImageDimension=\"" << m_configDlg->maxImageDimension() << "\""
       << " textColor=\"" << m_configDlg->textColor().name().replace("#", "0x") << "\""
       << " frameColor=\"" << m_configDlg->frameColor().name().replace("#", "0x") << "\""
       << " bgColor=\"" << m_configDlg->backgroundColor().name().replace("#", "0x") << "\""
       << " frameWidth=\"" << m_configDlg->frameWidth() << "\""
       << " stagePadding=\"" << m_configDlg->stagePadding() << "\""
       << " thumbnailColumns=\"" << m_configDlg->thumbnailColumns() << "\""
       << " thumbnailRows=\"" << m_configDlg->thumbnailRows() << "\""
       << " navPosition=\"" << m_configDlg->navPosition() << "\""
       << " navDirection=\"" << m_configDlg->navDirection() << "\""
       << " title=\"" << m_configDlg->title() << "\""
       << " imagePath=\"\" thumbPath=\"\">" << endl;
}

void SimpleViewerExport::cfgAddImage(QTextStream &ts, const KURL &kurl)
{
    QString comment;

    if(m_configDlg->showExifComments())
    {
        KIPI::ImageInfo info = m_interface->info(kurl);
        comment = info.description();
    }
    else
    {
        comment = "";
    }

    ts << "<IMAGE>" << endl;
    ts << "<NAME>" << kurl.filename() << "</NAME>" << endl;
    ts << "<CAPTION>" <<  comment  <<  "</CAPTION>" << endl;
    ts << "</IMAGE>" << endl;
}

void SimpleViewerExport::cfgCreateFooter(QTextStream &ts)
{
    ts << "</SIMPLEVIEWER_DATA>" << endl;
}

} // namespace KIPISimpleViewerExportPlugin

#include "simpleviewerexport.moc"
