/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-24
 * Description : Raw converter batch dialog
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011      by Veaceslav Munteanu <slavuttici at gmail dot com>
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

#include "batchdialog.moc"

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
#include <QPixmap>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidgetItemIterator>

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

// Local includes

#include "actions.h"
#include "actionthread.h"
#include "myimagelist.h"
#include "kpimagedialog.h"
#include "kpaboutdata.h"
#include "kpversion.h"
#include "kpimageinfo.h"
#include "kphostsettings.h"
#include "kpsavesettingswidget.h"
#include "rawdecodingiface.h"
#include "kpprogresswidget.h"

using namespace KDcrawIface;

namespace KIPIRawConverterPlugin
{

class BatchDialog::Private
{
public:

    Private()
    {
        busy                = false;
        page                = 0;
        progressBar         = 0;
        listView            = 0;
        thread              = 0;
        saveSettingsBox     = 0;
        decodingSettingsBox = 0;
    }

    bool                  busy;

    QWidget*              page;

    QStringList           fileList;

    KPProgressWidget*     progressBar;

    MyImageList*          listView;

    ActionThread*         thread;

    KPSaveSettingsWidget* saveSettingsBox;

    DcrawSettingsWidget*  decodingSettingsBox;
};

BatchDialog::BatchDialog()
    : KPToolDialog(0), d(new Private)
{
    setButtons(Help | Default | Apply | Close );
    setDefaultButton(Close);
    setButtonToolTip(Close, i18n("Exit RAW Converter"));
    setCaption(i18n("RAW Image Batch Converter"));
    setModal(false);
    setMinimumSize(700, 500);

    d->page = new QWidget(this);
    setMainWidget(d->page);
    QGridLayout* const mainLayout = new QGridLayout(d->page);

    //---------------------------------------------

    d->listView = new MyImageList(d->page);

    // ---------------------------------------------------------------

    d->decodingSettingsBox = new DcrawSettingsWidget(d->page, DcrawSettingsWidget::SIXTEENBITS |
                                                              DcrawSettingsWidget::COLORSPACE |
                                                              DcrawSettingsWidget::POSTPROCESSING |
                                                              DcrawSettingsWidget::BLACKWHITEPOINTS);
    d->decodingSettingsBox->setObjectName("RawSettingsBox Expander");
    d->saveSettingsBox     = new KPSaveSettingsWidget(d->page);
    d->saveSettingsBox->setPromptButtonText(i18n("Store it under different name"));

#if KDCRAW_VERSION <= 0x000500
    d->decodingSettingsBox->addItem(d->saveSettingsBox, i18n("Save settings"));
    d->decodingSettingsBox->updateMinimumWidth();
#else
    d->decodingSettingsBox->insertItem(DcrawSettingsWidget::COLORMANAGEMENT+1, d->saveSettingsBox,
                                       SmallIcon("document-save"), i18n("Save settings"),
                                       QString("savesettings"), false);
#endif

    d->progressBar = new KPProgressWidget(d->page);
    d->progressBar->setMaximumHeight( fontMetrics().height()+2 );
    d->progressBar->hide();

    mainLayout->addWidget(d->listView,            0, 0, 3, 1);
    mainLayout->addWidget(d->decodingSettingsBox, 0, 1, 1, 1);
    mainLayout->addWidget(d->progressBar,         1, 1, 1, 1);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());
    mainLayout->setColumnStretch(0, 10);
#if KDCRAW_VERSION <= 0x000500
    mainLayout->setRowStretch(2, 10);
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

    d->thread = new ActionThread(this);

    // ---------------------------------------------------------------

    connect(d->saveSettingsBox, SIGNAL(signalSaveFormatChanged()),
            this, SLOT(slotIdentify()));

    connect(d->saveSettingsBox, SIGNAL(signalConflictButtonChanged(int)),
            this, SLOT(slotIdentify()));

    connect(d->decodingSettingsBox, SIGNAL(signalSixteenBitsImageToggled(bool)),
            d->saveSettingsBox, SLOT(slotPopulateImageFormat(bool)));

    connect(d->decodingSettingsBox, SIGNAL(signalSixteenBitsImageToggled(bool)),
            this, SLOT(slotSixteenBitsImageToggled(bool)));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(defaultClicked()),
            this, SLOT(slotDefault()));

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotStartStop()));

    connect(d->thread, SIGNAL(signalStarting(KIPIRawConverterPlugin::ActionData)),
            this, SLOT(slotAction(KIPIRawConverterPlugin::ActionData)));

    connect(d->thread, SIGNAL(signalFinished(KIPIRawConverterPlugin::ActionData)),
            this, SLOT(slotAction(KIPIRawConverterPlugin::ActionData)));

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(slotThreadFinished()));

    connect(d->listView, SIGNAL(signalImageListChanged()),
            this, SLOT(slotIdentify()));

    connect(d->progressBar, SIGNAL(signalProgressCanceled()),
            this, SLOT(slotStartStop()));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
}

BatchDialog::~BatchDialog()
{
    delete d;
}

void BatchDialog::slotSixteenBitsImageToggled(bool)
{
    // Dcraw do not provide a way to set brigness of image in 16 bits color depth.
    // We always set on this option. We drive brightness adjustment as post processing.
    d->decodingSettingsBox->setEnabledBrightnessSettings(true);
}

void BatchDialog::closeEvent(QCloseEvent* e)
{
    if (!e)
        return;

    // Stop current conversion if necessary
    if (d->busy)
        slotStartStop();

    saveSettings();
    d->listView->listView()->clear();
    e->accept();
}

void BatchDialog::slotClose()
{
    // Stop current conversion if necessary
    if (d->busy)
        slotStartStop();

    saveSettings();
    d->listView->listView()->clear();
    done(Close);
}

void BatchDialog::slotDefault()
{
    d->decodingSettingsBox->resetToDefault();
    d->saveSettingsBox->resetToDefault();
}

void BatchDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group  = config.group(QString("RawConverter Settings"));

    d->decodingSettingsBox->readSettings(group);
    d->saveSettingsBox->readSettings(group);
    d->saveSettingsBox->slotPopulateImageFormat(d->decodingSettingsBox->settings().sixteenBitsImage);

    KConfigGroup group2 = config.group(QString("Batch Raw Converter Dialog"));
    restoreDialogSize(group2);
}

void BatchDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group  = config.group(QString("RawConverter Settings"));

    d->decodingSettingsBox->writeSettings(group);
    d->saveSettingsBox->writeSettings(group);

    KConfigGroup group2 = config.group(QString("Batch Raw Converter Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void BatchDialog::slotStartStop()
{
    if (!d->busy)
    {
        d->fileList.clear();
        QTreeWidgetItemIterator it(d->listView->listView());

        while (*it)
        {
            MyImageListViewItem* const lvItem = dynamic_cast<MyImageListViewItem*>(*it);

            if (lvItem)
            {
                if (!lvItem->isDisabled() && (lvItem->state() != MyImageListViewItem::Success))
                {
                    lvItem->setIcon(1, QIcon());
                    lvItem->setState(MyImageListViewItem::Waiting);
                    d->fileList.append(lvItem->url().path());
                }
            }
            ++it;
        }

        if (d->fileList.empty())
        {
            KMessageBox::error(this, i18n("There is no RAW file in the list to process."));
            busy(false);
            slotAborted();
            return;
        }

        d->progressBar->setMaximum(d->fileList.count());
        d->progressBar->setValue(0);
        d->progressBar->show();
        d->progressBar->progressScheduled(i18n("RAW Converter"), true, true);
        d->progressBar->progressThumbnailChanged(KIcon("rawconverter").pixmap(22));

        d->thread->setSettings(d->decodingSettingsBox->settings(), d->saveSettingsBox->fileFormat());
        processAll();
    }
    else
    {
        d->fileList.clear();
        d->thread->cancel();
        busy(false);

        d->listView->cancelProcess();

        QTimer::singleShot(500, this, SLOT(slotAborted()));
    }
}

void BatchDialog::slotAborted()
{
    d->progressBar->setValue(0);
    d->progressBar->hide();
    d->progressBar->progressCompleted();
}

void BatchDialog::addItems(const KUrl::List& itemList)
{
    d->listView->slotAddImages(itemList);
}

void BatchDialog::slotIdentify() // Set Identity and Target file
{
    QString ext;

    switch(d->saveSettingsBox->fileFormat())
    {
        case KPSaveSettingsWidget::OUTPUT_JPEG:
            ext = ".jpg";
            break;
        case KPSaveSettingsWidget::OUTPUT_TIFF:
            ext = ".tif";
            break;
        case KPSaveSettingsWidget::OUTPUT_PPM:
            ext = ".ppm";
            break;
        case KPSaveSettingsWidget::OUTPUT_PNG:
            ext = ".png";
            break;
    }

    KUrl::List urlList = d->listView->imageUrls(true);

    for (KUrl::List::const_iterator  it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        QFileInfo fi((*it).path());

        if(d->saveSettingsBox->conflictRule() == KPSaveSettingsWidget::OVERWRITE)
        {
            QString dest              = fi.completeBaseName() + ext;
            MyImageListViewItem* item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->findItem(*it));
            if (item) item->setDestFileName(dest);
        }
        else
        {
            QString dest = fi.absolutePath() + QString("/") + fi.completeBaseName() + ext;
            QFileInfo a(dest);

            bool fileNotFound = (a.exists());

            if (!fileNotFound)
            {
                dest = fi.completeBaseName() + ext;
            }

            else
            {
                int i = 0;
                while(fileNotFound)
                {
                    a = QFileInfo(dest);

                    if (!a.exists())
                    {
                        fileNotFound = false;
                    }
                    else
                    {
                        i++;
                        dest = fi.absolutePath() + QString("/") + fi.completeBaseName() + QString("_") + QString::number(i) + ext;
                    }
                }

                dest = fi.completeBaseName() + QString("_") + QString::number(i) + ext;
            }

            MyImageListViewItem* const item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->findItem(*it));

            if (item)
            {
                item->setDestFileName(dest);
            }
        }
    }

    if (!urlList.empty())
    {
        d->thread->identifyRawFiles(urlList);

        if (!d->thread->isRunning())
            d->thread->start();
    }
}

void BatchDialog::processAll()
{
    d->thread->processRawFiles(d->listView->imageUrls(true));

    if (!d->thread->isRunning())
        d->thread->start();
}

void BatchDialog::slotThreadFinished()
{
    busy(false);
    slotAborted();
}

void BatchDialog::busy(bool busy)
{
    d->busy = busy;
    enableButton(User1, !d->busy);
    enableButton(User2, !d->busy);

    if (d->busy)
    {
        setButtonIcon(Apply,    KIcon("process-stop"));
        setButtonText(Apply,    i18n("&Abort"));
        setButtonToolTip(Apply, i18n("Abort the current RAW file conversion"));
    }
    else
    {
        setButtonIcon(Apply,    KIcon("system-run"));
        setButtonText(Apply,    i18n("Con&vert"));
        setButtonToolTip(Apply, i18n("Start converting the RAW images using current settings."));
    }

    d->decodingSettingsBox->setEnabled(!d->busy);
    d->saveSettingsBox->setEnabled(!d->busy);
    d->listView->listView()->viewport()->setEnabled(!d->busy);

    d->busy ? d->page->setCursor(Qt::WaitCursor) : d->page->unsetCursor();
}

void BatchDialog::processed(const KUrl& url, const QString& tmpFile)
{
    MyImageListViewItem* const item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->findItem(url));

    if (!item)
        return;

    QString destFile(item->destPath());

    if (d->saveSettingsBox->conflictRule() != KPSaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0)
        {
            item->setStatus(QString("Failed to save image"));
        }
    }

    if (!destFile.isEmpty())
    {
        if (KPMetadata::hasSidecar(tmpFile))
        {
            if (KDE::rename(KPMetadata::sidecarPath(tmpFile),
                            KPMetadata::sidecarPath(destFile)) != 0)
            {
                item->setStatus(QString("Failed to move sidecar"));
            }
        }

#ifndef Q_OS_WIN
        if (::rename(QFile::encodeName(tmpFile), QFile::encodeName(destFile)) != 0)
#else
        if (::MoveFileEx(tmpFile.utf16(), destFile.utf16(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == 0)
#endif
        {
            item->setStatus(QString("Failed to save image."));
            d->listView->processed(url, false);
        }
        else
        {
            item->setDestFileName(QFileInfo(destFile).fileName());
            d->listView->processed(url, true);
            item->setStatus(QString("Success"));

            // Assign Kipi host attributes from original RAW image.

            KPImageInfo info(url);
            info.cloneData(KUrl(destFile));
        }
    }

    d->progressBar->setValue(d->progressBar->value()+1);
}

void BatchDialog::processingFailed(const KUrl& url)
{
    d->listView->processed(url, false);
    d->progressBar->setValue(d->progressBar->value()+1);
}

void BatchDialog::slotAction(const KIPIRawConverterPlugin::ActionData& ad)
{
    QString text;

    if (ad.starting)            // Something have been started...
    {
        switch (ad.action)
        {
            case(IDENTIFY):
                break;
            case(PROCESS):
            {
                busy(true);
                d->listView->processing(ad.fileUrl);
                d->progressBar->progressStatusChanged(i18n("Processing %1", ad.fileUrl.fileName()));
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
                case(IDENTIFY):
                    break;
                case(PROCESS):
                {
                    processingFailed(ad.fileUrl);
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
                case(IDENTIFY):
                {
                    MyImageListViewItem* const item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->findItem(ad.fileUrl));

                    if (item)
                    {
                        item->setIdentity(ad.message);
                    }
                    break;
                }
                case(PROCESS):
                {
                    processed(ad.fileUrl, ad.destPath);
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
