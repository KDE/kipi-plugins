/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
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

// Qt includes.

#include <qpushbutton.h>
#include <qlabel.h>
#include <q3frame.h>
#include <q3header.h>
#include <q3listview.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qsplitter.h>
#include <q3whatsthis.h>
#include <qlineedit.h>

// KDE includes.

#include <klocale.h>
#include <khtml_part.h>
#include <khtmlview.h>

// Local includes.

#include "picasawebwidget.h"
#include "picasawebwidget.moc"

namespace KIPIPicasawebExportPlugin
{

PicasawebWidget::PicasawebWidget(QWidget* parent)
               : UploadWidget(parent)
{
    setName("PicasawebWidget");

    resize( QSize(600, 400).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );
}

PicasawebWidget::~PicasawebWidget()
{
}

void PicasawebWidget::slotSelectionChecked()
{
//    kDebug()<<"Slot Selection Checked "<<endl;
//    m_addPhotoBtn->setEnabled(m_selectImagesButton->isChecked());
}

void PicasawebWidget::slotResizeChecked()
{
//    m_dimensionSpinBox->setEnabled(m_resizeCheckBox->isChecked());
}

} // namespace KIPIPicasawebExportPlugin
