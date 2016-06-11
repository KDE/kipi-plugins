/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
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

#include "login.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDialog>
#include <QFrame>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>

namespace KIPIFlickrPlugin
{

FlickrLogin::FlickrLogin(QWidget* const parent, const QString& header,
                         const QString& _name, const QString& _passwd)
    : QDialog(parent)
{
    setWindowTitle(header);

    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QDialogButtonBox* const buttonBox = new QDialogButtonBox(QDialogButtonBox::Help|QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QVBoxLayout* const mainLayout     = new QVBoxLayout(this);
    setLayout(mainLayout);
    QPushButton* const okButton       = buttonBox->button(QDialogButtonBox::Ok);

    connect(buttonBox, SIGNAL(accepted()),
            this, SLOT(accept()));

    connect(buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));

    okButton->setDefault(true);
    setModal(false);

    m_headerLabel     = new QLabel(this);
    m_headerLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
    m_headerLabel->setText(header);

    QFrame* const hline = new QFrame(this);
    hline->setLineWidth(1);
    hline->setMidLineWidth(0);
    hline->setFrameShape(QFrame::HLine);
    hline->setFrameShadow(QFrame::Sunken);
    hline->setMinimumSize(0, 2);
    hline->updateGeometry();

    QGridLayout* const centerLayout = new QGridLayout();

    m_nameEdit   = new QLineEdit(this);
    m_passwdEdit = new QLineEdit(this);
    m_passwdEdit->setEchoMode(QLineEdit::Password);

    QLabel* const nameLabel = new QLabel(this);
    nameLabel->setText(i18nc("flickr login", "Username:"));

    QLabel* const passwdLabel = new QLabel(this);
    passwdLabel->setText(i18n("Password:"));

    centerLayout->addWidget(m_nameEdit,   0, 1);
    centerLayout->addWidget(m_passwdEdit, 1, 1);
    centerLayout->addWidget(nameLabel,    0, 0);
    centerLayout->addWidget(passwdLabel,  1, 0);
    centerLayout->setContentsMargins(spacing, spacing, spacing, spacing);
    centerLayout->setSpacing(spacing);

    mainLayout->addWidget(m_headerLabel);
    mainLayout->addWidget(hline);
    mainLayout->addWidget(buttonBox);
    mainLayout->addLayout(centerLayout);
    mainLayout->setContentsMargins(QMargins());
    mainLayout->setSpacing(spacing);

    resize(QSize(300, 150).expandedTo(minimumSizeHint()));

    m_nameEdit->setText(_name);
    m_passwdEdit->setText(_passwd);
}

FlickrLogin::~FlickrLogin()
{
}

QString FlickrLogin::name() const
{
    return m_nameEdit->text();
}

QString FlickrLogin::password() const
{
    return m_passwdEdit->text();
}

} // namespace KIPIFlickrPlugin
