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

#include "lastpage.h"

// Qt includes

#include <QLabel>
#include <QPixmap>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>

namespace KIPIExpoBlendingPlugin
{

LastPage::LastPage(KAssistantDialog* dlg)
        : KIPIPlugins::WizardPage(dlg, i18n("Alignement is Complete"))
{
    KVBox *vbox   = new KVBox(this);
    QLabel *title = new QLabel(vbox);
    title->setOpenExternalLinks(true);
    title->setWordWrap(true);
    title->setText(i18n("<qt>"
                        "<p><h1><b>Bracketed Images Alignement is Done</b></h1></p>"
                        "<p>Congratulation. Your images are aligned and ready to be fusioned. </p>"
                        "To perform this operation, <b>enfuse</b> program from "
                        "<a href='http://enblend.sourceforge.net'>Enblend</a> "
                        "project will be used. Please, take a sure that it's installed "
                        "on your computer.</p>"
                        "<p>Press \"Finish\" button to fuse your items and make a pseudo HDR image.</p>"
                        "</qt>"));

    setPageWidget(vbox);

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_expoblending/pics/assistant-enfuse.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));
}

LastPage::~LastPage()
{
}

}   // namespace KIPIExpoBlendingPlugin
