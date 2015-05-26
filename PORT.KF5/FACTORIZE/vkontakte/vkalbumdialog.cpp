/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a kipi plugin to export images to VKontakte web service
 *
 * Copyright (C) 2011 by Roman Tsisyk <roman at tsisyk dot com>
 * Copyright (C) 2011, 2015  Alexander Potashev <aspotashev@gmail.com>
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

#include "vkalbumdialog.moc"

// Qt includes

#include <QtGui/QFormLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

// KDE includes

#include <klocale.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kmessagebox.h>
#include <kcombobox.h>

// libvkontakte includes

#include <libkvkontakte/albuminfo.h>

namespace KIPIVkontaktePlugin
{

VkontakteAlbumDialog::VkontakteAlbumDialog(QWidget* const parent)
    : KDialog(parent), m_album()
{
    initDialog(false);
}

VkontakteAlbumDialog::VkontakteAlbumDialog(QWidget* const parent,
                                           const VkontakteAlbumDialog::AlbumInfo &album)
    : KDialog(parent), m_album(album)
{
    initDialog(true);
}

VkontakteAlbumDialog::~VkontakteAlbumDialog()
{
    // nothing
}

void VkontakteAlbumDialog::initDialog(bool editing)
{
    setWindowTitle(editing ? i18nc("@title:window", "Edit album")
                           : i18nc("@title:window", "New album"));
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);

    QWidget* const mainWidget = new QWidget(this);
    setMainWidget(mainWidget);
    mainWidget->setMinimumSize(400, 300);

    QGroupBox* const albumBox = new QGroupBox(i18nc("@title:group Header above Title and Summary fields", "Album"), mainWidget);
    albumBox->setWhatsThis(i18n("These are basic settings for the new VKontakte album."));

    m_titleEdit = new KLineEdit(m_album.title);
    m_titleEdit->setWhatsThis(i18n("Title of the album that will be created (required)."));

    m_summaryEdit = new KTextEdit(m_album.description);
    m_summaryEdit->setWhatsThis(i18n("Description of the album that will be created (optional)."));


    QFormLayout* const albumBoxLayout  = new QFormLayout;
    albumBoxLayout->addRow(i18n("Title:"),   m_titleEdit);
    albumBoxLayout->addRow(i18n("Summary:"), m_summaryEdit);
    albumBox->setLayout(albumBoxLayout);

    QGroupBox* const privacyBox         = new QGroupBox(i18n("Privacy Settings"), mainWidget);
    QGridLayout* const privacyBoxLayout = new QGridLayout;

    m_albumPrivacyCombo = new KComboBox(privacyBox);
    m_albumPrivacyCombo->addItem(i18n("Only me"),               QVariant(Vkontakte::AlbumInfo::PRIVACY_PRIVATE));
    m_albumPrivacyCombo->addItem(i18n("My friends"),            QVariant(Vkontakte::AlbumInfo::PRIVACY_FRIENDS));
    m_albumPrivacyCombo->addItem(i18n("Friends of my friends"), QVariant(Vkontakte::AlbumInfo::PRIVACY_FRIENDS_OF_FRIENDS));
    m_albumPrivacyCombo->addItem(i18n("Everyone"),              QVariant(Vkontakte::AlbumInfo::PRIVACY_PUBLIC));
    privacyBoxLayout->addWidget(new QLabel(i18n("Album available to:")), 0, 0);
    privacyBoxLayout->addWidget(m_albumPrivacyCombo, 0, 1);

    m_commentsPrivacyCombo = new KComboBox(privacyBox);
    m_commentsPrivacyCombo->addItem(i18n("Only me"),               QVariant(Vkontakte::AlbumInfo::PRIVACY_PRIVATE));
    m_commentsPrivacyCombo->addItem(i18n("My friends"),            QVariant(Vkontakte::AlbumInfo::PRIVACY_FRIENDS));
    m_commentsPrivacyCombo->addItem(i18n("Friends of my friends"), QVariant(Vkontakte::AlbumInfo::PRIVACY_FRIENDS_OF_FRIENDS));
    m_commentsPrivacyCombo->addItem(i18n("Everyone"),              QVariant(Vkontakte::AlbumInfo::PRIVACY_PUBLIC));
    privacyBoxLayout->addWidget(new QLabel(i18n("Comments available to:")), 1, 0);
    privacyBoxLayout->addWidget(m_commentsPrivacyCombo, 1, 1);

    privacyBox->setLayout(privacyBoxLayout);

    QVBoxLayout* const mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addWidget(albumBox);
    mainLayout->addWidget(privacyBox);
    mainLayout->setSpacing(spacingHint());
    mainWidget->setLayout(mainLayout);

    if (editing)
    {
        m_titleEdit->setText(m_album.title);
        m_summaryEdit->setText(m_album.description);
        m_albumPrivacyCombo->setCurrentIndex(m_albumPrivacyCombo->findData(m_album.privacy));
        m_commentsPrivacyCombo->setCurrentIndex(m_commentsPrivacyCombo->findData(m_album.commentPrivacy));
    }

    m_titleEdit->setFocus();
}

void VkontakteAlbumDialog::slotButtonClicked(int button)
{
    if (button == Ok)
    {
        if (m_titleEdit->text().isEmpty())
        {
            KMessageBox::error(this, i18n("Title cannot be empty."),
                               i18n("Error"));
            return;
        }

        m_album.title = m_titleEdit->text();
        m_album.description = m_summaryEdit->toPlainText();

        if (m_albumPrivacyCombo->currentIndex() != -1)
            m_album.privacy = m_albumPrivacyCombo->itemData(m_albumPrivacyCombo->currentIndex()).toInt();
        else // for safety, see info about VK API bug below
            m_album.privacy = Vkontakte::AlbumInfo::PRIVACY_PRIVATE;

        if (m_commentsPrivacyCombo->currentIndex() != -1)
            m_album.commentPrivacy = m_commentsPrivacyCombo->itemData(m_commentsPrivacyCombo->currentIndex()).toInt();
        else // VK API has a bug: if "comment_privacy" is not set, it will be set to PRIVACY_PUBLIC
            m_album.commentPrivacy = Vkontakte::AlbumInfo::PRIVACY_PRIVATE;
    }

    return KDialog::slotButtonClicked(button);
}

const VkontakteAlbumDialog::AlbumInfo &VkontakteAlbumDialog::album() const
{
    return m_album;
}

} // namespace KIPIVkontaktePlugin
