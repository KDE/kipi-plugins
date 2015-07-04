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
#include <QGridLayout>
#include <QApplication>
#include <QStyle>
#include <QComboBox>

// KDE includes

#include <kstandarddirs.h>
#include <klocalizedstring.h>
#include <kdialog.h>
#include <kiconloader.h>

// Local includes

#include "kipiplugins_debug.h"
#include "minidlnabinary.h"
#include "kpbinarysearch.h"

using namespace KIPIPlugins;

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
        binariesWidget          = 0;
        binariesLbl             = 0;
    }

    QLabel*         iconLbl;
    QLabel*         titleLbl;
    QLabel*         headerLbl;
    QLabel*         binariesLbl;

    KPBinarySearch* binariesWidget;
    MinidlnaBinary  minidlnaBinary;
};

WelcomePage::WelcomePage(Wizard* const wizard)
    : QWidget(wizard),
      d(new Private)
{
    QGridLayout* const mainLayout        = new QGridLayout(this);
    QWidget* const settingsBox           = new QWidget(this);
    QGridLayout* const settingsBoxLayout = new QGridLayout(settingsBox);

    d->iconLbl  = new QLabel(this);
    d->iconLbl->setPixmap(KIconLoader::global()->loadIcon("kipi-dlna", KIconLoader::NoGroup, 64));

    d->titleLbl = new QLabel(this);
    d->titleLbl->setOpenExternalLinks(true);
    d->titleLbl->setFocusPolicy(Qt::NoFocus);
    d->titleLbl->setText(i18n("<b><h2><a href=\"http://www.dlna.org\">"
                              "<font color=\"#9ACD32\">DLNA Export</font>"
                              "</a></h2></b>"));

    d->binariesLbl    = new QLabel(i18n("<b>DLNAExport Binaries' requirement</b>"), settingsBox);
    d->binariesWidget = new KPBinarySearch(settingsBox);
    d->binariesWidget->addBinary(d->minidlnaBinary);

    settingsBoxLayout->addWidget(d->binariesLbl,             1, 0, 1, 1);
    settingsBoxLayout->addWidget(d->binariesWidget,          2, 0, 1, 2);
    settingsBoxLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    mainLayout->addWidget(d->iconLbl,                        1, 0, 1, 2, Qt::AlignCenter);
    mainLayout->addWidget(d->titleLbl,                       2, 0, 1, 2, Qt::AlignCenter);
    mainLayout->addWidget(settingsBox,                       3, 0, 5, 2);
    mainLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    mainLayout->setMargin(0);

    connect(d->binariesWidget, SIGNAL(signalBinariesFound(bool)),
            wizard, SLOT(slotBinariesFound(bool)));
    
    d->minidlnaBinary.setup();
}

WelcomePage::~WelcomePage()
{
    delete d;
}

QString WelcomePage::getMinidlnaBinaryPath() const
{
    return d->minidlnaBinary.path();
}

}   // namespace KIPIDLNAExportPlugin
