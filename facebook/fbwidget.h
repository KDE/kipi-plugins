/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
 *
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

#ifndef FBWIDGET_H
#define FBWIDGET_H

// Qt includes
#include <QWidget>

class QLabel;
class QSpinBox;
class QCheckBox;
class QButtonGroup;
class KComboBox;
class KPushButton;

namespace KIPI
{
    class Interface;
    class UploadWidget;
}

namespace KIPIPlugins
{
    class ImagesList;
}

namespace KIPIFacebookPlugin
{

enum FbDownloadType
{
    FbMyAlbum = 0,
    FbFriendAlbum,
    FbPhotosMe,
    FbPhotosFriend
};

class FbWidget : public QWidget
{
    Q_OBJECT

public:

    FbWidget(QWidget* parent, KIPI::Interface *iface, bool import);
    ~FbWidget();

    QString getDestinationPath();

    void updateLabels(const QString& name = "", const QString& url = "", bool uplPerm = false);

    long long getFriendID();
    long long getAlbumID();

    KIPIPlugins::ImagesList* imagesList() const;

Q_SIGNALS:

    void reloadAlbums(long long userID);

private Q_SLOTS:

    void slotReloadAlbumsRequest();
    void slotDownloadTypeChanged(int dlType);
    void slotFriendsIndexChanged(int index);
    void slotResizeChecked();

private:

    KIPIPlugins::ImagesList* m_imgList;
    KIPI::UploadWidget*      m_uploadWidget;

    QLabel*                  m_headerLbl;
    QLabel*                  m_userNameDisplayLbl;
    QLabel*                  m_permissionLbl;
    KPushButton*             m_changeUserBtn;
    KPushButton*             m_changePermBtn;

    QButtonGroup*            m_dlGrp;
    KComboBox*               m_friendsCoB;
    KComboBox*               m_albumsCoB;
    KPushButton*             m_newAlbumBtn;
    KPushButton*             m_reloadAlbumsBtn;

    QCheckBox*               m_resizeChB;
    QSpinBox*                m_dimensionSpB;
    QSpinBox*                m_imageQualitySpB;

    friend class FbWindow;
};

} // namespace KIPIFacebookPlugin

#endif // FBWIDGET_H
