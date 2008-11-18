/* ============================================================
 * File  : caltemplate.h
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Orgad Shaneh <orgads@gmail.com>
 * Date  : 2008-11-13
 * Description: template selection for calendar
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

#ifndef __CALTEMPLATE_H__
#define __CALTEMPLATE_H__

// Qt includes.

#include <QWidget>

// UI include.

#include "ui_caltemplate.h"

namespace KIPICalendarPlugin
{

class CalTemplate : public QWidget
{
public:

    CalTemplate(QWidget* parent);
    ~CalTemplate();

private:

    Ui::CalTemplate ui;

};

}  // NameSpace KIPICalendarPlugin

#endif // __CALTEMPLATE_H__
