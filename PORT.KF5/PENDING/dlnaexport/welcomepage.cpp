/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#include "welcomepage.h"

// Qt includes

#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <kstandarddirs.h>
#include <klocalizedstring.h>
#include <kdialog.h>
#include <kiconloader.h>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIDLNAExportPlugin
{

class WelcomePage::Private
{
public:

    Private()
    {
        iconLbl                 = 0;
        titleLbl                = 0;
        headerLbl               = 0;
        descLbl                 = 0;
    }

    QLabel*         iconLbl;
    QLabel*         titleLbl;
    QLabel*         headerLbl;
    QLabel*         descLbl;
};

WelcomePage::WelcomePage(QWidget* const parent)
    : QWidget(parent),
      d(new Private)
{
    QGridLayout* const mainLayout = new QGridLayout(this);

    d->iconLbl  = new QLabel(this);
    d->iconLbl->setPixmap(KIconLoader::global()->loadIcon("kipi-dlna", KIconLoader::NoGroup, 64));

    d->titleLbl = new QLabel(this);
    d->titleLbl->setOpenExternalLinks(true);
    d->titleLbl->setFocusPolicy(Qt::NoFocus);
    d->titleLbl->setText(i18n("<b><h2><a href=\"http://www.dlna.org\">"
                              "<font color=\"#9ACD32\">DLNA Export</font>"
                              "</a></h2></b>"));

    d->descLbl  = new QLabel(this);
    d->descLbl->setWordWrap(true);
    d->descLbl->setOpenExternalLinks(true);
    d->descLbl->setText(i18n("<qt>"
                        "<p><h1><b>Welcome to DLNA export tool</b></h1></p>"
                        "<p>This tool will export your collections collections over the network using "
                        "<a href='https://en.wikipedia.org/wiki/Universal_Plug_and_Play'>Universal Plug and Play</a> protocol.</p>"
                        "<p>It will permit you to browse shared collections through a tablet or cellular with a DLNA compatible application, "
                        "or with a DLNA certified device as a game console Media player, or TV screen.</p>"
                        "<p>This assistant will help you to configure how to export collection. It must still open while sharing items.</p>"
                        "<p>For more information, please take a look at "
                        "<a href='https://en.wikipedia.org/wiki/Digital_Living_Network_Alliance'>this page</a></p>"
                        "</qt>"));

    mainLayout->addWidget(d->iconLbl,                        1, 0, 1, 2, Qt::AlignCenter);
    mainLayout->addWidget(d->titleLbl,                       2, 0, 1, 2, Qt::AlignCenter);
    mainLayout->addWidget(d->descLbl,                        3, 0, 5, 2);
    mainLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    mainLayout->setContentsMargins(QMargins());
}

WelcomePage::~WelcomePage()
{
    delete d;
}

}   // namespace KIPIDLNAExportPlugin
