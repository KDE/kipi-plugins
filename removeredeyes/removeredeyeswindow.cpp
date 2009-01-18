/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : the main window of the removeredeyes plugin
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "removeredeyeswindow.h"
#include "removeredeyeswindow.moc"

// Qt includes.

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

// KDE includes.

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktabwidget.h>
#include <ktemporaryfile.h>
#include <ktoolinvocation.h>

// LibKIPI includes.

#include <libkipi/interface.h>

// Local includes.

#include "SaveMethodFactory.h"
#include "kpaboutdata.h"
#include "myimageslist.h"
#include "previewwidget.h"
#include "removalsettings.h"
#include "settingstab.h"
#include "simplesettings.h"
#include "storagesettingsbox.h"
#include "unprocessedsettingsbox.h"
#include "workerthread.h"
#include "workerthreaddata.h"

namespace KIPIRemoveRedEyesPlugin
{

struct RedEyesWindowPriv
{
    RedEyesWindowPriv()
    {
        interface           = 0;
        about               = 0;
        progress            = 0;
        tabWidget           = 0;
        imageList           = 0;
        thread              = 0;
        settingsTab         = 0;
        previewWidget       = 0;
    }

    bool                      busy;
    int                       runtype;

    QProgressBar*             progress;

    KTabWidget*               tabWidget;

    KTemporaryFile            originalImageTempFile;
    KTemporaryFile            correctedImageTempFile;
    KTemporaryFile            maskImageTempFile;

    MyImagesList*             imageList;
    PreviewWidget*            previewWidget;
    RemovalSettings           settings;
    SettingsTab*              settingsTab;
    WorkerThread*             thread;

    KIPI::Interface*          interface;
    KIPIPlugins::KPAboutData* about;
};

RemoveRedEyesWindow::RemoveRedEyesWindow(KIPI::Interface *interface)
                   : KDialog(0), d(new RedEyesWindowPriv)
{
    setWindowTitle(i18n("Automatic Red Eyes Removal"));
    setButtons(Help|User1|User2|Close);
    setDefaultButton(Close);
    setModal(false);

    d->busy          = false;

    d->thread        = new WorkerThread(this);
    d->runtype       = WorkerThread::Testrun;
    d->interface     = interface;
    d->tabWidget     = new KTabWidget;

    d->imageList     = new MyImagesList(interface);
    d->previewWidget = new PreviewWidget;

    d->progress      = new QProgressBar;
    d->progress->hide();

    // ----------------------------------------------------------

    QString suffix(".jpg");
    d->originalImageTempFile.setSuffix(suffix);
    d->correctedImageTempFile.setSuffix(suffix);
    d->maskImageTempFile.setSuffix(suffix);

    // ----------------------------------------------------------

    d->about = new KIPIPlugins::KPAboutData(ki18n("Remove Red-Eyes"),
                                            0,
                                            KAboutData::License_GPL,
                                            ki18n("A plugin to automatically "
                                                  "detect and remove red-eyes"),
                                            ki18n("(c) 2008, Andi Clemens"));

    d->about->addAuthor(ki18n("Andi Clemens"), ki18n("Author and Maintainer"),
                              "andi dot clemens at gmx dot net");

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook = new QAction(i18n("Plugin Handbook"), this);

    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setDelayedMenu(helpMenu->menu());

    // ----------------------------------------------------------

    KGuiItem correctBtn = KStandardGuiItem::ok();
    correctBtn.setText(i18n("Correct &Photos"));
    correctBtn.setToolTip(i18n("Start correcting the listed images"));
    setButtonGuiItem(User1, correctBtn);

    KGuiItem testrunBtn = KStandardGuiItem::apply();
    testrunBtn.setText(i18n("&Testrun"));
    testrunBtn.setToolTip(i18n("Simulate the correction process, without saving the results."));
    setButtonGuiItem(User2, testrunBtn);

    // ----------------------------------------------------------

    d->settingsTab = new SettingsTab;

    // ----------------------------------------------------------

    QWidget* imagesTab           = new QWidget;
    QVBoxLayout* imagesTabLayout = new QVBoxLayout;
    imagesTabLayout->addWidget(d->imageList);
    imagesTab->setLayout(imagesTabLayout);

    // ----------------------------------------------------------

    QWidget* previewTab           = new QWidget;
    QVBoxLayout* previewTabLayout = new QVBoxLayout;
    previewTabLayout->addWidget(d->previewWidget);
    previewTab->setLayout(previewTabLayout);

    // ----------------------------------------------------------

    QWidget* mainWidget     = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;

    d->tabWidget->insertTab(FileList, imagesTab,        i18n("Files List"));
    d->tabWidget->insertTab(Settings, d->settingsTab,   i18n("Settings"));
//    d->tabWidget->insertTab(Preview,  previewTab,       i18n("Preview"));

    mainLayout->addWidget(d->tabWidget, 5);
    mainLayout->addWidget(d->progress);
    mainWidget->setLayout(mainLayout);
    setMainWidget(mainWidget);

    // ----------------------------------------------------------

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(helpClicked()));

    // ----------------------------------------------------------

    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(helpClicked()));

    connect(d->imageList, SIGNAL(signalFoundRAWImages(bool)),
            this, SLOT(foundRAWImages(bool)));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(startCorrection()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(startTestrun()));

    connect(this, SIGNAL(myCloseClicked()),
            this, SLOT(closeClicked()));

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(threadFinished()));

    connect(d->tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(tabwidgetChanged(int)));

    connect(d->settingsTab, SIGNAL(settingsChanged()),
            d->previewWidget, SLOT(reset()));

    // ----------------------------------------------------------

    KIPI::ImageCollection images = interface->currentSelection();

    if (images.isValid())
        d->imageList->slotAddImages(images.images());

    // ----------------------------------------------------------

    readSettings();
    setBusy(false);
}

RemoveRedEyesWindow::~RemoveRedEyesWindow()
{
    delete d->about;
    delete d;
}

void RemoveRedEyesWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("RemoveRedEyes Settings");

    d->settings.storageMode           = group.readEntry("Storage Mode", (int)StorageSettingsBox::Subfolder);
    d->settings.unprocessedMode       = group.readEntry("Unprocessed Mode", (int)UnprocessedSettingsBox::Ask);
    d->settings.subfolderName         = group.readEntry("Subfolder Name", "corrected");
    d->settings.simpleMode            = group.readEntry("Simple Mode", (int)SimpleSettings::Fast);
    d->settings.suffixName            = group.readEntry("Filename Suffix", "_corr");
    d->settings.minBlobsize           = group.readEntry("Minimum Blob Size", 10);
    d->settings.minRoundness          = group.readEntry("Minimum Roundness", 3.2);
    d->settings.neighborGroups        = group.readEntry("Neighbor Groups", 2);
    d->settings.scaleFactor           = group.readEntry("Scaling Factor", 1.2);
    d->settings.useStandardClassifier = group.readEntry("Use Standard Classifier", true);
    d->settings.classifierFile        = group.readEntry("Classifier", STANDARD_CLASSIFIER);
    d->settings.addKeyword            = group.readEntry("Add keyword", false);
    d->settings.keywordName           = group.readEntry("Keyword Name", "removed_redeyes");

    d->settingsTab->loadSettings(d->settings);
}

void RemoveRedEyesWindow::writeSettings()
{
    d->settings = d->settingsTab->readSettingsForSave();

    KConfig config("kipirc");
    KConfigGroup grp = config.group("RemoveRedEyes Settings");

    grp.writeEntry("Simple Mode",               d->settings.simpleMode);
    grp.writeEntry("Storage Mode",              d->settings.storageMode);
    grp.writeEntry("Unprocessed Mode",          d->settings.unprocessedMode);
    grp.writeEntry("Subfolder Name",            d->settings.subfolderName);
    grp.writeEntry("Filename Suffix",           d->settings.suffixName);

    grp.writeEntry("Minimum Blob Size",         d->settings.minBlobsize);
    grp.writeEntry("Minimum Roundness",         d->settings.minRoundness);
    grp.writeEntry("Neighbor Groups",           d->settings.neighborGroups);
    grp.writeEntry("Scaling Factor",            d->settings.scaleFactor);
    grp.writeEntry("Use Standard Classifier",   d->settings.useStandardClassifier);
    grp.writeEntry("Classifier",                d->settings.classifierFile);

    grp.writeEntry("Add keyword",               d->settings.addKeyword);
    grp.writeEntry("Keyword Name",              d->settings.keywordName);

    KConfigGroup dialogGroup = config.group("RemoveRedEyes Dialog");
    saveDialogSize(dialogGroup);
    config.sync();
}

void RemoveRedEyesWindow::updateSettings()
{
    d->settings = d->settingsTab->readSettings();
}

bool RemoveRedEyesWindow::acceptStorageSettings()
{
    if (d->settings.storageMode == StorageSettingsBox::Overwrite)
    {
        QString message = i18n("<p>You choose the <b>'overwrite' correction mode</b>!<br/>"
                               "Are you sure you want to loose your original image files?</p>");

        if (KMessageBox::questionYesNo(this, message, i18n("Overwrite mode"))
            == KMessageBox::No)
        {
            return false;
        }
    }
    return true;
}

void RemoveRedEyesWindow::startCorrection()
{
    updateSettings();
    if (!acceptStorageSettings())
        return;
    d->runtype = WorkerThread::Correction;

    d->imageList->resetEyeCounterColumn();
    d->tabWidget->setCurrentIndex(FileList);

    KUrl::List urls = d->imageList->imageUrls();
    startWorkerThread(urls);
}

void RemoveRedEyesWindow::startTestrun()
{
    updateSettings();
    d->runtype = WorkerThread::Testrun;

    d->imageList->resetEyeCounterColumn();
    d->tabWidget->setCurrentIndex(FileList);

    KUrl::List urls = d->imageList->imageUrls();
    startWorkerThread(urls);
}

void RemoveRedEyesWindow::startPreview()
{
    KIPIPlugins::ImagesListViewItem* item = dynamic_cast<KIPIPlugins::ImagesListViewItem*>(
                                            d->imageList->listView()->currentItem());
    if (!item)
    {
        d->previewWidget->reset();
        return;
    }

    if (!d->originalImageTempFile.open()  ||
        !d->correctedImageTempFile.open() ||
        !d->maskImageTempFile.open())
    {
        kDebug(51000) << "unable to create temp file for image preview!" << endl;
    }

    updateSettings();

    if (item->url().path() == d->previewWidget->currentImage())
        return;

    d->previewWidget->setCurrentImage(item->url().path());
    d->runtype = WorkerThread::Preview;

    KUrl::List oneFile;
    oneFile.append(item->url());
    startWorkerThread(oneFile);
}

void RemoveRedEyesWindow::cancelCorrection()
{
    if (d->busy && d->thread->isRunning())
    {
        d->thread->cancel();
        KApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    }
}

void RemoveRedEyesWindow::closeClicked()
{
    writeSettings();
    done(Close);
}

void RemoveRedEyesWindow::helpClicked()
{
    KToolInvocation::invokeHelp("removeredeyes", "kipi-plugins");
}

void RemoveRedEyesWindow::startWorkerThread(const KUrl::List& urls)
{
    if (urls.isEmpty())
        return;

    if (d->busy)
        return;

    if (!d->thread)
    {
        kError(51000) << "Creation of WorkerThread failed!" << endl;
        setBusy(false);
        return;
    }

    d->thread->setImagesList(urls);
    d->thread->setRunType(d->runtype);
    d->thread->loadSettings(d->settings);
    d->thread->setSaveMethod(SaveMethodFactory::factory(d->settings.storageMode));

    d->thread->setTempFile(d->originalImageTempFile.fileName(),
                           WorkerThread::OriginalImage);
    d->thread->setTempFile(d->correctedImageTempFile.fileName(),
                           WorkerThread::CorrectedImage);
    d->thread->setTempFile(d->maskImageTempFile.fileName(),
                           WorkerThread::MaskImage);

    setBusy(true);

    initProgressBar(urls.count());
    if (d->progress->isHidden())
        d->progress->show();

    connect(d->thread, SIGNAL(calculationFinished(WorkerThreadData*)),
            this, SLOT(calculationFinished(WorkerThreadData*)));

    // start image processing
    if (!d->thread->isRunning())
        d->thread->start();
}

void RemoveRedEyesWindow::setBusy(bool busy)
{
    d->busy = busy;

    if (busy)
    {
        // disable connection to make sure that the "test run" and "correct photos"
        // buttons are not enabled again on ImageListChange
        disconnect(d->imageList, SIGNAL(signalImageListChanged(bool)),
                this, SLOT(imageListChanged(bool)));

        disconnect(this, SIGNAL(myCloseClicked()),
                   this, SLOT(closeClicked()));

        setButtonGuiItem(Close, KStandardGuiItem::cancel());
        enableButton(User1, false); // correction button
        enableButton(User2, false); // testrun button

        connect(this, SIGNAL(myCloseClicked()),
                this, SLOT(cancelCorrection()));

        d->settingsTab->setEnabled(false);
    }
    else
    {
        // enable connection again to make sure that an empty image list will
        // disable the "test run" and "correct photos" buttons
        connect(d->imageList, SIGNAL(signalImageListChanged(bool)),
                this, SLOT(imageListChanged(bool)));

        disconnect(this, SIGNAL(myCloseClicked()),
                   this, SLOT(cancelCorrection()));

        setButtonGuiItem(Close, KStandardGuiItem::close());
        enableButton(User1, true);  // correction button
        enableButton(User2, true);  // testrun button

        connect(this, SIGNAL(myCloseClicked()),
                this, SLOT(closeClicked()));

        d->settingsTab->setEnabled(true);
    }
}

void RemoveRedEyesWindow::handleUnprocessedImages()
{
    if (d->imageList->hasUnprocessedImages())
    {
        updateSettings();

        switch (d->settings.unprocessedMode)
        {
            case UnprocessedSettingsBox::Ask:
            {
                QString message = i18n("<p>Some of the images could not be analyzed "
                                       "with the current settings or they do not "
                                       "contain any red-eyes at all.</p>"
                                       "<p><b>Would you like to remove those images "
                                       "from the list?</b></p>");

                if (KMessageBox::questionYesNo(this, message, i18n("Remove unprocessed images?"))
                        == KMessageBox::Yes)
                {
                    d->imageList->removeUnprocessedImages();
                }
                break;
            }

            case UnprocessedSettingsBox::Remove:
                d->imageList->removeUnprocessedImages();
                break;
        }
    }
}

void RemoveRedEyesWindow::imageListChanged(bool)
{
    bool isEmpty = d->imageList->imageUrls().isEmpty();
    enableButton(User1, !isEmpty);  // correction button
    enableButton(User2, !isEmpty);  // testrun button
}

void RemoveRedEyesWindow::tabwidgetChanged(int tab)
{
    if (tab == Preview)
        startPreview();
}

void RemoveRedEyesWindow::foundRAWImages(bool raw)
{
    if (raw)
    {
        QString message = i18n("<p>You tried to add <b>RAW images</b> to the plugin,<br/>"
                               "but those filetypes are not supported.</p>"
                               "<p><b>They were automatically removed from the list.</b></p>");

        KMessageBox::information(this, message, i18n("RAW images found"));
    }
}

void RemoveRedEyesWindow::calculationFinished(WorkerThreadData* data)
{
    if (!data)
        return;

    int current     = data->current;
    const KUrl& url = data->urls;
    int eyes        = data->eyes;
    delete data;

    d->progress->setValue(current);
    d->imageList->addEyeCounterByUrl(url, eyes);
}

void RemoveRedEyesWindow::slotButtonClicked(int button)
{
    emit buttonClicked(static_cast<KDialog::ButtonCode> (button));

    switch (button)
    {
        case User2:                 // testrun
            emit user2Clicked();
            break;
        case User1:                 // correction
            emit user1Clicked();
            break;
        case Cancel:
            emit cancelClicked();
            break;
        case Close:
            emit myCloseClicked();
            break;
        case Help:
            emit helpClicked();
            break;
        case Default:
            emit defaultClicked();
            break;
    }
}

void RemoveRedEyesWindow::threadFinished()
{
    d->progress->hide();
    setBusy(false);
    KApplication::restoreOverrideCursor();

    switch (d->runtype)
    {
        case WorkerThread::Testrun:
            handleUnprocessedImages();
            break;

        case WorkerThread::Correction:
            // show summary and close the plugin
            showSummary();
            break;

        case WorkerThread::Preview:
            // load generated preview images
            d->previewWidget->setPreviewImage(PreviewWidget::OriginalImage,
                    d->originalImageTempFile.fileName());
            d->previewWidget->setPreviewImage(PreviewWidget::CorrectedImage,
                    d->correctedImageTempFile.fileName());
            d->previewWidget->setPreviewImage(PreviewWidget::MaskImage,
                    d->maskImageTempFile.fileName());
            break;
    }

    disconnect(d->thread, SIGNAL(calculationFinished(WorkerThreadData*)),
               this, SLOT(calculationFinished(WorkerThreadData*)));
}

void RemoveRedEyesWindow::showSummary()
{
    QString message = i18np("<p>%1 image has been successfully processed.</p>",
                            "<p>%1 images have been successfully processed.</p>",
                            d->imageList->processedImages());
    message.append(i18n("<h2>Correction Complete!</h2>"));

    KMessageBox::information(this, message, i18n("Correction Complete"));
    closeClicked();
}

void RemoveRedEyesWindow::initProgressBar(int max)
{
    d->progress->reset();
    d->progress->setRange(0, max);

    if (d->runtype == WorkerThread::Preview)
    {
        // create a busy indicator progressbar
        d->progress->setRange(0, 0);
    }

    d->progress->setValue(0);
}

} // namespace KIPIRemoveRedEyesPlugin
