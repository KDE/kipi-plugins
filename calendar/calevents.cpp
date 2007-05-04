/* ============================================================
 * File  : calevents.cpp
 * Author: Maciek Borowka <maciek_AT_borowka.net>
 * Date  : 2005-11-23
 * Description : The implementation of a new "create calendar"
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

#include <qpushbutton.h>

#include <kiconloader.h>
#include <kfiledialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <kdebug.h>

#include "calevents.h"

namespace KIPICalendarPlugin {

CalEvents::CalEvents(QWidget *parent, const char *name)
 : CalEventsBase(parent, name)
{
    KIconLoader * icons = new KIconLoader( QString( "MenuDlg" ) );
    ohBtn->setPixmap( icons->loadIcon( QString( "fileopen" ), KIcon::Toolbar ) );
    fhBtn->setPixmap( icons->loadIcon( QString( "fileopen" ), KIcon::Toolbar ) );
}


CalEvents::~CalEvents()
{
}


void CalEvents::ohChooseSlot()
{
  QString temp;

  temp = KFileDialog::getOpenFileName(ohFileEdit->text(),
                                      QString( "*.ics" ),
                                      this,
                                      i18n("Select 'Official Holidays' calendar file") );
  if( temp.isEmpty() )
    return;

  ohFileEdit->setText( temp );
}

void CalEvents::fhChooseSlot()
{
  QString temp;

  temp = KFileDialog::getOpenFileName(fhFileEdit->text(),
                                      QString( "*.ics" ),
                                      this,
                                      i18n("Select 'Family Holidays' calendar file") );
  if( temp.isEmpty() )
    return;

  fhFileEdit->setText( temp );
}

}

#include "calevents.moc"
