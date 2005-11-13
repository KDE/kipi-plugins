/* ============================================================
 * File  : flickrwidget.cpp
 * Author: Vardhman Jain <vardhman @ gmail.com>
 * Date  : 2005-07-07
 * Description :
 *
 * Copyright 2005 by Vardhman Jain

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
#include <qlineedit.h>
#include "flickrwidget.h"

namespace KIPIFlickrExportPlugin
{

FlickrWidget::FlickrWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "FlickrWidget" );
    QVBoxLayout*  flickrWidgetLayout
        = new QVBoxLayout( this, 5, 5, "FlickrWidgetLayout");

    // ------------------------------------------------------------------------

    QLabel*       headerLabel;
    QFrame*       headerLine;

    headerLabel = new QLabel( this, "headerLabel" );
    flickrWidgetLayout->addWidget( headerLabel, 0 );
    headerLine = new QFrame( this, "headerLine" );
    headerLine->setFrameShape( QFrame::HLine );
    headerLine->setFrameShadow( QFrame::Sunken );
    flickrWidgetLayout->addWidget( headerLine, 0 );

    // ------------------------------------------------------------------------

    QSplitter* splitter = new QSplitter(this);
    flickrWidgetLayout->addWidget( splitter, 5 );

    m_tagView = new QListView( splitter, "m_tagView" );
    //m_tagView->addColumn( i18n( "Albums" ) );
    //m_tagView->setResizeMode( QListView::AllColumns );

    // ------------------------------------------------------------------------

    m_photoView = NULL;//new KHTMLPart( splitter, "m_photoView" );

    // ------------------------------------------------------------------------

    QVBoxLayout*  rightButtonGroupLayout;
    QSpacerItem*  spacer;
    QButtonGroup* rightButtonGroup;

    rightButtonGroup = new QButtonGroup( splitter, "rightButtonGroup" );
    rightButtonGroupLayout = new QVBoxLayout( rightButtonGroup );
    rightButtonGroupLayout->setSpacing( 5 );
    rightButtonGroupLayout->setMargin( 5 );

    //m_newAlbumBtn = new QPushButton( rightButtonGroup, "m_newAlbumBtn" );
    //m_newAlbumBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //rightButtonGroupLayout->addWidget( m_newAlbumBtn, 0, Qt::AlignHCenter );

    m_addPhotoBtn = new QPushButton( rightButtonGroup, "m_addPhotoBtn" );
    m_addPhotoBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    rightButtonGroupLayout->addWidget( m_addPhotoBtn, 0, Qt::AlignHCenter );

    QGridLayout* tagsLayout=new QGridLayout(rightButtonGroupLayout,1,2);
    QLabel* tagsLabel=new QLabel(i18n("Tags:"),rightButtonGroup);
    tagsLayout->addWidget(tagsLabel,0,0);
    m_tagsLineEdit=new QLineEdit(rightButtonGroup,"m_tagsLineEdit");
    tagsLayout->addWidget(m_tagsLineEdit,0,1);
    // ------------------------------------------------------------------------

    QGroupBox* optionsBox = new QGroupBox(i18n("Override Default Options"),
                                          rightButtonGroup);
    optionsBox->setColumnLayout(0, Qt::Vertical);
    optionsBox->layout()->setSpacing(5);
    optionsBox->layout()->setMargin(5);
    QGridLayout* optionsBoxLayout = new QGridLayout(optionsBox->layout(),3,3);

    // ------------------------------------------------------------------------


    m_publicCheckBox = new QCheckBox(optionsBox);
    m_publicCheckBox->setText(i18n("As in accessible for people", "Public ?"));
 //  m_publicCheckBox->show();
    optionsBoxLayout->addWidget(m_publicCheckBox, 0, 1);

    m_familyCheckBox = new QCheckBox(optionsBox);
    m_familyCheckBox->setText(i18n("Family ?"));
    optionsBoxLayout->addWidget(m_familyCheckBox, 0,2 );// 0, 0, 0, 3);

    m_friendsCheckBox = new QCheckBox(optionsBox);
    m_friendsCheckBox->setText(i18n("Friends ?"));
   optionsBoxLayout->addWidget(m_friendsCheckBox,0,3);// 0, 0, 0, 4);


    m_resizeCheckBox = new QCheckBox(optionsBox);
    m_resizeCheckBox->setText(i18n("Resize photos before uploading"));
  // m_resizeCheckBox->show();
   optionsBoxLayout->addMultiCellWidget(m_resizeCheckBox, 1,1,0,4,1 );

    m_dimensionSpinBox  = new QSpinBox(0, 5000, 10, optionsBox);
    m_dimensionSpinBox->setValue(600);
    m_dimensionSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    optionsBoxLayout->addWidget(m_dimensionSpinBox, 2,1 );

	QLabel* resizeLabel = new QLabel(i18n("Maximum dimension:"), optionsBox);
    optionsBoxLayout->addWidget(resizeLabel, 2, 0);
    
	m_imageQualitySpinBox  = new QSpinBox(0, 100, 1, optionsBox);
    m_imageQualitySpinBox->setValue(85);
    m_imageQualitySpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    optionsBoxLayout->addWidget(m_imageQualitySpinBox, 3,1 );
    
	QLabel* imageQualityLabel = new QLabel(i18n("Image Quality (higher is better):"), optionsBox);//The term Compression factor may be to technical to write in the label
    optionsBoxLayout->addWidget(imageQualityLabel, 3, 0);

    m_resizeCheckBox->setChecked(false);
    m_dimensionSpinBox->setEnabled(false);
    connect(m_resizeCheckBox, SIGNAL(clicked()), SLOT(slotResizeChecked()));

    // ------------------------------------------------------------------------

    rightButtonGroupLayout->addWidget(optionsBox);

    // ------------------------------------------------------------------------

    spacer = new QSpacerItem( 20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding );
    rightButtonGroupLayout->addItem( spacer );

    // ------------------------------------------------------------------------

    headerLabel->setText( i18n( "<h2>Flickr Export</h2>" ) );
	m_tagView->hide();
//    m_tagView->header()->setLabel( 0, i18n( "Albums" ) );
    //m_newAlbumBtn->setText( i18n( "&New Album" ) );
    m_addPhotoBtn->setText( i18n( "&Add Photos" ) );

    // ------------------------------------------------------------------------

    resize( QSize(600, 400).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

FlickrWidget::~FlickrWidget()
{
}

void FlickrWidget::slotResizeChecked()
{
    m_dimensionSpinBox->setEnabled(m_resizeCheckBox->isChecked());
}

}

#include "flickrwidget.moc"
