/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : the main window of the removeredeyes plugin
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include <QButtonGroup>
#include <QCheckBox>
#include <QCustomEvent>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QRadioButton>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>

// KDE includes.

#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kseparator.h>
#include <ktabwidget.h>
#include <ktoolinvocation.h>
#include <kurlrequester.h>

// LibKIPI includes.

#include <libkipi/interface.h>

// Local includes.

#include "advancedsettings.h"
#include "imageslist.h"
#include "kpaboutdata.h"
#include "removalsettings.h"
#include "simplesettings.h"
#include "storagesettingsbox.h"
#include "workerthread.h"
#include "workerthreaddata.h"

namespace KIPIRemoveRedEyesPlugin
{

class RedEyesWindowPriv
{

public:

    RedEyesWindowPriv()
    {
        interface           = 0;
        about               = 0;

        progress            = 0;
        settingsSwitcherBtn = 0;
        settingsStack       = 0;
        progressTimer       = 0;

        tabWidget           = 0;

        advancedSettings    = 0;
        imageList           = 0;
        settings            = 0;
        simpleSettings      = 0;
        wth                 = 0;
    }

    bool                        busy;
    bool                        simpleCorrectionMode;
    int                         runtype;

    KIPI::Interface*            interface;
    KIPIPlugins::KPAboutData*   about;

    QProgressBar*               progress;
    QPushButton*                settingsSwitcherBtn;
    QStackedWidget*             settingsStack;
    QTimer*                     progressTimer;

    KTabWidget*                 tabWidget;

    AdvancedSettings*           advancedSettings;
    ImagesList*                 imageList;
    RemovalSettings*            settings;
    SimpleSettings*             simpleSettings;
    WorkerThread*               wth;
};

RemoveRedEyesWindow::RemoveRedEyesWindow(KIPI::Interface *interface, QWidget *parent)
                   : KDialog(parent),
                     d(new RedEyesWindowPriv)
{
    setWindowTitle(i18n("Remove Red-Eyes From Your Photos"));
    setButtons(Help|User1|User2|Close);
    setDefaultButton(Close);
    setModal(false);

    d->busy                 = false;
    d->simpleCorrectionMode = true;
    d->runtype              = WorkerThread::TestRun;
    d->settings             = new RemovalSettings;
    d->interface            = interface;
    d->tabWidget            = new KTabWidget;
    d->settingsSwitcherBtn  = new QPushButton;
    d->imageList            = new ImagesList(interface);

    d->progressTimer        = new QTimer(this);
    d->progressTimer->setSingleShot(true);

    d->progress             = new QProgressBar;
    d->progress->setMaximumHeight(fontMetrics().height() + 2);
    d->progress->hide();

    // about & help ----------------------------------------------------------

    d->about = new KIPIPlugins::KPAboutData(ki18n("Remove Red-Eyes"),
                                            0,
                                            KAboutData::License_GPL,
                                            ki18n("A plugin to automatically "
                                                  "detect and remove red-eyes"),
                                            ki18n("(c) 2008, Andi Clemens"));

    d->about->addAuthor(ki18n("Andi Clemens"), ki18n("Author and Maintainer"),
                              "andi dot clemens at gmx dot net");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook = new QAction(i18n("Plugin Handbook"), this);

    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));

    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setDelayedMenu(helpMenu->menu());

    // ----------------------------------------------------------

    setButtonGuiItem(User1, KGuiItem(i18n("Correct Photos"), KIcon("dialog-ok-apply")));
    setButtonGuiItem(User2, KGuiItem(i18n("Test Run"), KIcon("dialog-information")));
    setButtonToolTip(User1, i18n("Start correcting the listed images"));
    setButtonToolTip(User2, i18n("Simulate the correction process, without saving the results."));
    setButtonToolTip(Close, i18n("Exit"));

    // settings stack widget ----------------------------------------------------------

    d->simpleSettings       = new SimpleSettings;
    d->advancedSettings     = new AdvancedSettings;

    d->settingsStack = new QStackedWidget;
    d->settingsStack->insertWidget(Simple, d->simpleSettings);
    d->settingsStack->insertWidget(Advanced, d->advancedSettings);
    d->settingsStack->setCurrentIndex(Simple);
    setSettingsMode(Simple);

    // Set layouts --------------------------------------------------------------

    QWidget* settingsTab    = new QWidget;
    QGridLayout* settingsTabLayout = new QGridLayout;
    settingsTabLayout->addWidget(d->settingsStack,          0, 0, 1, 2);
    settingsTabLayout->addWidget(d->settingsSwitcherBtn,    1, 0, 1, 1);
    settingsTabLayout->setColumnStretch(1, 10);
    settingsTab->setLayout(settingsTabLayout);

    QWidget* imagesTab           = new QWidget;
    QVBoxLayout* imagesTabLayout = new QVBoxLayout;
    imagesTabLayout->addWidget(d->imageList);
    imagesTabLayout->addWidget(d->progress);
    imagesTab->setLayout(imagesTabLayout);

    QWidget* mainWidget     = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;
    d->tabWidget->insertTab(FileList, imagesTab,   i18n("Files List"));
    d->tabWidget->insertTab(Settings, settingsTab, i18n("Settings"));

    mainLayout->addWidget(d->tabWidget, 5);
    mainWidget->setLayout(mainLayout);
    setMainWidget(mainWidget);

    // connections ------------------------------------------------------------------

    connect(d->settingsSwitcherBtn, SIGNAL(clicked()),
            this, SLOT(settingsModeChanged()));

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressBarTimedOut()));

    connect(d->progress, SIGNAL(valueChanged(int)),
            this, SLOT(slotProgressBarChanged(int)));

    connect(this, SIGNAL(signalTestRunFinished()),
            this, SLOT(checkForNoneCorrectedImages()));

    connect(d->imageList, SIGNAL(signalFoundRAWImages(bool)),
            this, SLOT(slotFoundRAWImages(bool)));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(startCorrection()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(startTestrun()));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    // ------------------------------------------------------------------

    KIPI::ImageCollection images = interface->currentSelection();

    if (images.isValid())
        d->imageList->slotAddImages(images.images());

    readSettings();
    setBusy(false);
}

RemoveRedEyesWindow::~RemoveRedEyesWindow()
{
    delete d->settings;
    delete d;
}

void RemoveRedEyesWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("RemoveRedEyes Settings");

    d->settings->storageMode            = group.readEntry("Storage Mode", (int)StorageSettingsBox::Subfolder);
    d->settings->subfolderName          = group.readEntry("Subfolder Name", "corrected");
    d->settings->simpleMode             = group.readEntry("Simple Mode", (int)SimpleSettings::Fast);
    d->settings->prefixName             = group.readEntry("Filename Prefix", "_corr");
    d->settings->minBlobsize            = group.readEntry("Minimum Blob Size", 10);
    d->settings->minRoundness           = group.readEntry("Minimum Roundness", 3.2);
    d->settings->neighborGroups         = group.readEntry("Neighbor Groups", 2);
    d->settings->scaleFactor            = group.readEntry("Scaling Factor", 1.2);
    d->settings->useStandardClassifier  = group.readEntry("Use Standard Classifier", true);
    d->settings->classifierFile         = group.readEntry("Classifier", STANDARD_CLASSIFIER);

    d->simpleSettings->loadSettings(d->settings);
    d->advancedSettings->loadSettings(d->settings);
}

void RemoveRedEyesWindow::writeSettings()
{
    updateSettings();

    KConfig config("kipirc");
    KConfigGroup grp = config.group("RemoveRedEyes Settings");

    grp.writeEntry("Simple Mode",       d->settings->simpleMode);
    grp.writeEntry("Storage Mode",      d->settings->storageMode);
    grp.writeEntry("Subfolder Name",    d->settings->subfolderName);
    grp.writeEntry("Filename Prefix",   d->settings->prefixName);

    if (!d->simpleCorrectionMode)
    {
        grp.writeEntry("Minimum Blob Size",         d->settings->minBlobsize);
        grp.writeEntry("Minimum Roundness",         d->settings->minRoundness);
        grp.writeEntry("Neighbor Groups",           d->settings->neighborGroups);
        grp.writeEntry("Scaling Factor",            d->settings->scaleFactor);
        grp.writeEntry("Use Standard Classifier",   d->settings->useStandardClassifier);
        grp.writeEntry("Classifier",                d->settings->classifierFile);
    }

    KConfigGroup dialogGroup = config.group("RemoveRedEyes Dialog");
    saveDialogSize(dialogGroup);
    config.sync();
}

void RemoveRedEyesWindow::updateSettings()
{
    if (d->simpleCorrectionMode)
        d->settings = d->simpleSettings->readSettings();
    else
        d->settings = d->advancedSettings->readSettings();
}

void RemoveRedEyesWindow::startCorrection()
{
    updateSettings();
    startWorkerThread(WorkerThread::Correction);
}

void RemoveRedEyesWindow::abortCorrection()
{
}

void RemoveRedEyesWindow::startTestrun()
{
    updateSettings();
    startWorkerThread(WorkerThread::TestRun);
}

void RemoveRedEyesWindow::slotClose()
{
    writeSettings();
    done(Close);
}

void RemoveRedEyesWindow::slotHelp()
{
    KToolInvocation::invokeHelp("removeredeyes", "kipi-plugins");
}

void RemoveRedEyesWindow::startWorkerThread(int type)
{
    KUrl::List urls = d->imageList->imageUrls();
    if (urls.isEmpty())
        return;

    if (d->busy)
        return;

    // create the worker thread
    d->wth = new WorkerThread(this, d->settings, type, urls);

    if (!d->wth)
    {
        kError(51000) << "Creation of WorkerThread failed!" << endl;
        setBusy(false);
        return;
    }
    setBusy(true);

    if (d->progress->isHidden())
        d->progress->show();
    d->progress->reset();
    d->progress->setRange(0, urls.count());
    d->progress->setValue(0);

    connect(d->wth, SIGNAL(calculationFinished(WorkerThreadData*)),
            this, SLOT(calculationFinished(WorkerThreadData*)));

    // start image processing
    d->wth->start();
}

void RemoveRedEyesWindow::setBusy(bool busy)
{
    d->busy = busy;

    if (busy)
    {
        // disable connection to make sure that the "test run" and "correct photos"
        // buttons are not enabled again on ImageListChange
        disconnect(d->imageList, SIGNAL(signalImageListChanged(bool)),
                this, SLOT(slotImageListChanged(bool)));

        disconnect(this, SIGNAL(closeClicked()),
                   this, SLOT(slotClose()));

        connect(this, SIGNAL(closeClicked()),
                this, SLOT(abortCorrection()));

        d->imageList->resetEyeCounterColumn();
        d->tabWidget->setCurrentIndex(FileList);

        setButtonGuiItem(Close, KGuiItem(i18n("Cancel"), KIcon("dialog-cancel")));
        enableButton(User1, false);
        enableButton(User2, false);
    }
    else
    {
        // enable connection again to make sure that an empty image list will
        // disable the "test run" and "correct photos" buttons
        connect(d->imageList, SIGNAL(signalImageListChanged(bool)),
                this, SLOT(slotImageListChanged(bool)));

        disconnect(this, SIGNAL(closeClicked()),
                   this, SLOT(abortCorrection()));

        connect(this, SIGNAL(closeClicked()),
                this, SLOT(slotClose()));

        setButtonGuiItem(Close, KGuiItem(i18n("Close"), KIcon("dialog-close")));
        enableButton(User1, true);
        enableButton(User2, true);
    }
}

void RemoveRedEyesWindow::slotProgressBarChanged(int v)
{
    int total = d->imageList->imageUrls().count();
    if (v == total)
        d->progressTimer->start(500);
}

void RemoveRedEyesWindow::slotProgressBarTimedOut()
{
    d->progress->hide();
    setBusy(false);

    if (d->runtype == WorkerThread::TestRun)
    {
        emit signalTestRunFinished();
    }

    disconnect(d->wth, SIGNAL(calculationFinished(WorkerThreadData*)),
               this, SLOT(calculationFinished(WorkerThreadData*)));
}

void RemoveRedEyesWindow::checkForNoneCorrectedImages()
{
    // check if imageslist has none corrected eyes
    if (d->imageList->hasNoneCorrectedImages())
    {
        if (KMessageBox::questionYesNo(this,
                                       i18n("<p>Some of the images could not be analyzed "
                                            "with the current settings or they do not "
                                            "contain any red-eyes at all.</p>"
                                            "<p><b>Would you like to remove those images "
                                            "from the list?</b></p>"),
                                            i18n("Remove unprocessed images?")) == KMessageBox::Yes)
        {
            d->imageList->removeNoneCorrectedImages();
        }
    }
}

void RemoveRedEyesWindow::slotImageListChanged(bool)
{
    if (d->imageList->imageUrls().isEmpty())
    {
        enableButton(User1, false);
        enableButton(User2, false);
    }
    else
    {
        enableButton(User1, true);
        enableButton(User2, true);
    }
}

void RemoveRedEyesWindow::slotFoundRAWImages(bool raw)
{
    if (raw)
    {
        KMessageBox::information(this,
                                 i18n("<p>You tried to add <b>RAW images</b> to the red-eye batch removal plugin, "
                                      "but those filetypes are not supported.</p>"
                                      "<p><b>They were automatically removed from the list.</b></p>"),
                                      i18n("RAW images found"));
    }
}

void RemoveRedEyesWindow::calculationFinished(WorkerThreadData* data)
{
    if (!data)
        return;

    // to prevent memory leak, save the interesting parts
    // of the event data in local vars and delete the data object
    int current     = data->current();
    const KUrl& url = data->url();
    int eyes        = data->eyes();
    delete data;

    d->progress->setValue(current);
    d->imageList->slotAddEyeCounterByUrl(url, eyes);
}

void RemoveRedEyesWindow::settingsModeChanged()
{
    switch (d->settingsStack->currentIndex())
    {
        case Simple:
            setSettingsMode(Advanced);
            break;

        case Advanced:
            setSettingsMode(Simple);
            break;
    }
}

void RemoveRedEyesWindow::setSettingsMode(SettingsMode mode)
{
    switch (mode)
    {
        case Simple:
            d->settingsSwitcherBtn->setText(i18n("Advanced Mode"));
            d->settingsStack->setCurrentIndex(Simple);
            d->simpleCorrectionMode = true;
            break;

        case Advanced:
            d->settingsSwitcherBtn->setText(i18n("Simple Mode"));
            d->settingsStack->setCurrentIndex(Advanced);
            d->simpleCorrectionMode = false;
            break;
    }
}

} // namespace KIPIRemoveRedEyesPlugin
