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

#include "welcomepage.moc"

// Qt includes

#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

// KDE includes

#include <kstandarddirs.h>
#include <klocale.h>
#include <kdialog.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kcombobox.h>
#include "welcomepage.h"

namespace KIPIDLNAExportPlugin
{

class WelcomePage::Private
{
public:

    Private()
    {
        iconLbl              = 0;
        titleLbl             = 0;
        headerLbl            = 0;
        imageGetOption       = 0;
        getImageLbl          = 0;
        getImplementationLbl = 0;
    }

    QLabel*    iconLbl;
    QLabel*    titleLbl;
    QLabel*    headerLbl;
    QLabel*    getImageLbl;
    QLabel*    getImplementationLbl;

    KComboBox* imageGetOption;
    KComboBox* implementationGetOption;
};

WelcomePage::WelcomePage(QWidget* const parent)
    : QWidget(parent), d(new Private)
{
    QVBoxLayout* mainLayout        = new QVBoxLayout(this);
    QWidget* settingsBox           = new QWidget(this);
    QGridLayout* settingsBoxLayout = new QGridLayout(settingsBox);

    d->iconLbl  = new QLabel(this);
    d->iconLbl->setPixmap(KIconLoader::global()->loadIcon("dlna", KIconLoader::NoGroup, 64));
    d->iconLbl->setAlignment(Qt::AlignCenter);

    d->titleLbl = new QLabel(this);
    d->titleLbl->setOpenExternalLinks(true);
    d->titleLbl->setFocusPolicy(Qt::NoFocus);
    d->titleLbl->setAlignment(Qt::AlignCenter);
    d->titleLbl->setText(QString("<b><h2><a href='http://www.dlna.org'>"
                                 "<font color=\"#9ACD32\">DLNA Export</font>"
                                 "</a></h2></b>"));

    // ComboBox for image selection method
    d->getImageLbl    = new QLabel(i18n("&Choose image selection method:"),settingsBox);
    d->imageGetOption = new KComboBox(settingsBox);
    d->imageGetOption->insertItem(WelcomePage::IMAGEDIALOG, "From file system");
    d->imageGetOption->insertItem(WelcomePage::COLLECTION, "Collections");
    d->getImageLbl->setBuddy(d->imageGetOption);

    // ComboBox for implementation selection method
    d->getImplementationLbl    = new QLabel(i18n("&Choose the implementation:"),settingsBox);
    d->implementationGetOption = new KComboBox(settingsBox);
    d->implementationGetOption->insertItem(WelcomePage::HUPNP, "HUPnP API");
    d->implementationGetOption->insertItem(WelcomePage::MINIDLNA, "miniDLNA");
    d->getImplementationLbl->setBuddy(d->implementationGetOption);
    
    
    settingsBoxLayout->addWidget(d->getImageLbl,             1, 0, 1, 1);
    settingsBoxLayout->addWidget(d->imageGetOption,          1, 1, 1, 1);
    settingsBoxLayout->addWidget(d->getImplementationLbl,    2, 0, 1, 1);
    settingsBoxLayout->addWidget(d->implementationGetOption, 2, 1, 1, 1);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());

    mainLayout->addWidget(d->iconLbl);
    mainLayout->addWidget(d->titleLbl);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);
}

WelcomePage::~WelcomePage()
{
}

WelcomePage::ImageGetOption WelcomePage::getImageDialogOptionSelected() const
{
    if (d->imageGetOption->currentIndex() == 0)
        return WelcomePage::COLLECTION;
    else
        return WelcomePage::IMAGEDIALOG;
}

WelcomePage::ImplementationGetOption WelcomePage::getImplementationOptionSelected() const
{
    if (d->implementationGetOption->currentIndex() == 0)
        return WelcomePage::HUPNP;
    else
        return WelcomePage::MINIDLNA;
}


}   // namespace KIPIDLNAExportPlugin
