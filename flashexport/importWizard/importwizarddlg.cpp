/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-13
 * Description : a plugin to export images to flash
 *
 * Copyright (C) 2011-2013 by Veaceslav Munteanu <slavuttici at gmail dot com>
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

#include "importwizarddlg.h"

// Qt includes

#include <QMenu>
#include <QStandardPaths>
#include <QApplication>
#include <QMessageBox>
#include <QStandardPaths>

// KDE includes

#include <klocalizedstring.h>
#include <kjobwidgets.h>
#include <kconfig.h>
#include <kio/job.h>
#include <kio/deletejob.h>
#include <kconfiggroup.h>

// Libkipi includes

#include <KIPI/Interface>

// Locale incudes.

#include "aboutdata.h"
#include "intropage.h"
#include "firstrunpage.h"
#include "selectionpage.h"
#include "generalpage.h"
#include "lookpage.h"
#include "progresspage.h"
#include "kipiplugins_debug.h"

namespace KIPIFlashExportPlugin
{

class ImportWizardDlg::Private
{
public:

    Private()
    {
        mngr          = 0;
        simple        = 0;
        introPage     = 0;
        firstrunPage  = 0;
        selectionPage = 0;
        lookPage      = 0;
        generalPage   = 0;
        progressPage  = 0;
        settings      = 0;
        exporting     = false;
    }

    FlashManager*                  mngr;
    SimpleViewer*                  simple;
    SimpleViewerSettingsContainer* settings;

    IntroPage*                     introPage;
    FirstRunPage*                  firstrunPage;
    SelectionPage*                 selectionPage;
    LookPage*                      lookPage;
    GeneralPage*                   generalPage;
    ProgressPage*                  progressPage;

    bool                           exporting;
};

ImportWizardDlg::ImportWizardDlg(FlashManager* const mngr, QWidget* const parent)
    : KPWizardDialog(parent),
      d(new Private)
{
    setModal(false);
    setWindowTitle(i18n("Flash Export Wizard"));
    setAboutData(new FlashExportAboutData());

    d->mngr              = mngr;
    //SimpleViewer must be initialized, or we will get a null poiter.
    d->mngr->initSimple();
    d->simple            = mngr->simpleView();
    qCDebug(KIPIPLUGINS_LOG) << "pointer of simpleview" << mngr->simpleView();

    //-----------------------------------------------------------------

    d->settings          = new SimpleViewerSettingsContainer();
    d->introPage         = new IntroPage(this);
    d->firstrunPage      = new FirstRunPage(this);
    d->selectionPage     = new SelectionPage(d->mngr, this);
    d->lookPage          = new LookPage(this);
    d->generalPage       = new GeneralPage(this);
    d->progressPage      = new ProgressPage(d->mngr, this);

    //------------------------------------------------------------

    connect(d->firstrunPage, SIGNAL(signalUrlObtained()),
            this, SLOT(slotActivate()));

    connect(this, SIGNAL(rejected()),
            d->simple,SLOT(slotCancel()));

    connect(d->simple, SIGNAL(signalProcessingDone()),
            this, SLOT(slotFinishEnable()));

    connect(this, SIGNAL(currentIdChanged(int)),
            this, SLOT(slotCurrentIdChanged(int)));

    // ---------------------------------------------------------------

    resize(600, 500);
}

ImportWizardDlg::~ImportWizardDlg()
{
    delete d;
}

void ImportWizardDlg::slotActivate()
{
    qCDebug(KIPIPLUGINS_LOG) << "Installing " << d->firstrunPage->getUrl();

    if (d->mngr->installPlugin(d->firstrunPage->getUrl()))
    {
        d->firstrunPage->setComplete(true);
    }
    else
    {
        QMessageBox::critical(this, i18n("SimpleViewer installation failed"),
                              i18n("<p>Failed to install SimpleViewer. </p>"
                                   "<p>Please check if:</p>"
                                   "<p>- archive corresponds to plugin selected on previous page.</p>"
                                   "<p>- archive is up-to-date and is not corrupted.</p>"));
    }
}

void ImportWizardDlg::slotFinishEnable()
{
    d->progressPage->setComplete(true);
    d->exporting = false;
}

FlashManager* ImportWizardDlg::manager() const
{
    return d->mngr;
}

bool ImportWizardDlg::validateCurrentPage()
{
    if (currentPage() == d->introPage)
    {
        d->introPage->settings(d->settings);
        d->simple->appendPluginFiles(d->settings->plugType);
        d->lookPage->setPageContent(d->settings->plugType);
        readSettings();
        d->selectionPage->setPageContent(d->settings->imgGetOption);
    }

    // Must have at least one collection (or some images) to proceed.
    if (currentPage() == d->selectionPage)
    {
        if (d->selectionPage->isSelectionEmpty(d->settings->imgGetOption))
        {
            QMessageBox::information(this, i18n("Problem to export collection"),
                                     i18n("You must select at least one collection to export."));
            return false;
        }
    }

    if (currentPage() == d->generalPage && !d->exporting)
    {
        saveSettings();
        // Disable Finish button while exporting
        d->progressPage->setComplete(false);

        if (!checkIfFolderExist())
        {
            return false;
        }

        d->exporting = true;
        next();
        d->simple->startExport();
        return false;
    }

    return true;
}

int ImportWizardDlg::nextId() const
{
    // Using KPWizardDialog::nextId() to skip firstrun page if
    // plugin is already installed
    if (checkIfPluginInstalled())
    {
        if (currentPage() == d->introPage)
        {
            return currentId() + 2;
        }
    }
    else
    {
        //next must be disabled until receive Url via slotActivate.
        d->firstrunPage->setComplete(false);
    }

    return KPWizardDialog::nextId();
}

void ImportWizardDlg::slotCurrentIdChanged(int id)
{
    if (page(id) == d->generalPage && d->exporting)
    {
        d->simple->slotCancel();
        d->exporting = false;
    }
}

bool ImportWizardDlg::checkIfFolderExist()
{
    KIO::StatJob* const job = KIO::stat(d->settings->exportUrl, KIO::StatJob::DestinationSide, 0);
    KJobWidgets::setWindow(job, QApplication::activeWindow());
    job->exec();

    if (!job->error())
    {
        int ret = QMessageBox::warning(this, i18n("Target Folder Exists"),
                                       i18n("Target folder %1 already exists.\n"
                                            "Do you want to overwrite it? All data in this folder will be lost.",
                                            d->settings->exportUrl.path()),
                                       QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)
                                      );

        switch(ret)
        {
            case QMessageBox::Yes:
            {
                auto deleteJob = KIO::del(d->settings->exportUrl);
                KJobWidgets::setWindow(deleteJob, QApplication::activeWindow());

                if (!deleteJob->exec())
                {
                    QMessageBox::critical(this, i18n("Cannot Delete Folder"),
                                          i18n("Could not delete %1.\n"
                                               "Please choose another export folder.",
                                               d->settings->exportUrl.path()));
                    return false;
                }

                return true;
            }
            case QMessageBox::No:
                return false;

            case QMessageBox::Cancel:
                return false;

            default:
                return false;
        }
    }

    return true;
}

bool ImportWizardDlg::checkIfPluginInstalled() const
{
    switch(d->settings->plugType)
    {
        case 0:
            return ! QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                            QLatin1String("kipiplugin_flashexport/simpleviewer/simpleviewer.swf")).isEmpty();
            break;
        case 1:
            return ! QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                            QLatin1String("kipiplugin_flashexport/autoviewer/autoviewer.swf")).isEmpty();
            break;
        case 2:
            return ! QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                            QLatin1String("kipiplugin_flashexport/tiltviewer/TiltViewer.swf")).isEmpty();
            break;
        case 3:
            return ! QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                            QLatin1String("kipiplugin_flashexport/postcardviewer/viewer.swf")).isEmpty();
            break;
        default:
            qCDebug(KIPIPLUGINS_LOG) << "Unknown plugin type";
            return false;
    }
}

void ImportWizardDlg::readSettings()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group("FlashExport Settings");

    d->settings->thumbnailRows        = group.readEntry("ThumbnailRows", 3);
    d->settings->thumbnailColumns     = group.readEntry("ThumbnailColumns", 3);
    d->settings->backgroundColor      = group.readEntry("BackgroundColor", QColor("#181818"));
    d->settings->frameColor           = group.readEntry("FrameColor", QColor("#ffffff"));
    d->settings->frameWidth           = group.readEntry("FrameWidth", 1);
    d->settings->title                = group.readEntry("Title", QString());
    d->settings->exportUrl            = group.readEntry("ExportUrl", QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QLatin1String("/simpleviewer")));
    d->settings->resizeExportImages   = group.readEntry("ResizeExportImages", true);
    d->settings->imagesExportSize     = group.readEntry("ImagesExportSize", 640);
    d->settings->showComments         = group.readEntry("ShowComments", true);
    d->settings->enableRightClickOpen = group.readEntry("EnableRightClickOpen", false);
    d->settings->fixOrientation       = group.readEntry("FixOrientation", true);
    d->settings->openInKonqueror      = group.readEntry("OpenInKonqueror", true);
    d->settings->showKeywords         = group.readEntry("ShowKeywords", true);

    //---Simpleviewer settings ----
    d->settings->textColor            = group.readEntry("TextColor", QColor("#ffffff"));
    d->settings->thumbnailPosition    = (SimpleViewerSettingsContainer::ThumbPosition)group.readEntry("ThumbnailPosition", (int)SimpleViewerSettingsContainer::RIGHT);
    d->settings->stagePadding         = group.readEntry("StagePadding", 20);
    d->settings->maxImageDimension    = group.readEntry("MaxImageDimension", 640);

    //---Autoviewer settings ----
    d->settings->imagePadding         = group.readEntry("ImagePadding", 20);
    d->settings->displayTime          = group.readEntry("DisplayTime", 6);

    //---Tiltviewer settings ----
    d->settings->showFlipButton       = group.readEntry("ShowFlipButton", true);
    d->settings->useReloadButton      = group.readEntry("UseReloadButton",true);
    d->settings->bkgndInnerColor      = group.readEntry("BackgroundInnerColor", QColor("#ffffff"));
    d->settings->bkgndOuterColor      = group.readEntry("BackgroundOuterColor", QColor("#ffffff"));
    d->settings->backColor            = group.readEntry("BackColor", QColor("#FFDCA8"));

    //---Postcardviewer settings ----
    d->settings->cellDimension        = group.readEntry("CellDimension", 800);
    d->settings->zoomOutPerc          = group.readEntry("ZoomOutPerc", 15);
    d->settings->zoomInPerc           = group.readEntry("ZoomInPerc", 100);

    d->generalPage->setSettings(d->settings);
    d->lookPage->setSettings(d->settings);
}

void ImportWizardDlg::saveSettings()
{
    d->selectionPage->settings(d->settings);
    d->generalPage->settings(d->settings);
    d->lookPage->settings(d->settings);

    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group("FlashExport Settings");

    group.writeEntry("ThumbnailRows", d->settings->thumbnailRows);
    group.writeEntry("ThumbnailColumns", d->settings->thumbnailColumns);
    group.writeEntry("BackgroundColor", d->settings->backgroundColor);
    group.writeEntry("FrameColor", d->settings->frameColor);
    group.writeEntry("FrameWidth", d->settings->frameWidth);
    group.writeEntry("Title", d->settings->title);
    group.writeEntry("ExportUrl", QUrl(d->settings->exportUrl));
    group.writeEntry("ResizeExportImages", d->settings->resizeExportImages);
    group.writeEntry("ImagesExportSize", d->settings->imagesExportSize);
    group.writeEntry("ShowComments", d->settings->showComments);
    group.writeEntry("EnableRightClickOpen", d->settings->enableRightClickOpen);
    group.writeEntry("FixOrientation", d->settings->fixOrientation);
    group.writeEntry("OpenInKonqueror", d->settings->openInKonqueror);
    group.writeEntry("ShowKeywords", d->settings->showKeywords);

    //---Simpleviewer settings ---
    group.writeEntry("ThumbnailPosition", (int)d->settings->thumbnailPosition);
    group.writeEntry("TextColor", d->settings->textColor);
    group.writeEntry("StagePadding", d->settings->stagePadding);
    group.writeEntry("MaxImageDimension", d->settings->maxImageDimension);

    //---Autoviewer settings ---
    group.writeEntry("ImagePadding",d->settings->imagePadding);
    group.writeEntry("DisplayTime",d->settings->displayTime);

    //---Tiltviewer settings ----
    group.writeEntry("ShowFlipButton", d->settings->showFlipButton);
    group.writeEntry("UseReloadButton",d->settings->useReloadButton);
    group.writeEntry("BackgroundInnerColor", d->settings->bkgndInnerColor);
    group.writeEntry("BackgroundOuterColor", d->settings->bkgndOuterColor);
    group.writeEntry("BackColor", d->settings->backColor);

    //---Postcardviewer settings ----
    group.writeEntry("CellDimension", d->settings->cellDimension);
    group.writeEntry("ZoomOutPerc", d->settings->zoomOutPerc);
    group.writeEntry("ZoomInPerc", d->settings->zoomInPerc);

    config.sync();
    d->simple->setSettings(d->settings);
}

} // namespace KIPIFlashExportPlugin
