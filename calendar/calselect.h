/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#ifndef CALSELECT_H
#define CALSELECT_H

// Qt includes

#include <QWidget>
#include <QVector>

// LibKIPi includes

#include <libkipi/interface.h>

// Local includes

#include "ui_calselect.h"

namespace KIPICalendarPlugin
{

class MonthWidget;

class CalSelect : public QWidget
{
    Q_OBJECT

public:

    CalSelect( KIPI::Interface* interface, QWidget *parent );
    ~CalSelect();

private:

    void setupView( KIPI::Interface* interface );

private Q_SLOTS:

    void yearChanged(int year);

private:

    Ui::CalSelect          ui;

    QVector<MonthWidget*>  mwVector_;
    QGridLayout           *monthBoxLayout_;
};

}  // NameSpace KIPICalendarPlugin

#endif // CALSELECT_H
