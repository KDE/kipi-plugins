/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-24
 * Description : DNG converter batch dialog
 *
 * Copyright (C) 2008-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Jens Mueller <tschenser at gmx dot de>
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

// LibKIPI includes

#include <libkipi/imageinfo.h>
#include <libkipi/interface.h>

// Local includes

#include "aboutdata.h"
#include "actions.h"
#include "actionthread.h"
#include "myimagelist.h"
#include "dngwriter.h"
#include "imagedialog.h"
#include "pluginsversion.h"
#include "settingswidget.h"

using namespace DNGIface;
using namespace KIPIPlugins;

namespace KIPIDNGConverterPlugin
{

class BatchDialog::BatchDialogPriv
{
public:

    BatchDialogPriv()
    {
        busy               = false;
        page               = 0;
        progressBar        = 0;
        listView           = 0;
        thread             = 0;
        settingsBox        = 0;
        iface              = 0;
        about              = 0;
    }

    bool                   busy;

    QWidget*               page;

    QStringList            fileList;

    QProgressBar*          progressBar;

    MyImageList*           listView;

    ActionThread*          thread;

    SettingsWidget*        settingsBox;

    KIPI::Interface*       iface;

    DNGConverterAboutData* about;
};

BatchDialog::BatchDialog(KIPI::Interface* iface, DNGConverterAboutData* about)
           : KDialog(0), d(new BatchDialogPriv)
{
    d->iface = iface;
    d->about = about;

    setWindowIcon(KIcon("dngconverter"));
    setButtons(Help | Default | Apply | Close | User1 | User2);
    setDefaultButton(KDialog::Close);
    setButtonToolTip(Close, i18n("Exit DNG Converter"));
    setCaption(i18n("Batch convert RAW camera images to DNG"));
    setModal(false);
    setButtonIcon(User1, KIcon("list-add"));
    setButtonText(User1, i18n("&Add"));
    setButtonToolTip(User1, i18n("Add new Raw files to the list"));
    setButtonIcon(User2, KIcon("list-remove"));
    setButtonText(User2, i18n("&Remove"));
    setButtonToolTip(User2, i18n("Remove selected Raw files from the list"));

    d->page = new QWidget( this );
    setMainWidget( d->page );
    QGridLayout* mainLayout = new QGridLayout(d->page);

    //---------------------------------------------

    d->listView = new MyImageList(d->iface, d->page);

    // ---------------------------------------------------------------

    d->settingsBox = new SettingsWidget(d->page);

    d->progressBar = new QProgressBar(d->page);
    d->progressBar->setMaximumHeight( fontMetrics().height()+2 );
    d->progressBar->hide();

    mainLayout->addWidget(d->listView,    0, 0, 3, 1);
    mainLayout->addWidget(d->settingsBox, 0, 1, 1, 1);
    mainLayout->addWidget(d->progressBar, 1, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(2, 10);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());

    // ---------------------------------------------------------------
    // About data and help button.

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

    d->thread            = new ActionThread(this);

    // ---------------------------------------------------------------

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(defaultClicked()),
            this, SLOT(slotDefault()));

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotStartStop()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotAddItems()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotRemoveItems()));

    connect(d->thread, SIGNAL(starting(const KIPIDNGConverterPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIDNGConverterPlugin::ActionData&)));

    connect(d->thread, SIGNAL(finished(const KIPIDNGConverterPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIDNGConverterPlugin::ActionData&)));

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
    KToolInvocation::invokeHelp("dngconverter", "kipi-plugins");
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
    d->settingsBox->setDefaultSettings();
}

void BatchDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("DNGConverter Settings"));

    d->settingsBox->setBackupOriginalRawFile(group.readEntry("BackupOriginalRawFile", false));
    d->settingsBox->setCompressLossLess(group.readEntry("CompressLossLess", true));
    d->settingsBox->setUpdateFileDate(group.readEntry("UpdateFileDate", false));
    d->settingsBox->setCompressLossLess(group.readEntry("PreviewMode", (int)(DNGWriter::MEDIUM)));
    d->settingsBox->setConflictRule(
        (SettingsWidget::ConflictRule)group.readEntry("Conflict",
            (int)(SettingsWidget::OVERWRITE)));

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
            KMessageBox::error(this, i18n("The list does not contain any Raw files to process."));
            busy(false);
            slotAborted();
            return;
        }

        d->progressBar->setMaximum(d->fileList.count());
        d->progressBar->setValue(0);
        d->progressBar->show();

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

void BatchDialog::slotAddItems()
{
    KIPIPlugins::ImageDialog dlg(this, d->iface, false, true);
    KUrl::List urls = dlg.urls();
    if (!urls.isEmpty())
    {
        addItems(urls);
    }
}

void BatchDialog::slotRemoveItems()
{
    bool find;
    do
    {
        find = false;
        QTreeWidgetItemIterator it(d->listView->listView());
        while (*it)
        {
            MyImageListViewItem* item = dynamic_cast<MyImageListViewItem*>(*it);
            if (item->isSelected())
            {
                delete item;
                find = true;
                break;
            }
        ++it;
        }
    }
    while(find);
}

void BatchDialog::slotAborted()
{
    d->progressBar->setValue(0);
    d->progressBar->hide();
}

void BatchDialog::addItems(const KUrl::List& itemList)
{
    d->listView->slotAddImages(itemList);
    KUrl::List urlList = d->listView->imageUrls(true);
    kDebug() << urlList;

    for (KUrl::List::const_iterator  it = urlList.constBegin();
         it != urlList.constEnd(); ++it)
    {
        QFileInfo fi((*it).path());
        QString dest              = fi.completeBaseName() + QString(".dng");
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

    d->thread->setBackupOriginalRawFile(d->settingsBox->backupOriginalRawFile());
    d->thread->setCompressLossLess(d->settingsBox->compressLossLess());
    d->thread->setPreviewMode(d->settingsBox->previewMode());
    d->thread->setUpdateFileDate(d->settingsBox->updateFileDate());
    d->thread->processRawFile(KUrl(file));
    if (!d->thread->isRunning())
    {
        d->thread->start();
    }
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
        setButtonToolTip(Apply, i18n("Abort the conversion of Raw files."));
    }
    else
    {
        setButtonIcon(Apply, KIcon("system-run"));
        setButtonText(Apply, i18n("Con&vert"));
        setButtonToolTip(Apply, i18n("Start converting the Raw images using the current settings."));
    }

    d->settingsBox->setEnabled(!d->busy);
    d->listView->listView()->viewport()->setEnabled(!d->busy);

    d->busy ? d->page->setCursor(Qt::WaitCursor) : d->page->unsetCursor();
}

void BatchDialog::processed(const KUrl& url, const QString& tmpFile)
{
    MyImageListViewItem* item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->findItem(url));
    if (!item) return;
    QString destFile(item->destPath());

    if (d->settingsBox->conflictRule() != SettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0)
        {
            KIO::RenameDialog dlg(this, i18n("Save Raw Image converted from '%1' as",
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

            if (d->iface)
            {
                // Assign Kipi host attributes from original RAW image.

                KIPI::ImageInfo orgInfo = d->iface->info(url);
                KIPI::ImageInfo newInfo = d->iface->info(KUrl(destFile));
                newInfo.cloneData(orgInfo);
            }
        }
    }

    d->progressBar->setValue(d->progressBar->value()+1);
}

void BatchDialog::processingFailed(const KUrl& /*url*/)
{
    d->listView->processed(false);
    d->progressBar->setValue(d->progressBar->value()+1);
}

void BatchDialog::slotAction(const KIPIDNGConverterPlugin::ActionData& ad)
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
                kWarning() << "KIPIDNGConverterPlugin: Unknown action";
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
                    kWarning() << "KIPIDNGConverterPlugin: Unknown action";
                    break;
                }
            }
        }
    }
}

} // namespace KIPIDNGConverterPlugin
