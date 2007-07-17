/* ============================================================
 * File  : picasawebwidget.cpp
 * Author: Vardhman Jain <vardhman @ gmail.com>
 * Date  : 2007-07-07
 * Description :
 *
 * Copyright 2007 by Vardhman Jain <vardhman @ gmail.com>

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
 *
 * ============================================================ */

#include <klocale.h>
#include <khtml_part.h>
#include <khtmlview.h>

#include <qpushbutton.h>
#include <qlabel.h>
#include <qframe.h>
#include <qheader.h>
#include <qlistview.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qgroupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qsplitter.h>
#include <qwhatsthis.h>
#include <qlineedit.h>


#include "picasawebwidget.h"

namespace KIPIPicasawebExportPlugin
{

PicasawebWidget::PicasawebWidget( QWidget* parent, const char* name, WFlags fl )
    : UploadWidget( parent, name, fl )
{
    if ( !name )
	    setName( "PicasawebWidget" );
    //resize( QSize(600, 400).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );
}

PicasawebWidget::~PicasawebWidget()
{
}

void PicasawebWidget::slotSelectionChecked(){
//	kdDebug()<<"Slot Selection Checked "<<endl;
//	m_addPhotoBtn->setEnabled(m_selectImagesButton->isChecked());
}

void PicasawebWidget::slotResizeChecked()
{
  //  m_dimensionSpinBox->setEnabled(m_resizeCheckBox->isChecked());
}

}

#include "picasawebwidget.moc"
