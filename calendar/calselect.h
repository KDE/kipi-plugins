/* ============================================================
 * File  : calselect.h
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Orgad Shaneh <orgads@gmail.com>
 * Date  : 2008-11-13
 * Description: select year and images for months
 *
 * Copyright 2003 by Renchi Raju
 * Copyright 2008 by Orgad Shaneh
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

#ifndef __CALSELECT_H__
#define __CALSELECT_H__

// Qt includes.

#include <QWidget>
#include <QVector>

// LibKIPi includes.

#include <libkipi/interface.h>

// Local includes.

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

    Ui::CalSelect ui;

    void setupView( KIPI::Interface* interface );

    QVector<MonthWidget *> mwVector_;
    QGridLayout          *monthBoxLayout_;

private slots:

    void yearChanged(int year);

};

}  // NameSpace KIPICalendarPlugin

#endif // __CALSELECT_H__
