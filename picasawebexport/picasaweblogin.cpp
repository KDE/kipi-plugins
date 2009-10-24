/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "picasaweblogin.h"
#include "picasaweblogin.moc"

// Qt includes

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

// KDE includes

#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>

namespace KIPIPicasawebExportPlugin
{

PicasawebLogin::PicasawebLogin(QWidget* parent, const QString& header,
                               const QString& _name, const QString& _passwd)
              : QDialog(parent)
{
    setSizeGripEnabled(false);

    QVBoxLayout* vbox = new QVBoxLayout(this);

    m_headerLabel = new QLabel(this);
    m_headerLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
    m_headerLabel->setText(header);

    QFrame* hline = new QFrame(this);
    hline->setObjectName("hline");
    hline->setFrameShape( QFrame::HLine );
    hline->setFrameShadow( QFrame::Sunken );
    hline->setFrameShape( QFrame::HLine );

    QGridLayout* centerLayout = new QGridLayout();

    m_nameEdit   = new KLineEdit(this);
    m_passwdEdit = new KLineEdit(this);
    m_passwdEdit->setEchoMode(KLineEdit::Password);

    QLabel* nameLabel = new QLabel(this);
    nameLabel->setText(i18n( "Google Login:" ));
    // centerLayout->addWidget(nameLabel, 0, 0);

    QLabel* passwdLabel = new QLabel(this);
    passwdLabel->setText(i18n("Password:"));

    centerLayout->addWidget(m_nameEdit,   0, 1);
    centerLayout->addWidget(m_passwdEdit, 1, 1);
    centerLayout->addWidget(nameLabel,    0, 0);
    centerLayout->addWidget(passwdLabel,  1, 0);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton *okBtn     = new QPushButton(this);
    okBtn->setAutoDefault(true);
    okBtn->setDefault(true);
    okBtn->setText(i18n("&OK"));

    QPushButton *cancelBtn = new QPushButton(this);
    cancelBtn->setText(i18n("&Cancel"));

    btnLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->setMargin(0);
    btnLayout->setSpacing(5);

    vbox->setMargin(5);
    vbox->setSpacing(5);
    vbox->setObjectName("vbox");
    vbox->addWidget(m_headerLabel);
    vbox->addWidget(hline);
    vbox->addLayout(centerLayout);
    vbox->addLayout(btnLayout);

    resize(QSize(300, 150).expandedTo(minimumSizeHint()));

    //TODO: KDE4PORT
    //clearWState( WState_Polished );

    m_nameEdit->setText(_name);
    m_passwdEdit->setText(_passwd);

    // signals and slots connections

    connect(okBtn, SIGNAL(clicked()),
            this, SLOT(accept()));

    connect(cancelBtn, SIGNAL(clicked()),
            this, SLOT(reject()));
}

PicasawebLogin::~PicasawebLogin()
{
}

QString PicasawebLogin::name() const
{
    return m_nameEdit->text();
}

QString PicasawebLogin::password() const
{
    return m_passwdEdit->text();
}

QString PicasawebLogin::username() const
{
    return m_nameEdit->text();
}

void PicasawebLogin::setUsername(const QString& username)
{
    kDebug()<<" The username passed to me is "<<username ;
    m_nameEdit->setText(username);
    kDebug()<<" The username passed to me is "<<username ;
}

void PicasawebLogin::setPassword(const QString& password)
{
    m_passwdEdit->setText(password);
}

} // namespace KIPIPicasawebExportPlugin
