/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
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
#include <qfile.h>
#include <qdir.h>
#include <qcstring.h>
#include <qdatastream.h>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kio/netaccess.h>
#include <kfilemetainfo.h>
#include <kstandarddirs.h>
#include <kio/job.h>
#include <kzip.h>
#include <kmessagebox.h>
#include <kaboutdata.h>
#include <ktempdir.h>

// KIPI includes

#include <libkipi/version.h>
#include <libkipi/batchprogressdialog.h>
#include <libkipi/imageinfo.h>

// LibKDcraw includes.

#include <libkdcraw/dcrawbinary.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "firstrundlg.h"
#include "svedialog.h"
#include "simpleviewerexport.h"
#include "simpleviewerexport.moc"

namespace KIPISimpleViewerExportPlugin
{

// maxium size of a simpleviewer thumbnail
// TODO: read from configfile
const int maxThumbSize = 45;
const QString viewer("viewer.swf");

void SimpleViewerExport::run(KIPI::Interface* interface, QObject *parent)
{
    SimpleViewerExport *plugin = new SimpleViewerExport(interface, parent);

    if(!plugin->checkSimpleViewer())
    {
        if(!plugin->installSimpleViewer())
        {
            KMessageBox::error(kapp->activeWindow(), i18n("SimpleViewer installation failed"));
            return;
        }
    }

    if(plugin->configure())
        plugin->startExport();

    delete plugin;
}

SimpleViewerExport::SimpleViewerExport(KIPI::Interface* interface, QObject *parent)
                  : QObject(parent)
{
    m_interface    = interface;
    m_configDlg    = 0;
    m_totalActions = 0;
    m_action       = 0;
    m_canceled     = true;
    m_dataLocal    = locateLocal("data", "kipiplugin_simpleviewerexport/simpleviewer/", true);
    m_tempDir      = 0;

    m_simpleViewerFiles.append(viewer);
    m_simpleViewerFiles.append("swfobject.js");

    const KAboutData *data = KApplication::kApplication()->aboutData();
    m_hostName             = QString::QString( data->appName() );
    m_hostURL              = data->homepage();

    if (m_hostURL.isEmpty())
    {
        m_hostName = "Kipi";
        m_hostURL  = "http://www.kipi-plugins.org";
    }
}

SimpleViewerExport::~SimpleViewerExport()
{
    if(m_tempDir)
        delete m_tempDir;
}

bool SimpleViewerExport::configure()
{
    m_canceled = false;

    if(!m_configDlg)
        m_configDlg = new SVEDialog(m_interface, kapp->activeWindow());

    bool configured = false;
    while(!configured)
    {
        if(m_configDlg->exec() == QDialog::Rejected)
            return false;

        configured = true;

        if(KIO::NetAccess::exists(m_configDlg->exportURL(), false, kapp->activeWindow()))
        {
            int ret = KMessageBox::warningYesNoCancel(kapp->activeWindow(),
                                                      i18n("Target folder %1 already exists.\n"
                                                           "Do you want to overwrite it (all data in this folder will be lost)")
                                                           .arg(m_configDlg->exportURL()));

            switch(ret)
            {
                case KMessageBox::Yes:
                    if(!KIO::NetAccess::del(m_configDlg->exportURL(), kapp->activeWindow()))
                    {
                        KMessageBox::error(kapp->activeWindow(), i18n("Could not delete %1\n"
                                "Please choose another export folder").arg(m_configDlg->exportURL()));
                        configured = false;
                    }
                    break;

                case KMessageBox::No:
                    configured = false;
                    break;

                case KMessageBox::Cancel:
                    return false;
                    break;
            };
        }
    }

    return true;
}

void SimpleViewerExport::startExport()
{
    if(m_canceled)
        return;

    m_progressDlg = new KIPI::BatchProgressDialog(kapp->activeWindow(), i18n("Flash Export"));

    connect(m_progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    m_progressDlg->show();
    kapp->processEvents();

    // Estimate the number of actions for the KIPI progress dialog.
    m_progressDlg->addedAction(i18n("Estimate the number of actions to do..."), KIPI::StartingMessage);
    m_albumsList = m_configDlg->getSelectedAlbums();
    m_totalActions = 0;
    for( QValueList<KIPI::ImageCollection>::Iterator it = m_albumsList.begin() ;
         !m_canceled && (it != m_albumsList.end()) ; ++it )
    {
        m_totalActions += (*it).images().count();
    }

    // +copying SimpleViewer, +creating index.html
    m_totalActions += 2;

    m_progressDlg->setProgress(0, m_totalActions);

    slotProcess();

#if KDE_VERSION >= 0x30200
        m_progressDlg->setButtonCancel(KStdGuiItem::close());
#else
        m_progressDlg->setButtonCancelText(i18n("&Close"));
#endif

}

void SimpleViewerExport::slotCancel()
{
    m_progressDlg->addedAction(i18n("Export canceled"), KIPI::ErrorMessage);
    m_canceled = true;
}

void SimpleViewerExport::slotProcess()
{
    if(m_canceled)
        return;

    m_progressDlg->addedAction(i18n("Initialising..."), KIPI::StartingMessage);

    if(!m_canceled && !createExportDirectories())
    {
            m_progressDlg->addedAction(i18n("Failed to create export directories"),
                                       KIPI::ErrorMessage);
        return;
    }

    if(!m_canceled && !exportImages())
    {
        m_progressDlg->addedAction(i18n("Failed to export the images"),
                                   KIPI::ErrorMessage);
        return;
    }

    if(!m_canceled && !createIndex())
    {
        m_progressDlg->addedAction(i18n("Failed to create index.html"),
                                   KIPI::ErrorMessage);
        return;
    }

    if(!m_canceled && !copySimpleViewer())
    {
        m_progressDlg->addedAction(i18n("Failed to copy SimpleViewer files"),
                                   KIPI::ErrorMessage);
        return;
    }

    if(!m_canceled && !upload())
    {
        m_progressDlg->addedAction(i18n("Failed to upload the gallery"),
                                   KIPI::ErrorMessage);
        return;
    }

    if(m_canceled)
    {
        int ret = KMessageBox::warningYesNo(kapp->activeWindow(),
                                            i18n("Export was canceled.\n"
                                            "Do you want to delete the yet created files in %1 ?")
                                            .arg(m_configDlg->exportURL()));
        if(ret == KMessageBox::Yes)
        {
            KIO::NetAccess::del(m_configDlg->exportURL(), kapp->activeWindow());
        }
    }

    if(!m_canceled)
        m_progressDlg->addedAction(i18n("Finished..."), KIPI::SuccessMessage);
}

bool SimpleViewerExport::createExportDirectories()
{
    m_tempDir = new KTempDir(locateLocal("tmp", "simpleviewerexport"));
    m_tempDir->setAutoDelete(true);

    m_progressDlg->addedAction(i18n("Creating directories..."), KIPI::StartingMessage);

    KURL root = m_configDlg->exportURL();
    if(!KIO::NetAccess::mkdir(root, kapp->activeWindow()))
    {
        m_progressDlg->addedAction(i18n("Could not create folder '%1'").arg(root.url()),
                                   KIPI::ErrorMessage);
        return(false);
    }

    KURL thumbsDir = m_tempDir->name();
    thumbsDir.addPath("/thumbs");
    if(!KIO::NetAccess::mkdir(thumbsDir, kapp->activeWindow()))
    {
        m_progressDlg->addedAction(i18n("Could not create folder '%1'").arg(thumbsDir.url()),
                                   KIPI::ErrorMessage);
        return(false);
    }

    KURL imagesDir = m_tempDir->name();
    imagesDir.addPath("/images");
    if(!KIO::NetAccess::mkdir(imagesDir, kapp->activeWindow()))
    {
        m_progressDlg->addedAction(i18n("Could not create folder '%1'").arg(imagesDir.url()),
                                   KIPI::ErrorMessage);
        return(false);
    }

    m_progressDlg->setProgress(++m_action, m_totalActions);
    m_progressDlg->addedAction(i18n("Directories created..."), KIPI::SuccessMessage);

    return true;
}

bool SimpleViewerExport::exportImages()
{
    if(m_canceled)
        return false;

    m_progressDlg->addedAction(i18n("Creating images and thumbnails..."), KIPI::StartingMessage);

    KURL thumbsDir(m_tempDir->name());
    thumbsDir.addPath("/thumbs");

    KURL imagesDir(m_tempDir->name());
    imagesDir.addPath("/images");

    KURL xmlFile(m_tempDir->name());
    xmlFile.addPath("/gallery.xml");
    QFile file(xmlFile.path());
    file.open(IO_WriteOnly);

    QDomDocument xmlDoc;
    xmlDoc.appendChild(xmlDoc.createProcessingInstruction( QString::fromLatin1("xml"),
                       QString::fromLatin1("version=\"1.0\" encoding=\"UTF-8\"") ) );
    QDomElement galleryElem = xmlDoc.createElement(QString::fromLatin1("simpleviewerGallery")); 
    xmlDoc.appendChild( galleryElem );
    galleryElem.setAttribute(QString::fromLatin1("maxImageWidth"),    m_configDlg->maxImageDimension());
    galleryElem.setAttribute(QString::fromLatin1("maxImageHeight"),   m_configDlg->maxImageDimension());
    galleryElem.setAttribute(QString::fromLatin1("textColor"),        m_configDlg->textColor().name().replace("#", "0x"));
    galleryElem.setAttribute(QString::fromLatin1("frameColor"),       m_configDlg->frameColor().name().replace("#", "0x"));
    galleryElem.setAttribute(QString::fromLatin1("bgColor"),          m_configDlg->backgroundColor().name().replace("#", "0x"));
    galleryElem.setAttribute(QString::fromLatin1("frameWidth"),       m_configDlg->frameWidth());
    galleryElem.setAttribute(QString::fromLatin1("stagePadding"),     m_configDlg->stagePadding());
    galleryElem.setAttribute(QString::fromLatin1("thumbnailColumns"), m_configDlg->thumbnailColumns());
    galleryElem.setAttribute(QString::fromLatin1("thumbnailRows"),    m_configDlg->thumbnailRows());
    galleryElem.setAttribute(QString::fromLatin1("navPosition"),      m_configDlg->navPosition());
    galleryElem.setAttribute(QString::fromLatin1("navDirection"),     m_configDlg->navDirection());
    galleryElem.setAttribute(QString::fromLatin1("title"),            m_configDlg->title());
    galleryElem.setAttribute(QString::fromLatin1("imagePath"),        QString());
    galleryElem.setAttribute(QString::fromLatin1("thumbPath"),        QString());

    int maxSize       = m_configDlg->imagesExportSize();
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
            m_progressDlg->addedAction(i18n("Processing %1").arg((*it).filename()),
                                       KIPI::StartingMessage);
            QImage image;

            // Check if RAW file.
            QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
            QFileInfo fileInfo(kurl.path());
            if (rawFilesExt.upper().contains( fileInfo.extension(false).upper() ))
                KDcrawIface::KDcraw::loadDcrawPreview(image, kurl.path());
            else
                image.load(kurl.path());

            if(image.isNull())
            {
                m_progressDlg->addedAction(i18n("Could not open image '%1'").arg(kurl.filename()),
                                           KIPI::WarningMessage);
                continue;
            }

            QImage thumbnail;
            if(!createThumbnail(image, thumbnail))
            {
                m_progressDlg->addedAction(i18n("Could not create thumbnail from '%1'").arg(kurl.filename()),
                                           KIPI::WarningMessage);
                continue;
            }

            if(resizeImages && !resizeImage(image, maxSize, image))
            {
                m_progressDlg->addedAction(i18n("Could not resize image '%1'").arg(kurl.filename()),
                                           KIPI::WarningMessage);
                continue;
            }

            KURL thumbnailPath(thumbsDir);
            thumbnailPath.addPath(kurl.filename());
            thumbnail.save(thumbnailPath.path(), "JPEG");

            KURL imagePath(imagesDir);
            imagePath.addPath(kurl.filename());
            image.save(imagePath.path(), "JPEG");

            cfgAddImage(xmlDoc, galleryElem, kurl);
            m_progressDlg->setProgress(++m_action, m_totalActions);
        }
    }

    QCString data(xmlDoc.toCString());
    QDataStream stream( &file );
    stream.writeRawBytes(data.data(), data.size());
    file.close();

    m_progressDlg->addedAction(i18n("Images and thumbnails created..."), KIPI::SuccessMessage);

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

void SimpleViewerExport::cfgAddImage(QDomDocument &xmlDoc, QDomElement &galleryElem, const KURL &kurl)
{
    if(m_canceled)
        return;

    QString comment;

    if(m_configDlg->showExifComments())
    {
        KIPI::ImageInfo info = m_interface->info(kurl);
        comment = info.description();
    }
    else
    {
        comment = QString();
    }

    QDomElement img = xmlDoc.createElement(QString::fromLatin1("image")); 
    galleryElem.appendChild(img);

    QDomElement name = xmlDoc.createElement(QString::fromLatin1("name")); 
    img.appendChild(name);
    QDomText nametxt = xmlDoc.createTextNode(kurl.fileName());
    name.appendChild(nametxt);

    QDomElement caption = xmlDoc.createElement(QString::fromLatin1("caption")); 
    img.appendChild(caption);
    QDomText captiontxt = xmlDoc.createTextNode(comment);
    caption.appendChild(captiontxt);
}

bool SimpleViewerExport::createIndex()
{
    if(m_canceled)
        return false;

    m_progressDlg->addedAction(i18n("Creating index.html..."), KIPI::StartingMessage);

    QString indexTemplateName = locate("data", "kipiplugin_simpleviewerexport/index.template");
    if(indexTemplateName.isEmpty())
    {
        //TODO: errormsg
        kdDebug() << "No indexTemplateName" << endl;
        return false;
    }

    QFile infile(indexTemplateName);
    infile.open(IO_ReadOnly);
    QTextStream in(&infile);
    QString indexTemplate = in.read();
    infile.close();

    indexTemplate.replace("{TITLE}", m_configDlg->title());
    indexTemplate.replace("{COLOR}", m_configDlg->textColor().name());
    indexTemplate.replace("{BGCOLOR}", m_configDlg->backgroundColor().name());
    indexTemplate.replace("{HOSTURL}", m_hostURL);
    indexTemplate.replace("{HOSTNAME}", m_hostName);

    QFile outfile(m_tempDir->name() + "/index.html");
    outfile.open(IO_WriteOnly);
    QTextStream out(&outfile);
    out << indexTemplate;
    outfile.close();

    m_progressDlg->setProgress(++m_action, m_totalActions);
    m_progressDlg->addedAction(i18n("index.html created..."), KIPI::SuccessMessage);

    return true;
}

bool SimpleViewerExport::copySimpleViewer()
{
    if(m_canceled)
        return false;

    m_progressDlg->addedAction(i18n("Copying flash files..."), KIPI::StartingMessage);

    QString dataDir;

    // Due to its license, simpleviewer is installed in $KDEHOME
    dataDir = locate("data", "kipiplugin_simpleviewerexport/simpleviewer/");
    if(dataDir.isEmpty())
        installSimpleViewer();
    if(dataDir.isEmpty())
        return false;

    QStringList files;
    QStringList entries;
    QDir dir;

    dir.setPath(dataDir);
    entries = dir.entryList(QDir::Files);
    for(QStringList::Iterator it = entries.begin(); it != entries.end(); ++it) 
    {
        files.append(dir.absPath() + "/" + *it);
    }

    // files distributed with the plugin are installed in $KDEDIRS
    dataDir = locate("data", "kipiplugin_simpleviewerexport/simpleviewer_html/");
    dir.setPath(dataDir);
    entries = dir.entryList(QDir::Files);
    for(QStringList::Iterator it = entries.begin(); it != entries.end(); ++it) 
    {
        files.append(dir.absPath() + "/" + *it);
    }
    // TODO: catch errors
    KIO::CopyJob *copyJob = KIO::copy(files, m_configDlg->exportURL(), true);

    m_progressDlg->addedAction(i18n("flash files copied..."), KIPI::SuccessMessage);

    return true;
}

bool SimpleViewerExport::upload()
{
    if(m_canceled)
        return false;

    m_progressDlg->addedAction(i18n("Uploading gallery..."), KIPI::StartingMessage);

    if(!KIO::NetAccess::dircopy(m_tempDir->name() + "./", m_configDlg->exportURL(), 0))
        return false;

    m_progressDlg->addedAction(i18n("Gallery uploaded..."), KIPI::SuccessMessage);

    return true;
}

bool SimpleViewerExport::checkSimpleViewer() const
{
    return ! locate("data", "kipiplugin_simpleviewerexport/simpleviewer/"+viewer).isEmpty();
}

bool SimpleViewerExport::installSimpleViewer()
{
    FirstRunDlg *firstRunDlg = new FirstRunDlg(kapp->activeWindow());
    if(firstRunDlg->exec() == QDialog::Accepted)
    {
        QString url = firstRunDlg->getURL();
        delete firstRunDlg;

        if(unzip(url))
        {
            return true;
        }
        else
        {
            // ErrorMessage
        }
    }

    return false;
}

bool SimpleViewerExport::unzip(const QString &url)
{
    KZip zip(url);

    if(!openArchive(zip))
    {
        return false;
    }

    return extractArchive(zip);
}

bool SimpleViewerExport::openArchive(KZip &zip)
{
    if(!zip.open(IO_ReadOnly))
    {
        kdDebug() << "open archive failed\n";
        return false;
    }
    return true;
}

bool SimpleViewerExport::extractArchive(KZip &zip)
{
    // read root directory content
    QStringList names = zip.directory()->entries();
    if(names.count() != 1)
    {
        kdDebug() << "Wrong SimpleViewer Version or corrupted archive" << endl;
        kdDebug() << "Content of the archive root folder" << names << endl;
        return false;
    }

    // open root directory
    const KArchiveEntry *root = zip.directory()->entry(names[0]);
    if(!root || !root->isDirectory())
    {
        kdDebug() << "could not open " << names[0] << " of zipname" << endl;
        return false;
    }

    const KArchiveDirectory *dir = dynamic_cast<const KArchiveDirectory*>(root);

    // extract the needed files from SimpleViewer archive
    for(QStringList::Iterator it = m_simpleViewerFiles.begin(); 
        it != m_simpleViewerFiles.end(); ++it ) 
    {
        const KArchiveEntry *entry = dir->entry(*it);
        if(!extractFile(entry))
        {
            //TODO error msg
            kdDebug() << "could not open " << *it << " of zipname" << endl;
            return false;
        }
    }

    return true;
}

bool SimpleViewerExport::extractFile(const KArchiveEntry *entry)
{
    if( !entry || !entry->isFile() ) 
        return false;

    const KArchiveFile *entryFile = dynamic_cast<const KArchiveFile*>(entry);
    QByteArray array = entryFile->data();

    QFile file( m_dataLocal + entry->name() );
    if(file.open( IO_WriteOnly )) 
    {
        int ret = file.writeBlock(array);
        file.close();
        return ret > 0 ? true : false;
    }

    return false;
}

} // namespace KIPISimpleViewerExportPlugin
