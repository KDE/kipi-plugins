//////////////////////////////////////////////////////////////////////////////
//
//    RENAMEIMAGESOPTIONSDIALOG.CPP
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

// Include files for Qt

#include <qvbox.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qwhatsthis.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>

// Include files for KDE

#include <klocale.h>
#include <knuminput.h>
#include <klineedit.h>
#include <kdatewidget.h>

// Local includes

#include "renameimagesoptionsdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

RenameImagesOptionsDialog::RenameImagesOptionsDialog(QWidget *parent)
                        : KDialogBase( parent, "RenameImagesOptionsDialog", true,
                          i18n("Rename Image file options"), Ok|Cancel, Ok, false)
{
    QWidget* box = new QWidget( this );
    setMainWidget(box);
    QVBoxLayout *dvlay = new QVBoxLayout( box, 10, spacingHint() );
    QString whatsThis;

    //---------------------------------------------

    QGroupBox * groupBox1 = new QGroupBox( 2, Qt::Horizontal, i18n("Template to use for renaming images"), box );

    m_labelPrefix = new QLabel( i18n("Prefix string:"), groupBox1);
    m_prefix = new KLineEdit(groupBox1);
    QWhatsThis::add( m_prefix, i18n("<p>Enter here the prefix numeric enumerator string.") );

    m_labelStart = new QLabel( i18n("Numeric enumerator start value:"), groupBox1);
    m_enumeratorStart = new KIntSpinBox( 1, 100000, 1, 1, 10, groupBox1);
    QWhatsThis::add( m_enumeratorStart, i18n("<p>Enter here the first numeric enumerator value (1 to 100000).") );

    m_addOriginalFileName = new QCheckBox(i18n("Add original file name"), groupBox1);
    QWhatsThis::add( m_addOriginalFileName, i18n("<p>If you enable this option, "
                     "the original file name will added in the new file name.") );
    m_addOriginalFileName->setChecked( false );

    m_addImageFileDate = new QCheckBox(i18n("Add file date"), groupBox1);
    QWhatsThis::add( m_addImageFileDate, i18n("<p>If you enable this option, "
                     "the last modified image file date will added in the new file name (format: DDMMYYYY).") );
    m_addImageFileDate->setChecked( false );

    m_addImageFileSize = new QCheckBox(i18n("Add file size"), groupBox1);
    QWhatsThis::add( m_addImageFileSize, i18n("<p>If you enable this option, "
                     "the image file size will added in the new file name.") );
    m_addImageFileSize->setChecked( false );

    dvlay->addWidget( groupBox1 );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( 2, Qt::Horizontal, i18n("File list order"), box );

    m_labelSort = new QLabel (i18n("Sort original list:"), groupBox2);
    m_sortType = new QComboBox( false, groupBox2 );
    m_sortType->insertItem(i18n("by File Name"));
    m_sortType->insertItem(i18n("by File Size"));
    m_sortType->insertItem(i18n("by Modification Date"));
    m_sortType->setCurrentText (i18n("by File Name"));
    QWhatsThis::add( m_sortType, i18n("<p>Select here the original file list sorting method.") );

    m_reverseOrder = new QCheckBox(i18n("Reverse order"), groupBox2);
    QWhatsThis::add( m_reverseOrder, i18n("<p>If you enable this option, "
                     "the order of images files in the list will be reversed.") );
    m_reverseOrder->setChecked( false );

    dvlay->addWidget( groupBox2 );

    //---------------------------------------------

    QGroupBox * groupBox3 = new QGroupBox( 1, Qt::Horizontal, i18n("Files' dates"), box );

    m_dateChange = new QCheckBox( i18n("Change date of image files"), groupBox3);
    QWhatsThis::add( m_dateChange, i18n("<p>If you enable this option, "
                                        "the image files' dates can be changed.") );
    m_dateChange->setChecked( false );

    m_kDatePicker = new KDateWidget(  QDate::currentDate(), groupBox3 );
    m_kDatePicker->setEnabled( false );
    QWhatsThis::add( m_kDatePicker, i18n("<p>You can set here the time stamp of the image files.") );

    connect( m_dateChange, SIGNAL( toggled(bool) ), m_kDatePicker, SLOT( setEnabled(bool) ) );

    dvlay->addWidget( groupBox3 );
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

RenameImagesOptionsDialog::~RenameImagesOptionsDialog()
{
}

}  // NameSpace KIPIBatchProcessImagesPlugin

#include "renameimagesoptionsdialog.moc"
