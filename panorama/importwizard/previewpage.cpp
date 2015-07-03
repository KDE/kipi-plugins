/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2011-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2011-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "previewpage.h"

// Qt includes

#include <QLabel>
#include <QMutex>
#include <QMutexLocker>
#include <QTextDocument>
#include <QStandardPaths>
#include <QVBoxLayout>

// KDE includes

#include <KLocalizedString>

// Local includes

#include "kipiplugins_debug.h"
#include "kptooldialog.h"
#include "kppreviewmanager.h"
#include "kpbatchprogressdialog.h"
#include "manager.h"
#include "enblendbinary.h"
#include "makebinary.h"
#include "nonabinary.h"
#include "pto2mkbinary.h"

namespace KIPIPanoramaPlugin
{

struct PreviewPage::Private
{
    Private(Manager* const m)
        : title(0), 
          previewWidget(0), 
          previewBusy(false), 
          stitchingBusy(false),
          postProcessing(0),
          curProgress(0),
          totalProgress(0),
          canceled(false), 
          mngr(m),
          dlg(0)
    {}

    QLabel*                title;

    KPPreviewManager*      previewWidget;
    bool                   previewBusy;
    bool                   stitchingBusy;
    KPBatchProgressWidget* postProcessing;
    int                    curProgress, totalProgress;
    QMutex                 actionMutex;      // This is a precaution in case the user does a back / next action at the wrong moment
    bool                   canceled;

    QString                output;

    Manager*               mngr;

    KAssistantDialog*      dlg;
};

PreviewPage::PreviewPage(Manager* const mngr, KAssistantDialog* const dlg)
    : KPWizardPage(dlg, i18nc("@title:window", "<b>Preview and Post-Processing</b>")),
      d(new Private(mngr))
{
    d->dlg            = dlg;

    QVBoxLayout* const vbox = new QVBoxLayout();

    d->title          = new QLabel(this);
    d->title->setOpenExternalLinks(true);
    d->title->setWordWrap(true);
    vbox->addWidget(d->title);

    d->previewWidget  = new KPPreviewManager(this);
    d->previewWidget->setButtonText(i18nc("@action:button", "Details..."));
    d->previewWidget->show();
    vbox->addWidget(d->previewWidget);

    d->postProcessing = new KPBatchProgressWidget(this);
    d->postProcessing->hide();
    vbox->addWidget(d->postProcessing);

    setLayout(vbox);

    QPixmap leftPix(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kipiplugin_panorama/pics/assistant-hugin.png")));
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->postProcessing, SIGNAL(signalProgressCanceled()),
            this, SLOT(slotCancel()));
}

PreviewPage::~PreviewPage()
{
    delete d;
}

void PreviewPage::slotCancel()
{
    d->dlg->reject();
}

bool PreviewPage::cancel()
{
    disconnect(d->mngr->thread(), SIGNAL(starting(KIPIPanoramaPlugin::ActionData)),
               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    d->canceled = true;
    d->mngr->thread()->cancel();
    d->mngr->thread()->finish();
    d->postProcessing->progressCompleted();

    QMutexLocker lock(&d->actionMutex);

    if (d->previewBusy)
    {
        d->previewBusy = false;
        d->previewWidget->setBusy(false);
        d->previewWidget->setText(i18n("Preview Processing Cancelled."));
        return true;
    }
    else if (d->stitchingBusy)
    {
        d->stitchingBusy = false;
        lock.unlock();
        resetPage();
        return false;
    }

    return true;
}

void PreviewPage::computePreview()
{
    // Cancel any stitching being processed
    if (d->stitchingBusy)
    {
        cancel();
    }

    QMutexLocker lock(&d->actionMutex);

    connect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    connect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    d->canceled = false;

    d->previewWidget->setBusy(true, i18n("Processing Panorama Preview..."));
    d->previewBusy = true;

    d->mngr->resetPreviewPto();
    d->mngr->resetPreviewUrl();
    d->mngr->resetPreviewMkUrl();
    d->mngr->thread()->generatePanoramaPreview(d->mngr->viewAndCropOptimisePtoData(),
                                               d->mngr->previewPtoUrl(),
                                               d->mngr->previewMkUrl(),
                                               d->mngr->previewUrl(),
                                               d->mngr->preProcessedMap(),
                                               d->mngr->makeBinary().path(),
                                               d->mngr->pto2MkBinary().path(),
                                               d->mngr->enblendBinary().path(),
                                               d->mngr->nonaBinary().path());
}

void PreviewPage::startStitching()
{
    // Cancel any preview being processed
    bool previewReady = true;

    if (d->previewBusy)
    {
        previewReady = false;
        cancel();
    }

    QMutexLocker lock(&d->actionMutex);

    connect(d->mngr->thread(), SIGNAL(starting(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    connect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    connect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    d->canceled      = false;
    d->stitchingBusy = true;
    d->curProgress   = 0;
    d->totalProgress = d->mngr->preProcessedMap().size() + 1;
    d->previewWidget->hide();

    QSize panoSize      = d->mngr->viewAndCropOptimisePtoData()->project.size;
    QRect panoSelection = d->mngr->viewAndCropOptimisePtoData()->project.crop;

    if (previewReady)
    {
        QSize previewSize = d->mngr->previewPtoData()->project.size;
        QRectF selection  = d->previewWidget->getSelectionArea();
        QRectF proportionSelection(selection.x()      / previewSize.width(),
                                   selection.y()      / previewSize.height(),
                                   selection.width()  / previewSize.width(),
                                   selection.height() / previewSize.height());

        // At this point, if no selection area was created, proportionSelection is null,
        // hence panoSelection becomes a null rectangle
        panoSelection = QRect(proportionSelection.x()      * panoSize.width(),
                              proportionSelection.y()      * panoSize.height(),
                              proportionSelection.width()  * panoSize.width(),
                              proportionSelection.height() * panoSize.height());
    }

    d->title->setText(i18n("<qt>"
                           "<p><h1>Panorama Post-Processing</h1></p>"
                           "</qt>"));

    d->postProcessing->reset();
    d->postProcessing->setTotal(d->totalProgress);
    d->postProcessing->progressScheduled(i18nc("@title:group", "Panorama Post-Processing"), QIcon::fromTheme(QStringLiteral("kipi-panorama")).pixmap(22, 22));
    d->postProcessing->show();

    d->mngr->resetPanoPto();
    d->mngr->resetMkUrl();
    d->mngr->resetPanoUrl();
    d->mngr->thread()->compileProject(d->mngr->viewAndCropOptimisePtoData(),
                                      d->mngr->panoPtoUrl(),
                                      d->mngr->mkUrl(),
                                      d->mngr->panoUrl(),
                                      d->mngr->preProcessedMap(),
                                      d->mngr->format(),
                                      panoSelection,
                                      d->mngr->makeBinary().path(),
                                      d->mngr->pto2MkBinary().path(),
                                      d->mngr->enblendBinary().path(),
                                      d->mngr->nonaBinary().path());
}

void PreviewPage::resetPage()
{
    d->title->setText(QString());
    d->postProcessing->progressCompleted();
    d->postProcessing->hide();
    d->previewWidget->show();
    computePreview();
}

void PreviewPage::slotAction(const KIPIPanoramaPlugin::ActionData& ad)
{
    QString      text;
    QMutexLocker lock(&d->actionMutex);

    if (!ad.starting)           // Something is complete...
    {
        if (!ad.success)        // Something is failed...
        {
            switch (ad.action)
            {
                case CREATEPREVIEWPTO:
                case NONAFILEPREVIEW:
                case STITCHPREVIEW:
                case CREATEMKPREVIEW:
                {
                    if (!d->previewBusy)
                    {
                        return;
                    }

                    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->output      = ad.message;
                    d->previewWidget->setBusy(false);
                    d->previewBusy = false;
                    qCWarning(KIPIPLUGINS_LOG) << "Preview compilation failed: " << ad.message;
                    QString errorString(xi18n("<qt><h2><b>Error</b></h2><p><message>%1</message></p></qt>", Qt::escape(ad.message).replace(QLatin1String("\n"), QLatin1String("</p><p>"))));
                    d->previewWidget->setText(errorString);
                    d->previewWidget->setSelectionAreaPossible(false);

                    emit signalPreviewStitchingFinished(false);

                    break;
                }
                case CREATEMK:
                {
                    if (!d->stitchingBusy)
                    {
                        return;
                    }
                    disconnect(d->mngr->thread(), SIGNAL(starting(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->stitchingBusy = false;
                    QString message  = xi18nc("Here a makefile is a script for GNU Make", "Cannot create makefile: <message>%3</message>", ad.message);
                    qCWarning(KIPIPLUGINS_LOG) << "pto2mk call failed";
                    d->postProcessing->addedAction(message, ErrorMessage);
                    emit signalStitchingFinished(false);
                    break;
                }
                case CREATEFINALPTO:
                {
                    if (!d->stitchingBusy)
                    {
                        return;
                    }

                    disconnect(d->mngr->thread(), SIGNAL(starting(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->stitchingBusy = false;
                    QString message  = xi18nc("a project file is a .pto file, as given to hugin to build a panorama",
                                              "Cannot create project file: <message>%3</message>",
                                              ad.message);
                    qCWarning(KIPIPLUGINS_LOG) << "pto creation failed";
                    d->postProcessing->addedAction(message, ErrorMessage);
                    emit signalStitchingFinished(false);
                    break;
                }
                case NONAFILE:
                {
                    if (!d->stitchingBusy)
                    {
                        return;
                    }

                    disconnect(d->mngr->thread(), SIGNAL(starting(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->stitchingBusy = false;
                    QString message  = xi18nc("Error message for image file number %1 out of %2", "Processing file %1 / %2: <message>%3</message>",
                                              QString::number(ad.id + 1),
                                              QString::number(d->totalProgress - 1),
                                              ad.message
                                             );
                    qCWarning(KIPIPLUGINS_LOG) << "Nona call failed for file #" << ad.id;
                    d->postProcessing->addedAction(message, ErrorMessage);
                    emit signalStitchingFinished(false);
                    break;
                }
                case STITCH:
                {
                    if (!d->stitchingBusy)
                    {
                        return;
                    }

                    disconnect(d->mngr->thread(), SIGNAL(starting(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->stitchingBusy = false;
                    d->postProcessing->addedAction(xi18nc("Error message for panorama compilation", "Panorama compilation: <message>%1</message>", Qt::escape(ad.message)), ErrorMessage);
                    qCWarning(KIPIPLUGINS_LOG) << "Enblend call failed";
                    emit signalStitchingFinished(false);
                    break;
                }
                default:
                {
                    qCWarning(KIPIPLUGINS_LOG) << "Unknown action (preview) " << ad.action;
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case CREATEPREVIEWPTO:
                case CREATEMKPREVIEW:
                case NONAFILEPREVIEW:
                case CREATEFINALPTO:
                case CREATEMK:
                {
                    // Nothing to do yet, a step is finished, that's all
                    break;
                }
                case STITCHPREVIEW:
                {
                    if (!d->previewBusy)
                    {
                        return;
                    }

                    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    qCDebug(KIPIPLUGINS_LOG) << "Preview Stitching finished";
                    d->previewBusy = false;

                    d->title->setText(i18n("<qt>"
                                           "<p><h1>Panorama Preview</h1></p>"
                                           "<p>Draw a rectangle if you want to crop the image.</p>"
                                           "<p>Pressing the <i>Next</i> button will then launch the final "
                                           "stitching process.</p>"
                                           "</qt>"));
                    d->previewWidget->setSelectionAreaPossible(true);
                    d->previewWidget->load(QUrl::fromLocalFile(d->mngr->previewUrl().toLocalFile()), true);
                    QSize panoSize    = d->mngr->viewAndCropOptimisePtoData()->project.size;
                    QRect panoCrop    = d->mngr->viewAndCropOptimisePtoData()->project.crop;
                    QSize previewSize = d->mngr->previewPtoData()->project.size;
                    d->previewWidget->setSelectionArea(QRectF(
                        ((double) panoCrop.left())   / panoSize.width()  * previewSize.width(),
                        ((double) panoCrop.top())    / panoSize.height() * previewSize.height(),
                        ((double) panoCrop.width())  / panoSize.width()  * previewSize.width(),
                        ((double) panoCrop.height()) / panoSize.height() * previewSize.height()
                    ));
                    qCDebug(KIPIPLUGINS_LOG) << "Preview URL: " << d->mngr->previewUrl();

                    emit signalPreviewStitchingFinished(true);

                    break;
                }
                case NONAFILE:
                {
                    QString message = i18nc("Success for image file number %1 out of %2", "Processing file %1 / %2", QString::number(ad.id + 1), QString::number(d->totalProgress - 1));
                    d->postProcessing->addedAction(message, SuccessMessage);
                    d->curProgress++;
                    d->postProcessing->setProgress(d->curProgress, d->totalProgress);
                    qCDebug(KIPIPLUGINS_LOG) << "Nona URL #" << ad.id;
                    break;
                }
                case STITCH:
                {
                    if (!d->stitchingBusy)
                    {
                        return;
                    }

                    disconnect(d->mngr->thread(), SIGNAL(starting(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->stitchingBusy = false;
                    d->postProcessing->addedAction(i18nc("Success for panorama compilation", "Panorama compilation"), SuccessMessage);
                    d->curProgress++;
                    d->postProcessing->setProgress(d->curProgress, d->totalProgress);
                    d->postProcessing->progressCompleted();
                    d->postProcessing->hide();
                    qCDebug(KIPIPLUGINS_LOG) << "Panorama stitched";
                    emit signalStitchingFinished(true);
                    break;
                }
                default:
                {
                    qCWarning(KIPIPLUGINS_LOG) << "Unknown action (preview) " << ad.action;
                    break;
                }
            }
        }
    }
    else           // Some step is started...
    {
        switch (ad.action)
        {
            case CREATEPREVIEWPTO:
            case CREATEMKPREVIEW:
            case NONAFILEPREVIEW:
            case STITCHPREVIEW:
            case CREATEFINALPTO:
            case CREATEMK:
            {
                // Nothing to do...
                break;
            }
            case NONAFILE:
            {
                qCDebug(KIPIPLUGINS_LOG) << "Start" << ad.id;
                QString message = i18nc("Compilation started for image file number %1 out of %2", "Processing file %1 / %2", QString::number(ad.id + 1), QString::number(d->totalProgress - 1));
                d->postProcessing->addedAction(message, StartingMessage);
                break;
            }
            case STITCH:
            {
                d->postProcessing->addedAction(i18nc("Panorama compilation started", "Panorama compilation"), StartingMessage);
                break;
            }
            default:
            {
                qCWarning(KIPIPLUGINS_LOG) << "Unknown starting action (preview) " << ad.action;
                break;
            }
        }
    }
}

}   // namespace KIPIPanoramaPlugin
