/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description : select year and images for months.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "calselect.moc"

// KDE includes

#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>

// Libkipi includes

#include <libkipi/imagecollection.h>

// Local includes

#include "calsettings.h"
#include "monthwidget.h"

#define MAX_MONTHS (13)

namespace KIPICalendarPlugin
{

CalSelect::CalSelect(KIPI::Interface* interface, QWidget* parent)
    : QWidget(parent), mwVector_(MAX_MONTHS)
{
    setupView(interface);
}

CalSelect::~CalSelect()
{
}

void CalSelect::setupView(KIPI::Interface* interface)
{
    ui.setupUi(this);

    connect(ui.yearSpin, SIGNAL(valueChanged(int)),
            this, SLOT(yearChanged(int)));

    const KCalendarSystem* cal = KGlobal::locale()->calendar();
    int currentYear            = cal->year(QDate::currentDate());

    KUrl::List urlList;
    KIPI::ImageCollection images = interface->currentSelection();

    if (images.isValid() && !images.images().isEmpty())
    {
        urlList = images.images();
    }

    QDate d;
    cal->setDate(d, currentYear, 1, 1);
    int months     = cal->monthsInYear(d);
    // span the monthWidgets over 2 rows. inRow should usually be 6 or 7 (for 12 or 13 months)
    int inRow      = (months / 2) + ((months % 2) != 0);
    MonthWidget* w = 0;

    for (int i = 0; i < MAX_MONTHS; ++i)
    {
        w = new MonthWidget(interface, ui.monthBox, i + 1);

        if (i < urlList.count())
        {
            w->setImage(urlList[i]);
        }

        if (i < months)
        {
            ui.monthBoxLayout->addWidget(w, i / inRow, i % inRow);
        }
        else
        {
            w->hide();
        }

        mwVector_.insert(i, w);
    }

    ui.yearSpin->setRange(cal->year(cal->earliestValidDate()) + 1,
                          cal->year(cal->latestValidDate()) - 1);
    ui.yearSpin->setValue(currentYear);
}

void CalSelect::yearChanged(int year)
{
    int i, months;
    QDate d, oldD;
    const KCalendarSystem* cal = KGlobal::locale()->calendar();
    cal->setDate(d, year, 1, 1);
    cal->setDate(oldD, CalSettings::instance()->year(), 1, 1);
    months = cal->monthsInYear(d);

    if ((cal->monthsInYear(oldD) != months) && !mwVector_.isEmpty())
    {
        // hide the last months that are not present on the current year
        for (i = months; (i < cal->monthsInYear(oldD)) && (i < mwVector_.count()); ++i)
        {
            mwVector_.at(i)->hide();
        }

        // span the monthWidgets over 2 rows. inRow should usually be 6 or 7 (for 12 or 13 months)
        int inRow = (months / 2) + ((months % 2) != 0);

        // remove all the monthWidgets, then readd the needed ones
        for (i = 0; i < cal->monthsInYear(oldD); ++i)
        {
            ui.monthBoxLayout->removeWidget(mwVector_.at(i));
        }

        for (i = 0; (i < months) && (i < mwVector_.count()); ++i)
        {
            ui.monthBoxLayout->addWidget(mwVector_.at(i), i / inRow, i % inRow);

            if (mwVector_.at(i)->isHidden())
            {
                mwVector_.at(i)->show();
            }

            mwVector_.at(i)->update();
        }
    }

    CalSettings::instance()->setYear(year);
}

}  // NameSpace KIPICalendarPlugin
