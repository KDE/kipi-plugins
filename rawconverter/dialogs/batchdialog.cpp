/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-24
 * Description : Raw converter batch dialog
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011 by Veaceslav Munteanu <slavuttici at gmail dot com>
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
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidgetItemIterator>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kcursor.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kio/renamedialog.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/dcrawsettingswidget.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imageinfo.h>

// Local includes

#include "actions.h"
#include "actionthread.h"
#include "myimagelist.h"
#include "imagedialog.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "rawdecodingiface.h"
#include "savesettingswidget.h"

using namespace KDcrawIface;
using namespace KIPIPlugins;

namespace KIPIRawConverterPlugin
{

class BatchDialog::BatchDialogPriv
{
public:

    BatchDialogPriv()
    {
        busy                = false;
        page                = 0;
        progressBar         = 0;
        listView            = 0;
        thread              = 0;
        saveSettingsBox     = 0;
        decodingSettingsBox = 0;
        about               = 0;
        iface               = 0;
    }

    bool                 busy;

    QWidget*             page;

    QStringList          fileList;

    QProgressBar*        progressBar;

    MyImageList*         listView;

    ActionThread*        thread;

    SaveSettingsWidget*  saveSettingsBox;

    DcrawSettingsWidget* decodingSettingsBox;

    KPAboutData*         about;

    KIPI::Interface*     iface;
};

BatchDialog::BatchDialog(KIPI::Interface* iface)
           : KDialog(0), d(new BatchDialogPriv)
{
    d->iface = iface;

    setButtons(Help | Default | Apply | Close );
    setDefaultButton(Close);
    setButtonToolTip(Close, i18n("Exit RAW Converter"));
    setCaption(i18n("RAW Image Batch Converter"));
    setModal(false);

    d->page = new QWidget( this );
    setMainWidget( d->page );
    QGridLayout *mainLayout = new QGridLayout(d->page);

    //---------------------------------------------

    d->listView = new MyImageList(d->iface, d->page);

    // ---------------------------------------------------------------

    d->decodingSettingsBox = new DcrawSettingsWidget(d->page, DcrawSettingsWidget::SIXTEENBITS |
                                                              DcrawSettingsWidget::COLORSPACE |
                                                              DcrawSettingsWidget::POSTPROCESSING |
                                                              DcrawSettingsWidget::BLACKWHITEPOINTS);
    d->decodingSettingsBox->setObjectName("RawSettingsBox Expander");
    d->saveSettingsBox     = new SaveSettingsWidget(d->page);

#if KDCRAW_VERSION <= 0x000500
    d->decodingSettingsBox->addItem(d->saveSettingsBox, i18n("Save settings"));
    d->decodingSettingsBox->updateMinimumWidth();
#else
    d->decodingSettingsBox->insertItem(DcrawSettingsWidget::COLORMANAGEMENT+1, d->saveSettingsBox,
                                       SmallIcon("document-save"), i18n("Save settings"),
                                       QString("savesettings"), false);
#endif

    d->progressBar = new QProgressBar(d->page);
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
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("RAW Image Converter"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin to batch convert RAW images"),
                   ki18n("(c) 2003-2005, Renchi Raju\n"
                         "(c) 2006-2011, Gilles Caulier"));

    d->about->addAuthor(ki18n("Renchi Raju"),
                       ki18n("Author"),
                             "renchi dot raju at gmail dot com");

    d->about->addAuthor(ki18n("Gilles Caulier"),
                       ki18n("Developer and maintainer"),
                             "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction* handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ---------------------------------------------------------------

    d->thread = new ActionThread(this, d->iface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());

    // ---------------------------------------------------------------

    connect(d->saveSettingsBox, SIGNAL(signalSaveFormatChanged()),
            this, SLOT(slotSaveFormatChanged()));

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

    connect(d->thread, SIGNAL(starting(KIPIRawConverterPlugin::ActionData)),
            this, SLOT(slotAction(KIPIRawConverterPlugin::ActionData)));

    connect(d->thread, SIGNAL(finished(KIPIRawConverterPlugin::ActionData)),
            this, SLOT(slotAction(KIPIRawConverterPlugin::ActionData)));

    connect(d->listView, SIGNAL(signalImageListChanged()),
            this, SLOT(slotIdentify()));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
}

BatchDialog::~BatchDialog()
{
    delete d->about;
    delete d;
}

void BatchDialog::slotHelp()
{
    KToolInvocation::invokeHelp("rawconverter", "kipi-plugins");
}

void BatchDialog::slotSixteenBitsImageToggled(bool)
{
    // Dcraw do not provide a way to set brigness of image in 16 bits color depth.
    // We always set on this option. We drive brightness adjustment as post processing.
    d->decodingSettingsBox->setEnabledBrightnessSettings(true);
}

void BatchDialog::closeEvent(QCloseEvent* e)
{
    if (!e) return;

    // Stop current conversion if necessary
    if (d->busy) slotStartStop();
    saveSettings();
    d->listView->listView()->clear();
    e->accept();
}

void BatchDialog::slotClose()
{
    // Stop current conversion if necessary
    if (d->busy) slotStartStop();
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
            MyImageListViewItem* lvItem = dynamic_cast<MyImageListViewItem*>(*it);
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

        d->thread->setRawDecodingSettings(d->decodingSettingsBox->settings(), d->saveSettingsBox->fileFormat());
        processOne();
    }
    else
    {
        d->fileList.clear();
        d->thread->cancel();
        busy(false);

        d->listView->processed(false);

        QTimer::singleShot(500, this, SLOT(slotAborted()));
    }
}

void BatchDialog::slotAborted()
{
    d->progressBar->setValue(0);
    d->progressBar->hide();
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
        case SaveSettingsWidget::OUTPUT_JPEG:
            ext = ".jpg";
            break;
        case SaveSettingsWidget::OUTPUT_TIFF:
            ext = ".tif";
            break;
        case SaveSettingsWidget::OUTPUT_PPM:
            ext = ".ppm";
            break;
        case SaveSettingsWidget::OUTPUT_PNG:
            ext = ".png";
            break;
    }
    KUrl::List urlList = d->listView->imageUrls(true);

    for (KUrl::List::const_iterator  it = urlList.constBegin();
         it != urlList.constEnd(); ++it)
    {
        QFileInfo fi((*it).path());
        QString dest              = fi.completeBaseName() + ext;
        MyImageListViewItem* item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->findItem(*it));
        if (item) item->setDestFileName(dest);
    }

    if (!urlList.empty())
    {
        d->thread->identifyRawFiles(urlList);
        if (!d->thread->isRunning())
            d->thread->start();
    }
}
void BatchDialog::slotSaveFormatChanged()
{
    QString ext;

    switch(d->saveSettingsBox->fileFormat())
    {
        case SaveSettingsWidget::OUTPUT_JPEG:
            ext = "jpg";
            break;
        case SaveSettingsWidget::OUTPUT_TIFF:
            ext = "tif";
            break;
        case SaveSettingsWidget::OUTPUT_PPM:
            ext = "ppm";
            break;
        case SaveSettingsWidget::OUTPUT_PNG:
            ext = "png";
            break;
    }

    QTreeWidgetItemIterator it(d->listView->listView());
    while (*it)
    {
        MyImageListViewItem* lvItem = dynamic_cast<MyImageListViewItem*>(*it);
        if (lvItem)
        {
            if (!lvItem->isDisabled())
            {
                QFileInfo fi(lvItem->url().path());
                QString dest = fi.completeBaseName() + QString(".") + ext;
                lvItem->setDestFileName(dest);
            }
        }
        ++it;
    }
}

void BatchDialog::processOne()
{
    if (d->fileList.empty())
    {
        busy(false);
        slotAborted();
        return;
    }

    QString file(d->fileList.first());
    d->fileList.pop_front();

    d->thread->processRawFile(KUrl(file));
    if (!d->thread->isRunning())
        d->thread->start();
}

void BatchDialog::busy(bool busy)
{
    d->busy = busy;
    enableButton(User1, !d->busy);
    enableButton(User2, !d->busy);

    if (d->busy)
    {
        setButtonIcon(Apply, KIcon("process-stop"));
        setButtonText(Apply, i18n("&Abort"));
        setButtonToolTip(Apply, i18n("Abort the current RAW file conversion"));
    }
    else
    {
        setButtonIcon(Apply, KIcon("system-run"));
        setButtonText(Apply, i18n("Con&vert"));
        setButtonToolTip(Apply, i18n("Start converting the RAW images using current settings."));
    }

    d->decodingSettingsBox->setEnabled(!d->busy);
    d->saveSettingsBox->setEnabled(!d->busy);
    d->listView->listView()->viewport()->setEnabled(!d->busy);

    d->busy ? d->page->setCursor(Qt::WaitCursor) : d->page->unsetCursor();
}

void BatchDialog::processed(const KUrl& url, const QString& tmpFile)
{
    MyImageListViewItem* item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->findItem(url));
    if (!item) return;
    QString destFile(item->destPath());

    if (d->saveSettingsBox->conflictRule() != SaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0)
        {
            KIO::RenameDialog dlg(this, i18n("Save RAW image converted from '%1' as",
                                  url.fileName()),
                                  tmpFile, destFile,
                                  KIO::RenameDialog_Mode(KIO::M_SINGLE | KIO::M_OVERWRITE | KIO::M_SKIP));

            switch (dlg.exec())
            {
                case KIO::R_CANCEL:
                case KIO::R_SKIP:
                {
                    destFile.clear();
                    d->listView->processed(false);
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
        if (::rename(QFile::encodeName(tmpFile), QFile::encodeName(destFile)) != 0)
        {
            KMessageBox::error(this, i18n("Failed to save image %1", destFile));
            d->listView->processed(false);
        }
        else
        {
            item->setDestFileName(QFileInfo(destFile).fileName());
            d->listView->processed(true);

            // Assign Kipi host attributes from original RAW image.

            KIPI::ImageInfo orgInfo = d->iface->info(url);
            KIPI::ImageInfo newInfo = d->iface->info(KUrl(destFile));
            newInfo.cloneData(orgInfo);
        }
    }

    d->progressBar->setValue(d->progressBar->value()+1);
}

void BatchDialog::processingFailed(const KUrl& /*url*/)
{
    d->listView->processed(false);
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
                break;
            }
            default:
            {
                kWarning() << "KIPIRawConverterPlugin: Unknown action";
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
                    processOne();
                    break;
                }
                default:
                {
                    kWarning() << "KIPIRawConverterPlugin: Unknown action";
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
                    MyImageListViewItem* item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->findItem(ad.fileUrl));
                    if (item)
                    {
                        item->setIdentity(ad.message);
                    }
                    break;
                }
                case(PROCESS):
                {
                    processed(ad.fileUrl, ad.destPath);
                    processOne();
                    break;
                }
                default:
                {
                    kWarning() << "KIPIRawConverterPlugin: Unknown action";
                    break;
                }
            }
        }
    }
}

} // namespace KIPIRawConverterPlugin
