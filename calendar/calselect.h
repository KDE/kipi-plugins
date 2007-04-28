/* ============================================================
 * File  : calselect.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-03
 * Description :
 *
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>

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

#ifndef CALSELECT_H
#define CALSELECT_H

// Qt includes.

#include <qwidget.h>
#include <qptrvector.h>

// LibKIPi includes.

#include <libkipi/interface.h>

class QSpinBox;

namespace KIPICalendarPlugin
{

class MonthWidget;

class CalSelect : public QWidget
{
    Q_OBJECT

public:

    CalSelect( KIPI::Interface* interface, QWidget *parent, const char* name=0);
    ~CalSelect();

private:

    void setupView( KIPI::Interface* interface );

    QPtrVector<MonthWidget> *mwVector_;
    QSpinBox                *yearSpin_;

private slots:

    void slotYearChanged(int year);

};

}  // NameSpace KIPICalendarPlugin

#endif /* CALSELECT_H */
