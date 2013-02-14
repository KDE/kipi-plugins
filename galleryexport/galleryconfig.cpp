/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
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

#include "galleryconfig.moc"

// Qt includes

#include <QFrame>
#include <QGridLayout>
#include <QPushButton>

// KDE includes

#include <kdebug.h>
#include <klocale.h>

// LibKIPI includes

#include <libkipi/version.h>
#include <libkipi/interface.h>

// Local includes

#include "galleries.h"

namespace KIPIGalleryExportPlugin
{

class GalleryEdit::Private
{
public:

    Private()
    {
        galleryVersion = 0;
        nameEdit       = 0;
        urlEdit        = 0;
        usernameEdit   = 0;
        passwordEdit   = 0;
        gallery        = 0;
    }

    QCheckBox* galleryVersion;
    KLineEdit* nameEdit;
    KLineEdit* urlEdit;
    KLineEdit* usernameEdit;
    KLineEdit* passwordEdit;
    Gallery*   gallery;
};

GalleryEdit::GalleryEdit(QWidget* const pParent, Gallery* const pGallery, const QString& title)
    : KDialog(pParent, Qt::Dialog), d(new Private())
{
    d->gallery = pGallery;

    setCaption(title);

    QFrame *page = new QFrame(this);
    QGridLayout* centerLayout = new QGridLayout();
    page->setMinimumSize(500, 200);
    setMainWidget(page);

    d->nameEdit = new KLineEdit(this);
    centerLayout->addWidget(d->nameEdit, 0, 1);

    d->urlEdit = new KLineEdit(this);
    centerLayout->addWidget(d->urlEdit, 1, 1);

    d->usernameEdit = new KLineEdit(this);
    centerLayout->addWidget(d->usernameEdit, 2, 1);

    d->passwordEdit = new KLineEdit(this);
    d->passwordEdit->setEchoMode(KLineEdit::Password);
    centerLayout->addWidget(d->passwordEdit, 3, 1);

    QLabel* namelabel = new QLabel(this);
    namelabel->setText(i18nc("gallery login settings", "Name:"));
    centerLayout->addWidget(namelabel, 0, 0);

    QLabel* urlLabel = new QLabel(this);
    urlLabel->setText(i18nc("gallery login settings", "URL:"));
    centerLayout->addWidget(urlLabel, 1, 0);

    QLabel* usernameLabel = new QLabel(this);
    usernameLabel->setText(i18nc("gallery login settings", "Username:"));
    centerLayout->addWidget(usernameLabel, 2, 0);

    QLabel* passwdLabel = new QLabel(this);
    passwdLabel->setText(i18nc("gallery login settings", "Password:"));
    centerLayout->addWidget(passwdLabel, 3, 0);

    //---------------------------------------------

    d->galleryVersion = new QCheckBox(i18n("Use &Gallery 2"), this);
    d->galleryVersion->setChecked(2 == pGallery->version());
    centerLayout->addWidget(d->galleryVersion, 4, 1);

    //---------------------------------------------

    page->setLayout(centerLayout);

    resize(QSize(300, 150).expandedTo(minimumSizeHint()));

    // setting initial data
    d->nameEdit->setText(pGallery->name());
    d->urlEdit->setText(pGallery->url());
    d->usernameEdit->setText(pGallery->username());
    d->passwordEdit->setText(pGallery->password());

    connect(this, SIGNAL(okClicked()), 
            this, SLOT(slotOk()));
}

GalleryEdit::~GalleryEdit()
{
    delete d;
}

void GalleryEdit::slotOk()
{
    if (d->nameEdit->isModified())
        d->gallery->setName(d->nameEdit->text());

    if (d->urlEdit->isModified())
        d->gallery->setUrl(d->urlEdit->text());

    if (d->usernameEdit->isModified())
        d->gallery->setUsername(d->usernameEdit->text());

    if (d->passwordEdit->isModified())
        d->gallery->setPassword(d->passwordEdit->text());

    if (d->galleryVersion->isChecked())
        d->gallery->setVersion(2);
    else
        d->gallery->setVersion(1);

    d->gallery->save();
    accept();
}

} // namespace KIPIGalleryExportPlugin
