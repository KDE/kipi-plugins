/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "previewpage.moc"

// Qt includes

#include <QLabel>

// KDE includes

#include <kstandarddirs.h>
#include <kapplication.h>
#include <kvbox.h>
#include <klocale.h>
#include <kdialog.h>
#include <kdebug.h>

// Local includes

#include "outputdialog.h"
#include "batchprogressdialog.h"
#include "savesettingswidget.h"
#include "manager.h"
#include "previewmanager.h"
#include "enblendbinary.h"
#include "makebinary.h"
#include "nonabinary.h"
#include "pto2mkbinary.h"

namespace KIPIPanoramaPlugin
{

struct PreviewPage::PreviewPagePriv
{
    PreviewPagePriv(Manager *m)
        : title(0), previewWidget(0), previewBusy(false), postProcessing(0), mngr(m)
    {}

    QLabel*              title;
    QUrl                 previewUrl;
    PreviewManager*      previewWidget;
    bool                 previewBusy;
    BatchProgressWidget* postProcessing;
    int                  curProgress, totalProgress;

    QString              output;

    Manager*             mngr;
};

PreviewPage::PreviewPage(Manager* mngr, KAssistantDialog* dlg)
        : KIPIPlugins::WizardPage(dlg, i18n("<b>Preview and Post-Processing</b>")),
          d(new PreviewPagePriv(mngr))
{
    KVBox* vbox       = new KVBox(this);
    d->title          = new QLabel(vbox);
    d->title->setOpenExternalLinks(true);
    d->title->setWordWrap(true);

    d->previewWidget  = new PreviewManager(vbox);
    d->previewWidget->setButtonText(i18n("Details..."));
    d->previewWidget->show();

    d->postProcessing = new BatchProgressWidget(vbox);
    d->postProcessing->hide();

    vbox->setSpacing(KDialog::spacingHint());
    vbox->setMargin(KDialog::spacingHint());

    setPageWidget(vbox);

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_panorama/pics/assistant-hugin.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->mngr->thread(), SIGNAL(starting(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));
}

PreviewPage::~PreviewPage()
{
    delete d;
}

void PreviewPage::cancel()
{
    disconnect(d->mngr->thread(), SIGNAL(finished(KIPIPanoramaPlugin::ActionData)),
               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    d->mngr->thread()->cancel();

    if (d->previewBusy)
    {
        d->previewWidget->setBusy(false);
        d->previewWidget->setText(i18n("Preview Processing Cancelled."));
    }
}

void PreviewPage::computePreview()
{
    d->previewWidget->setBusy(true, i18n("Processing Panorama Preview..."));
    d->previewBusy = true;

    connect(d->mngr->thread(), SIGNAL(finished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    emit signalPreviewGenerating();
    d->mngr->thread()->generatePanoramaPreview(d->mngr->autoOptimiseUrl(),
                                               d->mngr->preProcessedMap(),
                                               d->mngr->makeBinary().path(),
                                               d->mngr->pto2MkBinary().path(),
                                               d->mngr->enblendBinary().path(),
                                               d->mngr->nonaBinary().path());
    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void PreviewPage::startStitching()
{
    d->curProgress   = 0;
    d->totalProgress = d->mngr->preProcessedMap().size() + 1;
    d->previewWidget->hide();
    d->title->setText(i18n("<qt>"
                           "<p><h1>Panorama Post-Processing</h1></p>"
                           "</qt>"));

    d->postProcessing->setTotal(d->totalProgress);
    d->postProcessing->show();

    connect(d->mngr->thread(), SIGNAL(finished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    d->mngr->thread()->compileProject(d->mngr->autoOptimiseUrl(),
                                      d->mngr->preProcessedMap(),
                                      d->mngr->format(),
                                      d->mngr->makeBinary().path(),
                                      d->mngr->pto2MkBinary().path(),
                                      d->mngr->enblendBinary().path(),
                                      d->mngr->nonaBinary().path());
    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void PreviewPage::resetPage()
{
    d->title->setText(i18n("<qt>"
                           "<p><h1>Panorama Preview</h1></p>"
                           "<p>Pressing the <i>Next</i> button launches the final "
                           "stitching process.</p>"
                           "</qt>"));
    d->postProcessing->hide();
    d->previewWidget->show();
    computePreview();
}

void PreviewPage::slotAction(const KIPIPanoramaPlugin::ActionData& ad)
{
    QString text;

    if (!ad.starting)           // Something is complete...
    {
        if (!ad.success)        // Something is failed...
        {
            switch (ad.action)
            {
                case PREVIEW :
                {
                    disconnect(d->mngr->thread(), SIGNAL(finished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->output = ad.message;
                    d->previewWidget->setBusy(false);
                    d->previewBusy = false;
                    kDebug() << "Preview compilation failed: " << ad.message;
                    QString errorString("<qt><h2><b>Error</b></h2><p>" + ad.message + "</p></qt>");
                    errorString.replace('\n', "</p><p>");
                    d->previewWidget->setText(errorString);
                    emit signalPreviewGenerated(KUrl());
                    break;
                }
                case STITCH:
                {
                    disconnect(d->mngr->thread(), SIGNAL(finished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->postProcessing->addedAction(QString("Panorama compilation: ") + ad.message, ErrorMessage);
                    kDebug() << "Enblend call failed";
                    break;
                }
                case NONAFILE:
                {
                    QStringList message;
                    message << "Processing file ";
                    message << QString::number(ad.id + 1);
                    message << " / ";
                    message << QString::number(d->totalProgress - 1);
                    message << ": ";
                    message << ad.message;
                    kDebug() << "Nona call failed for file #" << ad.id;
                    d->postProcessing->addedAction(message.join(""), ErrorMessage);
                    break;
                }
                default:
                {
                    kWarning() << "Unknown action " << ad.action;
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case PREVIEW:
                {
                    disconnect(d->mngr->thread(), SIGNAL(finished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->previewUrl = ad.outUrl;
                    d->mngr->setPreviewUrl(ad.outUrl);
                    d->previewWidget->load(ad.outUrl.toLocalFile(), true);
                    kDebug() << "Preview URL: " << ad.outUrl.toLocalFile();

                    emit signalPreviewGenerated(ad.outUrl);
                    break;
                }
                case STITCH:
                {
                    disconnect(d->mngr->thread(), SIGNAL(finished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->postProcessing->addedAction(QString("Panorama compilation"), SuccessMessage);
                    d->curProgress++;
                    d->postProcessing->setProgress(d->curProgress, d->totalProgress);
                    d->postProcessing->hide();
                    kDebug() << "Panorama URL: " << ad.outUrl.toLocalFile();
                    emit signalStitchingFinished(ad.outUrl);
                    break;
                }
                case NONAFILE:
                {
                    QStringList message;
                    message << "Processing file ";
                    message << QString::number(ad.id + 1);
                    message << " / ";
                    message << QString::number(d->totalProgress - 1);
                    d->postProcessing->addedAction(message.join(""), SuccessMessage);
                    d->curProgress++;
                    d->postProcessing->setProgress(d->curProgress, d->totalProgress);
                    kDebug() << "Nona URL #" << ad.id << " URL: " << ad.outUrl.toLocalFile();
                    break;
                }
                default:
                {
                    kWarning() << "Unknown action " << ad.action;
                    break;
                }
            }
        }
    }
    else
    {
        switch (ad.action)
        {
            case STITCH:
            {
                d->postProcessing->addedAction(QString("Panorama compilation"), StartingMessage);
                break;
            }
            case NONAFILE:
            {
                QStringList message;
                message << "Processing file ";
                message << QString::number(ad.id + 1);
                message << " / ";
                message << QString::number(d->totalProgress - 1);
                d->postProcessing->addedAction(message.join(""), StartingMessage);
                break;
            }
            default:
            {
                kWarning() << "Unknown starting action " << ad.action;
                break;
            }
        }
    }
}

}   // namespace KIPIPanoramaPlugin
