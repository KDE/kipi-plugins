/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "preprocessingpage.moc"

// Qt includes

#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QPixmap>
#include <QPushButton>
#include <QCheckBox>

// KDE includes

#include <kstandarddirs.h>
#include <kdialog.h>
#include <kvbox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <kpixmapsequence.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "outputdialog.h"
#include "cpcleanbinary.h"
#include "cpfindbinary.h"
#include "manager.h"
#include "actionthread.h"

using namespace KIPIPlugins;

namespace KIPIPanoramaPlugin
{

struct PreProcessingPage::PreProcessingPagePriv
{
    PreProcessingPagePriv()
        : progressCount(0), progressLabel(0), progressTimer(0),
          title(0), celesteCheckBox(0), detailsBtn(0), mngr(0)
    {
        progressPix = KPixmapSequence("process-working", KIconLoader::SizeSmallMedium);
    }

    int             progressCount;
    QLabel*         progressLabel;
    QTimer*         progressTimer;

    QLabel*         title;

    QCheckBox*      celesteCheckBox;

    QString         output;

    QPushButton*    detailsBtn;

    KPixmapSequence progressPix;

    Manager*        mngr;
};

PreProcessingPage::PreProcessingPage(Manager* mngr, KAssistantDialog* dlg)
                 : WizardPage(dlg, i18n("<b>Pre-Processing Images</b>")),
                   d(new PreProcessingPagePriv)
{
    d->mngr             = mngr;
    d->progressTimer    = new QTimer(this);
    KVBox* vbox         = new KVBox(this);
    d->title            = new QLabel(vbox);
    d->title->setWordWrap(true);
    d->title->setOpenExternalLinks(true);

    KConfig config("kipirc");
    KConfigGroup group  = config.group(QString("Panorama Settings"));

    d->celesteCheckBox  = new QCheckBox(i18n("Detect moving skies"), vbox);
    d->celesteCheckBox->setChecked(group.readEntry("Celeste", false));
    d->celesteCheckBox->setToolTip(i18n("Automatic detection of clouds to prevent wrong keypoints matching "
                                        "between images due to moving clouds."));
    d->celesteCheckBox->setWhatsThis(i18n("<b>Detect moving skies</b>: During the control points selection and matching, "
                                          "this option discards any points that are associated to a possible cloud. This "
                                          "is useful to prevent moving clouds from altering the control points matching "
                                          "process."));

    QLabel* space1   = new QLabel(vbox);
    KHBox* hbox      = new KHBox(vbox);
    d->detailsBtn    = new QPushButton(hbox);
    d->detailsBtn->setText(i18n("Details..."));
    d->detailsBtn->hide();
    QLabel* space2   = new QLabel(hbox);
    hbox->setStretchFactor(space2, 10);

    QLabel* space3   = new QLabel(vbox);
    d->progressLabel = new QLabel(vbox);
    d->progressLabel->setAlignment(Qt::AlignCenter);
    QLabel* space4   = new QLabel(vbox);

    vbox->setStretchFactor(space1, 2);
    vbox->setStretchFactor(space3, 2);
    vbox->setStretchFactor(space4, 10);
    vbox->setSpacing(KDialog::spacingHint());
    vbox->setMargin(KDialog::spacingHint());

    setPageWidget(vbox);

    resetTitle();

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_panorama/pics/assistant-preprocessing.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->mngr->thread(), SIGNAL(starting(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));

    connect(d->detailsBtn, SIGNAL(clicked()),
            this, SLOT(slotShowDetails()));
}

PreProcessingPage::~PreProcessingPage()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Panorama Settings"));
    group.writeEntry("Celeste", d->celesteCheckBox->isChecked());
    config.sync();

    delete d;
}

void PreProcessingPage::process()
{
    d->title->setText(i18n("<qt>"
                           "<p>Pre-processing is in progress, please wait.</p>"
                           "<p>This can take a while...</p>"
                           "</qt>"));

    d->celesteCheckBox->hide();
    d->progressTimer->start(300);

    connect(d->mngr->thread(), SIGNAL(finished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    d->mngr->thread()->setPreProcessingSettings(d->celesteCheckBox->isChecked(),
                                                d->mngr->hdr(),
                                                d->mngr->format(),
                                                d->mngr->rawDecodingSettings());
    d->mngr->thread()->preProcessFiles(d->mngr->itemsList(), d->mngr->cpCleanBinary().path(),
                                       d->mngr->cpFindBinary().path());
    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void PreProcessingPage::cancel()
{
    disconnect(d->mngr->thread(), SIGNAL(finished(KIPIPanoramaPlugin::ActionData)),
               this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    d->mngr->thread()->cancel();
    d->progressTimer->stop();
    d->progressLabel->clear();
    resetTitle();
}

void PreProcessingPage::resetPage()
{
    resetTitle();
}

void PreProcessingPage::slotProgressTimerDone()
{
    d->progressLabel->setPixmap(d->progressPix.frameAt(d->progressCount));

    d->progressCount++;
    if (d->progressCount == 8)
        d->progressCount = 0;

    d->progressTimer->start(300);
}

void PreProcessingPage::slotShowDetails()
{
    OutputDialog dlg(kapp->activeWindow(),
                     i18n("Pre-Processing Messages"),
                     d->output);
                     i18n("Pre-Processing Messages"),
    dlg.setAboutData((KPAboutData*)d->mngr->about(), QString("panorama"));
    dlg.exec();
}

void PreProcessingPage::slotAction(const KIPIPanoramaPlugin::ActionData& ad)
{
    QString text;

    if (!ad.starting)           // Something is complete...
    {
        if (!ad.success)        // Something is failed...
        {
            switch (ad.action)
            {
                case(PREPROCESS):
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
                    emit signalPreProcessed(ItemUrlsMap());
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
                case(PREPROCESS):
                {
                    d->progressTimer->stop();
                    d->progressLabel->clear();
                    d->mngr->setCPFindUrl(ad.ptoUrl);
                    emit signalPreProcessed(ad.preProcessedUrlsMap);
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
                           QString(d->mngr->cpFindBinary().path()),
                           d->mngr->cpFindBinary().url().url(),
                           d->mngr->cpFindBinary().projectName()));
    d->detailsBtn->hide();
    d->celesteCheckBox->show();
}

}   // namespace KIPIPanoramaPlugin
