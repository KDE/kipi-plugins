/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "intropage.moc"

// Qt includes

#include <QLabel>
#include <QPixmap>
#include <QGroupBox>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>

// local includes

#include "kpbinarysearch.h"
#include "alignbinary.h"
#include "enfusebinary.h"
#include "pfsbinary.h"
#include "pfshdrgenbinary.h"

namespace KIPIExpoBlendingPlugin
{

class IntroPage::IntroPagePriv
{
public:

    IntroPagePriv(Manager* const m)
        : mngr(m), 
          binariesWidget(0)
    {
    }

    Manager*                     mngr;
    KIPIPlugins::KPBinarySearch* binariesWidget;
};

IntroPage::IntroPage(Manager* const mngr, KAssistantDialog* const dlg)
    : KPWizardPage(dlg, i18n("Welcome to Exposure Blending/HDR Image Creation Tool")),
      d(new IntroPagePriv(mngr))
{
    KVBox *vbox   = new KVBox(this);
    QLabel *title = new QLabel(vbox);
    title->setWordWrap(true);
    title->setOpenExternalLinks(true);
    title->setText(i18n("<qt>"
                        "<p><h1><b>Welcome to Exposure Blending tool</b></h1></p>"
                        "<p>This tool fuses bracketed images with different exposure to make pseudo "
                        "<a href='http://en.wikipedia.org/wiki/High_dynamic_range_imaging'>HDR image</a>.</p>"
                        "<p>It can also be used to merge focus bracketed stack to get a single image "
                        "with increased depth of field.</p>"
                        "<p>This assistant will help you to configure how to import images before "
                        "merging them to a single one.</p>"
                        "<p>Bracketed images must be taken with the same camera, "
                        "in the same conditions, and if possible using a tripod.</p>"
                        "<p>For more information, please take a look at "
                        "<a href='http://en.wikipedia.org/wiki/Bracketing'>this page</a></p>"
                        "</qt>"));

    QGroupBox* binaryBox        = new QGroupBox(vbox);
    QGridLayout* binaryLayout   = new QGridLayout;
    binaryBox->setLayout(binaryLayout);
    binaryBox->setTitle(i18n("Exposure Blending Binaries"));
    d->binariesWidget = new KIPIPlugins::KPBinarySearch(binaryBox);
    d->binariesWidget->addBinary(d->mngr->alignBinary());
    d->binariesWidget->addBinary(d->mngr->enfuseBinary());
    d->binariesWidget->addBinary(d->mngr->pfscalibrateBinary());
    d->binariesWidget->addBinary(d->mngr->pfshdrgenBinary());
#ifdef Q_WS_MAC
    d->binariesWidget->addDirectory("/Applications/Hugin/HuginTools");
#endif

    connect(d->binariesWidget, SIGNAL(signalBinariesFound(bool)),
            this, SIGNAL(signalIntroPageIsValid(bool)));

    emit signalIntroPageIsValid(d->binariesWidget->allBinariesFound());

    setPageWidget(vbox);

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_expoblending/pics/assistant-tripod.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));
}

IntroPage::~IntroPage()
{
}

bool IntroPage::binariesFound()
{
    return d->binariesWidget->allBinariesFound();
}

}   // namespace KIPIExpoBlendingPlugin
