/* ============================================================
 * File  : calwizard.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-03
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

#ifndef CALWIZARD_H
#define CALWIZARD_H

#include <kwizard.h>
#include <qstringlist.h>
#include <qguardedptr.h>

#include <libkipi/interface.h>

class KPrinter;
class QLabel;
class QPainter;
class QProgressBar;

namespace DKCalendar
{

class CalTemplate;
class CalSelect;
class CalSettings;
class CalBlockPainter;

class CalWizard : public KWizard
{
    Q_OBJECT

public:

    CalWizard( KIPI::Interface* interface );
    ~CalWizard();

private:

    CalSettings  *cSettings_;
    CalTemplate  *wTemplate_;
    CalSelect    *wSelect_;
    QLabel       *wPrint_;
    QWidget      *wFinish_;
    QLabel       *wFinishLabel_;
    QProgressBar *wFinishProgressTotal_;
    QProgressBar *wFinishProgressCurrent_;

    KPrinter    *printer_;
    QPainter    *painter_;

    QValueList<int>   monthNumbers_;
    QStringList       monthImages_;
    int               totPages_;
    int               currPage_;
    QGuardedPtr<CalBlockPainter> cb_;

private slots:

    void slotPageSelected(const QString& name);
    void slotPrintOnePage();
    void slotHelp();
};

}

#endif /* CALWIZARD_H */
