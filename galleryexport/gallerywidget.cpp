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
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include "gallerywidget.h"

namespace KIPIGalleryExportPlugin
{

GalleryWidget::GalleryWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "GalleryWidget" );
    QVBoxLayout*  GalleryWidgetLayout
        = new QVBoxLayout( this, 5, 5, "GalleryWidgetLayout"); 

    QLabel*       headerLabel;
    QFrame*       headerLine;

    headerLabel = new QLabel( this, "headerLabel" );
    GalleryWidgetLayout->addWidget( headerLabel );
    headerLine = new QFrame( this, "headerLine" );
    headerLine->setFrameShape( QFrame::HLine );
    headerLine->setFrameShadow( QFrame::Sunken );
    GalleryWidgetLayout->addWidget( headerLine );

    QHBoxLayout*  hbox = new QHBoxLayout( 0, 0, 5, "hbox"); 

    m_albumView = new QListView( this, "m_albumView" );
    m_albumView->addColumn( i18n( "Albums" ) );
    m_albumView->setResizeMode( QListView::AllColumns );
    hbox->addWidget( m_albumView );

    m_photoView = new KHTMLPart( this, "m_photoView" );
    hbox->addWidget( m_photoView->view() );

    QVBoxLayout*  rightButtonGroupLayout;
    QSpacerItem*  spacer;
    QButtonGroup* rightButtonGroup;

    rightButtonGroup = new QButtonGroup( this, "rightButtonGroup" );
    rightButtonGroup->setColumnLayout(0, Qt::Vertical );
    rightButtonGroup->layout()->setSpacing( 5 );
    rightButtonGroup->layout()->setMargin( 5 );
    rightButtonGroupLayout = new QVBoxLayout( rightButtonGroup->layout() );
    rightButtonGroupLayout->setAlignment( Qt::AlignTop );

    m_newAlbumBtn = new QPushButton( rightButtonGroup, "m_newAlbumBtn" );
    rightButtonGroupLayout->addWidget( m_newAlbumBtn );

    m_addPhotoBtn = new QPushButton( rightButtonGroup, "m_addPhotoBtn" );
    rightButtonGroupLayout->addWidget( m_addPhotoBtn );
    spacer = new QSpacerItem( 20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding );
    rightButtonGroupLayout->addItem( spacer );
    hbox->addWidget( rightButtonGroup );

    GalleryWidgetLayout->addLayout( hbox );

    headerLabel->setText( i18n( "<h2>Gallery Export</h2>" ) );
    m_albumView->header()->setLabel( 0, i18n( "Albums" ) );
    m_newAlbumBtn->setText( i18n( "&New Album" ) );
    m_addPhotoBtn->setText( i18n( "&Add Photos" ) );

    resize( QSize(600, 400).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

GalleryWidget::~GalleryWidget()
{
}

}
