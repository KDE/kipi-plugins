/* ============================================================
 * File  : picasawebwidget.h
 * Author: Vardhman Jain <vardhman @ gmail.com>
 * Date  : 2007-16-07
 * Copyright 2007 by Vardhman Jain <vardhman @ gmail.com>
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

#ifndef PICASAWEBWIDGET_H
#define PICASAWEBWIDGET_H

#include <qwidget.h>

#include "uploadwidget.h"

class QListView;
class QPushButton;
class QSpinBox;
class QCheckBox;
class KHTMLPart;
class QLineEdit;
class QRadioButton;
class UploadWidget;
namespace KIPIPicasawebExportPlugin
{

class PicasawebWidget : public UploadWidget
{
    Q_OBJECT
    
public:

    PicasawebWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~PicasawebWidget();

private slots:

    void slotResizeChecked();
    void slotSelectionChecked();

private:

    QListView*    m_tagView;
    KHTMLPart*    m_photoView;
    //QCheckBox*    m_resizeCheckBox;
	QButtonGroup* m_fileSrcButtonGroup;
	QCheckBox*    m_exportApplicationTags;
	friend class PicasawebWindow;
};

}

#endif // PICASAWEBWIDGET_H
