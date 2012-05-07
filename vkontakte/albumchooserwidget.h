/*
 * A KIPI plugin to export images to VKontakte web service
 * Copyright (C) 2011, 2012
 * Alexander Potashev <aspotashev at gmail dot com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef ALBUMCHOOSERWIDGET_H
#define ALBUMCHOOSERWIDGET_H

#include <QGroupBox>

#include <libkvkontakte/albuminfo.h>

// TODO: share this code with `vkwindow.cpp`
#define SLOT_JOB_DONE_INIT(JobClass) \
    JobClass *job = dynamic_cast<JobClass *>(kjob); \
    Q_ASSERT(job);          \
    if (job->error())       \
    {                       \
        handleVkError(job); \
        return;             \
    }

class KJob;
class QToolButton;
class KPushButton;
class KComboBox;

namespace KIPIVkontaktePlugin
{

class VkAPI;


class AlbumChooserWidget : public QGroupBox
{
    Q_OBJECT

public:

    AlbumChooserWidget(QWidget *parent, VkAPI *vkapi);
    ~AlbumChooserWidget();

    void clearList();

    Vkontakte::AlbumInfoPtr currentAlbum();
    void selectAlbum(int aid);

protected Q_SLOTS:

    void slotNewAlbumRequest();
    void slotEditAlbumRequest();
    void slotDeleteAlbumRequest();
    void slotReloadAlbumsRequest();

    void startAlbumCreation(Vkontakte::AlbumInfoPtr album);
    void startAlbumEditing(Vkontakte::AlbumInfoPtr album);
    void startAlbumDeletion(Vkontakte::AlbumInfoPtr album);
    void startAlbumsReload();

    void slotAlbumCreationDone(KJob *kjob);
    void slotAlbumEditingDone(KJob *kjob);
    void slotAlbumDeletionDone(KJob *kjob);
    void slotAlbumsReloadDone(KJob *kjob);

protected:

    void updateBusyStatus(bool busy);
    void handleVkError(KJob *kjob);

private:

    KComboBox *m_albumsCombo;
    KPushButton *m_newAlbumButton;
    KPushButton *m_reloadAlbumsButton;
    QToolButton *m_editAlbumButton;
    QToolButton *m_deleteAlbumButton;

    QList<Vkontakte::AlbumInfoPtr> m_albums;

    /** album with this "aid" will be selected in slotAlbumsReloadDone() */
    int m_albumToSelect;

    VkAPI *m_vkapi;
};

} // namespace KIPIVkontaktePlugin

#endif // ALBUMCHOOSERWIDGET_H
