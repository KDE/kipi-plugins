/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

#include "rajcelogindialog.moc"

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

namespace KIPIRajceExportPlugin
{

RajceLoginDialog::RajceLoginDialog(QWidget* const parent, const QString& _name, const QString& _passwd)
    : QDialog(parent)
{
    setSizeGripEnabled(false);

    QVBoxLayout* const vbox          = new QVBoxLayout(this);
    QGridLayout* const centerLayout  = new QGridLayout();
    m_nameEdit                       = new KLineEdit(this);
    m_passwdEdit                     = new KLineEdit(this);
    m_passwdEdit->setEchoMode(KLineEdit::Password);

    QLabel* const nameLabel = new QLabel(this);
    nameLabel->setText(i18n( "Login:" ));
    // centerLayout->addWidget(nameLabel, 0, 0);

    QLabel* const passwdLabel = new QLabel(this);
    passwdLabel->setText(i18n("Password:"));

    centerLayout->addWidget(m_nameEdit,   0, 1);
    centerLayout->addWidget(m_passwdEdit, 1, 1);
    centerLayout->addWidget(nameLabel,    0, 0);
    centerLayout->addWidget(passwdLabel,  1, 0);

    QHBoxLayout* const btnLayout = new QHBoxLayout();
    QPushButton* const okBtn     = new QPushButton(this);
    okBtn->setAutoDefault(true);
    okBtn->setDefault(true);
    okBtn->setText(i18n("&OK"));

    QPushButton* const cancelBtn = new QPushButton(this);
    cancelBtn->setText(i18n("&Cancel"));

    btnLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->setMargin(0);
    btnLayout->setSpacing(5);

    vbox->setMargin(5);
    vbox->setSpacing(5);
    vbox->setObjectName("vbox");
    vbox->addLayout(centerLayout);
    vbox->addLayout(btnLayout);

    resize(QSize(300, 150).expandedTo(minimumSizeHint()));

    m_nameEdit->setText(_name);
    m_passwdEdit->setText(_passwd);

    // signals and slots connections

    connect(okBtn, SIGNAL(clicked()),
            this, SLOT(accept()));

    connect(cancelBtn, SIGNAL(clicked()),
            this, SLOT(reject()));
}

RajceLoginDialog::~RajceLoginDialog()
{
}

QString RajceLoginDialog::password() const
{
    return m_passwdEdit->text();
}

void RajceLoginDialog::setPassword(const QString& value)
{
    m_passwdEdit->setText(value);
}

void RajceLoginDialog::setUsername(const QString& value)
{
    m_nameEdit->setText(value);
}

QString RajceLoginDialog::username() const
{
    return m_nameEdit->text();
}

} // namespace KIPIRajceExportPlugin
