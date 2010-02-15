/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-15
 * Description : a plugin to export to a remote Piwigo server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
 * Copyright (C) 2010 by Frederic Coiffier <frederic dot coiffier at free dot com>
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

#include "piwigoconfig.h"
#include "piwigoconfig.moc"

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

#include "piwigos.h"

namespace KIPIPiwigoExportPlugin
{

PiwigoEdit::PiwigoEdit(QWidget* pParent, Piwigo* pPiwigo, const QString& title)
        : KDialog(pParent, Qt::Dialog)
{
    mpPiwigo = pPiwigo;

    setCaption(title);

    QFrame *page = new QFrame(this);
    QGridLayout* centerLayout = new QGridLayout();
    page->setMinimumSize(500, 128);
    setMainWidget(page);

    mpUrlEdit = new KLineEdit(this);
    centerLayout->addWidget(mpUrlEdit, 1, 1);

    mpUsernameEdit = new KLineEdit(this);
    centerLayout->addWidget(mpUsernameEdit, 2, 1);

    mpPasswordEdit = new KLineEdit(this);
    mpPasswordEdit->setEchoMode(KLineEdit::Password);
    centerLayout->addWidget(mpPasswordEdit, 3, 1);

    QLabel* urlLabel = new QLabel(this);
    urlLabel->setText(i18nc("piwigo login settings", "URL:"));
    centerLayout->addWidget(urlLabel, 1, 0);

    QLabel* usernameLabel = new QLabel(this);
    usernameLabel->setText(i18nc("piwigo login settings", "Username:"));
    centerLayout->addWidget(usernameLabel, 2, 0);

    QLabel* passwdLabel = new QLabel(this);
    passwdLabel->setText(i18nc("piwigo login settings", "Password:"));
    centerLayout->addWidget(passwdLabel, 3, 0);

    //---------------------------------------------

    page->setLayout(centerLayout);

    resize(QSize(300, 150).expandedTo(minimumSizeHint()));

    // setting initial data
    mpUrlEdit->setText(pPiwigo->url());
    mpUsernameEdit->setText(pPiwigo->username());
    mpPasswordEdit->setText(pPiwigo->password());

    connect(this, SIGNAL( okClicked() ),
            this, SLOT( slotOk() ));
}

PiwigoEdit::~PiwigoEdit()
{
}

void PiwigoEdit::slotOk()
{
    if (mpUrlEdit->isModified())
        mpPiwigo->setUrl(mpUrlEdit->text());

    if (mpUsernameEdit->isModified())
        mpPiwigo->setUsername(mpUsernameEdit->text());

    if (mpPasswordEdit->isModified())
        mpPiwigo->setPassword(mpPasswordEdit->text());

    mpPiwigo->save();
    accept();
}

} // namespace KIPIPiwigoExportPlugin
