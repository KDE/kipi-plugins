/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
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

//local includes

#include "kpsettingswidget.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QButtonGroup;
class QComboBox;
class QPushButton;

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

using namespace KIPIPlugins;

namespace KIPIFacebookPlugin
{

class FbWidget : public KPSettingsWidget
{
    Q_OBJECT

public:

    FbWidget(QWidget* const parent, KIPI::Interface* const iface, const QString& pluginName);
    ~FbWidget();

    virtual void updateLabels(const QString& name = QString(), const QString& url = QString());

//     QString   getAlbumID()         const;
//     QString   getDestinationPath() const;
/*
    KIPIPlugins::KPImagesList* imagesList()      const;
    KIPIPlugins::KPProgressWidget* progressBar() const;*/

Q_SIGNALS:

    void reloadAlbums(long long userID);

private Q_SLOTS:

    void slotReloadAlbumsRequest();
//     void slotResizeChecked();

private:

//     KIPIPlugins::KPImagesList*     m_imgList;
//     KIPI::UploadWidget*            m_uploadWidget;
//
//     QLabel*                        m_headerLbl;
//     QLabel*                        m_userNameDisplayLbl;
//     QLabel*                        m_permissionLbl;
//     QPushButton*                   m_changeUserBtn;
//
//     QButtonGroup*                  m_dlGrp;
//     QComboBox*                     m_albumsCoB;
//     QPushButton*                   m_newAlbumBtn;
//     QPushButton*                   m_reloadAlbumsBtn;
//
//     QCheckBox*                     m_resizeChB;
//     QSpinBox*                      m_dimensionSpB;
//     QSpinBox*                      m_imageQualitySpB;

//     KIPIPlugins::KPProgressWidget* m_progressBar;

    friend class FbWindow;
};

} // namespace KIPIFacebookPlugin

#endif // FBWIDGET_H
