/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "expoblendingdlg.moc"

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

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <khelpmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imageinfo.h>

// libKdcraw includes

#include <libkdcraw/rexpanderbox.h>

// Local includes

#include "manager.h"
#include "enfusesettings.h"
#include "enfusestack.h"
#include "savesettingswidget.h"
#include "aboutdata.h"
#include "pluginsversion.h"
#include "previewmanager.h"
#include "actionthread.h"
#include "bracketstack.h"
#include "outputdialog.h"

using namespace KIPIPlugins;
using namespace KDcrawIface;

namespace KIPIExpoBlendingPlugin
{

class ExpoBlendingDlgPriv
{
public:

    ExpoBlendingDlgPriv()
    {
        previewWidget     = 0;
        saveSettingsBox   = 0;
        bracketStack      = 0;
        enfuseStack       = 0;
        enfuseSettingsBox = 0;
        settingsExpander  = 0;
        mngr              = 0;
    }

    QString               inputFileName;
    QString               output;

    PreviewManager*       previewWidget;

    RExpanderBox*         settingsExpander;

    EnfuseSettingsWidget* enfuseSettingsBox;

    SaveSettingsWidget*   saveSettingsBox;

    BracketStackList*     bracketStack;
    EnfuseStackList*      enfuseStack;

    Manager*              mngr;
};

ExpoBlendingDlg::ExpoBlendingDlg(Manager* mngr, QWidget* parent)
               : KDialog(parent), d(new ExpoBlendingDlgPriv)
{
    d->mngr = mngr;

    setButtons(Help | Default | User1 | User2 | User3 | Close);
    setDefaultButton(Close);
    setCaption(i18n("Exposure Blending"));

    setButtonText(   User1, i18n("&Save"));
    setButtonToolTip(User1, i18n("Save selected enfused items processed."));
    setButtonIcon(   User1, KIcon("document-save"));

    setButtonText(   User2, i18n("Pro&cess"));
    setButtonToolTip(User2, i18n("Process bracketed images stack with current settings."));
    setButtonIcon(   User2, KIcon("system-run"));

    setButtonText(   User3, i18n("&Abort"));
    setButtonToolTip(User3, i18n("Abort current process"));
    setButtonIcon(   User3, KIcon("dialog-cancel"));

    setButtonToolTip(Close, i18n("Exit this tool"));
    setModal(false);

    // ---------------------------------------------------------------

    QWidget *page     = new QWidget(this);
    QGridLayout *grid = new QGridLayout(page);
    setMainWidget(page);

    d->previewWidget  = new PreviewManager(page);
    d->previewWidget->setButtonText(i18n("Details..."));
    d->bracketStack   = new BracketStackList(d->mngr->iface(), page);

    // ---------------------------------------------------------------

    d->settingsExpander  = new RExpanderBox(page);
    d->settingsExpander->setObjectName("Exposure Blending Settings Expander");

    d->enfuseSettingsBox = new EnfuseSettingsWidget(d->settingsExpander);
    d->saveSettingsBox   = new SaveSettingsWidget(d->settingsExpander, false);

    d->enfuseStack       = new EnfuseStackList(d->mngr, page);

    d->settingsExpander->addItem(d->enfuseSettingsBox, i18n("Enfuse Settings"), QString("expoblending"), true);
    d->settingsExpander->addItem(d->saveSettingsBox,   i18n("Save Settings"),   QString("savesettings"), true);
    d->settingsExpander->setItemIcon(0, SmallIcon("expoblending"));
    d->settingsExpander->setItemIcon(1, SmallIcon("document-save"));

    // ---------------------------------------------------------------

    grid->addWidget(d->previewWidget,    0, 0, 3, 1);
    grid->addWidget(d->bracketStack,     0, 1, 1, 1);
    grid->addWidget(d->settingsExpander, 1, 1, 1, 1);
    grid->addWidget(d->enfuseStack,      2, 1, 1, 1);
    grid->setMargin(0);
    grid->setSpacing(spacingHint());
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(1, 5);

    // ---------------------------------------------------------------
    // About data and help button.

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->mngr->about(), false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ---------------------------------------------------------------

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(defaultClicked()),
            this, SLOT(slotDefault()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotSaveItems()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotProcess()));

    connect(this, SIGNAL(user3Clicked()),
            this, SLOT(slotAbort()));

    connect(d->mngr->thread(), SIGNAL(starting(const KIPIExpoBlendingPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIExpoBlendingPlugin::ActionData&)));

    connect(d->mngr->thread(), SIGNAL(finished(const KIPIExpoBlendingPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIExpoBlendingPlugin::ActionData&)));

    connect(d->bracketStack, SIGNAL(signalAddItems(const KUrl::List&)),
            this, SLOT(slotAddItems(const KUrl::List&)));

    connect(d->previewWidget, SIGNAL(signalButtonClicked()),
            this, SLOT(slotPreviewButtonClicked()));

    connect(d->enfuseStack, SIGNAL(signalItemClicked(const KUrl&)),
            d->previewWidget, SLOT(slotLoad(const KUrl&)));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
    loadItems(d->mngr->itemsList());
}

ExpoBlendingDlg::~ExpoBlendingDlg()
{
    delete d;
}

void ExpoBlendingDlg::slotHelp()
{
    KToolInvocation::invokeHelp("expoblending", "kipi-plugins");
}

void ExpoBlendingDlg::closeEvent(QCloseEvent* e)
{
    if (!e) return;
    d->mngr->thread()->cancel();
    saveSettings();
    e->accept();
}

void ExpoBlendingDlg::slotClose()
{
    d->mngr->thread()->cancel();
    saveSettings();
    done(Close);
}

void ExpoBlendingDlg::slotPreviewButtonClicked()
{
    OutputDialog dlg(kapp->activeWindow(),
                     i18n("Enfuse Processing Messages"),
                     d->output);
                     i18n("Enfuse Processing Messages"),
    dlg.setAboutData((KPAboutData*)d->mngr->about(), QString("expoblending"));
    dlg.exec();
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
        d->mngr->thread()->identifyFiles(urls);
        if (!d->mngr->thread()->isRunning())
            d->mngr->thread()->start();
    }
}

void ExpoBlendingDlg::setIdentity(const KUrl& url, const QString& identity)
{
    BracketStackItem* item = d->bracketStack->findItem(url);
    if (item)
        item->setExposure(identity);
}

void ExpoBlendingDlg::busy(bool val)
{
    d->enfuseSettingsBox->setEnabled(!val);
    d->saveSettingsBox->setEnabled(!val);
    d->bracketStack->setEnabled(!val);
    enableButton(User1, !val ? !d->enfuseStack->urlsMap().isEmpty() : false);
    enableButton(User2, !val);
    enableButton(User3, val);
    enableButton(Close, !val);
    if (val)
        d->previewWidget->setButtonVisible(false);
}

void ExpoBlendingDlg::slotDefault()
{
    d->enfuseSettingsBox->setDefaultSettings();
    d->saveSettingsBox->setDefaultSettings();
}

void ExpoBlendingDlg::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("ExpoBlending Settings"));

    d->enfuseSettingsBox->readSettings(group);
    d->saveSettingsBox->readSettings(group);
    d->settingsExpander->readSettings();

    KConfigGroup group2 = config.group(QString("ExpoBlending Dialog"));
    restoreDialogSize(group2);
}

void ExpoBlendingDlg::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("ExpoBlending Settings"));

    d->enfuseSettingsBox->writeSettings(group);
    d->saveSettingsBox->writeSettings(group);
    d->settingsExpander->writeSettings();

    KConfigGroup group2 = config.group(QString("ExpoBlending Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void ExpoBlendingDlg::slotSaveItems()
{
    QMap<KUrl, QString> map = d->enfuseStack->urlsMap();
    if (map.isEmpty()) return;

    QMap<KUrl, QString>::iterator it;

    for (it = map.begin() ; it != map.end(); ++it)
    {
        KUrl newUrl = it.key();
        newUrl.setFileName(it.value());
        QFileInfo fi(newUrl.toLocalFile());

        // Check for overwrite ----------------------------------------------------------

        if ( fi.exists() )
        {
            int result = KMessageBox::warningYesNo(0, i18n("A file named \"%1\" already "
                                                        "exists. Are you sure you want "
                                                        "to overwrite it?",
                                                newUrl.fileName()),
                                                i18n("Overwrite File?"),
                                                KStandardGuiItem::overwrite(),
                                                KStandardGuiItem::cancel());

            if (result != KMessageBox::Yes)
                return;
        }

        if (::rename(QFile::encodeName(it.key().path()), QFile::encodeName(newUrl.path())) != 0)
        {
            KMessageBox::error(this, i18n("Failed to save image to %1", newUrl.path()));
            return;
        }
    }

    enableButton(User1, false);

    d->enfuseStack->clearSelected();

/*
    QString typeMime(d->saveSettingsBox->typeMime());
    QString defaultFileName(QString("expoblending") + d->saveSettingsBox->extension());
    QString place(QDir::homePath());

    if (d->mngr->iface())
        place = d->mngr->iface()->currentAlbum().uploadPath().path();

    QPointer<KFileDialog> imageFileSaveDialog = new KFileDialog(place, QString(), 0);

    imageFileSaveDialog->setModal(false);
    imageFileSaveDialog->setOperationMode(KFileDialog::Saving);
    imageFileSaveDialog->setMode(KFile::File);
    imageFileSaveDialog->setSelection(defaultFileName);
    imageFileSaveDialog->setCaption(i18n("New Image File Name"));
    imageFileSaveDialog->setMimeFilter(QStringList() << typeMime, typeMime);

    // Start dialog and check if canceled.
    if ( imageFileSaveDialog->exec() != KFileDialog::Accepted )
       return;

    KUrl newUrl = imageFileSaveDialog->selectedUrl();

    if (!newUrl.isValid())
    {
        KMessageBox::error(0, i18n("Failed to save file\n\"%1\" to\n\"%2\".",
                              newUrl.fileName(),
                              newUrl.path().section('/', -2, -2)));
        kWarning() << "target Url is not valid !";
        return;
    }

    delete imageFileSaveDialog;
*/
}

void ExpoBlendingDlg::slotProcess()
{
    KUrl::List selectedUrl = d->bracketStack->urls();
    if (selectedUrl.isEmpty()) return;

    ItemUrlsMap map = d->mngr->alignedMap();
    KUrl::List alignedList;

    foreach(KUrl url, selectedUrl)
    {
        KUrl alignedUrl = *(map.find(url));
        alignedList.append(alignedUrl);
    }

    d->mngr->thread()->setEnfuseSettings(d->enfuseSettingsBox->settings(), d->saveSettingsBox->fileFormat());
    d->mngr->thread()->enfuseFiles(alignedList, d->mngr->itemsList()[0]);
    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void ExpoBlendingDlg::slotAbort()
{
    d->mngr->thread()->cancel();
}

void ExpoBlendingDlg::processing(const KUrl& /*url*/)
{
    d->previewWidget->setBusy(true, i18n("Processing bracketed images..."));
}

void ExpoBlendingDlg::processed(const KUrl& /*url*/, const KUrl& tmpFile)
{
    d->enfuseStack->addItem(tmpFile);
}

void ExpoBlendingDlg::processingFailed(const KUrl& /*url*/)
{
    d->previewWidget->setBusy(false);
    d->previewWidget->setButtonVisible(true);
    d->previewWidget->setText(i18n("Failed to process bracketed images"), Qt::red);
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
            case(ENFUSE):
            {
                busy(true);
                processing(ad.inUrls[0]);
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
                case(ENFUSE):
                {
                    d->output = ad.message;
                    processingFailed(ad.inUrls[0]);
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
                case(ENFUSE):
                {
                    processed(ad.inUrls[0], ad.outUrls[0]);
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
    }
}

} // namespace KIPIExpoBlendingPlugin
