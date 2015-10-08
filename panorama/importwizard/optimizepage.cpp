/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2011-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "optimizepage.h"

// Qt includes

#include <QDir>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QTimer>
#include <QCheckBox>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>

// KDE includes

#include <kconfig.h>
#include <klocalizedstring.h>

// LibKDcraw includes

#include <KDCRAW/RWidgetUtils>

// Local includes

#include "kipiplugins_debug.h"
#include "aboutdata.h"
#include "kpoutputdialog.h"
#include "autooptimiserbinary.h"
#include "panomodifybinary.h"
#include "manager.h"
#include "actionthread.h"
#include "kputil.h"

using namespace KDcrawIface;

namespace KIPIPanoramaPlugin
{

struct OptimizePage::Private
{
    Private()
        : progressCount(0),
          progressLabel(0),
          progressTimer(0),
          canceled(false),
          title(0),
//        preprocessResults(0),
          horizonCheckbox(0),
//        projectionAndSizeCheckbox(0),
          detailsBtn(0),
          progressPix(KPWorkingPixmap()),
          mngr(0)
    {
    }

    int                        progressCount;
    QLabel*                    progressLabel;
    QTimer*                    progressTimer;
    QMutex                     progressMutex;      // This is a precaution in case the user does a back / next action at the wrong moment
    bool                       canceled;

    QLabel*                    title;
//  QLabel*                    preprocessResults;

    QCheckBox*                 horizonCheckbox;
//  QCheckBox*                 projectionAndSizeCheckboxs;

    QString                    output;

    QPushButton*               detailsBtn;

    KPWorkingPixmap progressPix;

    Manager*                   mngr;
};

OptimizePage::OptimizePage(Manager* const mngr, KPWizardDialog* const dlg)
    : KPWizardPage(dlg, i18nc("@title:window", "<b>Optimization</b>")),
      d(new Private)
{
    d->mngr                         = mngr;
    d->progressTimer                = new QTimer(this);
    KPVBox* const vbox               = new KPVBox(this);
    d->title                        = new QLabel(vbox);
    d->title->setOpenExternalLinks(true);
    d->title->setWordWrap(true);

    KConfig config(QStringLiteral("kipirc"));
    KConfigGroup group              = config.group("Panorama Settings");

    d->horizonCheckbox              = new QCheckBox(i18nc("@option:check", "Level horizon"), vbox);
    d->horizonCheckbox->setChecked(group.readEntry("Horizon", true));
    d->horizonCheckbox->setToolTip(i18nc("@info:tooltip", "Detect the horizon and adapt the project to make it horizontal."));
    d->horizonCheckbox->setWhatsThis(i18nc("@info:whatsthis", "<b>Level horizon</b>: Detect the horizon and adapt the projection so that "
                                           "the detected horizon is an horizontal line in the final panorama"));
/*
    if (!d->mngr->gPano())
    {
        d->projectionAndSizeCheckbox = new QCheckBox(i18nc("@option:check", "Automatic projection and output aspect"), vbox);
        d->projectionAndSizeCheckbox->setChecked(group.readEntry("Output Projection And Size", true));
        d->projectionAndSizeCheckbox->setToolTip(i18nc("@info:tooltip", "Adapt the projection of the panorama and the area rendered on the "
                                                       "resulting projection so that every photo fits in the resulting "
                                                       "panorama."));
        d->projectionAndSizeCheckbox->setWhatsThis(i18nc("@info:whatsthis", "<b>Automatic projection and output aspect</b>: Automatically "
                                                         "adapt the projection and the area rendered of the panorama to "
                                                         "get every photos into the panorama."));
    }
    else
    {
        d->projectionAndSizeCheckbox = new QCheckBox(i18nc("@option:check", "Automatic output aspect"), vbox);
        d->projectionAndSizeCheckbox->setChecked(group.readEntry("Output Projection And Size", true));
        d->projectionAndSizeCheckbox->setToolTip(i18nc("@info:tooltip", "Adapt the area rendered on the resulting projection so that "
                                                       "every photo fits in the resulting panorama."));
        d->projectionAndSizeCheckbox->setWhatsThis(i18nc("@info:whatsthis", "<b>Automatic output aspect</b>: Automatically adapt the area "
                                                         "rendered of the panorama to get every photos into the panorama."));
    }
*/

//  d->preprocessResults            = new QLabel(vbox);

    vbox->setStretchFactor(new QWidget(vbox), 2);

    QHBoxLayout* const hbox = new QHBoxLayout();

    d->detailsBtn           = new QPushButton(vbox);
    d->detailsBtn->setText(i18nc("@action:button", "Details..."));
    d->detailsBtn->hide();
    hbox->addWidget(d->detailsBtn);
    hbox->addStretch(10);

    vbox->setStretchFactor(new QWidget(vbox), 2);

    d->progressLabel        = new QLabel(vbox);
    d->progressLabel->setAlignment(Qt::AlignCenter);

    vbox->setStretchFactor(new QWidget(vbox), 10);

    setPageWidget(vbox);

    resetTitle();

    QPixmap leftPix(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kipiplugin_panorama/pics/assistant-hugin.png")));
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));

    connect(d->detailsBtn, SIGNAL(clicked()),
            this, SLOT(slotShowDetails()));
}

OptimizePage::~OptimizePage()
{
    KConfig config(QStringLiteral("kipirc"));
    KConfigGroup group = config.group("Panorama Settings");
    group.writeEntry("Horizon", d->horizonCheckbox->isChecked());
//  group.writeEntry("Output Projection And Size", d->projectionAndSizeCheckbox->isChecked());
    config.sync();

    delete d;
}

void OptimizePage::process()
{
    QMutexLocker lock(&d->progressMutex);

    d->title->setText(i18n("<qt>"
                           "<p>Optimization is in progress, please wait.</p>"
                           "<p>This can take a while...</p>"
                           "</qt>"));
    d->horizonCheckbox->hide();
//  d->projectionAndSizeCheckbox->hide();
    d->progressTimer->start(300);

    connect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));
    connect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    d->mngr->resetAutoOptimisePto();
    d->mngr->thread()->optimizeProject(d->mngr->cpCleanPtoUrl(),
                                       d->mngr->autoOptimisePtoUrl(),
                                       d->mngr->viewAndCropOptimisePtoUrl(),
                                       d->horizonCheckbox->isChecked(),
                                       d->mngr->gPano(),
                                       d->mngr->autoOptimiserBinary().path(),
                                       d->mngr->panoModifyBinary().path());
}

bool OptimizePage::cancel()
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

void OptimizePage::resetPage()
{
    d->canceled = false;
    resetTitle();
}

void OptimizePage::slotProgressTimerDone()
{
    d->progressLabel->setPixmap(d->progressPix.frameAt(d->progressCount));

    d->progressCount++;
    if (d->progressCount == 8)
        d->progressCount = 0;

    d->progressTimer->start(300);
}

void OptimizePage::slotAction(const KIPIPanoramaPlugin::ActionData& ad)
{
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
                case OPTIMIZE:
                case AUTOCROP:
                {
                    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));
                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    if (d->detailsBtn->isHidden())
                    {
                        d->title->setText(i18n("<qt>"
                                            "<p>Optimization has failed.</p>"
                                            "<p>Press \"Details\" to show processing messages.</p>"
                                            "</qt>"));
                        d->progressTimer->stop();
                        d->horizonCheckbox->hide();
//                      d->projectionAndSizeCheckbox->hide();
                        d->detailsBtn->show();
                        d->progressLabel->clear();
                        d->output = ad.message;
                        emit signalOptimized(false);
                    }
                    break;
                }
                default:
                {
                    qCWarning(KIPIPLUGINS_LOG) << "Unknown action (optimize) " << ad.action;
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case OPTIMIZE:
                {
                    return;
                }
                case AUTOCROP:
                {
                    disconnect(d->mngr->thread(), SIGNAL(stepFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));
                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(KIPIPanoramaPlugin::ActionData)),
                               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

                    d->progressTimer->stop();
                    d->progressLabel->clear();
                    emit signalOptimized(true);
                    break;
                }
                default:
                {
                    qCWarning(KIPIPLUGINS_LOG) << "Unknown action (optimize) " << ad.action;
                    break;
                }
            }
        }
    }
}

void OptimizePage::slotShowDetails()
{
    KPOutputDialog dlg(QApplication::activeWindow(),
                       i18nc("@title:window", "Pre-Processing Messages"),
                       d->output);

    dlg.setAboutData(new PanoramaAboutData());
    dlg.exec();
}

void OptimizePage::resetTitle()
{
    d->title->setText(i18n("<qt>"
                           "<p><h1><b>Images Pre-Processing is Done</b></h1></p>"
                           "<p>The optimization step according to your settings is ready to be performed.</p>"
                           "<p>This step can include an automatic leveling of the horizon, and also "
                           "an automatic projection selection and size</p>"
                           "<p>To perform this operation, the <command>%1</command> program from the "
                           "<a href='%2'>%3</a> project will be used.</p>"
                           "<p>Press the \"Next\" button to run the optimization.</p>"
                           "</qt>",
                           QDir::toNativeSeparators(d->mngr->autoOptimiserBinary().path()),
                           d->mngr->autoOptimiserBinary().url().url(),
                           d->mngr->autoOptimiserBinary().projectName()));

//  QPair<double, int> result = d->mngr->cpFindUrlData().standardDeviation();
//  d->preprocessResults->setText(i18n("Alignment error: %1px", result.first / ((double) result.second)));
    d->detailsBtn->hide();
    d->horizonCheckbox->show();
//  d->projectionAndSizeCheckbox->show();
}

}   // namespace KIPIPanoramaPlugin
