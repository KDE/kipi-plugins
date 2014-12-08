/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Yandex.Fotki web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2011      by Roman Tsisyk <roman at tsisyk dot com>
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

#include "logindialog.moc"

// Qt includes

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

// KDE includes
#include <kurllabel.h>
#include <ktoolinvocation.h> // for URLs
#include <kmessagebox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>

namespace KIPIYandexFotkiPlugin
{

LoginDialog::LoginDialog(QWidget* const parent, const QString& login, const QString& password)
    : QDialog(parent)
{
    setSizeGripEnabled(false);

    QVBoxLayout* const vbox = new QVBoxLayout(this);

    m_headerLabel = new QLabel(this);
    m_headerLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_headerLabel->setTextFormat(Qt::RichText);
    m_headerLabel->setText(QString("<b><h2>"
                                   "<font color=\"#ff000a\">%1</font>%2"
                                   "<font color=\"#009d00\">%3</font>"
                                   "</b></h2>")
                           .arg(i18nc("Yandex.Fotki", "Y"))
                           .arg(i18nc("Yandex.Fotki", "andex."))
                           .arg(i18nc("Yandex.Fotki", "Fotki")));

    QFrame* const hline = new QFrame(this);
    hline->setObjectName("hline");
    hline->setFrameShape( QFrame::HLine );
    hline->setFrameShadow( QFrame::Sunken );
    hline->setFrameShape( QFrame::HLine );

    QGridLayout* const centerLayout = new QGridLayout();

    m_loginEdit    = new KLineEdit(this);
    m_passwordEdit = new KLineEdit(this);
    m_passwordEdit->setEchoMode(KLineEdit::Password);

    QLabel* const loginLabel    = new QLabel(this);
    loginLabel->setText(i18n( "Login:" ));

    QLabel* const passwordLabel = new QLabel(this);
    passwordLabel->setText(i18n("Password:"));

    KUrlLabel* const forgotLabel = new KUrlLabel(this);
    forgotLabel->setText(i18n("Forgot your password?"));
    forgotLabel->setUrl("http://passport.yandex.ru/passport?mode=restore");

    connect(forgotLabel, SIGNAL(leftClickedUrl(QString)),
            this, SLOT(slotProcessUrl(QString)));

    centerLayout->addWidget(m_loginEdit,    0, 1);
    centerLayout->addWidget(m_passwordEdit, 1, 1);
    centerLayout->addWidget(loginLabel,     0, 0);
    centerLayout->addWidget(passwordLabel,  1, 0);
    centerLayout->addWidget(forgotLabel,    2, 1);

    QHBoxLayout* const btnLayout = new QHBoxLayout();
    QPushButton* const okBtn     = new QPushButton(this);
    okBtn->setAutoDefault(true);
    okBtn->setDefault(true);
    okBtn->setText(i18n("&Login"));

    QPushButton* const cancelBtn = new QPushButton(this);
    cancelBtn->setText(i18n("&Skip"));

    btnLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->setMargin(0);
    btnLayout->setSpacing(5);

    QHBoxLayout* const footerLayout = new QHBoxLayout();
    KUrlLabel *licenseLabel = new KUrlLabel(this);
    licenseLabel->setText(i18n("Yandex User Agreement"));
    licenseLabel->setUrl("http://fotki.yandex.ru/agreement.xml");

    connect(licenseLabel, SIGNAL(leftClickedUrl(QString)),
            this, SLOT(slotProcessUrl(QString)));

    footerLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    footerLayout->addWidget(licenseLabel);

    vbox->setMargin(5);
    vbox->setSpacing(5);
    vbox->setObjectName("vbox");
    vbox->addWidget(m_headerLabel);
    vbox->addWidget(hline);
    vbox->addLayout(centerLayout);
    vbox->addLayout(btnLayout);
    vbox->addLayout(footerLayout);

    resize(QSize(300, 150).expandedTo(minimumSizeHint()));

    setLogin(login);
    setPassword(password);

    connect(okBtn, SIGNAL(clicked()),
            this, SLOT(slotAccept()));

    connect(cancelBtn, SIGNAL(clicked()),
            this, SLOT(reject()));
}

LoginDialog::~LoginDialog()
{
}

QString LoginDialog::login() const
{
    return m_loginEdit->text();
}

QString LoginDialog::password() const
{
    return m_passwordEdit->text();
}

void LoginDialog::setLogin(const QString& login)
{
    m_loginEdit->setText(login);
}

void LoginDialog::setPassword(const QString& password)
{
    m_passwordEdit->setText(password);
}

void LoginDialog::slotAccept()
{
    if (!m_passwordEdit->text().isEmpty())
    {
        accept();
    }
    else
    {
        KMessageBox::error(this, i18n("Password cannot be empty."), i18n("Error"));
    }
}

void LoginDialog::slotProcessUrl(const QString& url)
{
    KToolInvocation::self()->invokeBrowser(url);
}

} // namespace KIPIYandexFotkiPlugin
