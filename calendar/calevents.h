/* ============================================================
 * File  : calevents.h
 * Author: Maciek Borowka <maciek_AT_borowka.net>
 * Date  : 2005-11-23
 * Description : The declaration of a new "create calendar"
 * wizard page.
 *
 * Copyright 2005 by Maciek Borowka
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

#ifndef KIPICALENDARPLUGINCALEVENTS_H
#define KIPICALENDARPLUGINCALEVENTS_H

#include <qwidget.h>

#include "caleventsbase.h"

namespace KIPICalendarPlugin {

/**
@author Maciek Borowka
*/
class CalEvents : public CalEventsBase
{
Q_OBJECT
public:
    CalEvents(QWidget *parent = 0, const char *name = 0);

    ~CalEvents();

public slots:
    virtual void ohChooseSlot();
    virtual void fhChooseSlot();
};

}

#endif
