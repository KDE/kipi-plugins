/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a kipi plugin to export images to Yandex.Fotki web service
 *
 * Copyright (C) 2011 by Roman Tsisyk <roman at tsisyk dot com>
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

#include "yfalbumdialog.h"

// Qt includes

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QApplication>
#include <QStyle>
#include <QLineEdit>
#include <QtWidgets/QPushButton>

// KDE includes

#include <klocalizedstring.h>
#include <ktextedit.h>
#include <kmessagebox.h>

// Local includes

#include "yfalbum.h"
#include "kipiplugins_debug.h"

namespace KIPIYandexFotkiPlugin
{

YandexFotkiAlbumDialog::YandexFotkiAlbumDialog(QWidget* const parent, YandexFotkiAlbum& album)
    : QDialog(parent), m_album(album)
{
    setWindowTitle(i18n("New album"));
    setMinimumSize(400, 300);

    QGroupBox* const albumBox = new QGroupBox(i18n("Album"), this);
    albumBox->setWhatsThis(i18n("These are basic settings for the new Yandex album."));

    m_titleEdit    = new QLineEdit(album.title());
    m_titleEdit->setWhatsThis(i18n("Title of the album that will be created (required)."));

    m_summaryEdit  = new KTextEdit(album.summary());
    m_summaryEdit->setWhatsThis(i18n("Description of the album that will be created (optional)."));

    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setWhatsThis(i18n("Password for the album (optional)."));

    QFormLayout* const albumBoxLayout  = new QFormLayout;
    albumBoxLayout->addRow(i18n("Title:"), m_titleEdit);
    albumBoxLayout->addRow(i18n("Summary:"), m_summaryEdit);
    albumBoxLayout->addRow(i18n("Password:"), m_passwordEdit);

    albumBox->setLayout(albumBoxLayout);

    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(true);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &YandexFotkiAlbumDialog::slotOkClicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Layout
    QVBoxLayout* const mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(albumBox);
    mainLayout->addWidget(buttonBox);
    mainLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    setLayout(mainLayout);
}

YandexFotkiAlbumDialog::~YandexFotkiAlbumDialog()
{
    // nothing
}

void YandexFotkiAlbumDialog::slotOkClicked()
{
    if (m_titleEdit->text().isEmpty())
    {
        KMessageBox::error(this, i18n("Title cannot be empty."),
                            i18n("Error"));
        return;
    }

    m_album.setTitle(m_titleEdit->text());
    m_album.setSummary(m_summaryEdit->toPlainText());

    if (m_passwordEdit->text().isEmpty())
    {
        m_album.setPassword(QString()); // force null string
    }
    else
    {
        m_album.setPassword(m_passwordEdit->text());
    }

    accept();
}

} // namespace KIPIYandexFotkiPlugin
