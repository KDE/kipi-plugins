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

#include "alignpage.moc"

// Qt includes

#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QPixmap>

// KDE includes

#include <kstandarddirs.h>
#include <kdialog.h>
#include <kvbox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kiconloader.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "manager.h"
#include "actionthread.h"

namespace KIPIExpoBlendingPlugin
{

class AlignPagePriv
{
public:

    AlignPagePriv()
    {
        progressPix   = SmallIcon("process-working", 22);
        progressCount = 0;
        progressTimer = 0;
        progressLabel = 0,
        mngr          = 0;
        title         = 0;
    }

    int      progressCount;
    QLabel*  progressLabel;
    QPixmap  progressPix;
    QTimer*  progressTimer;

    QLabel*  title;

    Manager* mngr;
};

AlignPage::AlignPage(Manager* mngr, KAssistantDialog* dlg)
         : WizardPage(dlg, i18n("<b>Align Bracketed Images</b>")),
           d(new AlignPagePriv)
{
    d->mngr          = mngr;
    d->progressTimer = new QTimer(this);
    KVBox *vbox      = new KVBox(this);
    d->title         = new QLabel(vbox);
    d->title->setWordWrap(true);
    d->title->setOpenExternalLinks(true);
    resetTitle();

    d->progressLabel = new QLabel(vbox);
    d->progressLabel->setAlignment(Qt::AlignCenter);

    setPageWidget(vbox);

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_expoblending/pics/assistant-align.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->mngr->thread(), SIGNAL(starting(const KIPIExpoBlendingPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIExpoBlendingPlugin::ActionData&)));

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));
}

AlignPage::~AlignPage()
{
    delete d;
}

void AlignPage::resetTitle()
{
    d->title->setText(i18n("<qt>"
                           "<p>Now, we will align bracketed images before to fuse it.</p>"
                           "<p>To do it, <b>align_image_stack</b> program from "
                           "<a href='http://hugin.sourceforge.net'>Hugin</a> "
                           "project will be used. Please, take a sure that it's installed "
                           "on your computer.</p>"
                           "<p>Press \"Next\" button to start auto-alignment process.</p>"
                           "</qt>"));
}

void AlignPage::processAlignement()
{
    d->title->setText(i18n("<qt>"
                           "<p>Pre-processing and auto-alignment is under progress, please wait.<p>"
                           "<p>This can take a while...</p>"
                           "</qt>"));

    d->progressTimer->start(300);

    connect(d->mngr->thread(), SIGNAL(finished(const KIPIExpoBlendingPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIExpoBlendingPlugin::ActionData&)));

    d->mngr->thread()->setAlignSettings(d->mngr->rawDecodingSettings());
    d->mngr->thread()->alignFiles(d->mngr->itemsList());
    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void AlignPage::cancelAlignement()
{
    disconnect(d->mngr->thread(), SIGNAL(finished(const KIPIExpoBlendingPlugin::ActionData&)),
               this, SLOT(slotAction(const KIPIExpoBlendingPlugin::ActionData&)));

    d->mngr->thread()->cancel();
    d->progressTimer->stop();
    d->progressLabel->clear();
    resetTitle();
}

void AlignPage::slotProgressTimerDone()
{
    d->progressLabel->setPixmap(QPixmap(d->progressPix.copy(0, d->progressCount*22, 22, 22)));

    d->progressCount++;
    if (d->progressCount == 8)
        d->progressCount = 0;

    d->progressTimer->start(300);
}

void AlignPage::slotAction(const KIPIExpoBlendingPlugin::ActionData& ad)
{
    QString text;

    if (!ad.starting)           // Something is complete...
    {
        if (!ad.success)        // Something is failed...
        {
            switch (ad.action)
            {
                case(ALIGN):
                {
                    d->title->setText(i18n("<qt>"
                                           "<p>Auto-alignment has failed !</p>"
                                           "<p>Please check your bracketed images...</p>"
                                           "</qt>"));
                    d->progressTimer->stop();
                    d->progressLabel->clear();
                    emit signalAligned(ItemUrlsMap());
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
                case(ALIGN):
                {
                    d->progressTimer->stop();
                    d->progressLabel->clear();
                    emit signalAligned(ad.alignedUrlsMap);
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
