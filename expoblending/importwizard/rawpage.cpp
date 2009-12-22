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

#include "rawpage.h"

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

// Libkdcraw includes

#include <libkdcraw/dcrawsettingswidget.h>

namespace KIPIExpoBlendingPlugin
{

class RawPagePriv
{
public:

    RawPagePriv()
    {
        title       = 0;
        rawSettings = 0;
    }

    QLabel*              title;

    DcrawSettingsWidget* rawSettings;
};

RawPage::RawPage(KAssistantDialog* dlg)
       : WizardPage(dlg, i18n("<b>RAW Decoding Settings</b>")),
         d(new RawPagePriv)
{
    QScrollArea *sa = new QScrollArea(this);
    KVBox *vbox     = new KVBox(sa->viewport());
    sa->setWidget(vbox);
    sa->setWidgetResizable(true);
    sa->setAutoFillBackground(false);
    sa->viewport()->setAutoFillBackground(false);
    vbox->setAutoFillBackground(false);

    d->title = new QLabel(vbox);
    d->title->setWordWrap(true);
    d->title->setOpenExternalLinks(true);
    d->title->setText(i18n("<qt>"
                           "<p>Your bracketed images stack contain Raw images.</p>"
                           "<p>Please set the right settings to import Raw files.</p>"
                           "</qt>"));

    d->rawSettings = new DcrawSettingsWidget(vbox, DcrawSettingsWidget::SIXTEENBITS    |
                                                   DcrawSettingsWidget::COLORSPACE     |
                                                   DcrawSettingsWidget::POSTPROCESSING |
                                                   DcrawSettingsWidget::BLACKWHITEPOINTS);
    d->rawSettings->setObjectName("RawSettingsBox Expander"); 
    
    setPageWidget(vbox);

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_expoblending/pics/assistant-raw.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));
}

RawPage::~RawPage()
{
    delete d;
}

RawDecodingSettings RawPage::rawdecodingSettings() const
{
}

}   // namespace KIPIExpoBlendingPlugin
