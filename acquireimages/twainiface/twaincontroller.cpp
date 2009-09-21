/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-27-10
 * Description : Twain interface
 *
 * Copyright (C) 2002-2003 Stephan Stapel <stephan dot stapel at web dot de>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "twaincontroller.h"
#include "twaincontroller.moc"

// Windows includes

#include <windows.h>

// Qt includes

#include <QPixmap>
#include <QDataStream>
#include <QByteArray>
#include <QPointer>

// KDE includes

#include <klocale.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kglobalsettings.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kpwriteimage.h"
#include "pluginsversion.h"

namespace KIPIAcquireImagesPlugin
{

TwainController::TwainController(KIPI::Interface* iface, QWidget* parent)
               : QWidget(parent), TwainIface()
{
    m_interface = iface;
    setParent(this);

    // This is a dummy widget not visible. We use QWidget to dispatch Windows event to
    // Twain interface. This is not possible to do it using QObject as well.
    hide();

    selectSource();
    if (!acquire())
        KMessageBox::error(this, i18n("Cannot acquire image..."));
}

TwainController::~TwainController()
{
    ReleaseTwain();
}

bool TwainController::winEvent(MSG* pMsg, long* result)
{
    processMessage(*pMsg);
    return false;
}

bool TwainController::selectSource()
{
    return (SelectSource() == true);
}

bool TwainController::acquire(unsigned int maxNumImages)
{
    int nMaxNum = 1;

    if (maxNumImages == UINT_MAX)
        nMaxNum = TWCPP_ANYCOUNT;

    return (Acquire(nMaxNum) == true);
}

bool TwainController::isValidDriver() const
{
    return (IsValidDriver() == true);
}

void TwainController::setParent(QWidget* parent)
{
    m_parent = parent;
    if (m_parent)
    {
        if (!onSetParent())
            KMessageBox::error(this, i18n("Cannot handle parent windows..."));
    }
}

bool TwainController::onSetParent()
{
    WId id = m_parent->winId();
    InitTwain(id);
    return IsValidDriver();
}

bool TwainController::processMessage(MSG& msg)
{
    if (msg.message == 528) // TODO: don't really know why...
        return false;

    if (m_hMessageWnd == 0)
        return false;

    return (ProcessMessage(msg) == true);
}

void TwainController::CopyImage(TW_MEMREF pdata, TW_IMAGEINFO& info)
{
    if (pdata && (info.ImageWidth != -1) && (info.ImageLength != - 1))
    {
        // Under Windows, Twain interface return a DIB data structure.
        // See http://en.wikipedia.org/wiki/Device-independent_bitmap#DIBs_in_memory for details.
        HGLOBAL hDIB     = (HGLOBAL)(long)pdata;
        int size         = (int)GlobalSize(hDIB);
        const char* bits = (const char*)GlobalLock(hDIB);

        // DIB is BMP without header. we will add it to load data in QImage using std loader from Qt.
        QByteArray baBmp;
        QDataStream ds(&baBmp, QIODevice::WriteOnly);

        ds.writeRawData("BM", 2);

        qint32 filesize = size + 14;
        ds << filesize;

        qint16 reserved = 0;
        ds << reserved;
        ds << reserved;

        qint32 pixOffset = 14 + 40 + 0;
        ds << pixOffset;

        ds.writeRawData(bits, size);

        QImage img = QImage::fromData(baBmp, "BMP");
        GlobalUnlock(hDIB);

        saveImage(img);

        close();
    }
}

void TwainController::saveImage(const QImage& img)
{
    QStringList writableMimetypes = KImageIO::mimeTypes(KImageIO::Writing);
    // Put first class citizens at first place
    writableMimetypes.removeAll("image/jpeg");
    writableMimetypes.removeAll("image/tiff");
    writableMimetypes.removeAll("image/png");
    writableMimetypes.insert(0, "image/png");
    writableMimetypes.insert(1, "image/jpeg");
    writableMimetypes.insert(2, "image/tiff");

    kDebug(51000) << "slotSaveImage: Offered mimetypes: " << writableMimetypes;

    QString defaultMimeType("image/png");
    QString defaultFileName("image.png");
    QString format("PNG");

    KUrl url = KGlobalSettings::documentPath();
    if (m_interface)
        url = m_interface->currentAlbum().uploadPath();

    QPointer<KFileDialog> imageFileSaveDialog = new KFileDialog(url, QString(), this);

/*  FIXME: This line make an eternal loop under Windows. why ???
    imageFileSaveDialog->setOperationMode(KFileDialog::Saving);*/

    imageFileSaveDialog->setMode(KFile::File);
    imageFileSaveDialog->setSelection(defaultFileName);
    imageFileSaveDialog->setCaption(i18n("New Image File Name"));
    imageFileSaveDialog->setModal(false);
    imageFileSaveDialog->setMimeFilter(writableMimetypes, defaultMimeType);

    // Start dialog and check if canceled.
    if ( imageFileSaveDialog->exec() != KFileDialog::Accepted )
       return;

    KUrl newURL = imageFileSaveDialog->selectedUrl();
    QFileInfo fi(newURL.path());

    // Check if target image format have been selected from Combo List of dialog.

    QStringList mimes = KImageIO::typeForMime(imageFileSaveDialog->currentMimeFilter());
    if (!mimes.isEmpty())
    {
        format = mimes.first().toUpper();
    }
    else
    {
        // Else, check if target image format have been add to target image file name using extension.

        format = fi.suffix().toUpper();

        // Check if format from file name extension is include on file mime type list.

        QStringList imgExtList = KImageIO::types(KImageIO::Writing);
        imgExtList << "TIF";
        imgExtList << "TIFF";
        imgExtList << "JPG";
        imgExtList << "JPE";

        if ( !imgExtList.contains( format ) )
        {
            KMessageBox::error(0, i18n("The target image file format \"%1\" is unsupported.", format));
            kWarning( 51000 ) << "target image file format " << format << " is unsupported!";
            return;
        }
    }

    if (!newURL.isValid())
    {
        KMessageBox::error(0, i18n("Failed to save file\n\"%1\" to\n\"%2\".",
                              newURL.fileName(),
                              newURL.path().section('/', -2, -2)));
        kWarning( 51000 ) << "target URL is not valid !";
        return;
    }

    // Check for overwrite ----------------------------------------------------------

    if ( fi.exists() )
    {
        int result = KMessageBox::warningYesNo(0, i18n("A file named \"%1\" already "
                                                       "exists. Are you sure you want "
                                                       "to overwrite it?",
                                               newURL.fileName()),
                                               i18n("Overwrite File?"),
                                               KStandardGuiItem::overwrite(),
                                               KStandardGuiItem::cancel());

        if (result != KMessageBox::Yes)
            return;
    }

    // Perform saving ---------------------------------------------------------------

    kapp->setOverrideCursor( Qt::WaitCursor );

    QImage prev     = img.scaled(1280, 1024, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QImage thumb    = img.scaled(160, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QByteArray prof = KIPIPlugins::KPWriteImage::getICCProfilFromFile(KDcrawIface::RawDecodingSettings::SRGB);

    KExiv2Iface::KExiv2 meta;
    meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
    meta.setImageDimensions(img.size());
    if (format != QString("JPEG"))
        meta.setImagePreview(prev);
    meta.setExifThumbnail(thumb);
    meta.setExifTagString("Exif.Image.DocumentName", QString("Scanned Image")); // not i18n
/*    meta.setExifTagString("Exif.Image.Make", d->saneWidget->make());
    meta.setXmpTagString("Xmp.tiff.Make", d->saneWidget->make());
    meta.setExifTagString("Exif.Image.Model", d->saneWidget->model());
    meta.setXmpTagString("Xmp.tiff.Model", d->saneWidget->model());*/
    meta.setImageDateTime(QDateTime::currentDateTime());
    meta.setImageOrientation(KExiv2Iface::KExiv2::ORIENTATION_NORMAL);
    meta.setImageColorWorkSpace(KExiv2Iface::KExiv2::WORKSPACE_SRGB);

    KIPIPlugins::KPWriteImage wImageIface;
    QByteArray data((const char*)img.bits(), img.numBytes());
    wImageIface.setImageData(data, img.width(), img.height(), false, true, prof, meta);

    if (format == QString("JPEG"))
    {
        wImageIface.write2JPEG(newURL.path());
    }
    else if (format == QString("PNG"))
    {
        wImageIface.write2PNG(newURL.path());
    }
    else if (format == QString("TIFF"))
    {
        wImageIface.write2TIFF(newURL.path());
    }
    else
    {
        img.save(newURL.path(), format.toAscii().data());
    }

    if (m_interface) m_interface->refreshImages( KUrl::List(newURL) );
    kapp->restoreOverrideCursor();

    delete imageFileSaveDialog;
}

}  // namespace KIPIAcquireImagesPlugin
