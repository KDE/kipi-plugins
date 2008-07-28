/* ============================================================
 * File  : gallerywidget.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-12-01
 * Description :
 *
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *
 * Modified by : Andrea Diamantini <adjam7@gmail.com>
 * Date        : 2008-07-11
 * Copyright 2008 by Andrea Diamantini <adjam7@gmail.com>
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
 *
 * ============================================================ */

// KDE includes
#include <KLocale>
#include <khtml_part.h>
#include <khtmlview.h>

// Qt includes
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QListWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QSplitter>
#include <QButtonGroup>
#include <QGroupBox>
#include <QWhatsThis>
#include <QGridLayout>
#include <QVBoxLayout>

// local includes
#include "gallerywidget.h"


namespace KIPIGalleryExportPlugin
{

GalleryWidget::GalleryWidget(QWidget* parent, const char* name, Qt::WFlags fl)
        : QWidget(parent, fl)
{
// FIXME     if ( !name )
//         setName( "GalleryWidget" );
    QVBoxLayout*  galleryWidgetLayout
    = new QVBoxLayout(this);   //, 5, 5, "GalleryWidgetLayout");

    // ------------------------------------------------------------------------

    QLabel*       headerLabel;
    QFrame*       headerLine;

    headerLabel = new QLabel("headerLabel" , this);
    galleryWidgetLayout->addWidget(headerLabel, 0);
    headerLine = new QFrame(this);
    headerLine->setFrameShape(QFrame::HLine);
    headerLine->setFrameShadow(QFrame::Sunken);
    galleryWidgetLayout->addWidget(headerLine, 0);

    // ------------------------------------------------------------------------

    QSplitter* splitter = new QSplitter(this);
    galleryWidgetLayout->addWidget(splitter, 5);

    m_albumView = new QListWidget(splitter);   // FIXME , "m_albumView" );
    m_albumView->addItem(i18n("Albums"));
    // m_albumView->setResizeMode( QListWidget::AllColumns );

    // ------------------------------------------------------------------------

    m_photoView = new KHTMLPart(); // FIXME splitter, "m_photoView" );

    // ------------------------------------------------------------------------

    QVBoxLayout*  rightButtonGroupLayout;
    QSpacerItem*  spacer;
    QButtonGroup* rightButtonGroup;

    rightButtonGroup = new QButtonGroup(splitter);   //, "rightButtonGroup" );
    rightButtonGroupLayout = new QVBoxLayout(); // rightButtonGroup );
    rightButtonGroupLayout->setSpacing(5);
    rightButtonGroupLayout->setMargin(5);

    m_newAlbumBtn = new QPushButton("m_newAlbumBtn");
    m_newAlbumBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    rightButtonGroupLayout->addWidget(m_newAlbumBtn, 0, Qt::AlignHCenter);

    m_addPhotoBtn = new QPushButton("m_addPhotoBtn");
    m_addPhotoBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    rightButtonGroupLayout->addWidget(m_addPhotoBtn, 0, Qt::AlignHCenter);

    // ------------------------------------------------------------------------

    QGroupBox* optionsBox = new QGroupBox(i18n("Override Default Options")); //   rightButtonGroup);
    //optionsBox->setColumnLayout(0, Qt::Vertical);
    optionsBox->layout()->setSpacing(5);
    optionsBox->layout()->setMargin(5);
    QGridLayout* optionsBoxLayout = new QGridLayout(this);   //optionsBox->layout());

    // ------------------------------------------------------------------------
    m_captTitleCheckBox = new QCheckBox(optionsBox);
    m_captTitleCheckBox->setText(i18n("Comment sets Title"));
    optionsBoxLayout->addWidget(m_captTitleCheckBox, 0, 0, 0, 1);

    m_captDescrCheckBox = new QCheckBox(optionsBox);
    m_captDescrCheckBox->setText(i18n("Comment sets Description"));
    optionsBoxLayout->addWidget(m_captDescrCheckBox, 1, 1, 0, 1);

    m_resizeCheckBox = new QCheckBox(optionsBox);
    m_resizeCheckBox->setText(i18n("Resize photos before uploading"));
    optionsBoxLayout->addWidget(m_resizeCheckBox, 2, 2, 0, 1);

    m_dimensionSpinBox  = new QSpinBox(this);   //0, 5000, 10, optionsBox);
    m_dimensionSpinBox->setValue(600);
    m_dimensionSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    optionsBoxLayout->addWidget(m_dimensionSpinBox, 3, 1);

    QLabel* resizeLabel = new QLabel(i18n("Maximum dimension:"), optionsBox);
    optionsBoxLayout->addWidget(resizeLabel, 3, 0);

    m_captTitleCheckBox->setChecked(true);
    m_captDescrCheckBox->setChecked(false);

    m_resizeCheckBox->setChecked(false);
    m_dimensionSpinBox->setEnabled(false);
    connect(m_resizeCheckBox, SIGNAL(clicked()), SLOT(slotResizeChecked()));

    // ------------------------------------------------------------------------

    rightButtonGroupLayout->addWidget(optionsBox);

    // ------------------------------------------------------------------------

    spacer = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding);
    rightButtonGroupLayout->addItem(spacer);

    // ------------------------------------------------------------------------

    headerLabel->setText(i18n("<h2>Gallery Export</h2>"));
    // FIXME m_albumView->header()->setLabel( 0, i18n( "Albums" ) );
    m_newAlbumBtn->setText(i18n("&New Album"));
    m_addPhotoBtn->setText(i18n("&Add Photos"));

    // ------------------------------------------------------------------------

    resize(QSize(600, 400).expandedTo(minimumSizeHint()));
    // FIXME clearWState( WState_Polished );
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
