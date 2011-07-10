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

#include "lastpage.h"

// Qt includes

#include <QLabel>
#include <QPixmap>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>

// Local includes

#include "manager.h"

namespace KIPIPanoramaPlugin
{

struct LastPage::LastPagePriv
{
    LastPagePriv() : mngr(0) {}

    Manager* mngr;
};

LastPage::LastPage(Manager* mngr, KAssistantDialog* dlg)
        : KIPIPlugins::WizardPage(dlg, i18n("Panorama Stitched")),
          d(new LastPagePriv)
{
    d->mngr       = mngr;
    KVBox *vbox   = new KVBox(this);
    QLabel *title = new QLabel(vbox);
    title->setOpenExternalLinks(true);
    title->setWordWrap(true);
    title->setText(i18n("<qt>"
                        "<p><h1><b>Panorama Stiching is Done</b></h1></p>"
                        "<p>Congratulations. Your images are stitched into a panorama.</p>"
                        "<p>Your panorama has been created with the filename %1.</p>"
                        "</qt>",
                        QString("TODO")
                        ));

    setPageWidget(vbox);

    //QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_panorama/pics/assistant-xxx.png");
    //setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));
}

LastPage::~LastPage()
{
    delete d;
}

}   // namespace KIPIPanoramaPlugin
