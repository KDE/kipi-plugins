/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Dropbox web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#ifndef DBWIDGET_H
#define DBWIDGET_H

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
    class KPProgressWidget;
}

namespace KIPIDropboxPlugin
{

class DropboxWidget : public QWidget
{
    Q_OBJECT

public:

    DropboxWidget(QWidget* const parent);
    ~DropboxWidget();

    void updateLabels(const QString& name = QString(), const QString& url = QString());

    KIPIPlugins::KPImagesList*     imagesList()  const;
    KIPIPlugins::KPProgressWidget* progressBar() const;

private Q_SLOTS:

    void slotResizeChecked();

private:

    KIPIPlugins::KPImagesList*     m_imgList;

    QLabel*                        m_headerLbl;
    QLabel*                        m_userNameDisplayLbl;
    KPushButton*                   m_changeUserBtn;

    KComboBox*                     m_albumsCoB;
    KPushButton*                   m_newAlbumBtn;
    KPushButton*                   m_reloadAlbumsBtn;

    QCheckBox*                     m_resizeChB;
    QSpinBox*                      m_dimensionSpB;
    QSpinBox*                      m_imageQualitySpB;

    KIPIPlugins::KPProgressWidget* m_progressBar;

    friend class DBWindow;
};

} // namespace KIPIDropboxPlugin

#endif /*DBWIDGET_H*/
