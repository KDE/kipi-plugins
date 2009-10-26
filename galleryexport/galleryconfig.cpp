/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
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

#include "galleryconfig.h"
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

GalleryEdit::GalleryEdit(QWidget* pParent, Gallery* pGallery, const QString& title)
           : KDialog(pParent, Qt::Dialog)
{
    mpGallery = pGallery;

    setCaption(title);

    QFrame *page = new QFrame(this);
    QGridLayout* centerLayout = new QGridLayout();
    page->setMinimumSize(500, 200);
    setMainWidget(page);

    mpNameEdit = new KLineEdit(this);
    centerLayout->addWidget(mpNameEdit, 0, 1);

    mpUrlEdit = new KLineEdit(this);
    centerLayout->addWidget(mpUrlEdit, 1, 1);

    mpUsernameEdit = new KLineEdit(this);
    centerLayout->addWidget(mpUsernameEdit, 2, 1);

    mpPasswordEdit = new KLineEdit(this);
    mpPasswordEdit->setEchoMode(KLineEdit::Password);
    centerLayout->addWidget(mpPasswordEdit, 3, 1);

    QLabel* namelabel = new QLabel(this);
    namelabel->setText(i18n("Name:"));
    centerLayout->addWidget(namelabel, 0, 0);

    QLabel* urlLabel = new QLabel(this);
    urlLabel->setText(i18n("URL:"));
    centerLayout->addWidget(urlLabel, 1, 0);

    QLabel* usernameLabel = new QLabel(this);
    usernameLabel->setText(i18n("Username:"));
    centerLayout->addWidget(usernameLabel, 2, 0);

    QLabel* passwdLabel = new QLabel(this);
    passwdLabel->setText(i18n("Password:"));
    centerLayout->addWidget(passwdLabel, 3, 0);

    //---------------------------------------------

    mpGalleryVersion = new QCheckBox(i18n("Use &Gallery 2"), this);
    mpGalleryVersion->setChecked(2 == pGallery->version());
    centerLayout->addWidget(mpGalleryVersion, 4, 1);

    //---------------------------------------------

    page->setLayout(centerLayout);

    resize(QSize(300, 150).expandedTo(minimumSizeHint()));

    // setting initial data
    mpNameEdit->setText(pGallery->name());
    mpUrlEdit->setText(pGallery->url());
    mpUsernameEdit->setText(pGallery->username());
    mpPasswordEdit->setText(pGallery->password());

    connect(this, SIGNAL( okClicked() ), 
            this, SLOT( slotOk() ));
}

GalleryEdit::~GalleryEdit()
{
}

void GalleryEdit::slotOk()
{
    if (mpNameEdit->isModified())
        mpGallery->setName(mpNameEdit->text());

    if (mpUrlEdit->isModified())
        mpGallery->setUrl(mpUrlEdit->text());

    if (mpUsernameEdit->isModified())
        mpGallery->setUsername(mpUsernameEdit->text());

    if (mpPasswordEdit->isModified())
        mpGallery->setPassword(mpPasswordEdit->text());

    if (mpGalleryVersion->isChecked())
        mpGallery->setVersion(2);
    else
        mpGallery->setVersion(1);

    mpGallery->save();
    accept();
}

} // namespace KIPIGalleryExportPlugin
