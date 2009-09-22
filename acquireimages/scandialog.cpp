/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-09-09
 * Description : scanner dialog
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "scandialog.h"
#include "scandialog.moc"

// Qt includes

#include <QDateTime>
#include <QPushButton>
#include <QPointer>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kimageio.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <kurl.h>

// LibKSane includes

#include <libksane/ksane.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kpaboutdata.h"
#include "kpwriteimage.h"
#include "pluginsversion.h"

namespace KIPIAcquireImagesPlugin
{

class ScanDialogPriv
{
public:

    ScanDialogPriv()
    {
        saneWidget = 0;
        interface  = 0;
        about      = 0;
    }

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about;

    KSaneIface::KSaneWidget  *saneWidget;
};

ScanDialog::ScanDialog(KIPI::Interface* interface, KSaneIface::KSaneWidget *saneWidget, QWidget *parent)
          : KDialog(parent), d(new ScanDialogPriv)
{
    d->saneWidget = saneWidget;
    d->interface  = interface;

    setButtons(Help|Close);
    setCaption(i18n("Scan Image"));
    setModal(true);

    setMainWidget(d->saneWidget);

    // -- About data and help button ----------------------------------------

    d->about = new KIPIPlugins::KPAboutData(ki18n("Acquire images"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin to acquire images using a flat scanner"),
                   ki18n("(c) 2003-2009, Gilles Caulier\n"
                         "(c) 2007-2009, Kare Sars"));

    d->about->addAuthor(ki18n("Gilles Caulier"),
                        ki18n("Author"),
                        "caulier dot gilles at gmail dot com");

    d->about->addAuthor(ki18n("Kare Sars"),
                        ki18n("Developer"),
                        "kare dot sars at kolumbus dot fi");

    d->about->addAuthor(ki18n("Angelo Naselli"),
                        ki18n("Developer"),
                        "anaselli at linux dot it");

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ------------------------------------------------------------------------

    readSettings();

    // ------------------------------------------------------------------------

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(d->saneWidget, SIGNAL(imageReady(QByteArray &, int, int, int, int)),
            this, SLOT(slotSaveImage(QByteArray &, int, int, int, int)));
}

ScanDialog::~ScanDialog()
{
    delete d->about;
    delete d;
}

void ScanDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Scan Tool Dialog"));
    restoreDialogSize(group);
}

void ScanDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Scan Tool Dialog"));
    saveDialogSize(group);
    config.sync();
}

void ScanDialog::slotClose()
{
    saveSettings();
    done(Close);
}

void ScanDialog::closeEvent(QCloseEvent *e)
{
    d->saneWidget->scanCancel();
    saveSettings();
    e->accept();
}

void ScanDialog::slotHelp()
{
    KToolInvocation::invokeHelp("acquireimages", "kipi-plugins");
}

void ScanDialog::slotSaveImage(QByteArray &ksane_data, int width, int height, int bytes_per_line, int ksaneformat)
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

    QPointer<KFileDialog> imageFileSaveDialog = new KFileDialog(d->interface->currentAlbum().uploadPath(),
                                                                QString(), 0);

    imageFileSaveDialog->setModal(false);
    imageFileSaveDialog->setOperationMode(KFileDialog::Saving);
    imageFileSaveDialog->setMode(KFile::File);
    imageFileSaveDialog->setSelection(defaultFileName);
    imageFileSaveDialog->setCaption(i18n("New Image File Name"));
    imageFileSaveDialog->setMimeFilter(writableMimetypes, defaultMimeType);

    // Start dialog and check if canceled.
    if ( imageFileSaveDialog->exec() != KFileDialog::Accepted )
       return;

    KUrl newURL = imageFileSaveDialog->selectedUrl();
    QFileInfo fi(newURL.path());

    // Check if target image format have been selected from Combo List of dialog.

    const QStringList mimes = KImageIO::typeForMime(imageFileSaveDialog->currentMimeFilter());
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
    saveSettings();

    KSaneIface::KSaneWidget::ImageFormat frmt = (KSaneIface::KSaneWidget::ImageFormat)ksaneformat;

    QImage img      = d->saneWidget->toQImage(ksane_data, width, height, bytes_per_line, frmt);
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
    meta.setExifTagString("Exif.Image.Make", d->saneWidget->make());
    meta.setXmpTagString("Xmp.tiff.Make", d->saneWidget->make());
    meta.setExifTagString("Exif.Image.Model", d->saneWidget->model());
    meta.setXmpTagString("Xmp.tiff.Model", d->saneWidget->model());
    meta.setImageDateTime(QDateTime::currentDateTime());
    meta.setImageOrientation(KExiv2Iface::KExiv2::ORIENTATION_NORMAL);
    meta.setImageColorWorkSpace(KExiv2Iface::KExiv2::WORKSPACE_SRGB);

    KIPIPlugins::KPWriteImage wImageIface;
    if (frmt != KSaneIface::KSaneWidget::FormatRGB_16_C)
    {
        QByteArray data((const char*)img.bits(), img.numBytes());
        wImageIface.setImageData(data, img.width(), img.height(), false, true, prof, meta);
    }
    else
    {
        // 16 bits color depth image.
        wImageIface.setImageData(ksane_data, width, height, true, false, prof, meta);
    }

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

    d->interface->refreshImages( KUrl::List(newURL) );
    kapp->restoreOverrideCursor();

    delete imageFileSaveDialog;
}

}  // namespace KIPIAcquireImagesPlugin
