/* ============================================================
 * File  : cspinbox.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-10-22
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef CSPINBOX_H
#define CSPINBOX_H

#include <qspinbox.h>
#include <stdlib.h>

namespace RawConverter
{

class CSpinBox : public QSpinBox {

public:

    CSpinBox(QWidget *parent, const char *name=0)
        : QSpinBox( parent, name ) {
    }

    CSpinBox( int min, int max, int step,
                  QWidget *parent, const char *name=0)
        : QSpinBox( min, max, step,parent, name ) {
    }

    QString  mapValueToText( int value ) {
        return QString("%1.%2").arg(value/10).arg(abs(value%10));
    }

    int mapTextToValue( bool* ) {
        return int(text().toFloat()*10);
    }

};

}

#endif /* CSPINBOX_H */
