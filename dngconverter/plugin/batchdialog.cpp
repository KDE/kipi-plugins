/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : DNG converter batch dialog
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2008-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Jens Mueller <tschenser at gmx dot de>
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
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kio/renamedialog.h>
#include <kde_file.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>

// Local includes

#include "aboutdata.h"
#include "actions.h"
#include "actionthread.h"
#include "myimagelist.h"
#include "dngwriter.h"
#include "settingswidget.h"
#include "kpimagedialog.h"
#include "kpversion.h"
#include "kpprogresswidget.h"
#include "kpimageinfo.h"

using namespace DNGIface;

namespace KIPIDNGConverterPlugin
{

class BatchDialog::Private
{
public:

    Private()
    {
        busy        = false;
        page        = 0;
        progressBar = 0;
        listView    = 0;
        thread      = 0;
        settingsBox = 0;
    }

    bool              busy;

    QWidget*          page;

    QStringList       fileList;

    KPProgressWidget* progressBar;

    MyImageList*      listView;

    ActionThread*     thread;

    SettingsWidget*   settingsBox;
};

BatchDialog::BatchDialog(DNGConverterAboutData* const about)
    : KPToolDialog(0), d(new Private)
{
    setWindowIcon(KIcon("kipi-dngconverter"));
    setButtons(Help | Default | User1 | Close);
    setDefaultButton(KDialog::Close);
    setButtonToolTip(Close, i18n("Exit DNG Converter"));
    setCaption(i18n("Batch convert RAW camera images to DNG"));
    setModal(false);
    setAboutData(about);

    d->page = new QWidget( this );
    setMainWidget( d->page );
    QGridLayout* const mainLayout = new QGridLayout(d->page);

    //---------------------------------------------

    d->listView    = new MyImageList(d->page);
    d->settingsBox = new SettingsWidget(d->page);
    d->progressBar = new KPProgressWidget(d->page);
    d->progressBar->setMaximumHeight(fontMetrics().height()+2);
    d->progressBar->hide();

    mainLayout->addWidget(d->listView,    0, 0, 3, 1);
    mainLayout->addWidget(d->settingsBox, 0, 1, 1, 1);
    mainLayout->addWidget(d->progressBar, 1, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(2, 10);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());

    // ---------------------------------------------------------------

    d->thread = new ActionThread(this);

    connect(d->thread, SIGNAL(signalStarting(KIPIDNGConverterPlugin::ActionData)),
            this, SLOT(slotAction(KIPIDNGConverterPlugin::ActionData)));

    connect(d->thread, SIGNAL(signalFinished(KIPIDNGConverterPlugin::ActionData)),
            this, SLOT(slotAction(KIPIDNGConverterPlugin::ActionData)));

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(slotThreadFinished()));

    // ---------------------------------------------------------------

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(defaultClicked()),
            this, SLOT(slotDefault()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotStartStop()));

    connect(d->listView, SIGNAL(signalImageListChanged()),
            this, SLOT(slotIdentify()));

    connect(d->progressBar, SIGNAL(signalProgressCanceled()),
            this, SLOT(slotStartStop()));

    connect(d->settingsBox, SIGNAL(buttonChanged(int)),
            this, SLOT(slotIdentify()));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
}

BatchDialog::~BatchDialog()
{
    delete d;
}

void BatchDialog::closeEvent(QCloseEvent* e)
{
    if (!e) return;

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
    d->fileList.clear();
    done(Close);
}

void BatchDialog::slotDefault()
{
    d->settingsBox->setDefaultSettings();
}

void BatchDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("DNGConverter Settings"));

    d->settingsBox->setBackupOriginalRawFile(group.readEntry("BackupOriginalRawFile",         false));
    d->settingsBox->setCompressLossLess(group.readEntry("CompressLossLess",                   true));
    d->settingsBox->setUpdateFileDate(group.readEntry("UpdateFileDate",                       false));
    d->settingsBox->setCompressLossLess(group.readEntry("PreviewMode",                        (int)(DNGWriter::MEDIUM)));
    d->settingsBox->setConflictRule((SettingsWidget::ConflictRule)group.readEntry("Conflict", (int)(SettingsWidget::OVERWRITE)));

    KConfigGroup group2 = config.group(QString("Batch DNG Converter Dialog"));
    restoreDialogSize(group2);
}

void BatchDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("DNGConverter Settings"));

    group.writeEntry("BackupOriginalRawFile", d->settingsBox->backupOriginalRawFile());
    group.writeEntry("CompressLossLess",      d->settingsBox->compressLossLess());
    group.writeEntry("UpdateFileDate",        d->settingsBox->updateFileDate());
    group.writeEntry("PreviewMode",           (int)d->settingsBox->previewMode());
    group.writeEntry("Conflict",              (int)d->settingsBox->conflictRule());

    KConfigGroup group2 = config.group(QString("Batch DNG Converter Dialog"));
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
            KMessageBox::error(this, i18n("The list does not contain any Raw files to process."));
            busy(false);
            slotAborted();
            return;
        }

        d->progressBar->setMaximum(d->fileList.count());
        d->progressBar->setValue(0);
        d->progressBar->show();
        d->progressBar->progressScheduled(i18n("DNG Converter"), true, true);
        d->progressBar->progressThumbnailChanged(KIcon("kipi-dngconverter").pixmap(22));

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

void BatchDialog::addItems(const KUrl::List& itemList)
{
    d->listView->slotAddImages(itemList);
}

void BatchDialog::slotAborted()
{
    d->progressBar->setValue(0);
    d->progressBar->hide();
    d->progressBar->progressCompleted();
}

void BatchDialog::slotIdentify()
{
    KUrl::List urlList = d->listView->imageUrls(true);

    for (KUrl::List::const_iterator  it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        QFileInfo fi((*it).path());

        if(d->settingsBox->conflictRule() == SettingsWidget::OVERWRITE)
        {
            QString dest                    = fi.completeBaseName() + QString(".dng");
            MyImageListViewItem* const item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->findItem(*it));

            if (item)
                item->setDestFileName(dest);
        }

        else
        {
            QString dest = fi.absolutePath() + QString("/") + fi.completeBaseName() + QString(".dng");
            QFileInfo a(dest);
            bool fileNotFound = (a.exists());

            if (!fileNotFound)
            {
                dest = fi.completeBaseName() + QString(".dng");
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
                        dest = fi.absolutePath() + QString("/") + fi.completeBaseName() + QString("_") + QString::number(i) + QString(".dng");
                    }
                }

                dest = fi.completeBaseName() + QString("_") + QString::number(i) + QString(".dng");
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
    d->thread->setBackupOriginalRawFile(d->settingsBox->backupOriginalRawFile());
    d->thread->setCompressLossLess(d->settingsBox->compressLossLess());
    d->thread->setPreviewMode(d->settingsBox->previewMode());
    d->thread->setUpdateFileDate(d->settingsBox->updateFileDate());
    d->thread->processRawFiles(d->listView->imageUrls(true));

    if (!d->thread->isRunning())
    {
        d->thread->start();
    }
}

void BatchDialog::slotThreadFinished()
{
    busy(false);
    slotAborted();
}

void BatchDialog::busy(bool busy)
{
    d->busy = busy;

    if (d->busy)
    {
        setButtonIcon(User1,    KIcon("process-stop"));
        setButtonText(User1,    i18n("&Abort"));
        setButtonToolTip(User1, i18n("Abort the conversion of Raw files."));
    }
    else
    {
        setButtonIcon(User1,    KIcon("system-run"));
        setButtonText(User1,    i18n("Con&vert"));
        setButtonToolTip(User1, i18n("Start converting the Raw images using the current settings."));
    }

    d->settingsBox->setEnabled(!d->busy);
    d->listView->listView()->viewport()->setEnabled(!d->busy);
    d->busy ? d->page->setCursor(Qt::WaitCursor) : d->page->unsetCursor();
}

void BatchDialog::processed(const KUrl& url, const QString& tmpFile)
{
    MyImageListViewItem* const item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->findItem(url));

    if (!item)
        return;

    QString destFile(item->destPath());

    if (d->settingsBox->conflictRule() != SettingsWidget::OVERWRITE)
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
            if (!KPMetadata::moveSidecar(KUrl(tmpFile), KUrl(destFile)))
            {
                item->setStatus(QString("Failed to move sidecar"));
            }
        }

        if (KDE::rename(tmpFile, destFile) != 0)
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

void BatchDialog::processingFailed(const KUrl& url, int result)
{
    d->listView->processed(url, false);
    d->progressBar->setValue(d->progressBar->value()+1);

    MyImageListViewItem* const item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->findItem(url));

    if (!item)
        return;

    QString status;

    switch (result)
    {
        case DNGWriter::PROCESSFAILED:
            status = i18n("Process failed");
            break;
        case DNGWriter::PROCESSCANCELED:
            status = i18n("Process Canceled");
            break;
        case DNGWriter::FILENOTSUPPORTED:
            status = i18n("File not supported");
            break;
        default:
            status = i18n("Internal error");
            break;
    }

    item->setStatus(status);
}

void BatchDialog::slotAction(const KIPIDNGConverterPlugin::ActionData& ad)
{
    QString text;

    if (ad.starting)            // Something have been started...
    {
        switch (ad.action)
        {
            case(IDENTIFY):
            {
                break;
            }
            case(PROCESS):
            {
                busy(true);
                d->listView->processing(ad.fileUrl);
                d->progressBar->progressStatusChanged(i18n("Processing %1", ad.fileUrl.fileName()));
                break;
            }
            default:
            {
                kWarning() << "KIPIDNGConverterPlugin: Unknown action";
                break;
            }
        }
    }
    else
    {
        if (ad.result != DNGWriter::PROCESSCOMPLETE)        // Something is failed...
        {
            switch (ad.action)
            {
                case(IDENTIFY):
                {
                    break;
                }
                case(PROCESS):
                {
                    processingFailed(ad.fileUrl, ad.result);
                    break;
                }
                default:
                {
                    kWarning() << "KIPIDNGConverterPlugin: Unknown action";
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
                    kWarning() << "KIPIDNGConverterPlugin: Unknown action";
                    break;
                }
            }
        }
    }
}

} // namespace KIPIDNGConverterPlugin
