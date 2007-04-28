/* ============================================================
 * File  : flickrwidget.h
 * Author: Vardhman Jain <vardhman @ gmail.com>
 * Date  : 2005-07-07
 * Copyright 2005 by Vardhman Jain <vardhman @ gmail.com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * ============================================================ */

#ifndef FLICKRWIDGET_H
#define FLICKRWIDGET_H

#include <qwidget.h>

class QListView;
class QPushButton;
class QSpinBox;
class QCheckBox;
class KHTMLPart;
class QLineEdit;
class QRadioButton;
namespace KIPIFlickrExportPlugin
{

class FlickrWidget : public QWidget
{
    Q_OBJECT
    
public:

    FlickrWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FlickrWidget();

private slots:

    void slotResizeChecked();
    void slotSelectionChecked();

private:

    QListView*    m_tagView;
    KHTMLPart*    m_photoView;
    //QPushButton*  m_newAlbumBtn;
    QPushButton*  m_addPhotoBtn;
    QCheckBox*    m_resizeCheckBox;
    QCheckBox*    m_familyCheckBox;
    QCheckBox*    m_friendsCheckBox;
    QCheckBox*    m_publicCheckBox;
	QButtonGroup* m_fileSrcButtonGroup;
	QRadioButton* m_currentSelectionButton;
	QRadioButton* m_selectImagesButton;
    QSpinBox*     m_dimensionSpinBox;
    QSpinBox*     m_imageQualitySpinBox;
    QLineEdit*    m_tagsLineEdit;
    QPushButton*  m_changeUserButton;
	QLabel*       m_userNameDisplayLabel;
	QPushButton*  m_startUploadButton;
	QCheckBox*    m_exportApplicationTags;
	friend class FlickrWindow;
};

}

#endif // FLICKRWIDGET_H
