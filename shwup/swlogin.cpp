/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2009 by Timothée Groleau <kde at timotheegroleau dot com>
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

#include "swlogin.moc"

// Qt includes

#include <QFormLayout>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kcombobox.h>

// local includes

#include "switem.h"

namespace KIPIShwupPlugin
{

SwLogin::SwLogin(QWidget* const parent)
    : KDialog(parent)
{
    QString header(i18n("Shwup Login"));
    setWindowTitle(header);
    setButtons(Ok|Cancel);
    setDefaultButton(Cancel);
    setModal(true);

    QWidget* const mainWidget = new QWidget(this);
    setMainWidget(mainWidget);
    mainWidget->setMinimumSize(300, 0);

    // ------------------------------------------------------------------------
    m_emailEdt          = new KLineEdit;
    m_emailEdt->setWhatsThis(i18n("Email of shwup account (required)."));

    m_passwordEdt       = new KLineEdit;
    m_passwordEdt->setEchoMode( QLineEdit::Password );
    m_passwordEdt->setWhatsThis(i18n("Password of shwup account (required)."));

    QFormLayout* const loginBoxLayout = new QFormLayout;
    loginBoxLayout->addRow(i18nc("login settings", "Email:"),    m_emailEdt);
    loginBoxLayout->addRow(i18nc("login settings", "Password:"), m_passwordEdt);
    loginBoxLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    loginBoxLayout->setSpacing(KDialog::spacingHint());
    loginBoxLayout->setMargin(KDialog::spacingHint());
    mainWidget->setLayout(loginBoxLayout);
}

SwLogin::~SwLogin()
{
}

void SwLogin::getUserProperties(SwUser &user)
{
    user.email    = m_emailEdt->text();
    user.password = m_passwordEdt->text();
}

} // namespace KIPIShwupPlugin
