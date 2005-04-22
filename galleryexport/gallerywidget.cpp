/* ============================================================
 * File  : gallerywidget.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-12-01
 * Description : 
 * 
 * Copyright 2004 by Renchi Raju

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
#include <qgroupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qsplitter.h>
#include <qwhatsthis.h>

#include "gallerywidget.h"

namespace KIPIGalleryExportPlugin
{

GalleryWidget::GalleryWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "GalleryWidget" );
    QVBoxLayout*  galleryWidgetLayout
        = new QVBoxLayout( this, 5, 5, "GalleryWidgetLayout"); 

    // ------------------------------------------------------------------------
    
    QLabel*       headerLabel;
    QFrame*       headerLine;

    headerLabel = new QLabel( this, "headerLabel" );
    galleryWidgetLayout->addWidget( headerLabel, 0 );
    headerLine = new QFrame( this, "headerLine" );
    headerLine->setFrameShape( QFrame::HLine );
    headerLine->setFrameShadow( QFrame::Sunken );
    galleryWidgetLayout->addWidget( headerLine, 0 );

    // ------------------------------------------------------------------------

    QSplitter* splitter = new QSplitter(this);
    galleryWidgetLayout->addWidget( splitter, 5 );

    m_albumView = new QListView( splitter, "m_albumView" );
    m_albumView->addColumn( i18n( "Albums" ) );
    m_albumView->setResizeMode( QListView::AllColumns );

    // ------------------------------------------------------------------------

    m_photoView = new KHTMLPart( splitter, "m_photoView" );

    // ------------------------------------------------------------------------
    
    QVBoxLayout*  rightButtonGroupLayout;
    QSpacerItem*  spacer;
    QButtonGroup* rightButtonGroup;

    rightButtonGroup = new QButtonGroup( splitter, "rightButtonGroup" );
    rightButtonGroupLayout = new QVBoxLayout( rightButtonGroup );
    rightButtonGroupLayout->setSpacing( 5 );
    rightButtonGroupLayout->setMargin( 5 );

    m_newAlbumBtn = new QPushButton( rightButtonGroup, "m_newAlbumBtn" );
    m_newAlbumBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    rightButtonGroupLayout->addWidget( m_newAlbumBtn, 0, Qt::AlignHCenter );

    m_addPhotoBtn = new QPushButton( rightButtonGroup, "m_addPhotoBtn" );
    m_addPhotoBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    rightButtonGroupLayout->addWidget( m_addPhotoBtn, 0, Qt::AlignHCenter );

    // ------------------------------------------------------------------------

    QGroupBox* optionsBox = new QGroupBox(i18n("Override Default Options"),
                                          rightButtonGroup);
    optionsBox->setColumnLayout(0, Qt::Vertical);
    optionsBox->layout()->setSpacing(5);
    optionsBox->layout()->setMargin(5);
    QGridLayout* optionsBoxLayout = new QGridLayout(optionsBox->layout());

    // ------------------------------------------------------------------------

    m_resizeCheckBox = new QCheckBox(optionsBox);
    m_resizeCheckBox->setText(i18n("Resize photos before uploading"));
    optionsBoxLayout->addMultiCellWidget(m_resizeCheckBox, 0, 0, 0, 1);

    m_dimensionSpinBox  = new QSpinBox(0, 5000, 10, optionsBox);
    m_dimensionSpinBox->setValue(600);
    m_dimensionSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    optionsBoxLayout->addWidget(m_dimensionSpinBox, 1, 0);

    QLabel* resizeLabel = new QLabel(i18n("Maximum dimension"), optionsBox);
    optionsBoxLayout->addWidget(resizeLabel, 1, 1);

    m_resizeCheckBox->setChecked(false);
    m_dimensionSpinBox->setEnabled(false);
    connect(m_resizeCheckBox, SIGNAL(clicked()), SLOT(slotResizeChecked()));
    
    // ------------------------------------------------------------------------
    
    rightButtonGroupLayout->addWidget(optionsBox);

    // ------------------------------------------------------------------------
    
    spacer = new QSpacerItem( 20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding );
    rightButtonGroupLayout->addItem( spacer );

    // ------------------------------------------------------------------------
    
    headerLabel->setText( i18n( "<h2>Gallery Export</h2>" ) );
    m_albumView->header()->setLabel( 0, i18n( "Albums" ) );
    m_newAlbumBtn->setText( i18n( "&New Album" ) );
    m_addPhotoBtn->setText( i18n( "&Add Photos" ) );

    // ------------------------------------------------------------------------

    resize( QSize(600, 400).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

GalleryWidget::~GalleryWidget()
{
}

void GalleryWidget::slotResizeChecked()
{
    m_dimensionSpinBox->setEnabled(m_resizeCheckBox->isChecked());    
}

}

#include "gallerywidget.moc"
