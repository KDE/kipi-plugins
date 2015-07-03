/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "preprocessingpage.h"

// Qt includes

#include <QDir>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QPixmap>
#include <QPushButton>
#include <QCheckBox>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>

// KDE includes

#include <KConfig>
#include <KLocalizedString>
#include <kiconloader.h>
#include <kpixmapsequence.h>

// LibKIPI includes

#include <KIPI/Interface>

// Local includes

#include <kipiplugins_debug.h>
#include "aboutdata.h"
#include "kpoutputdialog.h"
#include "cpcleanbinary.h"
#include "cpfindbinary.h"
#include "manager.h"
#include "actionthread.h"

namespace KIPIPanoramaPlugin
{

struct PreProcessingPage::PreProcessingPagePriv
{
    PreProcessingPagePriv()
        : progressCount(0),
          progressLabel(0),
          progressTimer(0),
          canceled(false),
          nbFilesProcessed(0),
          title(0),
          celesteCheckBox(0),
          detailsBtn(0),
          mngr(0)
    {
        progressPix = KIconLoader::global()->loadPixmapSequence(QStringLiteral("process-working"), KIconLoader::SizeSmallMedium);
    }

    int             progressCount;
    QLabel*         progressLabel;
    QTimer*         progressTimer;
    QMutex          progressMutex;      // This is a precaution in case the user does a back / next action at the wrong moment
    bool            canceled;

    int             nbFilesProcessed;
    QMutex          nbFilesProcessed_mutex;

    QLabel*         title;

    QCheckBox*      celesteCheckBox;

    QString         output;

    QPushButton*    detailsBtn;

    KPixmapSequence progressPix;

    Manager*        mngr;
};

PreProcessingPage::PreProcessingPage(Manager* const mngr, KAssistantDialog* const dlg)
    : KPWizardPage(dlg, i18nc("@title:window", "<b>Pre-Processing Images</b>")),
      d(new PreProcessingPagePriv)
{
    d->mngr             = mngr;
    d->progressTimer    = new QTimer(this);
    QVBoxLayout* const vbox = new QVBoxLayout();
    d->title            = new QLabel(this);
    d->title->setWordWrap(true);
    d->title->setOpenExternalLinks(true);
    vbox->addWidget(d->title);

    KConfig config(QStringLiteral("kipirc"));
    KConfigGroup group  = config.group("Panorama Settings");

    d->celesteCheckBox  = new QCheckBox(i18nc("@option:check", "Detect moving skies"), this);
    d->celesteCheckBox->setChecked(group.readEntry("Celeste", false));
    d->celesteCheckBox->setToolTip(i18nc("@info:tooltip", "Automatic detection of clouds to prevent wrong keypoints matching "
                                         "between images due to moving clouds."));
    d->celesteCheckBox->setWhatsThis(i18nc("@info:whatsthis", "<b>Detect moving skies</b>: During the control points selection and matching, "
                                           "this option discards any points that are associated to a possible cloud. This "
                                           "is useful to prevent moving clouds from altering the control points matching "
                                           "process."));
    vbox->addWidget(d->celesteCheckBox);

    vbox->addStretch(2);

    QHBoxLayout* const hbox = new QHBoxLayout();

    d->detailsBtn    = new QPushButton(this);
    d->detailsBtn->setText(i18nc("@action:button", "Details..."));
    d->detailsBtn->hide();
    hbox->addWidget(d->detailsBtn);

    hbox->addStretch(10);

    vbox->addLayout(hbox);

    vbox->addStretch(2);

    d->progressLabel = new QLabel(this);
    d->progressLabel->setAlignment(Qt::AlignCenter);
    vbox->addWidget(d->progressLabel);

    vbox->addStretch(10);

    setLayout(vbox);

    resetTitle();

    QPixmap leftPix(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kipiplugin_panorama/pics/assistant-preprocessing.png")));
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));

    connect(d->detailsBtn, SIGNAL(clicked()),
            this, SLOT(slotShowDetails()));
}

PreProcessingPage::~PreProcessingPage()
{
    KConfig config(QStringLiteral("kipirc"));
    KConfigGroup group = config.group("Panorama Settings");
    group.writeEntry("Celeste", d->celesteCheckBox->isChecked());
    config.sync();

    delete d;
}

void PreProcessingPage::process()
{
    QMutexLocker lock(&d->progressMutex);

    d->title->setText(i18n("<qt>"
                           "<p>Pre-processing is in progress, please wait.</p>"
                           "<p>This can take a while...</p>"
                           "</qt>"));

    d->celesteCheckBox->hide();
    d->progressTimer->start(300);

    connect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));
    connect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

//     d->nbFilesProcessed = 0;

    d->mngr->resetBasePto();
    d->mngr->resetCpFindPto();
    d->mngr->resetCpCleanPto();
    d->mngr->thread()->preProcessFiles(d->mngr->itemsList(),
                                       d->mngr->preProcessedMap(),
                                       d->mngr->basePtoUrl(),
                                       d->mngr->cpFindPtoUrl(),
                                       d->mngr->cpCleanPtoUrl(),
                                       d->celesteCheckBox->isChecked(),
//                                        d->mngr->hdr(),
                                       d->mngr->format(),
                                       d->mngr->gPano(),
                                       d->mngr->rawDecodingSettings(),
                                       d->mngr->cpFindBinary().version(),
                                       d->mngr->cpCleanBinary().path(),
                                       d->mngr->cpFindBinary().path());
}

bool PreProcessingPage::cancel()
{
    d->canceled = true;

    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));
    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    d->mngr->thread()->cancel();

    QMutexLocker lock(&d->progressMutex);
    if (d->progressTimer->isActive())
    {
        d->progressTimer->stop();
        d->progressLabel->clear();
        resetTitle();
        return false;
    }
    return true;
}

void PreProcessingPage::resetPage()
{
    d->canceled = false;
    resetTitle();
}

void PreProcessingPage::slotProgressTimerDone()
{
    d->progressLabel->setPixmap(d->progressPix.frameAt(d->progressCount));

    d->progressCount = (d->progressCount + 1) % d->progressPix.frameCount();

    d->progressTimer->start(300);
}

void PreProcessingPage::slotShowDetails()
{
    KPOutputDialog dlg(QApplication::activeWindow(),
                       i18nc("@title:window", "Pre-Processing Messages"),
                       d->output);
    dlg.setAboutData(new PanoramaAboutData());
    dlg.exec();
}

void PreProcessingPage::slotAction(const KIPIPanoramaPlugin::ActionData& ad)
{
    qCDebug(KIPIPLUGINS_LOG) << "SlotAction";
    QString text;

    QMutexLocker lock(&d->progressMutex);

    if (!ad.starting)           // Something is complete...
    {
        if (!ad.success)        // Something is failed...
        {
            if (d->canceled)    // In that case, the error is expected
            {
                return;
            }
            switch (ad.action)
            {
                case PREPROCESS_INPUT:
                case CREATEPTO:
                case CPFIND:
                case CPCLEAN:
                {
                    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));
                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    qCWarning(KIPIPLUGINS_LOG) << "Job canceled: " << ad.action;
                    if (d->detailsBtn->isHidden())  // Ensures only the first failed task is shown
                    {
                        d->title->setText(i18n("<qt>"
                                                "<p>Pre-processing has failed.</p>"
                                                "<p>Press \"Details\" to show processing messages.</p>"
                                                "</qt>"));
                        d->progressTimer->stop();
                        d->celesteCheckBox->hide();
                        d->detailsBtn->show();
                        d->progressLabel->clear();
                        d->output = ad.message;
                        emit signalPreProcessed(false);
                    }
                    break;
                }
                default:
                {
                    qCWarning(KIPIPLUGINS_LOG) << "Unknown action (preprocessing) " << ad.action;
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case PREPROCESS_INPUT:
                {
//                     QMutexLocker nbProcessed(&d->nbFilesProcessed_mutex);

//                     d->nbFilesProcessed++;

                    break;
                }
                case CREATEPTO:
                case CPFIND:
                {
                    // Nothing to do, that just another step towards the end
                    break;
                }
                case CPCLEAN:
                {
                    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
                            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));
                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
                            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->progressTimer->stop();
                    d->progressLabel->clear();
                    emit signalPreProcessed(true);
                    break;
                }
                default:
                {
                    qCWarning(KIPIPLUGINS_LOG) << "Unknown action (preprocessing) " << ad.action;
                    break;
                }
            }
        }
    }
}

void PreProcessingPage::resetTitle()
{
    d->title->setText(i18n("<qt>"
                           "<p>Now, we will pre-process images before stitching them.</p>"
                           "<p>Pre-processing operations include Raw demosaicing. Raw images will be converted "
                           "to 16-bit sRGB images with auto-gamma.</p>"
                           "<p>Pre-processing also include a calculation of some control points to match "
                           "overlaps between images. For that purpose, the <b>%1</b> program from the "
                           "<a href='%2'>%3</a> project will be used.</p>"
                           "<p>Press \"Next\" to start pre-processing.</p>"
                           "</qt>",
                           QDir::toNativeSeparators(d->mngr->cpFindBinary().path()),
                           d->mngr->cpFindBinary().url().url(),
                           d->mngr->cpFindBinary().projectName()));
    d->detailsBtn->hide();
    d->celesteCheckBox->show();
}

}   // namespace KIPIPanoramaPlugin
