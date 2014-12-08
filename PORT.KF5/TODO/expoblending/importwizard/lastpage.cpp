/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "lastpage.h"

// Qt includes

#include <QDir>
#include <QLabel>
#include <QPixmap>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>

// Local includes

#include "enfusebinary.h"
#include "manager.h"

namespace KIPIExpoBlendingPlugin
{

class LastPage::Private
{
public:

    Private()
    {
        mngr = 0;
    }

    Manager* mngr;
};

LastPage::LastPage(Manager* const mngr, KAssistantDialog* const dlg)
        : KPWizardPage(dlg, i18n("Pre-Processing is Complete")),
          d(new Private)
{
    d->mngr             = mngr;
    KVBox* const vbox   = new KVBox(this);
    QLabel* const title = new QLabel(vbox);
    title->setOpenExternalLinks(true);
    title->setWordWrap(true);
    title->setText(i18n("<qt>"
                        "<p><h1><b>Bracketed Images Pre-Processing is Done</b></h1></p>"
                        "<p>Congratulations. Your images are ready to be fused. </p>"
                        "<p>To perform this operation, <b>%1</b> program from "
                        "<a href='%2'>Enblend</a> "
                        "project will be used.</p>"
                        "<p>Press \"Finish\" button to fuse your items and make a pseudo HDR image.</p>"
                        "</qt>",
                        QDir::toNativeSeparators(d->mngr->enfuseBinary().path()),
                        d->mngr->enfuseBinary().url().url()));

    QLabel* const space = new QLabel(vbox);
    vbox->setStretchFactor(space, 10);

    setPageWidget(vbox);

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_expoblending/pics/assistant-enfuse.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));
}

LastPage::~LastPage()
{
    delete d;
}

}   // namespace KIPIExpoBlendingPlugin
