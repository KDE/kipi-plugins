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
    title->setText(i18n("<qt>"
                        "<p><h1><b>Welcome to Exposure Blending tool</b></h1></p>"
                        "<p>This tool fuse bracketed image to make pseudo HDR.</p>"
                        "<p>This assistant will help you to configure how to import images before "
                        "to merge it to a single one.</p>"
                        "</qt>"));

    setPageWidget(vbox);
}

IntroPage::~IntroPage()
{
}

}   // namespace KIPIExpoBlendingPlugin
