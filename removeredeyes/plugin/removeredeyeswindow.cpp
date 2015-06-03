/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-06-08
 * Description : the main window of the removeredeyes plugin
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#include "removeredeyeswindow.moc"

// Qt includes

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktabwidget.h>
#include <ktemporaryfile.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "locator.h"
#include "locatorfactory.h"
#include "savemethodfactory.h"
#include "commonsettings.h"
#include "haarsettingswidget.h"
#include "kpaboutdata.h"
#include "kphostsettings.h"
#include "myimageslist.h"
#include "previewwidget.h"
#include "simplesettings.h"
#include "storagesettingsbox.h"
#include "unprocessedsettingsbox.h"
#include "workerthread.h"
#include "workerthreaddata.h"

namespace KIPIRemoveRedEyesPlugin
{

class RemoveRedEyesWindow::Private
{
public:

    Private() :
        total(0),
        processed(0),
        failed(0),
        totalLabel(0),
        processedLabel(0),
        failedLabel(0),
        busy(false),
        hasLocator(false),
        runtype(WorkerThread::Testrun),
        progress(0),
        settingsTab(0),
        locatorSettingsWidget(0),
        tabWidget(0),
        imageList(0),
        previewWidget(0),
        thread(0),
        unprocessedSettingsBox(0),
        storageSettingsBox(0),
        locator(0),
        saveMethod(0)
    {
    }

    static const QString      configGroupName;
    static const QString      configStorageModeEntry;
    static const QString      configExtraNameEntry;
    static const QString      configAddkeywordEntry;
    static const QString      configKeywordNameEntry;
    static const QString      configUnprocessedModeEntry;
    static const QString      configLocatorTypeEntry;
    static const QString      configLocatorDefaultType;

    int                       total;
    int                       processed;
    int                       failed;

    QLabel*                   totalLabel;
    QLabel*                   processedLabel;
    QLabel*                   failedLabel;

    bool                      busy;
    bool                      hasLocator;
    int                       runtype;

    QProgressBar*             progress;

    QWidget*                  settingsTab;
    QWidget*                  locatorSettingsWidget;

    KTabWidget*               tabWidget;

    KTemporaryFile            originalImageTempFile;
    KTemporaryFile            correctedImageTempFile;
    KTemporaryFile            maskImageTempFile;

    MyImagesList*             imageList;
    PreviewWidget*            previewWidget;
    CommonSettings            settings;
    WorkerThread*             thread;

    UnprocessedSettingsBox*   unprocessedSettingsBox;
    StorageSettingsBox*       storageSettingsBox;

    Locator*                  locator;
    SaveMethod*               saveMethod;
};

const QString RemoveRedEyesWindow::Private::configGroupName("RemoveRedEyes Settings");
const QString RemoveRedEyesWindow::Private::configStorageModeEntry("Storage Mode");
const QString RemoveRedEyesWindow::Private::configExtraNameEntry("Extra Name");
const QString RemoveRedEyesWindow::Private::configAddkeywordEntry("Add keyword");
const QString RemoveRedEyesWindow::Private::configKeywordNameEntry("Keyword Name");
const QString RemoveRedEyesWindow::Private::configUnprocessedModeEntry("Unprocessed Mode");
const QString RemoveRedEyesWindow::Private::configLocatorTypeEntry("Locator Type");
const QString RemoveRedEyesWindow::Private::configLocatorDefaultType("HaarClassifierLocator");

// --------------------------------------------------------

RemoveRedEyesWindow::RemoveRedEyesWindow()
    : KPToolDialog(0), d(new Private)
{
    setWindowTitle(i18n("Automatic Red-Eye Removal"));
    setButtons(Help|User1|User2|Close);
    setDefaultButton(Close);
    setModal(false);

    KPHostSettings hSettings;
    d->thread        = new WorkerThread(this, hSettings.metadataSettings().updateFileTimeStamp);
    d->runtype       = WorkerThread::Testrun;
    d->tabWidget     = new KTabWidget;

    d->imageList     = new MyImagesList;
    d->previewWidget = new PreviewWidget;

    d->progress      = new QProgressBar;
    d->progress->hide();

    // ----------------------------------------------------------

    QString suffix(".jpg");
    d->originalImageTempFile.setSuffix(suffix);
    d->correctedImageTempFile.setSuffix(suffix);
    d->maskImageTempFile.setSuffix(suffix);

    // ----------------------------------------------------------

    KPAboutData* const about = new KPAboutData(ki18n("Remove Red-Eye"),
                                   0,
                                   KAboutData::License_GPL,
                                   ki18n("A plugin to automatically "
                                         "detect and remove red-eye effect."),
                                   ki18n("(c) 2008-2013, Andi Clemens"));

    about->addAuthor(ki18n("Andi Clemens"), ki18n("Author and Maintainer"),
                     "andi dot clemens at googlemail dot com");

    about->setHandbookEntry("removeredeyes");
    setAboutData(about);

    // ----------------------------------------------------------

    KGuiItem correctBtn = KStandardGuiItem::ok();
    correctBtn.setText(i18n("Correct &Photos"));
    correctBtn.setToolTip(i18n("Start correcting the listed images"));
    setButtonGuiItem(User1, correctBtn);

    KGuiItem testrunBtn = KStandardGuiItem::apply();
    testrunBtn.setText(i18n("&Test-Run"));
    testrunBtn.setToolTip(i18n("Simulate the correction process, without saving the results."));
    setButtonGuiItem(User2, testrunBtn);

    // ----------------------------------------------------------

    d->settingsTab           = new QWidget;
    d->locatorSettingsWidget = new QWidget;

    QVBoxLayout* const mainSettingsLayout = new QVBoxLayout;
    d->unprocessedSettingsBox             = new UnprocessedSettingsBox;
    d->storageSettingsBox                 = new StorageSettingsBox;
    mainSettingsLayout->addWidget(d->storageSettingsBox);
    mainSettingsLayout->addWidget(d->unprocessedSettingsBox);
    mainSettingsLayout->addStretch(10);

    QGridLayout* const settingsTabLayout = new QGridLayout;
    settingsTabLayout->addWidget(d->locatorSettingsWidget, 0, 0, 1, 1);
    settingsTabLayout->addLayout(mainSettingsLayout,       0, 1, 1, 1);
    d->settingsTab->setLayout(settingsTabLayout);

    // ----------------------------------------------------------

    d->totalLabel     = new QLabel;
    d->processedLabel = new QLabel;
    d->failedLabel    = new QLabel;

    d->totalLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    d->processedLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    d->failedLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);

    QLabel* const l1 = new QLabel(i18nc("The total number of images in the list",  "Total:"));
    QLabel* const l2 = new QLabel(i18nc("number of images successfully processed", "Success:"));
    QLabel* const l3 = new QLabel(i18nc("number of images failed to process",      "Failed:"));

    QWidget* const summaryBox           = new QWidget;
    QHBoxLayout* const summaryBoxLayout = new QHBoxLayout;
    summaryBoxLayout->addWidget(l1);
    summaryBoxLayout->addWidget(d->totalLabel);
    summaryBoxLayout->addStretch(10);
    summaryBoxLayout->addWidget(l2);
    summaryBoxLayout->addWidget(d->processedLabel);
    summaryBoxLayout->addWidget(l3);
    summaryBoxLayout->addWidget(d->failedLabel);
    summaryBox->setLayout(summaryBoxLayout);

    // --------------------------------------------------------

    QWidget* const imagesTab           = new QWidget;
    QGridLayout* const imagesTabLayout = new QGridLayout();
    imagesTabLayout->addWidget(d->imageList, 0, 0, 1, 1);
    imagesTabLayout->addWidget(summaryBox,   1, 0, 1, 1);
    imagesTabLayout->setRowStretch(0, 10);
    imagesTabLayout->setMargin(0);
    imagesTabLayout->setSpacing(0);
    imagesTab->setLayout(imagesTabLayout);

    // ----------------------------------------------------------

    QWidget* const previewTab           = new QWidget;
    QVBoxLayout* const previewTabLayout = new QVBoxLayout;
    previewTabLayout->addWidget(d->previewWidget);
    previewTab->setLayout(previewTabLayout);

    // ----------------------------------------------------------

    d->tabWidget->insertTab(FileList, imagesTab,      i18n("File List"));
    d->tabWidget->insertTab(Settings, d->settingsTab, i18n("Settings"));
    //    d->tabWidget->insertTab(Preview,  previewTab,     i18n("Preview"));

    QWidget* const mainWidget     = new QWidget;
    QVBoxLayout* const mainLayout = new QVBoxLayout;
    mainLayout->addWidget(d->tabWidget, 5);
    mainLayout->addWidget(d->progress);
    mainWidget->setLayout(mainLayout);
    setMainWidget(mainWidget);

    // ----------------------------------------------------------

    connect(d->imageList, SIGNAL(signalFoundRAWImages(bool)),
            this, SLOT(foundRAWImages(bool)));

    connect(d->imageList, SIGNAL(signalImageListChanged()),
            this, SLOT(updateSummary()));

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

    connect(this, SIGNAL(locatorUpdated()),
            this, SLOT(locatorChanged()));

    //    connect(d->settingsTab, SIGNAL(settingsChanged()),
    //            d->previewWidget, SLOT(reset()));

    // ----------------------------------------------------------

    ImageCollection images = iface()->currentSelection();

    if (images.isValid())
    {
        d->imageList->slotAddImages(images.images());
    }

    // ----------------------------------------------------------

    readSettings();
    setBusy(false);
    resetSummary();
    updateSummary();
    imageListChanged();
}

RemoveRedEyesWindow::~RemoveRedEyesWindow()
{
    delete d->locator;
    delete d->saveMethod;
    delete d;
}

void RemoveRedEyesWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(d->configGroupName);

    KConfigGroup group2 = config.group(QString("RemoveRedEyes Dialog"));
    restoreDialogSize(group2);

    int storageMode = group.readEntry(d->configStorageModeEntry,                            (int)StorageSettingsBox::Subfolder);
    d->storageSettingsBox->setStorageMode(storageMode);
    d->storageSettingsBox->setExtra(group.readEntry(d->configExtraNameEntry,                "corrected"));
    d->storageSettingsBox->setAddKeyword(group.readEntry(d->configAddkeywordEntry,          false));
    d->storageSettingsBox->setKeyword(group.readEntry(d->configKeywordNameEntry,            "removed_redeyes"));
    d->unprocessedSettingsBox->setHandleMode(group.readEntry(d->configUnprocessedModeEntry, (int)UnprocessedSettingsBox::Ask));

    // set save method
    d->saveMethod = SaveMethodFactory::create(storageMode);

    // load locator
    QString locatorType = group.readEntry(d->configLocatorTypeEntry,                        d->configLocatorDefaultType);

    // If config file somehow has been messed up and readEntry() only delivers an empty string,
    // force loading of the default locator in here (I had trouble with my kipirc once and wondered why the plugin
    // is not working. I guess the value in the config file was a whitespace, which seems to be a valid entry.
    loadLocator(locatorType.isEmpty() ? d->configLocatorDefaultType : locatorType);

    updateSettings();
}

void RemoveRedEyesWindow::writeSettings()
{
    updateSettings();

    KConfig config("kipirc");
    KConfigGroup group = config.group(d->configGroupName);

    if (d->hasLocator)
    {
        group.writeEntry(d->configLocatorTypeEntry, d->locator->objectName());
    }

    group.writeEntry(d->configStorageModeEntry,     d->settings.storageMode);
    group.writeEntry(d->configUnprocessedModeEntry, d->settings.unprocessedMode);
    group.writeEntry(d->configExtraNameEntry,       d->settings.extraName);
    group.writeEntry(d->configAddkeywordEntry,      d->settings.addKeyword);
    group.writeEntry(d->configKeywordNameEntry,     d->settings.keywordName);

    KConfigGroup dialogGroup = config.group("RemoveRedEyes Dialog");
    saveDialogSize(dialogGroup);
    config.sync();
}

void RemoveRedEyesWindow::updateSettings()
{
    d->settings.addKeyword      = d->storageSettingsBox->addKeyword();
    d->settings.extraName       = d->storageSettingsBox->extra();
    d->settings.keywordName     = d->storageSettingsBox->keyword();
    d->settings.storageMode     = d->storageSettingsBox->storageMode();
    d->settings.unprocessedMode = d->unprocessedSettingsBox->handleMode();

    // reset save method
    delete d->saveMethod;

    d->saveMethod = SaveMethodFactory::create(d->settings.storageMode);
}

bool RemoveRedEyesWindow::acceptStorageSettings()
{
    if (d->settings.storageMode == StorageSettingsBox::Overwrite)
    {
        QString message = i18n("<p>You chose the <b>'overwrite' correction mode</b>.<br/>"
                               "Are you sure you want to lose your original image files?</p>");

        if (KMessageBox::questionYesNo(this, message, i18n("Overwrite mode")) == KMessageBox::No)
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
    {
        return;
    }

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
    KPImagesListViewItem* const item = dynamic_cast<KIPIPlugins::KPImagesListViewItem*>(d->imageList->listView()->currentItem());

    if (!item)
    {
        d->previewWidget->reset();
        return;
    }

    if (!d->originalImageTempFile.open()  ||
        !d->correctedImageTempFile.open() ||
        !d->maskImageTempFile.open())
    {
        kDebug() << "unable to create temp file for image preview!";
    }

    updateSettings();

    if (item->url().path() == d->previewWidget->currentImage())
    {
        return;
    }

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

    if (d->locator)
    {
        d->locator->writeSettings();
    }

    done(Close);
}

void RemoveRedEyesWindow::startWorkerThread(const KUrl::List& urls)
{
    if (urls.isEmpty())
    {
        return;
    }

    if (d->busy)
    {
        return;
    }

    if (!d->locator || !d->saveMethod)
    {
        return;
    }

    if (!d->thread)
    {
        kError() << "Creation of WorkerThread failed!";
        setBusy(false);
        return;
    }

    // --------------------------------------------------------

    d->thread->setImagesList(urls);
    d->thread->setRunType(d->runtype);
    d->thread->loadSettings(d->settings);
    d->thread->setSaveMethod(d->saveMethod);
    d->thread->setLocator(d->locator);

    d->thread->setTempFile(d->originalImageTempFile.fileName(),  WorkerThread::OriginalImage);
    d->thread->setTempFile(d->correctedImageTempFile.fileName(), WorkerThread::CorrectedImage);
    d->thread->setTempFile(d->maskImageTempFile.fileName(),      WorkerThread::MaskImage);

    // --------------------------------------------------------

    setBusy(true);

    initProgressBar(urls.count());

    if (d->progress->isHidden())
    {
        d->progress->show();
    }

    connect(d->thread, SIGNAL(calculationFinished(WorkerThreadData*)),
            this, SLOT(calculationFinished(WorkerThreadData*)));

    // start image processing
    if (!d->thread->isRunning())
    {
        d->thread->start();
    }
}

void RemoveRedEyesWindow::setBusy(bool busy)
{
    d->busy = busy;

    if (busy)
    {
        // disable connection to make sure that the "test run" and "correct photos"
        // buttons are not enabled again on ImageListChange
        disconnect(d->imageList, SIGNAL(signalImageListChanged()),
                   this, SLOT(imageListChanged()));

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
        connect(d->imageList, SIGNAL(signalImageListChanged()),
                this, SLOT(imageListChanged()));

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
                                       "with the current settings, or they do not "
                                       "contain any red-eye at all.</p>"
                                       "<p><b>Would you like to remove these images "
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

void RemoveRedEyesWindow::imageListChanged()
{
    bool isEmpty = d->imageList->imageUrls().isEmpty();
    enableButton(User1, !isEmpty);  // correction button
    enableButton(User2, !isEmpty);  // testrun button
}

void RemoveRedEyesWindow::tabwidgetChanged(int tab)
{
    if (tab == Preview)
    {
        startPreview();
    }
}

void RemoveRedEyesWindow::foundRAWImages(bool raw)
{
    if (raw)
    {
        QString message = i18n("<p>You tried to add <b>RAW images</b> to the plugin,<br/>"
                               "but those file-types are not supported.</p>"
                               "<p><b>They were automatically removed from the list.</b></p>");

        KMessageBox::information(this, message, i18n("RAW images found"));
    }
}

void RemoveRedEyesWindow::calculationFinished(WorkerThreadData* data)
{
    if (!data)
    {
        return;
    }

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
            d->previewWidget->setPreviewImage(PreviewWidget::OriginalImage,  d->originalImageTempFile.fileName());
            d->previewWidget->setPreviewImage(PreviewWidget::CorrectedImage, d->correctedImageTempFile.fileName());
            d->previewWidget->setPreviewImage(PreviewWidget::MaskImage,      d->maskImageTempFile.fileName());
            break;
    }

    disconnect(d->thread, SIGNAL(calculationFinished(WorkerThreadData*)),
               this, SLOT(calculationFinished(WorkerThreadData*)));
}

void RemoveRedEyesWindow::showSummary()
{
    QString message = i18np("<p>%1 image has been successfully processed.</p>",
                            "<p>%1 images have been successfully processed.</p>",
                            processedImages());
    message.append(i18n("<h2>Correction Complete</h2>"));

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

void RemoveRedEyesWindow::loadLocator(const QString& locator)
{
    if (locator.isEmpty())
    {
        return;
    }

    unloadLocator();

    d->locator                           = LocatorFactory::create(locator);
    QGridLayout* const settingsTabLayout = qobject_cast<QGridLayout*>(d->settingsTab->layout());

    if (d->locator)
    {
        d->locatorSettingsWidget = d->locator->settingsWidget();
        d->hasLocator            = true;
    }
    else
    {
        QString noLocatorMsg     = i18n("<h2>Could not load classifier options.<br/>"
                                        "The plugin is not executable.</h2>");
        d->locatorSettingsWidget = new QLabel(noLocatorMsg);
        d->hasLocator            = false;
        kDebug() << "Invalid locator: '" << locator << "'";
    }

    settingsTabLayout->addWidget(d->locatorSettingsWidget, 0, 0, 1, 1);
    emit locatorUpdated();
}

void RemoveRedEyesWindow::unloadLocator()
{
    delete d->locator;
    d->locator = 0;

    if (d->locatorSettingsWidget)
    {
        d->settingsTab->layout()->removeWidget(d->locatorSettingsWidget);
        delete d->locatorSettingsWidget;
        d->locatorSettingsWidget = 0;
    }

    d->hasLocator = false;
    emit locatorUpdated();
}

void RemoveRedEyesWindow::locatorChanged()
{
    if (d->hasLocator)
    {
        enableButton(User1, true); // correction button
        enableButton(User2, true); // testrun button
    }
    else
    {
        enableButton(User1, false); // correction button
        enableButton(User2, false); // testrun button
    }
}

int RemoveRedEyesWindow::totalImages() const
{
    return d->total;
}

int RemoveRedEyesWindow::processedImages() const
{
    return d->processed;
}

int RemoveRedEyesWindow::failedImages() const
{
    return d->failed;
}

void RemoveRedEyesWindow::resetSummary()
{
    d->total     = d->imageList->imageUrls().count();
    d->processed = 0;
    d->failed    = 0;
}

void RemoveRedEyesWindow::updateSummary()
{
    resetSummary();

    QTreeWidgetItemIterator it(d->imageList->listView());

    while (*it)
    {
        KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (item && !item->text(KPImagesListView::User1).isEmpty())
        {
            if (item->text(KPImagesListView::User1).toInt() > 0)
            {
                d->processed++;
            }
            else
            {
                d->failed++;
            }
        }

        ++it;
    }

    d->totalLabel->setText(QString("%1").arg(d->total));
    d->processedLabel->setText(QString("%1").arg(d->processed));
    d->failedLabel->setText(QString("%1").arg(d->failed));
}

} // namespace KIPIRemoveRedEyesPlugin
