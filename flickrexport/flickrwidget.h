/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef FLICKRWIDGET_H
#define FLICKRWIDGET_H

// Qt includes.

#include <qwidget.h>

class QListView;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QLineEdit;
class QRadioButton;

class KHTMLPart;

namespace KIPIFlickrExportPlugin
{

class FlickrWidget : public QWidget
{
    Q_OBJECT

public:

    FlickrWidget(QWidget* parent=0);
    ~FlickrWidget();

private slots:

    void slotResizeChecked();
    void slotSelectionChecked();
    void slotOpenUrl(const QString&);

private:

    QListView*    m_tagView;

    //QPushButton*  m_newAlbumBtn;
    QPushButton*  m_addPhotoButton;
    QPushButton*  m_changeUserButton;

    QCheckBox*    m_resizeCheckBox;
    QCheckBox*    m_familyCheckBox;
    QCheckBox*    m_friendsCheckBox;
    QCheckBox*    m_publicCheckBox;
    QCheckBox*    m_exportHostTagsCheckBox;

    QButtonGroup* m_fileSrcButtonGroup;

    QRadioButton* m_currentSelectionButton;
    QRadioButton* m_selectImagesButton;

    QSpinBox*     m_dimensionSpinBox;
    QSpinBox*     m_imageQualitySpinBox;

    QLineEdit*    m_tagsLineEdit;

    QLabel*       m_userNameDisplayLabel;

    KHTMLPart*    m_photoView;

    friend class FlickrWindow;
};

} // namespace KIPIFlickrExportPlugin

#endif // FLICKRWIDGET_H
