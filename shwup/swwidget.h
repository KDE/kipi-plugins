/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2009      by Timothée Groleau <kde at timotheegroleau dot com>
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

#ifndef SWWIDGET_H
#define SWWIDGET_H

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
    class KPImagesList;
}

namespace KIPIShwupPlugin
{

class SwWidget : public QWidget
{
    Q_OBJECT

public:

    SwWidget(QWidget* const parent, KIPI::Interface* const iface);
    ~SwWidget();

    QString getDestinationPath() const;

    void updateLabels(const QString& name = "", const QString& url = "");

    long long getAlbumID() const;

    KIPIPlugins::KPImagesList* imagesList() const;

Q_SIGNALS:

    void reloadAlbums();

private Q_SLOTS:

    void slotReloadAlbumsRequest();
    void slotResizeChecked();

private:

    KIPIPlugins::KPImagesList* m_imgList;
    KIPI::UploadWidget*        m_uploadWidget;

    QLabel*                    m_headerLbl;
    QLabel*                    m_userNameDisplayLbl;
    KPushButton*               m_changeUserBtn;

    KComboBox*                 m_albumsCoB;
    KPushButton*               m_newAlbumBtn;
    KPushButton*               m_reloadAlbumsBtn;

    QCheckBox*                 m_resizeChB;
    QSpinBox*                  m_dimensionSpB;
    QSpinBox*                  m_imageQualitySpB;

    friend class SwWindow;
};

} // namespace KIPIShwupPlugin

#endif // SWWIDGET_H
