/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2015      by Benjamin Girault, <benjamin dot girault at gmail dot com>
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

#include "expoblendingdlg.h"

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
#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QGroupBox>
#include <QWindow>

// KDE includes

#include <kconfig.h>
#include <klocalizedstring.h>
#include <kio/renamedialog.h>
#include <kwindowconfig.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "kipiplugins_debug.h"
#include "aboutdata.h"
#include "actionthread.h"
#include "bracketstack.h"
#include "enfusebinary.h"
#include "enfusesettings.h"
#include "enfusestack.h"
#include "kpoutputdialog.h"
#include "kppreviewmanager.h"
#include "kpsavesettingswidget.h"
#include "kpversion.h"
#include "manager.h"



namespace KIPIExpoBlendingPlugin
{

struct ExpoBlendingDlg::Private
{
    Private()
        : templateFileName(0),
          previewWidget(0),
          enfuseSettingsBox(0),
          saveSettingsBox(0),
          bracketStack(0),
          enfuseStack(0),
          mngr(0),
          firstImageDisplayed(false),
          previewButton(0)
    {
    }

    QString               inputFileName;
    QString               output;

    QLineEdit*            templateFileName;

    KPPreviewManager*     previewWidget;

    EnfuseSettingsWidget* enfuseSettingsBox;

    KPSaveSettingsWidget* saveSettingsBox;

    BracketStackList*     bracketStack;
    EnfuseStackList*      enfuseStack;

    Manager*              mngr;

    bool                  firstImageDisplayed;

    QPushButton*          previewButton;
};

ExpoBlendingDlg::ExpoBlendingDlg(Manager* const mngr, QWidget* const parent)
    : KPToolDialog(parent),
      d(new Private)
{
    d->mngr = mngr;

    setModal(false);
    setWindowTitle(i18n("Exposure Blending"));

    startButton()->setText(i18nc("@action:button", "&Save"));
    startButton()->setIcon(QIcon::fromTheme(QLatin1String("document-save")));
    startButton()->setToolTip(i18nc("@info:tooltip", "Process and save selected items."));

    d->previewButton = new QPushButton(this);
    d->previewButton->setText(i18nc("@action:button", "&Preview"));
    d->previewButton->setIcon(QIcon::fromTheme(QLatin1String("system-run")));
    d->previewButton->setToolTip(i18nc("@info:tooltip", "Process a preview of bracketed images stack with current settings."));
    addButton(d->previewButton, QDialogButtonBox::ActionRole);

    QPushButton* const defaultButton = new QPushButton(this);
    defaultButton->setText(i18nc("@action:button", "&Default"));
    defaultButton->setIcon(QIcon::fromTheme(QLatin1String("document-revert")));
    defaultButton->setToolTip(i18nc("@info:tooltip", "Revert current settings to default values."));
    addButton(defaultButton, QDialogButtonBox::ResetRole);

    setModal(false);
    setAboutData(new ExpoBlendingAboutData());

    // ---------------------------------------------------------------

    QWidget* const page             = new QWidget(this);
    QGridLayout* const grid         = new QGridLayout(page);
    setMainWidget(page);

    d->previewWidget                = new KPPreviewManager(page);
    d->previewWidget->setButtonText(i18nc("@action:button", "Details..."));

    // ---------------------------------------------------------------

    QScrollArea* const rightColumn  = new QScrollArea(page);
    rightColumn->setWidgetResizable(true);
    QVBoxLayout* const panel        = new QVBoxLayout();

    d->bracketStack                 = new BracketStackList(d->mngr->iface(), rightColumn->viewport());
    panel->addWidget(d->bracketStack, 1);

    // ---------------------------------------------------------------

    QGroupBox* const enfuse = new QGroupBox(rightColumn);
    enfuse->setTitle(i18n("Enfuse Settings"));
    QVBoxLayout* const elay = new QVBoxLayout(enfuse);
    enfuse->setLayout(elay);

    d->enfuseSettingsBox            = new EnfuseSettingsWidget(enfuse);
    elay->addWidget(d->enfuseSettingsBox);

    panel->addWidget(enfuse, 1);

    // ---------------------------------------------------------------

    QGroupBox* const save = new QGroupBox(rightColumn);
    save->setTitle(i18n("Save Settings"));
    QVBoxLayout* const slay = new QVBoxLayout(enfuse);
    save->setLayout(slay);

    d->saveSettingsBox              = new KPSaveSettingsWidget(save);
    slay->addWidget(d->saveSettingsBox);

    QHBoxLayout* const hbox         = new QHBoxLayout();

    QLabel* const customLabel       = new QLabel(save);
    customLabel->setText(i18nc("@label:textbox", "File Name Template: "));
    hbox->addWidget(customLabel);

    d->templateFileName             = new QLineEdit(save);
    d->templateFileName->setClearButtonEnabled(true);
    hbox->addWidget(d->templateFileName);

    d->saveSettingsBox->setCustomSettingsWidget(d->saveSettingsBox);
    slay->addLayout(hbox);

    panel->addWidget(save, 1);

    // ---------------------------------------------------------------

    d->enfuseStack = new EnfuseStackList(rightColumn->viewport());
    panel->addWidget(d->enfuseStack, 1);

    rightColumn->setLayout(panel);

    // ---------------------------------------------------------------

    grid->addWidget(d->previewWidget, 0, 0, 3, 1);
    grid->addWidget(rightColumn,      0, 1, 3, 1);
    grid->setContentsMargins(QMargins());
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(1, 5);

    // ---------------------------------------------------------------

    connect(this, SIGNAL(finished(int)),
            this, SLOT(slotFinished()));

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancelClicked()));

    connect(defaultButton, SIGNAL(clicked()),
            this, SLOT(slotDefault()));

    connect(startButton(), SIGNAL(clicked()),
            this, SLOT(slotProcess()));

    connect(d->previewButton, SIGNAL(clicked()),
            this, SLOT(slotPreview()));

    connect(d->mngr->thread(), SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
            this, SLOT(slotAction(KIPIExpoBlendingPlugin::ActionData)));

    connect(d->mngr->thread(), SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)),
            this, SLOT(slotAction(KIPIExpoBlendingPlugin::ActionData)));

    connect(d->bracketStack, SIGNAL(signalAddItems(const QList<QUrl>&)),
            this, SLOT(slotAddItems(const QList<QUrl>&)));

    connect(d->previewWidget, SIGNAL(signalButtonClicked()),
            this, SLOT(slotPreviewButtonClicked()));

    connect(d->enfuseStack, SIGNAL(signalItemClicked(QUrl)),
            this, SLOT(slotLoadProcessed(QUrl)));

    connect(d->templateFileName, SIGNAL(textChanged(QString)),
            this, SLOT(slotFileFormatChanged()));

    connect(d->saveSettingsBox, SIGNAL(signalSaveFormatChanged()),
            this, SLOT(slotFileFormatChanged()));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
    loadItems(d->mngr->itemsList());
}

ExpoBlendingDlg::~ExpoBlendingDlg()
{
    delete d;
}

void ExpoBlendingDlg::slotFinished()
{
    d->mngr->thread()->cancel();
    d->mngr->cleanUp();
    saveSettings();
}

void ExpoBlendingDlg::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

void ExpoBlendingDlg::slotCancelClicked()
{
    d->mngr->thread()->cancel();
}

void ExpoBlendingDlg::slotFileFormatChanged()
{
    d->enfuseStack->setTemplateFileName(d->saveSettingsBox->fileFormat(), d->templateFileName->text());
}

void ExpoBlendingDlg::slotPreviewButtonClicked()
{
    KPOutputDialog dlg(QApplication::activeWindow(),
                       i18nc("@title:window", "Enfuse Processing Messages"),
                       d->output);

    dlg.setAboutData(new ExpoBlendingAboutData());
    dlg.exec();
}

void ExpoBlendingDlg::loadItems(const QList<QUrl>& urls)
{
    d->bracketStack->clear();
    d->bracketStack->addItems(urls);
}

void ExpoBlendingDlg::slotAddItems(const QList<QUrl>& urls)
{
    if (!urls.empty())
    {
        d->mngr->thread()->identifyFiles(urls);

        if (!d->mngr->thread()->isRunning())
            d->mngr->thread()->start();
    }
}

void ExpoBlendingDlg::slotLoadProcessed(const QUrl& url)
{
    d->mngr->thread()->loadProcessed(url);

    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void ExpoBlendingDlg::setIdentity(const QUrl& url, const QString& identity)
{
    BracketStackItem* const item = d->bracketStack->findItem(url);

    if (item)
        item->setExposure(identity);
}

void ExpoBlendingDlg::busy(bool val)
{
    d->enfuseSettingsBox->setEnabled(!val);
    d->saveSettingsBox->setEnabled(!val);
    d->bracketStack->setEnabled(!val);

    startButton()->setEnabled(!val ? !d->enfuseStack->settingsList().isEmpty() : false);
    d->previewButton->setEnabled(!val);
    setRejectButtonMode(val ? QDialogButtonBox::Cancel : QDialogButtonBox::Close);

    if (val)
        d->previewWidget->setButtonVisible(false);
}

void ExpoBlendingDlg::slotDefault()
{
    d->enfuseSettingsBox->resetToDefault();
    d->saveSettingsBox->resetToDefault();
    d->templateFileName->setText(QLatin1String("enfuse"));
}

void ExpoBlendingDlg::readSettings()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group("ExpoBlending Settings");

    d->enfuseSettingsBox->readSettings(group);
    d->saveSettingsBox->readSettings(group);

    d->templateFileName->setText(group.readEntry("Template File Name", QString::fromLatin1("enfuse")));

    winId();
    KConfigGroup group2 = config.group("ExpoBlending Dialog");
    KWindowConfig::restoreWindowSize(windowHandle(), group2);
    resize(windowHandle()->size());
}

void ExpoBlendingDlg::saveSettings()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group("ExpoBlending Settings");

    d->enfuseSettingsBox->writeSettings(group);
    d->saveSettingsBox->writeSettings(group);

    group.writeEntry("Template File Name", d->templateFileName->text());

    KConfigGroup group2 = config.group("ExpoBlending Dialog");
    KWindowConfig::saveWindowSize(windowHandle(), group2);
    config.sync();
}

void ExpoBlendingDlg::slotPreview()
{
    QList<QUrl> selectedUrl = d->bracketStack->urls();

    if (selectedUrl.isEmpty())
        return;

    ItemUrlsMap map = d->mngr->preProcessedMap();
    QList<QUrl> preprocessedList;

    foreach(const QUrl& url, selectedUrl)
    {
        ItemPreprocessedUrls preprocessedUrls = map.value(url);
        preprocessedList.append(preprocessedUrls.previewUrl);
    }

    EnfuseSettings settings = d->enfuseSettingsBox->settings();
    settings.inputUrls      = d->bracketStack->urls();
    settings.outputFormat   = d->saveSettingsBox->fileFormat();
    d->mngr->thread()->enfusePreview(preprocessedList, d->mngr->itemsList()[0], settings, d->mngr->enfuseBinary().path());

    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void ExpoBlendingDlg::slotProcess()
{
    QList<EnfuseSettings> list = d->enfuseStack->settingsList();

    if (list.isEmpty())
        return;

    ItemUrlsMap map = d->mngr->preProcessedMap();
    QList<QUrl> preprocessedList;

    foreach(const EnfuseSettings& settings, list)
    {
        preprocessedList.clear();

        foreach(const QUrl& url, settings.inputUrls)
        {
            ItemPreprocessedUrls preprocessedUrls = map.value(url);
            preprocessedList.append(preprocessedUrls.preprocessedUrl);
        }

        d->mngr->thread()->enfuseFinal(preprocessedList, d->mngr->itemsList()[0], settings, d->mngr->enfuseBinary().path());

        if (!d->mngr->thread()->isRunning())
            d->mngr->thread()->start();
    }
}

void ExpoBlendingDlg::saveItem(const QUrl& temp, const EnfuseSettings& settings)
{
    QUrl newUrl = QUrl::fromLocalFile(temp.adjusted(QUrl::RemoveFilename).path() + settings.targetFileName);
    QFileInfo fi(newUrl.toLocalFile());

    if (d->saveSettingsBox->conflictRule() != KPSaveSettingsWidget::OVERWRITE)
    {
        if (fi.exists())
        {
            KIO::RenameDialog dlg(this,
                                  i18n("A file named \"%1\" already exists. Are you sure you want to overwrite it?", newUrl.fileName()),
                                  temp,
                                  newUrl,
                                  KIO::RenameDialog_Mode(KIO::M_OVERWRITE | KIO::M_SKIP));

            switch (dlg.exec())
            {
                case KIO::R_CANCEL:
                case KIO::R_SKIP:
                {
                    newUrl.clear();
                    d->enfuseStack->setOnItem(settings.previewUrl, false);
                    d->enfuseStack->processedItem(settings.previewUrl, false);

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

    qCDebug(KIPIPLUGINS_LOG) << "Renaming " << temp << " to " << newUrl;

    if (!newUrl.isEmpty())
    {
        // remove newUrl file if it exist
        if (temp.toLocalFile() != newUrl.toLocalFile() && QFile::exists(temp.toLocalFile()) && QFile::exists(newUrl.toLocalFile()))
        {
            QFile::remove(newUrl.toLocalFile());
        }

        if (!QFile::rename(temp.toLocalFile(), newUrl.toLocalFile()))
        {
            QMessageBox::critical(this, QString(), i18n("Failed to save image to %1.", newUrl.toLocalFile()));
            d->enfuseStack->setOnItem(settings.previewUrl, false);
            d->enfuseStack->processedItem(settings.previewUrl, false);
            return;
        }
        else
        {
            d->enfuseStack->removeItem(settings.previewUrl);
        }
    }

    if (d->enfuseStack->settingsList().isEmpty())
    {
        startButton()->setEnabled(false);
        busy(false);
        d->previewWidget->setBusy(false);
    }
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
            case(ENFUSEPREVIEW):
            {
                busy(true);
                d->previewWidget->setBusy(true, i18n("Processing preview of bracketed images..."));
                break;
            }
            case(ENFUSEFINAL):
            {
                busy(true);
                d->previewWidget->setBusy(true, i18n("Processing output of bracketed images..."));
                d->enfuseStack->processingItem(ad.enfuseSettings.previewUrl, true);
                break;
            }
            default:
            {
                qCWarning(KIPIPLUGINS_LOG) << "Unknown action";
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
                    d->previewWidget->setText(i18n("Failed to load processed image."), Qt::red);
                    busy(false);
                    break;
                }
                case(ENFUSEPREVIEW):
                {
                    d->output = ad.message;
                    d->previewWidget->setBusy(false);
                    d->previewWidget->setButtonVisible(true);
                    d->previewWidget->setText(i18n("Failed to process preview of bracketed images."), Qt::red);
                    busy(false);
                    break;
                }
                case(ENFUSEFINAL):
                {
                    slotCancelClicked();
                    d->output = ad.message;
                    d->previewWidget->setBusy(false);
                    d->previewWidget->setButtonVisible(true);
                    d->previewWidget->setText(i18n("Failed to process output of bracketed images."), Qt::red);
                    d->enfuseStack->processingItem(ad.enfuseSettings.previewUrl, false);
                    d->enfuseStack->setOnItem(ad.enfuseSettings.previewUrl, false);
                    busy(false);
                    break;
                }
                default:
                {
                    qCWarning(KIPIPLUGINS_LOG) << "Unknown action";
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
                case(ENFUSEPREVIEW):
                {
                    d->enfuseStack->addItem(ad.outUrls[0], ad.enfuseSettings);
                    busy(false);
                    break;
                }
                case(ENFUSEFINAL):
                {
                    d->enfuseStack->processingItem(ad.enfuseSettings.previewUrl, false);
                    saveItem(ad.outUrls[0], ad.enfuseSettings);
                    break;
                }
                default:
                {
                    qCWarning(KIPIPLUGINS_LOG) << "Unknown action";
                    break;
                }
            }
        }
    }
}

} // namespace KIPIExpoBlendingPlugin
