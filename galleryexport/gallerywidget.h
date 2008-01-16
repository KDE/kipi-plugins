/* ============================================================
 * File  : gallerywidget.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-12-01
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#ifndef GALLERYWIDGET_H
#define GALLERYWIDGET_H

#include <qwidget.h>

class QListView;
class QPushButton;
class QSpinBox;
class QCheckBox;
class KHTMLPart;

namespace KIPIGalleryExportPlugin
{

class GalleryWidget : public QWidget
{
    Q_OBJECT
    
public:

    GalleryWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~GalleryWidget();

private slots:

    void slotResizeChecked();

private:

    QListView*    m_albumView;
    KHTMLPart*    m_photoView;
    QPushButton*  m_newAlbumBtn;
    QPushButton*  m_addPhotoBtn;
    QCheckBox*    m_captTitleCheckBox;
    QCheckBox*    m_captDescrCheckBox;
    QCheckBox*    m_resizeCheckBox;
    QSpinBox*     m_dimensionSpinBox;

    friend class GalleryWindow;
};

}

#endif // GALLERYWIDGET_H
