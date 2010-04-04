/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-24
 * Description : DNG converter batch dialog
 *
 * Copyright (C) 2008-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010 by Jens Mueller <tschenser at gmx dot de>
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
#include <QHeaderView>
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
#include "clistviewitem.h"
#include "dngwriter.h"
#include "imagedialog.h"
#include "pluginsversion.h"
#include "settingswidget.h"

using namespace DNGIface;
using namespace KIPIPlugins;

namespace KIPIDNGConverterPlugin
{

class BatchDialogPriv
{
public:

    BatchDialogPriv()
    {
        busy                = false;
        convertBlink        = false;
        blinkConvertTimer   = 0;
        page                = 0;
        progressBar         = 0;
        listView            = 0;
        currentConvertItem  = 0;
        thread              = 0;
        settingsBox         = 0;
        iface               = 0;
        about               = 0;
    }

    bool                   busy;
    bool                   convertBlink;

    QTimer                *blinkConvertTimer;

    QWidget               *page;

    QStringList            fileList;

    QProgressBar          *progressBar;

    QTreeWidget           *listView;

    CListViewItem         *currentConvertItem;

    ActionThread          *thread;

    SettingsWidget        *settingsBox;

    KIPI::Interface       *iface;

    DNGConverterAboutData *about;
};

BatchDialog::BatchDialog(KIPI::Interface* iface, DNGConverterAboutData *about)
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
    QGridLayout *mainLayout = new QGridLayout(d->page);

    //---------------------------------------------

    d->listView = new QTreeWidget(d->page);
    d->listView->setColumnCount(3);
    d->listView->setIconSize(QSize(64, 64));
    d->listView->setRootIsDecorated(false);
    d->listView->setSortingEnabled(false);
    d->listView->setSelectionMode(QAbstractItemView::MultiSelection);
    d->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->listView->setAllColumnsShowFocus(true);
    d->listView->setMinimumWidth(450);

    QStringList labels;
    labels.append( i18n("Thumbnail") );
    labels.append( i18n("Raw File") );
    labels.append( i18n("Target File") );
    labels.append( i18n("Camera") );
    d->listView->setHeaderLabels(labels);
    d->listView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    d->listView->header()->setResizeMode(1, QHeaderView::Stretch);
    d->listView->header()->setResizeMode(2, QHeaderView::Stretch);
    d->listView->header()->setResizeMode(3, QHeaderView::Stretch);

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
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ---------------------------------------------------------------

    d->blinkConvertTimer = new QTimer(this);
    d->thread            = new ActionThread(this);

    // ---------------------------------------------------------------

    connect(d->blinkConvertTimer, SIGNAL(timeout()),
            this, SLOT(slotConvertBlinkTimerDone()));

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

    if (d->iface)
    {
        connect(d->iface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
                this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));
    }

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

void BatchDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;

    // Stop current conversion if necessary
    if (d->busy) slotStartStop();
    saveSettings();
    d->listView->clear();
    e->accept();
}

void BatchDialog::slotClose()
{
    // Stop current conversion if necessary
    if (d->busy) slotStartStop();
    saveSettings();
    d->listView->clear();
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

        QTreeWidgetItemIterator it(d->listView);
        while (*it)
        {
            CListViewItem *lvItem = dynamic_cast<CListViewItem*>(*it);
            if (lvItem)
            {
                if (lvItem->isEnabled())
                {
                    lvItem->setIcon(1, QIcon());
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
        d->blinkConvertTimer->stop();
        d->fileList.clear();
        d->thread->cancel();
        busy(false);

        if (d->currentConvertItem)
            d->currentConvertItem->setIcon(1, SmallIcon("dialog-cancel"));

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
        QTreeWidgetItemIterator it(d->listView);
        while (*it)
        {
            CListViewItem* item = dynamic_cast<CListViewItem*>(*it);
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
    KUrl::List urlList;

    QPixmap pix(SmallIcon("image-x-generic", KIconLoader::SizeLarge, KIconLoader::DisabledState));

    for (KUrl::List::const_iterator  it = itemList.constBegin();
         it != itemList.constEnd(); ++it)
    {
        KUrl url = *it;
        QFileInfo fi(url.path());
        if (fi.exists() && !findItem(url))
        {
            QString dest = fi.completeBaseName() + QString(".dng");
            new CListViewItem(d->listView, pix, url, dest);
            urlList.append(url);
        }
    }

    if (!urlList.empty())
    {
        if (!d->iface || !d->iface->hasFeature(KIPI::HostSupportsThumbnails))
            d->thread->thumbRawFiles(urlList);
        else
            d->iface->thumbnails(urlList, 256);

        d->thread->identifyRawFiles(urlList);
        if (!d->thread->isRunning())
            d->thread->start();
    }
}

void BatchDialog::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    CListViewItem *item = findItem(url);
    if (item)
    {
        if (!pix.isNull())
        {
            QPixmap pixmap = pix.scaled(64, 64, Qt::KeepAspectRatio);
            item->setThumbnail(pixmap);
        }
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
    d->listView->viewport()->setEnabled(!d->busy);

    d->busy ? d->page->setCursor(Qt::WaitCursor) : d->page->unsetCursor();
}

void BatchDialog::slotConvertBlinkTimerDone()
{
    if(d->convertBlink)
    {
        if (d->currentConvertItem)
            d->currentConvertItem->setProgressIcon(SmallIcon("arrow-right"));
    }
    else
    {
        if (d->currentConvertItem)
            d->currentConvertItem->setProgressIcon(SmallIcon("arrow-right-double"));
    }

    d->convertBlink = !d->convertBlink;
    d->blinkConvertTimer->start(500);
}

void BatchDialog::processing(const KUrl& url)
{
    d->currentConvertItem = findItem(url);
    if (d->currentConvertItem)
    {
        d->listView->setCurrentItem(d->currentConvertItem, true);
        d->listView->scrollToItem(d->currentConvertItem);
    }

    d->convertBlink = false;
    d->blinkConvertTimer->start(500);
}

void BatchDialog::processed(const KUrl& url, const QString& tmpFile)
{
    d->blinkConvertTimer->stop();
    QString destFile(d->currentConvertItem->destPath());

    if (d->settingsBox->conflictRule() != SettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0)
        {
            KIO::RenameDialog dlg(this, i18n("Save Raw Image converted from '%1' as",
                                  d->currentConvertItem->url().fileName()),
                                  tmpFile, destFile,
                                  KIO::RenameDialog_Mode(KIO::M_SINGLE | KIO::M_OVERWRITE | KIO::M_SKIP));

            switch (dlg.exec())
            {
                case KIO::R_CANCEL:
                case KIO::R_SKIP:
                {
                    destFile.clear();
                    d->currentConvertItem->setProgressIcon(SmallIcon("dialog-cancel"));
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
            d->currentConvertItem->setProgressIcon(SmallIcon("dialog-error"));
        }
        else
        {
            d->currentConvertItem->setDestFileName(QFileInfo(destFile).fileName());
            d->currentConvertItem->setProgressIcon(SmallIcon("dialog-ok"));

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
    d->currentConvertItem = 0;
}

void BatchDialog::processingFailed(const KUrl& /*url*/)
{
    d->currentConvertItem->setProgressIcon(SmallIcon("dialog-cancel"));
    d->progressBar->setValue(d->progressBar->value()+1);
    d->currentConvertItem = 0;
}

void BatchDialog::slotAction(const KIPIDNGConverterPlugin::ActionData& ad)
{
    QString text;

    if (ad.starting)            // Something have been started...
    {
        switch (ad.action)
        {
            case(IDENTIFY):
            case(THUMBNAIL):
                break;
            case(PROCESS):
            {
                busy(true);
                processing(ad.fileUrl);
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
                case(THUMBNAIL):
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
                    CListViewItem *item = findItem(ad.fileUrl);
                    if (item)
                    {
                        item->setIdentity(ad.message);
                    }
                    break;
                }
                case(THUMBNAIL):
                {
                    CListViewItem *item = findItem(ad.fileUrl);
                    if (item)
                    {
                        if (!ad.image.isNull())
                        {
                            QPixmap pix = QPixmap::fromImage(ad.image.scaled(64, 64, Qt::KeepAspectRatio));
                            item->setThumbnail(pix);
                        }
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

CListViewItem* BatchDialog::findItem(const KUrl& url)
{
    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        CListViewItem *lvItem = dynamic_cast<CListViewItem*>(*it);
        if (lvItem)
        {
            if (lvItem->url() == url)
            {
                return lvItem;
            }
        }
        ++it;
    }

    return 0;
}

} // namespace KIPIDNGConverterPlugin
