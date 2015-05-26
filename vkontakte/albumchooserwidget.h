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

#ifndef ALBUMCHOOSERWIDGET_H
#define ALBUMCHOOSERWIDGET_H

// Qt includes

#include <QString>
#include <QGroupBox>

// LibKvkontakte includes

#include <libkvkontakte/albuminfo.h>

// Local includes

#include "vkalbumdialog.h"

// TODO: share this code with `vkwindow.cpp`
#define SLOT_JOB_DONE_INIT(JobClass)                     \
    JobClass* const job = dynamic_cast<JobClass*>(kjob); \
    Q_ASSERT(job);                                       \
    if (job && job->error())                             \
    {                                                    \
        handleVkError(job);                              \
        return;                                          \
    }

class KJob;
class QToolButton;
class KPushButton;
class KComboBox;

namespace Vkontakte
{
    class VkApi;
}

namespace KIPIVkontaktePlugin
{

class AlbumChooserWidget : public QGroupBox
{
    Q_OBJECT

public:

    AlbumChooserWidget(QWidget* const parent, Vkontakte::VkApi* const vkapi);
    ~AlbumChooserWidget();

    void clearList();

    bool getCurrentAlbumInfo(VkontakteAlbumDialog::AlbumInfo &out);
    bool getCurrentAlbumId(int &out);
    void selectAlbum(int aid);

protected Q_SLOTS:

    void slotNewAlbumRequest();
    void slotEditAlbumRequest();
    void slotDeleteAlbumRequest();
    void slotReloadAlbumsRequest();

    void startAlbumCreation(const VkontakteAlbumDialog::AlbumInfo& album);
    void startAlbumEditing(int aid, const VkontakteAlbumDialog::AlbumInfo& album);
    void startAlbumDeletion(int aid);
    void startAlbumsReload();

    void slotAlbumCreationDone(KJob *kjob);
    void slotAlbumEditingDone(KJob *kjob);
    void slotAlbumDeletionDone(KJob *kjob);
    void slotAlbumsReloadDone(KJob *kjob);

protected:

    void updateBusyStatus(bool busy);
    void handleVkError(KJob *kjob);

private:

    KComboBox*                     m_albumsCombo;
    KPushButton*                   m_newAlbumButton;
    KPushButton*                   m_reloadAlbumsButton;
    QToolButton*                   m_editAlbumButton;
    QToolButton*                   m_deleteAlbumButton;

    QList<Vkontakte::AlbumInfoPtr> m_albums;

    /** album with this "aid" will be selected in slotAlbumsReloadDone() */
    int                            m_albumToSelect;

    Vkontakte::VkApi*              m_vkapi;
};

} // namespace KIPIVkontaktePlugin

#endif // ALBUMCHOOSERWIDGET_H
