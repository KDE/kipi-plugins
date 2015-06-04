/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-16-05
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
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

#include "selectuserdlg.moc"

// Qt includes

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>

// KDE includes

#include <kcombobox.h>
#include <klocalizedstring.h>
#include <kicon.h>
#include <kconfig.h>
#include <kconfiggroup.h>

namespace KIPIFlickrExportPlugin
{

SelectUserDlg::SelectUserDlg(QWidget* const parent, const QString& serviceName)
    : QDialog(parent)
{
    m_serviceName = serviceName;

    setWindowTitle(i18n("Flickr Account Selector"));
    setModal(true);

    QDialogButtonBox* buttonBox = new QDialogButtonBox();

    QPushButton* buttonNewAccount = new QPushButton(buttonBox);
    buttonNewAccount->setText(i18n("Add another account"));
    buttonNewAccount->setIcon(KIcon("network-workgroup"));

    buttonBox->addButton(buttonNewAccount, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Close);

    buttonBox->button(QDialogButtonBox::Close)->setDefault(true);

    m_okButton = buttonBox->button(QDialogButtonBox::Ok);

    if (m_serviceName == QString("23"))
    {
        setWindowIcon(KIcon("kipi-hq"));
    }
    else if (m_serviceName == QString("Zooomr"))
    {
        setWindowIcon(KIcon("kipi-zooomr"));
    }
    else
    {
        setWindowIcon(KIcon("kipi-flickr"));
    }

    m_uname = QString();

    m_label = new QLabel(this);
    m_label->setText("Choose the " + m_serviceName + " account to use for exporting images: ");

    m_userComboBox = new KComboBox(this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_label);
    mainLayout->addWidget(m_userComboBox);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotOkClicked()));
    connect(buttonNewAccount, SIGNAL(clicked()),
            this, SLOT(slotNewAccountClicked()));
}

SelectUserDlg::~SelectUserDlg()
{
    delete m_userComboBox;
    delete m_label;
}

void SelectUserDlg::reactivate()
{
    KConfig config("kipirc");

    m_userComboBox->clear();

    foreach(const QString& group, config.groupList())
    {
        if (!(group.contains(m_serviceName)))
            continue;

        KConfigGroup grp = config.group(group);

        if (QString::compare(grp.readEntry("username"), QString(), Qt::CaseInsensitive) == 0)
            continue;

        m_userComboBox->addItem(grp.readEntry("username"));
    }

    m_okButton->setEnabled(m_userComboBox->count() > 0);

    exec();
}

void SelectUserDlg::slotOkClicked()
{
    m_uname = m_userComboBox->currentText();
}

void SelectUserDlg::slotNewAccountClicked()
{
    m_uname = QString();
}

QString SelectUserDlg::getUname() const
{
    return m_uname;
}

SelectUserDlg* SelectUserDlg::getDlg()
{
    return this;
}

} // namespace KIPIFlickrExportPlugin
