/* ============================================================
 * File  : calwizard.h
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

#ifndef CALWIZARD_H
#define CALWIZARD_H

// Qt includes.

#include <qstringlist.h>
#include <qguardedptr.h>

// KDE includes.

#include <kwizard.h>

// LibKIPI includes.

#include <libkipi/interface.h>

class QLabel;
class QVBox;
class QPainter;
class QProgressBar;
class QPushButton;

class KPrinter;

namespace KIPICalendarPlugin
{

class CalTemplate;
class CalSelect;
class CalSettings;
class CalBlockPainter;

class CalWizard : public KWizard
{
    Q_OBJECT

public:

    CalWizard( KIPI::Interface* interface, QWidget *parent=0L );
    ~CalWizard();

private:

    CalSettings  *cSettings_;
    CalTemplate  *wTemplate_;
    CalSelect    *wSelect_;
    QVBox        *wPrint_;
    QLabel       *wPrintLabel_;
    QWidget      *wFinish_;
    QLabel       *wFinishLabel_;
    QProgressBar *wFinishProgressTotal_;
    QProgressBar *wFinishProgressCurrent_;

    QPushButton  *m_helpButton;
    
    KPrinter     *printer_;
    QPainter     *painter_;

    QValueList<int>   monthNumbers_;
    KURL::List        monthImages_;
    int               totPages_;
    int               currPage_;
    QGuardedPtr<CalBlockPainter> cb_;
    KIPI::Interface* interface_;

private slots:

    void slotPageSelected(const QString& name);
    void slotPrintOnePage();
    void slotHelp();
};

}  // NameSpace KIPICalendarPlugin

#endif /* CALWIZARD_H */
