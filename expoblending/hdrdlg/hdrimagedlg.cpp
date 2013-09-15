/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-09-04
 * Description : a tool to blend bracketed images/create HDR images.
 *
 * Copyright (C) 2013 by Soumajyoti Sarkar <ergy dot ergy at gmail dot com>
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

#include "hdrimagedlg.moc"

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

#include <QPointer>
#include <QCloseEvent>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QString>

// KDE includes

#include <klineedit.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kvbox.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kio/renamedialog.h>
#include <kde_file.h>
#include <kurl.h>

// LibKIPI includes

#include <libkipi/interface.h>

// libKdcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/rexpanderbox.h>

// Local includes

#include "aboutdata.h"
#include "actionthread.h"
#include "bracketstack.h"
#include "pfsbinary.h"
#include "pfshdrgenbinary.h"
#include "pfshdrstack.h"
#include "pfshdrsettings.h"
#include "enfusestack.h"
#include "kpoutputdialog.h"
#include "kppreviewmanager.h"
#include "kpsavesettingswidget.h"
#include "kpversion.h"
#include "manager.h"

using namespace KDcrawIface;

namespace KIPIExpoBlendingPlugin
{

class HdrImageDlg::HdrImageDlgPriv
{
public:

    HdrImageDlgPriv()
    {
        previewWidget       = 0;
        saveSettingsBox     = 0;
        bracketStack        = 0;
        pfshdrStack         = 0;
        settingsExpander    = 0;
        mngr                = 0;
        firstImageDisplayed = false;
        templateFileName    = 0;
    }

    QString               inputFileName;
    QString               output;
    QString               dirName;
    
    KUrl::List            inputList;

    KLineEdit*            templateFileName;

    KPPreviewManager*     previewWidget;

    RExpanderBox*         settingsExpander;

    KPSaveSettingsWidget* saveSettingsBox;

    BracketStackList*     bracketStack;
    PfsHdrStackList*      pfshdrStack;

    Manager*              mngr;

    bool                  firstImageDisplayed;
    
};

HdrImageDlg::HdrImageDlg(Manager* const mngr, QWidget* const parent)
    : KPToolDialog(parent), d(new HdrImageDlgPriv)
{
    d->mngr = mngr;

    setModal(false);
    setButtons(Help | Default | User1 | User2 | User3 | Close);
    setDefaultButton(Close);
    setCaption(i18n("HDR Image Creation"));

    setButtonText(   User1, i18n("&Save"));
    setButtonToolTip(User1, i18n("Save the selected HDR images"));
    setButtonIcon(   User1, KIcon("document-save"));

    setButtonText(   User2, i18n("&Preview"));
    setButtonToolTip(User2, i18n("Create an HDR image with current bracket stack."));
    setButtonIcon(   User2, KIcon("system-run"));

    setButtonText(   User3, i18n("&Abort"));
    setButtonToolTip(User3, i18n("Abort current process"));
    setButtonIcon(   User3, KIcon("dialog-cancel"));

    setButtonToolTip(Close, i18n("Exit this tool"));
    setModal(false);
    setAboutData(new HdrImageAboutData());

    // ---------------------------------------------------------------

    QWidget* page     = new QWidget(this);
    QGridLayout* grid = new QGridLayout(page);
    setMainWidget(page);

    d->previewWidget  = new KPPreviewManager(page);
    d->previewWidget->setButtonText(i18n("Details..."));

    // ---------------------------------------------------------------

    QScrollArea* sv      = new QScrollArea(page);
    KVBox* panel         = new KVBox(sv->viewport());
    panel->setAutoFillBackground(false);
    sv->setWidget(panel);
    sv->setWidgetResizable(true);
    sv->setAutoFillBackground(false);
    sv->viewport()->setAutoFillBackground(false);

    d->bracketStack      = new BracketStackList(d->mngr->iface(), panel);

    d->settingsExpander  = new RExpanderBox(panel);
    d->settingsExpander->setObjectName("HDR Image Settings Expander");

    d->saveSettingsBox   = new KPSaveSettingsWidget(d->settingsExpander);

    KHBox* hbox          = new KHBox(d->saveSettingsBox);
    QLabel* customLabel  = new QLabel(hbox);
    d->templateFileName  = new KLineEdit(hbox);
    d->templateFileName->setClearButtonShown(true);
    customLabel->setText(i18n("File Name Template: "));
    d->saveSettingsBox->setCustomSettingsWidget(hbox);

    d->pfshdrStack       = new PfsHdrStackList(panel);

    d->settingsExpander->addItem(d->saveSettingsBox,   i18n("Save Settings"),   QString("savesettings"), true);
    d->settingsExpander->setItemIcon(0, SmallIcon("Hdr Image"));
    d->settingsExpander->setItemIcon(1, SmallIcon("document-save"));

    // ---------------------------------------------------------------

    grid->addWidget(d->previewWidget, 0, 0, 3, 1);
    grid->addWidget(sv,               0, 1, 3, 1);
    grid->setMargin(0);
    grid->setSpacing(spacingHint());
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(1, 5);

    // ---------------------------------------------------------------

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(defaultClicked()),
            this, SLOT(slotDefault()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotProcess()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotPreview()));

    connect(this, SIGNAL(user3Clicked()),
            this, SLOT(slotAbort()));

    connect(d->mngr->thread(), SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
            this, SLOT(slotAction(KIPIExpoBlendingPlugin::ActionData)));

    connect(d->mngr->thread(), SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)),
            this, SLOT(slotAction(KIPIExpoBlendingPlugin::ActionData)));

    connect(d->bracketStack, SIGNAL(signalAddItems(KUrl::List)),
            this, SLOT(slotAddItems(KUrl::List)));

    connect(d->previewWidget, SIGNAL(signalButtonClicked()),
            this, SLOT(slotPreviewButtonClicked()));

    connect(d->pfshdrStack, SIGNAL(signalItemClicked(KUrl)),
            this, SLOT(slotLoadProcessed(KUrl)));

    connect(d->templateFileName, SIGNAL(textChanged(QString)),
            this, SLOT(slotFileFormatChanged()));

    connect(d->saveSettingsBox, SIGNAL(signalSaveFormatChanged()),
            this, SLOT(slotFileFormatChanged()));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
    loadItems(d->mngr->itemsList());
}

HdrImageDlg::~HdrImageDlg()
{
    delete d;
}

void ExpoBlendingDlg::closeEvent(QCloseEvent* e)
{
    if (!e) return;
    d->mngr->thread()->cancel();
    d->mngr->cleanUp();
    saveSettings();
    e->accept();
}

void ExpoBlendingDlg::slotClose()
{
    d->mngr->thread()->cancel();
    saveSettings();
    done(Close);
}

void ExpoBlendingDlg::slotFileFormatChanged()
{
    d->pfshdrStack->setTemplateFileName(d->saveSettingsBox->fileFormat(), d->templateFileName->text());
}

void ExpoBlendingDlg::slotPreviewButtonClicked()
{
    KPOutputDialog dlg(kapp->activeWindow(),
                       i18n("Hdr Image Creation Messages"),
                       d->output);

    dlg.setAboutData(new ExpoBlendingAboutData());
}

void ExpoBlendingDlg::loadItems(const KUrl::List& urls)
{
    d->bracketStack->clear();
    d->bracketStack->addItems(urls);
}

void ExpoBlendingDlg::slotAddItems(const KUrl::List& urls)
{
    if (!urls.empty())
    {
        d->mngr->thread()->identifyFiles(urls, d->mngr->exposureValuesMap());
        if (!d->mngr->thread()->isRunning())
            d->mngr->thread()->start();
    }
}

void ExpoBlendingDlg::slotLoadProcessed(const KUrl& url)
{
    d->mngr->thread()->loadProcessed(url);
    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void ExpoBlendingDlg::setIdentity(const KUrl& url, const QString& identity)
{
    BracketStackItem* item = d->bracketStack->findItem(url);
    if (item)
        item->setExposure(identity);
}

void ExpoBlendingDlg::busy(bool val)
{
    d->saveSettingsBox->setEnabled(!val);
    d->bracketStack->setEnabled(!val);
    enableButton(User1, !val ? !d->pfshdrStack->settingsList().isEmpty() : false);
    enableButton(User2, !val);
    enableButton(User3, val);
    enableButton(Close, !val);
    if (val)
        d->previewWidget->setButtonVisible(false);
}

void ExpoBlendingDlg::slotDefault()
{
    d->saveSettingsBox->resetToDefault();
    d->templateFileName->setText("PfsHdr");
}

void ExpoBlendingDlg::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Hdr Image Settings"));

    d->saveSettingsBox->readSettings(group);

#if KDCRAW_VERSION >= 0x020000
    d->settingsExpander->readSettings(group);
#else
    d->settingsExpander->readSettings();
#endif

    d->templateFileName->setText(group.readEntry("Template File Name", QString("PfsHdr")));

    KConfigGroup group2 = config.group(QString("Hdr Image Creation Dialog"));
    restoreDialogSize(group2);
}

void ExpoBlendingDlg::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Hdr Image Settings"));

    d->saveSettingsBox->writeSettings(group);

#if KDCRAW_VERSION >= 0x020000
    d->settingsExpander->writeSettings(group);
#else
    d->settingsExpander->writeSettings();
#endif

    group.writeEntry("Template File Name", d->templateFileName->text());

    KConfigGroup group2 = config.group(QString("Hdr Image Creation Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void ExpoBlendingDlg::slotPreview()
{
    KUrl::List selectedUrl = d->bracketStack->urls();
    if (selectedUrl.isEmpty()) return;
    
    ItemUrlsMap map = d->mngr->preProcessedMap();
    KUrl::List preprocessedList;

    foreach(const KUrl& url, selectedUrl)
    {
        ItemPreprocessedUrls preprocessedUrls = *(map.find(url));
        preprocessedList.append(preprocessedUrls.previewUrl);
    }

    d->inputList = preprocessedList;
    PfsHdrSettings settings = new PfsHdrSettings();
    settings.inputUrls      = d->bracketStack->urls();
    settings.outputFormat   = d->saveSettingsBox->fileFormat();
    d->mngr->thread()->hdrGen(preprocessedList, d->dirName, settings, 0);
    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void ExpoBlendingDlg::slotProcess()
{
    QList<PfsHdrSettings> list = d->pfshdrStack->settingsList();
    if (list.isEmpty()) return;

    ItemUrlsMap map = d->mngr->preProcessedMap();
    KUrl::List preprocessedList;

    foreach(const PfsHdrSettings& settings, list)
    {
        preprocessedList.clear();
        
        foreach(const KUrl& url, settings.inputUrls)
        {
            ItemPreprocessedUrls preprocessedUrls = *(map.find(url));
            preprocessedList.append(preprocessedUrls.preprocessedUrl);
        }

        d->mngr->thread()->hdrGen(preprocessedList, d->dirName, settings, 1);
        if (!d->mngr->thread()->isRunning())
            d->mngr->thread()->start();
    }
}

void ExpoBlendingDlg::saveItem(const KUrl& temp, const PfsHdrSettings& settings)
{
    KUrl newUrl = temp;
    newUrl.setFileName(settings.targetFileName);
    QFileInfo fi(newUrl.toLocalFile());

    if (d->saveSettingsBox->conflictRule() != KPSaveSettingsWidget::OVERWRITE)
    {
        if (fi.exists())
        {
            KIO::RenameDialog dlg(this, i18n("A file named \"%1\" already "
                                                "exists. Are you sure you want "
                                                "to overwrite it?",
                                                newUrl.fileName()),
                                    temp, newUrl,
                                    KIO::RenameDialog_Mode(KIO::M_SINGLE | KIO::M_OVERWRITE | KIO::M_SKIP));

            switch (dlg.exec())
            {
                case KIO::R_CANCEL:
                case KIO::R_SKIP:
                {
                    newUrl.clear();
                    d->pfshdrStack->setOnItem(settings.previewUrl, false);
                    d->pfshdrStack->processedItem(settings.previewUrl, false);

                    break;
                }
                case KIO::R_RENAME:
                {
                    newUrl = dlg.newDestUrl();
                    break;
                }
                default:    // Overwrite.
                    break;
            }
        }
    }

    kDebug() << "Renaming " << temp << " to " << newUrl;

    if (!newUrl.isEmpty())
    {
        if (KDE::rename(temp.toLocalFile(), newUrl.toLocalFile()) != 0)
        {
            KMessageBox::error(this, i18n("Failed to save image to %1", newUrl.toLocalFile()));
            d->pfshdrStack->setOnItem(settings.previewUrl, false);
            d->pfshdrStack->processedItem(settings.previewUrl, false);
            return;
        }
        else
        {
            d->pfshdrStack->removeItem(settings.previewUrl);
        }
    }

    if (d->pfshdrStack->settingsList().isEmpty())
    {
        enableButton(User1, false);
        busy(false);
        d->previewWidget->setBusy(false);
    }
}

void ExpoBlendingDlg::slotAbort()
{
    d->mngr->thread()->cancel();
}

void ExpoBlendingDlg::slotAction(const KIPIExpoBlendingPlugin::ActionData& ad)
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
            case(LOAD):
            {
                busy(true);
                break;
            }
            case(HDRCALIBRATEPREVIEW):
            {
                busy(true);
                d->previewWidget->setBusy(true, i18n("Processing preview of bracketed images..."));
                break;
            }
            case(HDRCALIBRATEFINAL):
            {
                busy(true);
                d->previewWidget->setBusy(true, i18n("Processing targets of bracketed images..."));
                d->pfshdrStack->processingItem(ad.pfshdrSettings.previewUrl, true);
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
                {
                    setIdentity(ad.inUrls[0], ad.message);
                    busy(false);
                    break;
                }
                case(LOAD):
                {
                    d->previewWidget->setText(i18n("Failed to load processed image"), Qt::red);
                    busy(false);
                    break;
                }
                case(HDRCALIBRATEPREVIEW):
                {
                    d->output = ad.message;
                    d->previewWidget->setBusy(false);
                    d->previewWidget->setButtonVisible(true);
                    d->previewWidget->setText(i18n("Failed to process preview of bracketed images"), Qt::red);
                    busy(false);
                    break;
                }
                case(HDRCALIBRATEFINAL):
                {
                    slotAbort();
                    d->output = ad.message;
                    d->previewWidget->setBusy(false);
                    d->previewWidget->setButtonVisible(true);
                    d->previewWidget->setText(i18n("Failed to process targets of bracketed images"), Qt::red);
                    d->pfshdrStack->processingItem(ad.pfshdr.previewUrl, false);
                    d->pfshdrStack->setOnItem(ad.pfshdrSettings.previewUrl, false);
                    busy(false);
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
                    setIdentity(ad.inUrls[0], ad.message);
                    busy(false);
                    break;
                }
                case(LOAD):
                {
                    d->previewWidget->setImage(ad.image, !d->firstImageDisplayed);
                    d->firstImageDisplayed |= true;
                    d->enfuseStack->setThumbnail(ad.inUrls[0], ad.image);
                    busy(false);
                    break;
                }
                case(HDRGEN):
                {
	            d->mngr->thread()->hdrCalibrate(d->listUrls, d->dirName, ad.pfshdrSettings, ad.option);
	            break;
	        }
	        case(CAMERARESPONSE):
	        { 
                    loadItems(d->mngr->itemsList());
	            if(ad.option == 0)
		    {
	                d->mngr->thread()->hdrOutExrPreview(d->listUrls, d->dirName, d->mngr->itemsList()[0], ad.pfshdrSettings);
		    }
		    if(ad.option == 1)
		    {
	                d->mngr->thread()->hdrOutExrFinal(d->listUrls, d->dirName, d->mngr->itemsList()[0], ad.pfshdrSettings);
		    }
	            break;
	        }
	        case(HDRCALIBRATEPREVIEW):
	        {
		    kDebug() << "Preview Done";
		    d->pfshdrStack->addItem(ad.outUrls[0], ad.pfshdrSettings);
		    busy(false);
		    break;
	        }
	        case(HDRCALIBRATEFINAL):
	        {
		    kDebug() << "Final Done";
		    d->pfshdrStack->processingItem(ad.pfshdrSettings.previewUrl, false);
		    saveItem(ad.outUrls[0], ad.pfshdrSettings);
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

} // namespace KIPIExpoBlendingPlugin
