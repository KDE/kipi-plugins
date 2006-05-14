/* ============================================================
 * File  : caltemplate.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-04
 * Description :
 *
 * Copyright 2003 by Renchi Raju
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

// Qt includes.

#include <qvariant.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qfontdatabase.h>
#include <qpaintdevicemetrics.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qprinter.h>

// KDE includes.

#include <klocale.h>
#include <kstandarddirs.h>

// Local includes.

#include "caltemplate.h"
#include "calwidget.h"
#include "calsettings.h"
#include "calpainter.h"

namespace KIPICalendarPlugin
{

CalTemplate::CalTemplate(QWidget* parent, const char* name)
           : QWidget(parent, name)
{
    QGridLayout *mainLayout = new QGridLayout(this, 2, 1, 5, 5);

    // ----------------------------------------------------------------

    QFrame *headerFrame = new QFrame( this );
    headerFrame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    QHBoxLayout* layout = new QHBoxLayout( headerFrame );
    layout->setMargin( 2 ); // to make sure the frame gets displayed
    layout->setSpacing( 0 );
    QLabel *pixmapLabelLeft = new QLabel( headerFrame, "pixmapLabelLeft" );
    pixmapLabelLeft->setScaledContents( false );
    layout->addWidget( pixmapLabelLeft );
    QLabel *labelTitle = new QLabel( i18n("Create Calendar"), headerFrame, "labelTitle" );
    layout->addWidget( labelTitle );
    layout->setStretchFactor( labelTitle, 1 );
    mainLayout->addMultiCellWidget( headerFrame, 0, 0, 0, 1 );

    QString directory;
    KGlobal::dirs()->addResourceType("kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data");
    directory = KGlobal::dirs()->findResourceDir("kipi_banner_left", "banner_left.png");

    pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
    pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
    labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );

    // ----------------------------------------------------------------

    previewSize_ = 300;

    QGroupBox *boxPreview_ = new QGroupBox( i18n("Preview"), this );
    boxPreview_->setColumnLayout(0, Qt::Vertical);
    boxPreview_->layout()->setMargin( 5 );

    QVBoxLayout *previewLayout = new QVBoxLayout(boxPreview_->layout());
    calWidget_ = new CalWidget(boxPreview_);
    previewLayout->addWidget(calWidget_, 0, Qt::AlignCenter);

    mainLayout->addWidget( boxPreview_, 1, 0 );

    // ---------------------------------------------------------------

    QGroupBox *gbox = new QGroupBox( i18n("Settings"), this );
    gbox->setColumnLayout(0, Qt::Vertical );
    gbox->layout()->setSpacing( 11 );
    gbox->layout()->setMargin( 6 );
    QVBoxLayout* gboxLayout = new QVBoxLayout( gbox->layout() );

    // ---------------------------------------------------------------

    QHBoxLayout *hlayout = new QHBoxLayout( 0, 0, 5 );
    hlayout->addWidget( new QLabel(i18n("Paper size:"), gbox) );

    comboPaperSize_ = new QComboBox(false, gbox);
    hlayout->addWidget(comboPaperSize_);
    gboxLayout->addLayout( hlayout );

    QStringList paperSizes;
    paperSizes << "A4";
    paperSizes << "US Letter";
    comboPaperSize_->insertStringList(paperSizes);

    connect(comboPaperSize_, SIGNAL(activated(int)),
            SLOT(slotParamsChanged()));

    // ---------------------------------------------------------------

    btnGroupImagePos_ = new QVButtonGroup(i18n("Image Position"), gbox);
    btnGroupImagePos_->setRadioButtonExclusive(true);

    // Insert the buttons in the order Top, Left, Right so that they
    // get the corresponding ids
    QRadioButton* radioBtn = new QRadioButton(i18n("Top"), btnGroupImagePos_);
    radioBtn->setChecked(true);
    Q_ASSERT(btnGroupImagePos_->id( radioBtn ) == CalParams::Top);

    radioBtn = new QRadioButton(i18n("Left"), btnGroupImagePos_);
    Q_ASSERT(btnGroupImagePos_->id( radioBtn ) == CalParams::Left);

    radioBtn = new QRadioButton(i18n("Right"), btnGroupImagePos_);
    Q_ASSERT(btnGroupImagePos_->id( radioBtn ) == CalParams::Right);

    gboxLayout->addWidget( btnGroupImagePos_ );

    connect(btnGroupImagePos_, SIGNAL(clicked(int)),
            SLOT(slotParamsChanged()));

    // ---------------------------------------------------------------

    checkBoxDrawLines_ = new QCheckBox(i18n("Draw lines in calendar"), gbox);
    gboxLayout->addWidget( checkBoxDrawLines_ );
    checkBoxDrawLines_->setChecked(true);

    connect(checkBoxDrawLines_, SIGNAL(toggled(bool)),
            SLOT(slotParamsChanged()));

    // ---------------------------------------------------------------

    hlayout = new QHBoxLayout( 0, 0, 5 );

    hlayout->addWidget(new QLabel(i18n("Image to text ratio:"), gbox));

    sliderRatio_ = new QSlider(50,300,5,100,Qt::Horizontal,gbox);
    hlayout->addWidget( sliderRatio_ );

    gboxLayout->addLayout( hlayout );

    connect(sliderRatio_, SIGNAL(valueChanged(int)),
            SLOT(slotParamsChanged()));

    // ---------------------------------------------------------------

    hlayout = new QHBoxLayout( 0, 0, 5 );

    hlayout->addWidget(new QLabel(i18n("Font:"), gbox));

    comboFont_ = new QComboBox(false, gbox);
    hlayout->addWidget( comboFont_ );

    QFontDatabase fontDB;
    QStringList families(fontDB.families());
    QStringList smoothScalableFamilies;
    for (QStringList::iterator it=families.begin(); it != families.end();
         ++it)
    {
        smoothScalableFamilies.append(*it);
    }
    comboFont_->insertStringList(smoothScalableFamilies);

    // fetch and set the default font selected in the combo.
    QFont f;
    comboFont_->setCurrentText( f.family() );


    gboxLayout->addLayout( hlayout );

    connect(comboFont_, SIGNAL(activated(int)),
            SLOT(slotParamsChanged()));


    // ---------------------------------------------------------------

    gboxLayout->addItem(new QSpacerItem(5,10,QSizePolicy::Minimum,
                                        QSizePolicy::Expanding));

    mainLayout->addWidget( gbox, 1, 1 );

    // ---------------------------------------------------------------

    timer_ = new QTimer(this);

    connect(timer_, SIGNAL(timeout()),
            SLOT(slotUpdatePreview()));

    timer_->start(0,true);
}

CalTemplate::~CalTemplate()
{
    delete timer_;
}


void CalTemplate::slotParamsChanged()
{
    timer_->start(10, true);
}

void CalTemplate::slotUpdatePreview()
{
    if (timer_->isActive()) return;

    CalParams& params = CalSettings::instance()->calParams;

    QString paperSize = comboPaperSize_->currentText();
    if (paperSize == "A4") {
        params.paperWidth  = 210;
        params.paperHeight = 297;
        params.pageSize    = KPrinter::A4;
    }
    else if (paperSize == "US Letter") {
        params.paperWidth  = 216;
        params.paperHeight = 279;
        params.pageSize    = KPrinter::Letter;
    }

    const int imgPos = btnGroupImagePos_->selectedId();

    if (imgPos == CalParams::Top) {
        params.imgPos = CalParams::Top;

        float zoom = QMIN((float)previewSize_/params.paperWidth,
                          (float)previewSize_/params.paperHeight);
        params.width  = (int)(params.paperWidth  * zoom);
        params.height = (int)(params.paperHeight * zoom);

    }
    else if (imgPos == CalParams::Left) {
        params.imgPos = CalParams::Left;

        float zoom = QMIN((float)previewSize_/params.paperWidth,
                          (float)previewSize_/params.paperHeight);
        params.width  = (int)(params.paperHeight  * zoom);
        params.height = (int)(params.paperWidth   * zoom);

    }
    else {
        params.imgPos = CalParams::Right;

        float zoom = QMIN((float)previewSize_/params.paperWidth,
                          (float)previewSize_/params.paperHeight);
        params.width  = (int)(params.paperHeight  * zoom);
        params.height = (int)(params.paperWidth   * zoom);
    }

    params.drawLines = checkBoxDrawLines_->isChecked();
    params.ratio     = sliderRatio_->value();
    params.baseFont  = QFont(comboFont_->currentText());
    calWidget_->recreate();
}

}  // NameSpace KIPICalendarPlugin

#include "caltemplate.moc"
