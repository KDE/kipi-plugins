/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#include "intropage.h"

// Qt includes

#include <QLabel>
#include <QPixmap>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>

namespace KIPIPanoramaPlugin
{

IntroPage::IntroPage(KAssistantDialog* dlg)
    : KIPIPlugins::WizardPage(dlg, i18n("Welcome to Panorama Tool"))
{
    KVBox* vbox   = new KVBox(this);
    QLabel* title = new QLabel(vbox);
    title->setWordWrap(true);
    title->setOpenExternalLinks(true);
    title->setText(i18n("<qt>"
                        "<p><h1><b>Welcome to Panorama tool</b></h1></p>"
                        "<p>This tool stich several images together to create a panorama, making the"
                        "seam between images not visible.</p>"
                        "<p>This assistant will help you to configure how to import images before "
                        "stiching them into a panorama.</p>"
                        "<p>Images must be taken from the same point of view.</p>"
                        "<p>For more information, please take a look at "
                        "<a href='http://hugin.sourceforge.net/tutorials/overview/en.shtml'>this page</a></p>"
                        "</qt>"));

    setPageWidget(vbox);

    //QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_panorama/pics/assistant-tripod.png");
    //setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));
}

IntroPage::~IntroPage()
{
}

}   // namespace KIPIPanoramaPlugin
