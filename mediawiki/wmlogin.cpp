/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to WikiMedia web service
 *
 * Copyright (C) 2011 by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
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

#include "wmlogin.moc"

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

namespace KIPIWikiMediaPlugin
{

WmLogin::WmLogin(QWidget* parent, const QString& header,
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
    m_wikiSelect = new QComboBox(this);
    m_passwdEdit->setEchoMode(KLineEdit::Password);

    m_wikiSelect->addItem(QString("test wikipedia"), QUrl("http://test.wikipedia.org/w/api.php"));
    m_wikiSelect->addItem(QString("en wikipedia"),   QUrl("http://en.wikipedia.org/w/api.php"));
    m_wikiSelect->addItem(QString("fr wikipedia"),   QUrl("http://fr.wikipedia.org/w/api.php"));

    QLabel* nameLabel = new QLabel(this);
    nameLabel->setText(i18n( "Wiki Login:" ));
    // centerLayout->addWidget(nameLabel, 0, 0);

    QLabel* passwdLabel = new QLabel(this);
    passwdLabel->setText(i18n("Password:"));

    QLabel* wikiLabel = new QLabel(this);
    wikiLabel->setText(i18n("Wiki:"));

    centerLayout->addWidget(m_nameEdit,   0, 1);
    centerLayout->addWidget(m_passwdEdit, 1, 1);
    centerLayout->addWidget(m_wikiSelect, 2, 1);
    centerLayout->addWidget(nameLabel,    0, 0);
    centerLayout->addWidget(passwdLabel,  1, 0);
    centerLayout->addWidget(wikiLabel,    2, 0);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* okBtn     = new QPushButton(this);
    okBtn->setAutoDefault(true);
    okBtn->setDefault(true);
    okBtn->setText(i18n("&OK"));

    QPushButton* cancelBtn = new QPushButton(this);
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

WmLogin::~WmLogin()
{
}

QString WmLogin::name() const
{
    return m_nameEdit->text();
}

QString WmLogin::password() const
{
    return m_passwdEdit->text();
}

QString WmLogin::username() const
{
    return m_nameEdit->text();
}

QUrl WmLogin::wiki() const
{
    kDebug() << m_wikiSelect->itemData(m_wikiSelect->currentIndex()).toUrl();
    return m_wikiSelect->itemData(m_wikiSelect->currentIndex()).toUrl();
}

void WmLogin::setUsername(const QString& username)
{
    kDebug()<<" The username passed to me is "<<username ;
    m_nameEdit->setText(username);
    kDebug()<<" The username passed to me is "<<username ;
}

void WmLogin::setPassword(const QString& password)
{
    m_passwdEdit->setText(password);
}

} // namespace KIPIWikiMediaPlugin
