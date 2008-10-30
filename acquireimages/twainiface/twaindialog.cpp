/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-10-29
 * Description : Twain interface
 *
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

#include "twaindialog.h"
#include "twaindialog.moc"

// Qt includes.

#include <QDateTime>
#include <QPushButton>
#include <QTimer>
#include <QLabel>

// KDE includes.

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
#include <kglobalsettings.h>

// LibKIPI includes.

#include <libkipi/interface.h>

// Local includes.

#include "qtwain.h"
#include "kpaboutdata.h"
#include "kpwriteimage.h"
#include "pluginsversion.h"

namespace KIPIAcquireImagesPlugin
{

class TwainDialogPriv
{
public:

    TwainDialogPriv()
    {
        interface  = 0;
        about      = 0;
        preview    = 0;
        twIface    = 0;
    }

    QLabel                   *preview;

    QImage                    img;

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about;

    QTwain                   *twIface;
};

TwainDialog::TwainDialog(KIPI::Interface* interface, QWidget *parent)
           : KDialog(parent)
{
    d = new TwainDialogPriv;
    d->interface = interface;
    d->twIface   = new QTwain(0);
    d->preview   = new QLabel(this);

    setButtons(Help|User1|Close);
    setButtonText(User1, i18n("Save As..."));
    setCaption(i18n("Scan Image"));
    setModal(true);
    setMainWidget(d->preview);

    // -- About data and help button ----------------------------------------

    d->about = new KIPIPlugins::KPAboutData(ki18n("Acquire images"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin to acquire images using a flat scanner"),
                   ki18n("(c) 2003-2008, Gilles Caulier"));

    d->about->addAuthor(ki18n("Gilles Caulier"),
                        ki18n("Author"),
                        "caulier dot gilles at gmail dot com");

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Plugin Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setDelayedMenu( helpMenu->menu() );

    // ------------------------------------------------------------------------

    readSettings();

    // ------------------------------------------------------------------------

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotSaveImage()));

    connect(d->twIface, SIGNAL(signalImageAcquired(const QImage&)),
            this, SLOT(slotImageAcquired(const QImage&)));
}

TwainDialog::~TwainDialog()
{
    delete d->about;
    delete d;
}

void TwainDialog::showEvent(QShowEvent*)
{
    // set the parent here to be sure to have a really
    // valid window as the twain parent!
    d->twIface->setParent(this);
    QTimer::singleShot(0, this, SLOT(slotInit()));
}

void TwainDialog::slotInit()
{
    d->twIface->selectSource();
    if (!d->twIface->acquire())
        QMessageBox::critical(this, QString(), i18n("Cannot acquire image..."));
}

bool TwainDialog::winEvent(MSG* pMsg, long* result)
{
    d->twIface->processMessage(*pMsg);
    return false;
}

void TwainDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Twain Tool Dialog"));
    restoreDialogSize(group);
}

void TwainDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Twain Tool Dialog"));
    saveDialogSize(group);
    config.sync();
}

void TwainDialog::slotClose()
{
    saveSettings();
    done(Close);
}

void TwainDialog::closeEvent(QCloseEvent *e)
{
    saveSettings();
    e->accept();
}

void TwainDialog::slotHelp()
{
    KToolInvocation::invokeHelp("acquireimages", "kipi-plugins");
}

void TwainDialog::slotImageAcquired(const QImage& img)
{
    d->img      = img;
    QPixmap pix = QPixmap::fromImage(d->img);
    d->preview->setPixmap(pix.scaled(320, 240, Qt::KeepAspectRatio));
}

void TwainDialog::slotSaveImage()
{
    QStringList writableMimetypes = KImageIO::mimeTypes(KImageIO::Writing);
    // Put first class citizens at first place
    writableMimetypes.removeAll("image/jpeg");
    writableMimetypes.removeAll("image/tiff");
    writableMimetypes.removeAll("image/png");
    writableMimetypes.insert(0, "image/png");
    writableMimetypes.insert(1, "image/jpeg");
    writableMimetypes.insert(2, "image/tiff");

    kDebug( 51000 ) << "slotSaveImage: Offered mimetypes: " << writableMimetypes;

    QString defaultMimeType("image/png");
    QString defaultFileName("image.png");
    QString format("PNG");

    KUrl uurl = KGlobalSettings::documentPath();
    if (d->interface) uurl = d->interface->currentAlbum().uploadPath();

    KFileDialog imageFileSaveDialog(uurl, QString(), this);

/*  FIXME: This line make an eternal loop under Windows. why ???
    imageFileSaveDialog.setOperationMode(KFileDialog::Saving);*/

    imageFileSaveDialog.setMode(KFile::File);
    imageFileSaveDialog.setSelection(defaultFileName);
    imageFileSaveDialog.setCaption(i18n("New Image File Name"));
    imageFileSaveDialog.setModal(false);
    imageFileSaveDialog.setMimeFilter(writableMimetypes, defaultMimeType);

    // Start dialog and check if canceled.
    if ( imageFileSaveDialog.exec() != KFileDialog::Accepted )
       return;

    KUrl newURL = imageFileSaveDialog.selectedUrl();
    QFileInfo fi(newURL.path());

    // Check if target image format have been selected from Combo List of dialog.

    QStringList mimes = KImageIO::typeForMime(imageFileSaveDialog.currentMimeFilter());
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

    QImage prev     = d->img.scaled(1280, 1024, Qt::KeepAspectRatio);
    QImage thumb    = d->img.scaled(160, 120, Qt::KeepAspectRatio);
    QByteArray prof = KIPIPlugins::KPWriteImage::getICCProfilFromFile(KDcrawIface::RawDecodingSettings::SRGB);

    KExiv2Iface::KExiv2 meta;
    meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
    meta.setImageDimensions(d->img.size());
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
    QByteArray data((const char*)d->img.bits(), d->img.numBytes());
    wImageIface.setImageData(data, d->img.width(), d->img.height(), false, true, prof, meta);

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
        d->img.save(newURL.path(), format.toAscii().data());
    }

    d->interface->refreshImages( KUrl::List(newURL) );
    kapp->restoreOverrideCursor();
}

}  // namespace KIPIAcquireImagesPlugin
