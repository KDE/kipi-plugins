/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
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

#include "optimizepage.moc"

// Qt includes

#include <QLabel>
#include <QPixmap>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>

// Local includes

#include "vigoptimizebinary.h"
#include "manager.h"

namespace KIPIPanoramaPlugin
{

struct OptimizePage::OptimizePagePriv
{
    OptimizePagePriv() : mngr(0) {}

    Manager* mngr;
};

OptimizePage::OptimizePage(Manager* mngr, KAssistantDialog* dlg)
        : KIPIPlugins::WizardPage(dlg, i18n("Optimization")),
          d(new OptimizePagePriv)
{
    d->mngr       = mngr;
    KVBox *vbox   = new KVBox(this);
    QLabel *title = new QLabel(vbox);
    title->setOpenExternalLinks(true);
    title->setWordWrap(true);
    title->setText(i18n("<qt>"
                        "<p><h1><b>Images Pre-Processing is Done</b></h1></p>"
                        "<p>The optimization step according to your will is ready to be performed.</p>"
                        "<p>To perform this operation, the <b>%1</b> program from the "
                        "<a href='%2'>%3</a> project will be used.</p>"
                        "<p>Press the \"Next\" button to run the optimization.</p>"
                        "</qt>",
                        QString(d->mngr->vigOptimizeBinary().path()),
                        d->mngr->vigOptimizeBinary().url().url(),
                        d->mngr->vigOptimizeBinary().projectName()));

    setPageWidget(vbox);

    //QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_panorama/pics/assistant-optimization.png");
    //setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));
}

OptimizePage::~OptimizePage()
{
    delete d;
}

}   // namespace KIPIPanoramaPlugin
