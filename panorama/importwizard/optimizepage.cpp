/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "optimizepage.moc"

// Qt includes

#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QTimer>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdialog.h>
#include <kdebug.h>
#include <kapplication.h>

// Local includes

#include "outputdialog.h"
#include "autooptimiserbinary.h"
#include "manager.h"
#include "actionthread.h"

namespace KIPIPanoramaPlugin
{

struct OptimizePage::OptimizePagePriv
{
    OptimizePagePriv() : progressLabel(0), progressTimer(0), title(0), detailsBtn(0), mngr(0)
    {
        progressPix = SmallIcon("process-working", 22);
    }

    int          progressCount;
    QLabel*      progressLabel;
    QPixmap      progressPix;
    QTimer*      progressTimer;

    QLabel*      title;

    QString      output;

    QPushButton* detailsBtn;

    Manager*     mngr;
};

OptimizePage::OptimizePage(Manager* mngr, KAssistantDialog* dlg)
    : KIPIPlugins::WizardPage(dlg, i18n("Optimization")),
      d(new OptimizePagePriv)
{
    d->mngr          = mngr;
    KVBox *vbox      = new KVBox(this);
    d->progressTimer = new QTimer(this);
    d->title         = new QLabel(vbox);
    d->title->setOpenExternalLinks(true);
    d->title->setWordWrap(true);

    QLabel* space1   = new QLabel(vbox);
    vbox->setStretchFactor(space1, 2);

    KHBox* hbox      = new KHBox(vbox);
    d->detailsBtn    = new QPushButton(hbox);
    d->detailsBtn->setText(i18n("Details..."));
    d->detailsBtn->hide();

    QLabel* space2   = new QLabel(hbox);
    hbox->setStretchFactor(space2, 10);

    QLabel* space3   = new QLabel(vbox);
    vbox->setStretchFactor(space3, 2);

    d->progressLabel = new QLabel(vbox);
    d->progressLabel->setAlignment(Qt::AlignCenter);

    QLabel* space4   = new QLabel(vbox);
    vbox->setStretchFactor(space4, 10);

    vbox->setSpacing(KDialog::spacingHint());
    vbox->setMargin(KDialog::spacingHint());

    setPageWidget(vbox);

    resetTitle();

    //QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_panorama/pics/assistant-optimization.png");
    //setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->mngr->thread(), SIGNAL(starting(const KIPIPanoramaPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIPanoramaPlugin::ActionData&)));

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));

    connect(d->detailsBtn, SIGNAL(clicked()),
            this, SLOT(slotShowDetails()));
}

OptimizePage::~OptimizePage()
{
    delete d;
}

void OptimizePage::resetTitle()
{
    d->title->setText(i18n("<qt>"
    "<p><h1><b>Images Pre-Processing is Done</b></h1></p>"
    "<p>The optimization step according to your will is ready to be performed.</p>"
    "<p>To perform this operation, the <b>%1</b> program from the "
    "<a href='%2'>%3</a> project will be used.</p>"
    "<p>Press the \"Next\" button to run the optimization.</p>"
    "</qt>",
    QString(d->mngr->autoOptimiserBinary().path()),
            d->mngr->autoOptimiserBinary().url().url(),
            d->mngr->autoOptimiserBinary().projectName()));
    d->detailsBtn->hide();
}

void OptimizePage::process()
{
    d->title->setText(i18n("<qt>"
    "<p>Optimization is under progress, please wait.</p>"
    "<p>This can take a while...</p>"
    "</qt>"));

    d->progressTimer->start(300);

    connect(d->mngr->thread(), SIGNAL(finished(const KIPIPanoramaPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIPanoramaPlugin::ActionData&)));

    d->mngr->thread()->optimizeProject(d->mngr->cpFindUrl());
    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void OptimizePage::cancel()
{
    disconnect(d->mngr->thread(), SIGNAL(finished(const KIPIPanoramaPlugin::ActionData&)),
               this, SLOT(slotAction(const KIPIPanoramaPlugin::ActionData&)));

    d->mngr->thread()->cancel();
    d->progressTimer->stop();
    d->progressLabel->clear();
    resetTitle();
}

void OptimizePage::slotProgressTimerDone()
{
    d->progressLabel->setPixmap(QPixmap(d->progressPix.copy(0, d->progressCount*22, 22, 22)));

    d->progressCount++;
    if (d->progressCount == 8)
        d->progressCount = 0;

    d->progressTimer->start(300);
}

void OptimizePage::slotAction(const KIPIPanoramaPlugin::ActionData& ad)
{
    QString text;

    if (!ad.starting)           // Something is complete...
    {
        if (!ad.success)        // Something is failed...
        {
            switch (ad.action)
            {
                case(OPTIMIZE):
                {
                    d->title->setText(i18n("<qt>"
                    "<p>Optimization has failed.</p>"
                    "<p>Press \"Details\" to show processing messages.</p>"
                    "</qt>"));
                    d->progressTimer->stop();
                    d->detailsBtn->show();
                    d->progressLabel->clear();
                    d->output = ad.message;
                    emit signalOptimized(KUrl());
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
                case(OPTIMIZE):
                {
                    d->progressTimer->stop();
                    d->progressLabel->clear();
                    emit signalOptimized(ad.ptoUrl);
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

void OptimizePage::slotShowDetails()
{
    OutputDialog dlg(kapp->activeWindow(),
                     i18n("Pre-Processing Messages"),
                     d->output);
    i18n("Pre-Processing Messages"),
         dlg.setAboutData((KPAboutData*)d->mngr->about(), QString("panorama"));
    dlg.exec();
}

}   // namespace KIPIPanoramaPlugin
