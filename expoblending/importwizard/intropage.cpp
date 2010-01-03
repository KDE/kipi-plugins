/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "intropage.h"

// Qt includes

#include <QLabel>
#include <QPixmap>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>

namespace KIPIExpoBlendingPlugin
{

IntroPage::IntroPage(KAssistantDialog* dlg)
         : KIPIPlugins::WizardPage(dlg, i18n("Welcome to Exposure Blending Tool"))
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

    setPageWidget(vbox);

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_expoblending/pics/assistant-tripod.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));
}

IntroPage::~IntroPage()
{
}

}   // namespace KIPIExpoBlendingPlugin
