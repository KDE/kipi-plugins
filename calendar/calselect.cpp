/* ============================================================
 * File  : calselect.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-03
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
 
#include <qhgroupbox.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qframe.h>
#include <qpixmap.h>

// KDE includes.

#include <klocale.h>
#include <kstandarddirs.h>

// Local includes.

#include "calselect.h"
#include "calsettings.h"
#include "monthwidget.h"

namespace KIPICalendarPlugin
{

CalSelect::CalSelect(KIPI::Interface* interface, QWidget *parent, const char* name)
         : QWidget(parent, name)
{
    mwVector_ = new QPtrVector<MonthWidget>(12);
    setupView( interface );
}

CalSelect::~CalSelect()
{
    delete mwVector_;
}

void CalSelect::setupView( KIPI::Interface* interface )
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this, 6, 11);

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
    mainLayout->addWidget( headerFrame );
    
    QString directory;
    KGlobal::dirs()->addResourceType("kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data");
    directory = KGlobal::dirs()->findResourceDir("kipi_banner_left", "banner_left.png");
    
    pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
    pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
    labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );
    
    // ----------------------------------------------------------------

    QHGroupBox *yearBox = new QHGroupBox(i18n("Select Year"), this);
    yearBox->layout()->addItem(new QSpacerItem(5,5,
                                               QSizePolicy::Expanding,
                                               QSizePolicy::Minimum));
    yearSpin_ = new QSpinBox(1900,3000,1,yearBox);
    yearSpin_->setValue(QDate::currentDate().year());
    slotYearChanged(yearSpin_->value());

    connect(yearSpin_, SIGNAL(valueChanged(int)),
            SLOT(slotYearChanged(int)));

    mainLayout->addWidget(yearBox);

    // ----------------------------------------------------------------

    QGroupBox *monthBox =
        new QGroupBox(i18n("Select Images"),
                      this);
    monthBox->setColumnLayout(0, Qt::Vertical );
    monthBox->layout()->setSpacing( 6 );
    monthBox->layout()->setMargin( 11 );

    QGridLayout *monthBoxLayout = new QGridLayout(monthBox->layout());
    monthBoxLayout->setAlignment( Qt::AlignCenter );

    MonthWidget *w;

    int index = 0;
    for (int i=0; i<2; i++) {
        for (int j=0; j<6; j++) {
            w = new MonthWidget( interface, monthBox,index+1);
            mwVector_->insert(index,w);
            monthBoxLayout->addWidget(w, i, j);
            index++;
        }
    }

    QLabel* tLabel =
        new QLabel(i18n("Left click on Months to Select Images. "
                        "Right click to Clear Month.\n"
                        "You can also drag and drop images onto the Months"),
                                monthBox);

    monthBoxLayout->addMultiCellWidget(tLabel, 2, 2, 0, 5);

    mainLayout->addWidget(monthBox);

    // ----------------------------------------------------------------

    mainLayout->addItem(new QSpacerItem(5,5,QSizePolicy::Minimum,
                                        QSizePolicy::Expanding));
}

void CalSelect::slotYearChanged(int year)
{
    CalSettings::instance()->setYear(year);
}

}  // NameSpace KIPICalendarPlugin

#include "calselect.moc"
