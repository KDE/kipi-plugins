/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : A KIPI plugin to export images to VKontakte web service.
 *
 * Copyright (C) 2011-2012, 2015  Alexander Potashev <aspotashev@gmail.com>
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

#include "albumchooserwidget.moc"

// Qt includes

#include <QWidget>
#include <QVBoxLayout>
#include <klocalizedstring.h>
#include <kguiitem.h>
#include <KComboBox>
#include <KPushButton>
#include <QToolButton>
#include <KMessageBox>

// LibKvkontakte includes

#include <libkvkontakte/albumlistjob.h>
#include <libkvkontakte/createalbumjob.h>
#include <libkvkontakte/editalbumjob.h>
#include <libkvkontakte/deletealbumjob.h>
#include <libkvkontakte/vkapi.h>

// Local includes

#include "vkalbumdialog.h"

namespace KIPIVkontaktePlugin
{

AlbumChooserWidget::AlbumChooserWidget(QWidget* const parent,
                                       Vkontakte::VkApi* const vkapi)
    : QGroupBox(i18nc("@title:group Header above controls for managing albums", "Album"), parent)
{
    m_vkapi         = vkapi;
    m_albumToSelect = -1;

    setWhatsThis(i18n("This is the VKontakte album that will be used for the transfer."));
    QVBoxLayout* const albumsBoxLayout = new QVBoxLayout(this);

    m_albumsCombo        = new KComboBox(this);
    m_albumsCombo->setEditable(false);

    m_newAlbumButton     = new KPushButton(KGuiItem(i18n("New Album"), "list-add", i18n("Create new VKontakte album")), this);
    m_reloadAlbumsButton = new KPushButton(KGuiItem(i18nc("reload albums list", "Reload"), "view-refresh",
                                           i18n("Reload albums list")), this);

    m_editAlbumButton    = new QToolButton(this);
    m_editAlbumButton->setToolTip(i18n("Edit selected album"));
    m_editAlbumButton->setEnabled(false);
    m_editAlbumButton->setIcon(KIcon("document-edit"));

    m_deleteAlbumButton  = new QToolButton(this);
    m_deleteAlbumButton->setToolTip(i18n("Delete selected album"));
    m_deleteAlbumButton->setEnabled(false);
    m_deleteAlbumButton->setIcon(KIcon("edit-delete"));

    QWidget* const currentAlbumWidget           = new QWidget(this);
    QHBoxLayout* const currentAlbumWidgetLayout = new QHBoxLayout(currentAlbumWidget);
    currentAlbumWidgetLayout->setContentsMargins(0, 0, 0, 0);
    currentAlbumWidgetLayout->addWidget(m_albumsCombo);
    currentAlbumWidgetLayout->addWidget(m_editAlbumButton);
    currentAlbumWidgetLayout->addWidget(m_deleteAlbumButton);

    QWidget* const albumButtons           = new QWidget(this);
    QHBoxLayout* const albumButtonsLayout = new QHBoxLayout(albumButtons);
    albumButtonsLayout->setContentsMargins(0, 0, 0, 0);
    albumButtonsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    albumButtonsLayout->addWidget(m_newAlbumButton);
    albumButtonsLayout->addWidget(m_reloadAlbumsButton);

    albumsBoxLayout->addWidget(currentAlbumWidget);
    albumsBoxLayout->addWidget(albumButtons);

    connect(m_newAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotNewAlbumRequest()));

    connect(m_editAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotEditAlbumRequest()));

    connect(m_deleteAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotDeleteAlbumRequest()));

    connect(m_reloadAlbumsButton, SIGNAL(clicked()),
            this, SLOT(slotReloadAlbumsRequest()));

    connect(m_vkapi, SIGNAL(authenticated()),
            this, SLOT(slotReloadAlbumsRequest()));
}

AlbumChooserWidget::~AlbumChooserWidget()
{
}

/**
 * @brief Clear the list of albums
 **/
void AlbumChooserWidget::clearList()
{
    m_albumsCombo->clear();
}

bool AlbumChooserWidget::getCurrentAlbumInfo(
    VkontakteAlbumDialog::AlbumInfo &out)
{
    int index = m_albumsCombo->currentIndex();

    if (index >= 0)
    {
        Vkontakte::AlbumInfoPtr album = m_albums.at(index);
        out.title = album->title();
        out.description = album->description();
        out.privacy = album->privacy();
        out.commentPrivacy = album->commentPrivacy();

        return true;
    }
    else
    {
        return false;
    }
}

bool AlbumChooserWidget::getCurrentAlbumId(int &out)
{
    int index = m_albumsCombo->currentIndex();

    if (index >= 0)
    {
        Vkontakte::AlbumInfoPtr album = m_albums.at(index);
        out = album->aid();

        return true;
    }
    else
    {
        return false;
    }
}

void AlbumChooserWidget::selectAlbum(int aid)
{
    /*
     * If the album list is not ready yet, select this album later
     */
    m_albumToSelect = aid;

    for (int i = 0; i < m_albums.size(); i ++)
    {
        if (m_albums.at(i)->aid() == aid)
        {
            m_albumsCombo->setCurrentIndex(i);
            break;
        }
    }
}

//------------------------------

void AlbumChooserWidget::slotNewAlbumRequest()
{
    QPointer<VkontakteAlbumDialog> dlg = new VkontakteAlbumDialog(this);

    if (dlg->exec() == QDialog::Accepted)
    {
        updateBusyStatus(true);
        startAlbumCreation(dlg->album());
    }

    delete dlg;
}

void AlbumChooserWidget::startAlbumCreation(const VkontakteAlbumDialog::AlbumInfo &album)
{
    Vkontakte::CreateAlbumJob* const job = new Vkontakte::CreateAlbumJob(m_vkapi->accessToken(),
                                                                         album.title, album.description,
                                                                         album.privacy, album.commentPrivacy);

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotAlbumCreationDone(KJob*)));

    job->start();
}

void AlbumChooserWidget::slotAlbumCreationDone(KJob* kjob)
{
    Vkontakte::CreateAlbumJob* const job = dynamic_cast<Vkontakte::CreateAlbumJob*>(kjob);
    Q_ASSERT(job);

    if (job == 0 || job->error())
    {
        handleVkError(job);
        updateBusyStatus(false);
    }
    else
    {
        // Select the newly created album in the combobox later (in "slotAlbumsReloadDone()")
        m_albumToSelect = job->album()->aid();

        startAlbumsReload();
        updateBusyStatus(true);
    }
}

//------------------------------

void AlbumChooserWidget::slotEditAlbumRequest()
{
    VkontakteAlbumDialog::AlbumInfo album;
    int aid = 0;
    if (!getCurrentAlbumInfo(album) || !getCurrentAlbumId(aid))
    {
        return;
    }

    QPointer<VkontakteAlbumDialog> dlg = new VkontakteAlbumDialog(this, album);

    if (dlg->exec() == QDialog::Accepted)
    {
        updateBusyStatus(true);
        startAlbumEditing(aid, dlg->album());
    }

    delete dlg;
}

void AlbumChooserWidget::startAlbumEditing(
    int aid, const VkontakteAlbumDialog::AlbumInfo &album)
{
    // Select the same album again in the combobox later (in "slotAlbumsReloadDone()")
    m_albumToSelect                    = aid;

    Vkontakte::EditAlbumJob* const job = new Vkontakte::EditAlbumJob(m_vkapi->accessToken(),
                                                                     aid, album.title, album.description,
                                                                     album.privacy, album.commentPrivacy);

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotAlbumEditingDone(KJob*)));

    job->start();
}

void AlbumChooserWidget::slotAlbumEditingDone(KJob* kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::EditAlbumJob)

    startAlbumsReload();

    updateBusyStatus(true);
}

//------------------------------

void AlbumChooserWidget::slotDeleteAlbumRequest()
{
    VkontakteAlbumDialog::AlbumInfo album;
    int aid = 0;
    if (!getCurrentAlbumInfo(album) || !getCurrentAlbumId(aid))
    {
        return;
    }

    if (KMessageBox::warningContinueCancel(
        this,
        i18n("<qt>Are you sure you want to remove the album <b>%1</b> including all photos in it?</qt>", album.title),
        i18nc("@title:window", "Confirm Album Deletion"),
        KStandardGuiItem::del(),
        KStandardGuiItem::cancel(),
        QString("kipi_vkontakte_delete_album_with_photos")) != KMessageBox::Continue)
    {
        return;
    }

    updateBusyStatus(true);
    startAlbumDeletion(aid);
}

void AlbumChooserWidget::startAlbumDeletion(int aid)
{
    Vkontakte::DeleteAlbumJob* const job = new Vkontakte::DeleteAlbumJob(m_vkapi->accessToken(), aid);

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotAlbumDeletionDone(KJob*)));

    job->start();
}

void AlbumChooserWidget::slotAlbumDeletionDone(KJob* kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::DeleteAlbumJob)

    startAlbumsReload();

    updateBusyStatus(true);
}

//------------------------------

void AlbumChooserWidget::slotReloadAlbumsRequest()
{
    updateBusyStatus(true);

    int aid = 0;
    if (getCurrentAlbumId(aid))
    {
        m_albumToSelect = aid;
    }

    startAlbumsReload();
}

void AlbumChooserWidget::startAlbumsReload()
{
    updateBusyStatus(true);

    Vkontakte::AlbumListJob* const job = new Vkontakte::AlbumListJob(m_vkapi->accessToken());

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotAlbumsReloadDone(KJob*)));

    job->start();
}

void AlbumChooserWidget::slotAlbumsReloadDone(KJob* kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::AlbumListJob)

    if (!job) return;

    m_albumsCombo->clear();
    m_albums = job->list();

    foreach (const Vkontakte::AlbumInfoPtr& album, m_albums)
        m_albumsCombo->addItem(KIcon("folder-image"), album->title());

    if (m_albumToSelect != -1)
    {
        selectAlbum(m_albumToSelect);
        m_albumToSelect = -1;
    }

    m_albumsCombo->setEnabled(true);

    if (!m_albums.empty())
    {
        m_editAlbumButton->setEnabled(true);
        m_deleteAlbumButton->setEnabled(true);
    }

    updateBusyStatus(false);
}

//------------------------------

void AlbumChooserWidget::updateBusyStatus(bool busy)
{
    setEnabled(!busy);
}

// TODO: share this code with `vkwindow.cpp`
void AlbumChooserWidget::handleVkError(KJob* kjob)
{
    KMessageBox::error(
        this,
        kjob == 0 ? i18n("Internal error: Null pointer to KJob instance.") : kjob->errorText(),
        i18nc("@title:window", "Request to VKontakte failed"));
}

} // namespace KIPIVkontaktePlugin
