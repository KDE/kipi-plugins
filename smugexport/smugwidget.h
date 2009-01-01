/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to export images to SmugMug web service
 *
 * Copyright (C) 2008 by Luka Renko <lure at kubuntu dot org>
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

#ifndef SMUGWIDGET_H
#define SMUGWIDGET_H

// Qt includes.
#include <QWidget>

class QLabel;
class QSpinBox;
class QCheckBox;
class QComboBox;
class QPushButton;

namespace KIPI
{
    class Interface;
}

namespace KIPIPlugins
{
    class ImagesList;
}

namespace KIPISmugExportPlugin
{

class SmugWidget : public QWidget
{
    Q_OBJECT

public:
    SmugWidget(QWidget* parent, KIPI::Interface *iface);
    ~SmugWidget();

    void updateLabels(const QString& email, const QString& name, const QString& nick);

private slots:
    void slotResizeChecked();

private:

    KIPIPlugins::ImagesList*   m_imgList;

    QLabel*       m_headerLbl;
    QLabel*       m_userNameDisplayLbl;
    QLabel*       m_emailLbl;
    QPushButton*  m_changeUserBtn;

    QComboBox*    m_albumsCoB;
    QPushButton*  m_newAlbumBtn;
    QPushButton*  m_reloadAlbumsBtn;

    QCheckBox*    m_resizeChB;
    QSpinBox*     m_dimensionSpB;
    QSpinBox*     m_imageQualitySpB;

    friend class SmugWindow;
};

} // namespace KIPISmugExportPlugin

#endif // SMUGWIDGET_H
