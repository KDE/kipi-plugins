/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-09-09
 * Description : scanner dialog
 *
 * Copyright (C) 2007-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <libkipi/imagecollection.h>

// Local includes

#include "aboutdata.h"
#include "kpversion.h"
#include "saveimgthread.h"

namespace KIPIAcquireImagesPlugin
{

class ScanDialog::Private
{
public:

    Private()
    {
        saneWidget = 0;
        saveThread = 0;
    }

    SaveImgThread* saveThread;

    KSaneWidget*   saneWidget;
};

ScanDialog::ScanDialog(KSaneWidget* const saneWidget, QWidget* const /*parent*/, ScanDialogAboutData* const about)
    : KPToolDialog(0), d(new Private)
{
    d->saneWidget = saneWidget;
    d->saveThread = new SaveImgThread(this);

    setButtons(Help|Close);
    setCaption(i18n("Scan Image"));
    setModal(false);
    setAboutData(about);

    setMainWidget(d->saneWidget);

    // ------------------------------------------------------------------------

    readSettings();

    // ------------------------------------------------------------------------

    connect(d->saneWidget, SIGNAL(imageReady(QByteArray&,int,int,int,int)),
            this, SLOT(slotSaveImage(QByteArray&,int,int,int,int)));

    connect(d->saveThread, SIGNAL(signalComplete(KUrl,bool)),
            this, SLOT(slotThreadDone(KUrl,bool)));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotCloseClicked()));
}

ScanDialog::~ScanDialog()
{
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

void ScanDialog::closeEvent(QCloseEvent *e)
{
    d->saneWidget->closeDevice();
    saveSettings();
    e->accept();
}

void ScanDialog::slotCloseClicked()
{
    d->saneWidget->closeDevice();
    saveSettings();
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

    if (iface())
        place = iface()->currentAlbum().uploadPath().path();

    QPointer<KFileDialog> imageFileSaveDialog = new KFileDialog(place, QString(), 0);

    imageFileSaveDialog->setModal(false);
    imageFileSaveDialog->setOperationMode(KFileDialog::Saving);
    imageFileSaveDialog->setMode(KFile::File);
    imageFileSaveDialog->setSelection(defaultFileName);
    imageFileSaveDialog->setCaption(i18n("New Image File Name"));
    imageFileSaveDialog->setMimeFilter(writableMimetypes, defaultMimeType);

    // Start dialog and check if canceled.
    if ( imageFileSaveDialog->exec() != KFileDialog::Accepted )
    {
        delete imageFileSaveDialog;
        return;
    }

    KUrl newURL = imageFileSaveDialog->selectedUrl();
    QFileInfo fi(newURL.toLocalFile());

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
	    delete imageFileSaveDialog;
            return;
        }
    }

    if (!newURL.isValid())
    {
        KMessageBox::error(0, i18n("Failed to save file\n\"%1\" to\n\"%2\".",
                              newURL.fileName(),
                              newURL.path().section('/', -2, -2)));
        kWarning() << "target URL is not valid !";
        delete imageFileSaveDialog;
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
        {
            delete imageFileSaveDialog;
            return;
        }
    }

    delete imageFileSaveDialog;
    setCursor(Qt::WaitCursor);
    setEnabled(false);
    saveSettings();

    // Perform saving ---------------------------------------------------------------

    d->saveThread->setImageData(ksane_data, width, height, bytes_per_line, ksaneformat);
    d->saveThread->setPreviewImage(d->saneWidget->toQImage(ksane_data, width, height,
                                   bytes_per_line, (KSaneWidget::ImageFormat)ksaneformat));
    d->saveThread->setTargetFile(newURL, format);
    d->saveThread->setScannerModel(d->saneWidget->make(), d->saneWidget->model());
    d->saveThread->start();
}

void ScanDialog::slotThreadDone(const KUrl& url, bool success)
{
    if (!success)
        KMessageBox::error(0, i18n("Cannot save \"%1\" file", url.fileName()));

    if (iface())
        iface()->refreshImages( KUrl::List(url) );

    unsetCursor();
    setEnabled(true);
}

}  // namespace KIPIAcquireImagesPlugin
