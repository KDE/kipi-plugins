/* ============================================================
 * File  : calselect.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-03
 * Description :
 *
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <kstandarddirs.h>

// Local includes.

#include "calselect.h"
#include "calsettings.h"
#include "monthwidget.h"

#define MAX_MONTHS (13)

namespace KIPICalendarPlugin
{

CalSelect::CalSelect(KIPI::Interface* interface, QWidget *parent, const char* name)
         : QWidget(parent, name)
{
    mwVector_ = new QPtrVector<MonthWidget>(MAX_MONTHS);
    monthBoxLayout_ = NULL;
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

    setCaption(i18n("Create Calendar"));
    QHGroupBox *yearBox = new QHGroupBox(i18n("Select Year"), this);
    yearBox->layout()->addItem(new QSpacerItem(5,5,
                                               QSizePolicy::Expanding,
                                               QSizePolicy::Minimum));
    yearSpin_ = new QSpinBox(KGlobal::locale()->calendar()->minValidYear(),
                             KGlobal::locale()->calendar()->maxValidYear(),
                             1,yearBox);
    yearSpin_->setValue(KGlobal::locale()->calendar()->year(QDate::currentDate()));
    slotYearChanged(yearSpin_->value());

    connect(yearSpin_, SIGNAL(valueChanged(int)),
            SLOT(slotYearChanged(int)));

    mainLayout->addWidget(yearBox);

    // ----------------------------------------------------------------

    QGroupBox *monthBox = new QGroupBox(i18n("Select Images"), this);
    monthBox->setColumnLayout(0, Qt::Vertical );
    monthBox->layout()->setSpacing( 6 );
    monthBox->layout()->setMargin( 11 );

    monthBoxLayout_ = new QGridLayout(monthBox->layout());
    monthBoxLayout_->setAlignment( Qt::AlignCenter );

    KURL::List urlList;
    KIPI::ImageCollection images = interface->currentSelection();
    if ( images.isValid() && !images.images().isEmpty())
        urlList = images.images();

    QDate d;
    KGlobal::locale()->calendar()->setYMD(d, yearSpin_->value(), 1, 1);
    unsigned int months = KGlobal::locale()->calendar()->monthsInYear(d);
    // span the monthWidgets over 2 rows. inRow should usually be 6 or 7 (for 12 or 13 months)
    int inRow = (months / 2) + ((months % 2) != 0);
    MonthWidget *w;

    for (unsigned int i=0; i<MAX_MONTHS; i++) {
        w = new MonthWidget( interface, monthBox, i+1);
        if (i < urlList.count())
            w->setImage(urlList[i]);
        if (i<months)
            monthBoxLayout_->addWidget(w, i / inRow, i % inRow);
        else
            w->hide();
        mwVector_->insert(i, w);
    }

    QLabel* tLabel =
        new QLabel(i18n("Left click on Months to Select Images. "
                        "Right click to Clear Month.\n"
                        "You can also drag and drop images onto the Months"),
                                monthBox);

    monthBoxLayout_->addMultiCellWidget(tLabel, 2, 2, 0, 5);

    mainLayout->addWidget(monthBox);

    // ----------------------------------------------------------------

    mainLayout->addItem(new QSpacerItem(5,5,QSizePolicy::Minimum,
                                        QSizePolicy::Expanding));
}

void CalSelect::slotYearChanged(int year)
{
    int i, months;
    QDate d, oldD;
    KGlobal::locale()->calendar()->setYMD(d, year, 1, 1);
    KGlobal::locale()->calendar()->setYMD(oldD, CalSettings::instance()->getYear(), 1, 1);
    months = KGlobal::locale()->calendar()->monthsInYear(d);

    if ((KGlobal::locale()->calendar()->monthsInYear(oldD) != months) && !mwVector_->isEmpty())
    {
        // hide the last months that are not present on the current year
        for (i=months; (i<KGlobal::locale()->calendar()->monthsInYear(oldD)) && (i<(int)mwVector_->count()); i++)
            mwVector_->at(i)->hide();

        // span the monthWidgets over 2 rows. inRow should usually be 6 or 7 (for 12 or 13 months)
        int inRow = (months / 2) + ((months % 2) != 0);
        // remove all the monthWidgets, then readd the needed ones
        for (i=0; i<KGlobal::locale()->calendar()->monthsInYear(oldD); i++) {
            monthBoxLayout_->remove(mwVector_->at(i));
        }
        for (i=0; (i<months) && (i<(int)mwVector_->count()); i++) {
            monthBoxLayout_->addWidget(mwVector_->at(i), i / inRow, i % inRow);
            if (mwVector_->at(i)->isHidden())
                mwVector_->at(i)->show();
            mwVector_->at(i)->update();
        }
    }
    CalSettings::instance()->setYear(year);
}

}  // NameSpace KIPICalendarPlugin

#include "calselect.moc"
