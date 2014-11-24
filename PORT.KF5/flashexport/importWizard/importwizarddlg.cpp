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

#include "importwizarddlg.moc"

// KDE includes

#include <kmenu.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kglobalsettings.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kapplication.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Locale incudes.

#include "aboutdata.h"
#include "intropage.h"
#include "firstrunpage.h"
#include "selectionpage.h"
#include "generalpage.h"
#include "lookpage.h"
#include "progresspage.h"

namespace KIPIFlashExportPlugin
{

class ImportWizardDlg::ImportWizardDlgPriv
{
public:

    ImportWizardDlgPriv()
    {
        mngr              = 0;
        simple            = 0;
        introPage         = 0;
        firstrunPage      = 0;
        selectionPage     = 0;
        lookPage          = 0;
        generalPage       = 0;
        progressPage      = 0;
        settings          = 0;
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
};

ImportWizardDlg::ImportWizardDlg(FlashManager* const mngr, QWidget* const parent)
    : KPWizardDialog(parent), d(new ImportWizardDlgPriv)
{
    setModal(false);
    setWindowTitle(i18n("Flash Export Wizard"));
    setAboutData(new FlashExportAboutData());

    d->mngr              = mngr;
    //SimpleViewer must be initialized, or we will get a null poiter.
    d->mngr->initSimple();
    d->simple            = mngr->simpleView();
    kDebug() << "pointer of simpleview" << mngr->simpleView();

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

    // ---------------------------------------------------------------

    resize(600, 500);
}

ImportWizardDlg::~ImportWizardDlg()
{
    delete d;
}

void ImportWizardDlg::slotActivate()
{
    if(d->mngr->installPlugin(d->firstrunPage->getUrl()))
        setValid(d->firstrunPage->page(),true);
    else
        KMessageBox::error(this, i18n("<p>SimpleViewer installation failed. </p>"
                                      "<p>Please check if:</p>"
                                      "<p>- archive corresponds to plugin selected on previous page.</p>"
                                      "<p>- archive is up-to-date and is not corrupted.</p>"));
}

void ImportWizardDlg::slotFinishEnable()
{
    setValid(d->progressPage->page(),true);
}

FlashManager* ImportWizardDlg::manager() const
{
    return d->mngr;
}

void ImportWizardDlg::next()
{
    if(currentPage() == d->introPage->page())
    {
        d->introPage->settings(d->settings);
        d->simple->appendPluginFiles(d->settings->plugType);
        d->lookPage->setPageContent(d->settings->plugType);
        readSettings();
        d->selectionPage->setPageContent(d->settings->imgGetOption);
    }

    // Using KAssistantDialog::next twice to skip firstrun page if
    // plugin is already installed
    if(checkIfPluginInstalled())
    {
        if(currentPage() == d->introPage->page())
            KAssistantDialog::next();
    }
    else
    {
        //next must be disabled until receive Url via slotActivate.
        setValid(d->firstrunPage->page(),false);
    }
    
    // Must have at least one collection (or some images) to proceed.
    if(currentPage() == d->selectionPage->page())
    {
        if (d->selectionPage->isSelectionEmpty(d->settings->imgGetOption))
        {
            KMessageBox::sorry(this, i18n("You must select at least one collection to export."));
            return;
        }
    }

    if(currentPage() == d->generalPage->page())
    {
        saveSettings();
        // Disable Finish button while exporting
        setValid(d->progressPage->page(),false);
        if(!checkIfFolderExist())
            return;
        KAssistantDialog::next();
        d->simple->startExport();
        return;
    }

    KAssistantDialog::next();
}

void ImportWizardDlg::back()
{
    if(checkIfPluginInstalled() && currentPage() == d->selectionPage->page())
        KAssistantDialog::back();

    if(currentPage() == d->progressPage->page())
        d->simple->slotCancel();

    KAssistantDialog::back();
}

bool ImportWizardDlg::checkIfFolderExist()
{
    if(KIO::NetAccess::exists(d->settings->exportUrl, KIO::NetAccess::DestinationSide, kapp->activeWindow()))
    {
        int ret = KMessageBox::warningYesNoCancel(this,
                                                  i18n("Target folder %1 already exists.\n"
                                                       "Do you want to overwrite it? All data in this folder will be lost.",
                                                       d->settings->exportUrl.path()));

        switch(ret)
        {
            case KMessageBox::Yes:
                if(!KIO::NetAccess::del(d->settings->exportUrl, kapp->activeWindow()))
                {
                    KMessageBox::error(this, i18n("Could not delete %1.\n"
                                       "Please choose another export folder.",
                                       d->settings->exportUrl.path()));
                    return false;
                }

                return true;

            case KMessageBox::No:
                return false;

            case KMessageBox::Cancel:
                return false;

            default:
                return false;
        }
    }

    return true;
}

bool ImportWizardDlg::checkIfPluginInstalled()
{ 
    switch(d->settings->plugType)
    {
        case 0:
            return ! KStandardDirs::locate("data","kipiplugin_flashexport/simpleviewer/simpleviewer.swf").isEmpty();
            break;
        case 1:
            return ! KStandardDirs::locate("data","kipiplugin_flashexport/autoviewer/autoviewer.swf").isEmpty();
            break;
        case 2:
            return ! KStandardDirs::locate("data","kipiplugin_flashexport/tiltviewer/TiltViewer.swf").isEmpty();
            break;
        case 3:
            return ! KStandardDirs::locate("data","kipiplugin_flashexport/postcardviewer/viewer.swf").isEmpty();
            break;
        default:
            kDebug() << "Unknown plugin type";
            return false;
    }

}
void ImportWizardDlg::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("FlashExport Settings");

    d->settings->thumbnailRows        = group.readEntry("ThumbnailRows", 3);
    d->settings->thumbnailColumns     = group.readEntry("ThumbnailColumns", 3);
    d->settings->backgroundColor      = group.readEntry("BackgroundColor", QColor("#181818"));
    d->settings->frameColor           = group.readEntry("FrameColor", QColor("#ffffff"));
    d->settings->frameWidth           = group.readEntry("FrameWidth", 1);
    d->settings->title                = group.readEntry("Title", QString());
    d->settings->exportUrl            = group.readEntry("ExportUrl", KUrl(KGlobalSettings::documentPath() + "/simpleviewer"));
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

    KConfig config("kipirc");
    KConfigGroup group = config.group("FlashExport Settings");

    group.writeEntry("ThumbnailRows", d->settings->thumbnailRows);
    group.writeEntry("ThumbnailColumns", d->settings->thumbnailColumns);
    group.writeEntry("BackgroundColor", d->settings->backgroundColor);
    group.writeEntry("FrameColor", d->settings->frameColor);
    group.writeEntry("FrameWidth", d->settings->frameWidth);
    group.writeEntry("Title", d->settings->title);
    group.writeEntry("ExportUrl",  d->settings->exportUrl);
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
