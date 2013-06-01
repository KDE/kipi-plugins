/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-22
 * Description : Raw converter single dialog
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "singledialog.moc"

// C ANSI includes

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

// C++ includes

#include <cstdio>

// Qt includes

#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QPushButton>
#include <QTimer>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kio/renamedialog.h>
#include <kde_file.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/dcrawsettingswidget.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

// Local includes

#include "kpaboutdata.h"
#include "kpversion.h"
#include "kpimageinfo.h"
#include "kppreviewmanager.h"
#include "kpsavesettingswidget.h"
#include "rawdecodingiface.h"
#include "actions.h"
#include "actionthread.h"

using namespace KDcrawIface;

namespace KIPIRawConverterPlugin
{

class SingleDialog::Private
{
public:

    Private()
    {
        previewWidget          = 0;
        thread                 = 0;
        saveSettingsBox        = 0;
        decodingSettingsBox    = 0;
        iface                  = 0;
        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }
    }

    QString               inputFileName;

    KUrl                  inputFile;

    KPPreviewManager*     previewWidget;

    ActionThread*         thread;

    KPSaveSettingsWidget* saveSettingsBox;

    DcrawSettingsWidget*  decodingSettingsBox;

    Interface*            iface;
};

SingleDialog::SingleDialog(const QString& file)
    : KPToolDialog(0), d(new Private)
{
    setButtons(Help | Default | User1 | User2 | User3 | Close);
    setDefaultButton(Close);
    setButtonText(User1, i18n("&Preview"));
    setButtonText(User2, i18n("Con&vert"));
    setButtonText(User3, i18n("&Abort"));
    setCaption(i18n("RAW Image Converter"));
    setModal(false);

    QWidget* const page           = new QWidget( this );
    setMainWidget( page );
    QGridLayout* const mainLayout = new QGridLayout(page);

    d->previewWidget       = new KPPreviewManager(page);

    // ---------------------------------------------------------------

    d->decodingSettingsBox = new DcrawSettingsWidget(page, DcrawSettingsWidget::SIXTEENBITS    |
                                                           DcrawSettingsWidget::COLORSPACE     |
                                                           DcrawSettingsWidget::POSTPROCESSING |
                                                           DcrawSettingsWidget::BLACKWHITEPOINTS);
    d->saveSettingsBox     = new KPSaveSettingsWidget(d->decodingSettingsBox);

#if KDCRAW_VERSION <= 0x000500
    d->decodingSettingsBox->addItem(d->saveSettingsBox, i18n("Save settings"));
    d->decodingSettingsBox->updateMinimumWidth();
#else
    d->decodingSettingsBox->addItem(d->saveSettingsBox, i18n("Save settings"), QString("savesettings"), false);
#endif

    mainLayout->addWidget(d->previewWidget,       0, 0, 2, 1);
    mainLayout->addWidget(d->decodingSettingsBox, 0, 1, 1, 1);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());
    mainLayout->setColumnStretch(0, 10);
#if KDCRAW_VERSION <= 0x000500
    mainLayout->setRowStretch(1, 10);
#endif

    // ---------------------------------------------------------------

    KPAboutData* const about = new KPAboutData(ki18n("RAW Image Converter"),
                                   0,
                                   KAboutData::License_GPL,
                                   ki18n("A Kipi plugin to convert RAW images"),
                                   ki18n("(c) 2003-2005, Renchi Raju\n"
                                         "(c) 2006-2013, Gilles Caulier\n"
                                         "(c) 2012, Smit Mehta"));

    about->addAuthor(ki18n("Renchi Raju"),
                     ki18n("Author"),
                           "renchi dot raju at gmail dot com");

    about->addAuthor(ki18n("Gilles Caulier"),
                     ki18n("Developer and maintainer"),
                           "caulier dot gilles at gmail dot com");

    about->addAuthor(ki18n("Smit Mehta"),
                     ki18n("Developer"),
                           "smit dot meh at gmail dot com");

    about->setHandbookEntry("rawconverter");
    setAboutData(about);

    // ---------------------------------------------------------------

    setButtonToolTip(User1, i18n("Generate a preview using current settings. "
                                 "Uses a simple bilinear interpolation for "
                                 "quick results."));

    setButtonToolTip(User2, i18n("Convert the RAW Image using current settings. "
                                 "This uses a high-quality adaptive algorithm."));

    setButtonToolTip(User3, i18n("Abort the current RAW file conversion"));

    setButtonToolTip(Close, i18n("Exit RAW Converter"));

    // ---------------------------------------------------------------

    d->thread = new ActionThread(this);

    // ---------------------------------------------------------------

    connect(d->decodingSettingsBox, SIGNAL(signalSixteenBitsImageToggled(bool)),
            d->saveSettingsBox, SLOT(slotPopulateImageFormat(bool)));

    connect(d->decodingSettingsBox, SIGNAL(signalSixteenBitsImageToggled(bool)),
            this, SLOT(slotSixteenBitsImageToggled(bool)));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(defaultClicked()),
            this, SLOT(slotDefault()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotUser1()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotUser2()));

    connect(this, SIGNAL(user3Clicked()),
            this, SLOT(slotUser3()));

    connect(d->thread, SIGNAL(starting(KIPIRawConverterPlugin::ActionData)),
            this, SLOT(slotAction(KIPIRawConverterPlugin::ActionData)));

    connect(d->thread, SIGNAL(finished(KIPIRawConverterPlugin::ActionData)),
            this, SLOT(slotAction(KIPIRawConverterPlugin::ActionData)));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
    setFile(file);
}

SingleDialog::~SingleDialog()
{
    delete d->thread;
    delete d;
}

void SingleDialog::setFile(const QString& file)
{
    d->inputFile     = file;
    d->inputFileName = QFileInfo(file).fileName();
    QTimer::singleShot(0, this, SLOT(slotIdentify()));
}

void SingleDialog::slotSixteenBitsImageToggled(bool)
{
    // Dcraw do not provide a way to set brigness of image in 16 bits color depth.
    // We always set on this option. We drive brightness adjustment as post processing.
    d->decodingSettingsBox->setEnabledBrightnessSettings(true);
}

void SingleDialog::closeEvent(QCloseEvent* e)
{
    if (!e) return;
    d->thread->cancel();
    saveSettings();
    e->accept();
}

void SingleDialog::slotClose()
{
    d->thread->cancel();
    saveSettings();
    done(Close);
}

void SingleDialog::slotDefault()
{
    d->decodingSettingsBox->resetToDefault();
    d->saveSettingsBox->resetToDefault();
}

void SingleDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("RawConverter Settings"));

    d->decodingSettingsBox->readSettings(group);
    d->saveSettingsBox->readSettings(group);
    d->saveSettingsBox->slotPopulateImageFormat(d->decodingSettingsBox->settings().sixteenBitsImage);

    KConfigGroup group2 = config.group(QString("Single Raw Converter Dialog"));
    restoreDialogSize(group2);
}

void SingleDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("RawConverter Settings"));

    d->decodingSettingsBox->writeSettings(group);
    d->saveSettingsBox->writeSettings(group);

    KConfigGroup group2 = config.group(QString("Single Raw Converter Dialog"));
    saveDialogSize(group2);
    config.sync();
}

// 'Preview' dialog button.
void SingleDialog::slotUser1()
{
    d->thread->setSettings(d->decodingSettingsBox->settings(), KPSaveSettingsWidget::OUTPUT_PNG);
    d->thread->processHalfRawFile(KUrl(d->inputFile));

    if (!d->thread->isRunning())
        d->thread->start();
}

// 'Convert' dialog button.
void SingleDialog::slotUser2()
{
    d->thread->setSettings(d->decodingSettingsBox->settings(), d->saveSettingsBox->fileFormat());
    d->thread->processRawFile(KUrl(d->inputFile));

    if (!d->thread->isRunning())
        d->thread->start();
}

// 'Abort' dialog button.
void SingleDialog::slotUser3()
{
    d->thread->cancel();
}

void SingleDialog::slotIdentify()
{
    if (!d->iface->hasFeature(HostSupportsThumbnails))
    {
        d->thread->thumbRawFile(KUrl(d->inputFile));
    }
    else
    {
        connect(d->iface, SIGNAL(gotThumbnail(KUrl,QPixmap)),
                this, SLOT(slotThumbnail(KUrl,QPixmap)));

        d->iface->thumbnail(KUrl(d->inputFile), 256);
    }

    d->thread->identifyRawFile(KUrl(d->inputFile), true);

    if (!d->thread->isRunning())
        d->thread->start();
}

void SingleDialog::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    setThumbnail(url.path(), pix);
}

void SingleDialog::busy(bool val)
{
    d->decodingSettingsBox->setEnabled(!val);
    d->saveSettingsBox->setEnabled(!val);
    enableButton(User1, !val);
    enableButton(User2, !val);
    enableButton(User3, val);
    enableButton(Close, !val);
}

void SingleDialog::setIdentity(const KUrl& /*url*/, const QString& identity)
{
    d->previewWidget->setText(d->inputFileName + QString(" :\n") + identity, Qt::white);
}

void SingleDialog::setThumbnail(const KUrl& url, const QPixmap& thumbnail)
{
    if (url == d->inputFile)
        d->previewWidget->setThumbnail(thumbnail);
}

void SingleDialog::previewing(const KUrl& /*url*/)
{
    d->previewWidget->setBusy(true, i18n("Generating Preview..."));
}

void SingleDialog::previewed(const KUrl& /*url*/, const QString& tmpFile)
{
    d->previewWidget->load(tmpFile);
    ::remove(QFile::encodeName(tmpFile));
}

void SingleDialog::previewFailed(const KUrl& /*url*/)
{
    d->previewWidget->setText(i18n("Failed to generate preview"), Qt::red);
}

void SingleDialog::processing(const KUrl& /*url*/)
{
    d->previewWidget->setBusy(true, i18n("Processing Images..."));
}

void SingleDialog::processed(const KUrl& url, const QString& tmpFile)
{
    d->previewWidget->load(tmpFile);
    QString filter("*.");
    QString ext;

    switch(d->saveSettingsBox->fileFormat())
    {
        case KPSaveSettingsWidget::OUTPUT_JPEG:
            ext = "jpg";
            break;
        case KPSaveSettingsWidget::OUTPUT_TIFF:
            ext = "tif";
            break;
        case KPSaveSettingsWidget::OUTPUT_PPM:
            ext = "ppm";
            break;
        case KPSaveSettingsWidget::OUTPUT_PNG:
            ext = "png";
            break;
    }

    filter += ext;
    QFileInfo fi(d->inputFile.path());
    QString destFile = fi.absolutePath() + QString("/") + fi.completeBaseName() + QString(".") + ext;

    if (d->saveSettingsBox->conflictRule() != KPSaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;

        if (::stat(QFile::encodeName(destFile), &statBuf) == 0)
        {
            KIO::RenameDialog dlg(this, i18n("Save RAW image converted from '%1' as", fi.fileName()),
                                  tmpFile, destFile,
                                  KIO::RenameDialog_Mode(KIO::M_SINGLE | KIO::M_OVERWRITE | KIO::M_SKIP));

            switch (dlg.exec())
            {
                case KIO::R_CANCEL:
                case KIO::R_SKIP:
                {
                    destFile.clear();
                    break;
                }
                case KIO::R_RENAME:
                {
                    destFile = dlg.newDestUrl().path();
                    break;
                }
                default:    // Overwrite.
                    break;
            }
        }
    }

    if (!destFile.isEmpty())
    {
        if (KPMetadata::hasSidecar(tmpFile))
        {
            if (KDE::rename(KPMetadata::sidecarPath(tmpFile),
                            KPMetadata::sidecarPath(destFile)) != 0)
            {
                KMessageBox::information(this, i18n("Failed to save sidecar file for image %1...", destFile));
            }
        }

#ifndef Q_OS_WIN
        if (::rename(QFile::encodeName(tmpFile), QFile::encodeName(destFile)) != 0)
#else
        if (::MoveFileEx(tmpFile.utf16(), destFile.utf16(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == 0)
#endif
        {
            KMessageBox::error(this, i18n("Failed to save image %1", destFile));
        }
        else
        {
            // Assign Kipi host attributes from original RAW image.

            KPImageInfo info(url);
            info.cloneData(KUrl(destFile));
        }
    }
}

void SingleDialog::processingFailed(const KUrl& /*url*/)
{
    d->previewWidget->setBusy(false);
    d->previewWidget->setText(i18n("Failed to convert RAW image"), Qt::red);
}

void SingleDialog::slotAction(const KIPIRawConverterPlugin::ActionData& ad)
{
    QString text;

    if (ad.starting)            // Something have been started...
    {
        switch (ad.action)
        {
            case(IDENTIFY_FULL):
            case(THUMBNAIL):
                break;
            case(PREVIEW):
            {
                busy(true);
                previewing(ad.fileUrl);
                break;
            }
            case(PROCESS):
            {
                busy(true);
                processing(ad.fileUrl);
                break;
            }
            default:
            {
                kWarning() << "Unknown action";
                break;
            }
        }
    }
    else
    {
        if (!ad.success)        // Something is failed...
        {
            switch (ad.action)
            {
                case(IDENTIFY_FULL):
                case(THUMBNAIL):
                    break;
                case(PREVIEW):
                {
                    previewFailed(ad.fileUrl);
                    busy(false);
                    break;
                }
                case(PROCESS):
                {
                    processingFailed(ad.fileUrl);
                    busy(false);
                    break;
                }
                default:
                {
                    kWarning() << "Unknown action";
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case(IDENTIFY_FULL):
                {
                    setIdentity(ad.fileUrl, ad.message);
                    busy(false);
                    break;
                }
                case(THUMBNAIL):
                {
                    QPixmap pix = QPixmap::fromImage(ad.image.scaled(256, 256, Qt::KeepAspectRatio));
                    setThumbnail(ad.fileUrl, pix);
                    busy(false);
                    break;
                }
                case(PREVIEW):
                {
                    previewed(ad.fileUrl, ad.destPath);
                    busy(false);
                    break;
                }
                case(PROCESS):
                {
                    processed(ad.fileUrl, ad.destPath);
                    busy(false);
                    break;
                }
                default:
                {
                    kWarning() << "Unknown action";
                    break;
                }
            }
        }
    }
}

} // namespace KIPIRawConverterPlugin
