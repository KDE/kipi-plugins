/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kconfig.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "outputdialog.h"
#include "alignbinary.h"
#include "manager.h"
#include "actionthread.h"

using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

class PreProcessingPagePriv
{
public:

    PreProcessingPagePriv()
    {
        progressPix   = SmallIcon("process-working", 22);
        progressCount = 0;
        progressTimer = 0;
        progressLabel = 0,
        mngr          = 0;
        title         = 0;
        detailsBtn    = 0;
    }

    int          progressCount;
    QLabel*      progressLabel;
    QPixmap      progressPix;
    QTimer*      progressTimer;

    QLabel*      title;

    QCheckBox*   alignCheckBox;

    QString      output;

    QPushButton* detailsBtn;

    Manager*     mngr;
};

PreProcessingPage::PreProcessingPage(Manager* mngr, KAssistantDialog* dlg)
                 : WizardPage(dlg, i18n("<b>Pre-Processing Bracketed Images</b>")),
                   d(new PreProcessingPagePriv)
{
    d->mngr          = mngr;
    d->progressTimer = new QTimer(this);
    KVBox *vbox      = new KVBox(this);
    d->title         = new QLabel(vbox);
    d->title->setWordWrap(true);
    d->title->setOpenExternalLinks(true);
    d->alignCheckBox = new QCheckBox(i18n("Align bracketed images"), vbox);
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("ExpoBlending Settings"));
    d->alignCheckBox->setChecked(group.readEntry("Auto Alignment", true));

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

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_expoblending/pics/assistant-preprocessing.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->mngr->thread(), SIGNAL(starting(const KIPIExpoBlendingPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIExpoBlendingPlugin::ActionData&)));

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));

    connect(d->detailsBtn, SIGNAL(clicked()),
            this, SLOT(slotShowDetails()));
}

PreProcessingPage::~PreProcessingPage()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("ExpoBlending Settings"));
    group.writeEntry("Auto Alignment", d->alignCheckBox->isChecked());
    config.sync();

    delete d;
}

void PreProcessingPage::resetTitle()
{
    d->title->setText(i18n("<qt>"
                           "<p>Now, we will pre-process bracketed images before to fuse it.</p>"
                           "<p>To perform auto-alignment, <b>%1</b> program from "
                           "<a href='%2'>%3</a> project will be used. "
                           "Alignment must be performed if you haven't used a tripod to take bracketed images. "
                           "Alignment operation can take a while.</p>"
                           "<p>Pre-Processing operations included Raw demosaicing. Raw images will be converted "
                           "to sixteenbits sRGB images with auto-gamma."
                           "<p>Press \"Next\" button to start pre-processing.</p>"
                           "</qt>",
                           QString(d->mngr->alignBinary().path()),
                           d->mngr->alignBinary().url().url(),
                           d->mngr->alignBinary().projectName()));
    d->detailsBtn->hide();
    d->alignCheckBox->show();
}

void PreProcessingPage::process()
{
    d->title->setText(i18n("<qt>"
                           "<p>Pre-processing is under progress, please wait.<p>"
                           "<p>This can take a while...</p>"
                           "</qt>"));

    d->alignCheckBox->hide();
    d->progressTimer->start(300);

    connect(d->mngr->thread(), SIGNAL(finished(const KIPIExpoBlendingPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIExpoBlendingPlugin::ActionData&)));

    d->mngr->thread()->setPreProcessingSettings(d->alignCheckBox->isChecked(), d->mngr->rawDecodingSettings());
    d->mngr->thread()->preProcessFiles(d->mngr->itemsList());
    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void PreProcessingPage::cancel()
{
    disconnect(d->mngr->thread(), SIGNAL(finished(const KIPIExpoBlendingPlugin::ActionData&)),
               this, SLOT(slotAction(const KIPIExpoBlendingPlugin::ActionData&)));

    d->mngr->thread()->cancel();
    d->progressTimer->stop();
    d->progressLabel->clear();
    resetTitle();
}

void PreProcessingPage::slotProgressTimerDone()
{
    d->progressLabel->setPixmap(QPixmap(d->progressPix.copy(0, d->progressCount*22, 22, 22)));

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
    dlg.setAboutData((KPAboutData*)d->mngr->about(), QString("expoblending"));
    dlg.exec();
}

void PreProcessingPage::slotAction(const KIPIExpoBlendingPlugin::ActionData& ad)
{
    QString text;

    if (!ad.starting)           // Something is complete...
    {
        if (!ad.success)        // Something is failed...
        {
            switch (ad.action)
            {
                case(PREPROCESSING):
                {
                    d->title->setText(i18n("<qt>"
                                           "<p>Pre-processing has failed !</p>"
                                           "<p>Please check your bracketed images stack...</p>"
                                           "<p>Press \"Details\" button to show processing messages.</p>"
                                           "</qt>"));
                    d->progressTimer->stop();
                    d->alignCheckBox->hide();
                    d->detailsBtn->show();
                    d->progressLabel->clear();
                    d->output = ad.message;
                    emit signalPreProcessed(ItemUrlsMap());
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
                case(PREPROCESSING):
                {
                    d->progressTimer->stop();
                    d->progressLabel->clear();
                    emit signalPreProcessed(ad.alignedUrlsMap);
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

}   // namespace KIPIExpoBlendingPlugin
