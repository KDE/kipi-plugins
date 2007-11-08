/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : e-mail settings page.
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// QT includes.

#include <QLayout>
#include <QPushButton>

// KDE includes.

#include <k3listview.h>
#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kimageio.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// LibKDcraw includes.

#include <libkdcraw/rawfiles.h>
#include <libkdcraw/dcrawbinary.h>

// Local includes.

#include "emailpage.h"
#include "emailpage.moc"

namespace KIPISendimagesPlugin
{

class EmailPagePriv
{
public:

    EmailPagePriv()
    {
        iface        = 0;
    }

    KIPI::Interface *iface;
};

EmailPage::EmailPage(QWidget* parent, KIPI::Interface *iface)
         : QWidget(parent)
{
    d = new EmailPagePriv;
    d->iface = iface;

    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------


    // --------------------------------------------------------

/*    grid->addWidget(d->listView, 0, 0, 3, 1);
    grid->addWidget(d->addButton, 0, 1, 1, 1);
    grid->addWidget(d->removeButton, 1, 1, 1, 1);*/
    grid->setColumnStretch(0, 10);                     
    grid->setRowStretch(2, 10);    
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());        

    // --------------------------------------------------------

}

EmailPage::~EmailPage()
{
    delete d;
}

}  // namespace KIPISendimagesPlugin
