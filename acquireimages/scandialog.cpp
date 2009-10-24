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
#include <QDir>

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

#include "aboutdata.h"
#include "pluginsversion.h"
#include "saveimgthread.h"

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
        saveThread = 0;
    }

    SaveImgThread            *saveThread;

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about;

    KSaneIface::KSaneWidget  *saneWidget;
};

ScanDialog::ScanDialog(KIPI::Interface* kinterface, KSaneIface::KSaneWidget* saneWidget,
                       QWidget* /*parent*/, ScanDialogAboutData *about)
          : KDialog(0), d(new ScanDialogPriv)
{
    d->saneWidget = saneWidget;
    d->interface  = kinterface;
    d->about      = about;
    d->saveThread = new SaveImgThread(this);

    setButtons(Help|Close);
    setCaption(i18n("Scan Image"));
    setModal(false);

    setMainWidget(d->saneWidget);

    // -- About data and help button ----------------------------------------

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

    connect(d->saneWidget, SIGNAL(imageReady(QByteArray&, int, int, int, int)),
            this, SLOT(slotSaveImage(QByteArray&, int, int, int, int)));

    connect(d->saveThread, SIGNAL(signalComplete(const KUrl&, bool)),
            this, SLOT(slotThreadDone(const KUrl&, bool)));
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

void ScanDialog::slotSaveImage(QByteArray& ksane_data, int width, int height, int bytes_per_line, int ksaneformat)
{
    QStringList writableMimetypes = KImageIO::mimeTypes(KImageIO::Writing);
    // Put first class citizens at first place
    writableMimetypes.removeAll("image/jpeg");
    writableMimetypes.removeAll("image/tiff");
    writableMimetypes.removeAll("image/png");
    writableMimetypes.insert(0, "image/png");
    writableMimetypes.insert(1, "image/jpeg");
    writableMimetypes.insert(2, "image/tiff");

    kDebug() << "slotSaveImage: Offered mimetypes: " << writableMimetypes;

    QString defaultMimeType("image/png");
    QString defaultFileName("image.png");
    QString format("PNG");
    QString place = QDir::homePath();

    if (d->interface)
        d->interface->currentAlbum().uploadPath();

    QPointer<KFileDialog> imageFileSaveDialog = new KFileDialog(place, QString(), 0);

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
            kWarning() << "target image file format " << format << " is unsupported!";
            return;
        }
    }

    if (!newURL.isValid())
    {
        KMessageBox::error(0, i18n("Failed to save file\n\"%1\" to\n\"%2\".",
                              newURL.fileName(),
                              newURL.path().section('/', -2, -2)));
        kWarning() << "target URL is not valid !";
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

    delete imageFileSaveDialog;
    setCursor(Qt::WaitCursor);
    setEnabled(false);
    saveSettings();

    // Perform saving ---------------------------------------------------------------

    d->saveThread->setImageData(ksane_data, width, height, bytes_per_line, ksaneformat);
    d->saveThread->setPreviewImage(d->saneWidget->toQImage(ksane_data, width, height,
                                   bytes_per_line, (KSaneIface::KSaneWidget::ImageFormat)ksaneformat));
    d->saveThread->setTargetFile(newURL, format);
    d->saveThread->setScannerModel(d->saneWidget->make(), d->saneWidget->model());
    d->saveThread->start();
}

void ScanDialog::slotThreadDone(const KUrl& url, bool sucess)
{
    if (!sucess)
        KMessageBox::error(0, i18n("Cannot save \"%1\" file", url.fileName()));

    if (d->interface)
        d->interface->refreshImages( KUrl::List(url) );

    unsetCursor();
    setEnabled(true);
}

}  // namespace KIPIAcquireImagesPlugin
