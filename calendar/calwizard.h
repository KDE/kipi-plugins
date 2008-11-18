/* ============================================================
 * File  : calwizard.h
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Tom Albers <tomalbers@kde.nl>
 *          Orgad Shaneh <orgads@gmail.com>
 * Date  : 2008-11-13
 * Description: main dialog
 *
 * Copyright 2003 by Renchi Raju
 * Copyright 2006 by Tom Albers
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

#ifndef __CALWIZARD_H__
#define __CALWIZARD_H__

// Qt includes.

#include <QMap>

// KDE includes.

#include <KAssistantDialog>

// UI includes.

#include "ui_calevents.h"
#include "ui_calprogress.h"

class QLabel;
class QPainter;
class QPrinter;
class QPushButton;
class QProgressBar;

namespace KIPI
{
    class Interface;
}

namespace KIPIPlugins
{
    class KPAboutData;
}

namespace KIPICalendarPlugin
{

class CalSelect;
class CalEvents;
class CalPainter;
class CalSettings;
class CalTemplate;
class CalFormatter;

class CalWizard : public KAssistantDialog
{
    Q_OBJECT

public:

    CalWizard( KIPI::Interface* interface, QWidget *parent=0 );
    virtual ~CalWizard();

private:

    CalSettings  *cSettings_;
    CalTemplate  *wTemplate_;
    CalSelect    *wSelect_;
    QWidget      *wEvents_;
    Ui::CalEvents calEventsUI;
    Ui::CalProgress calProgressUI;

    QLabel       *wPrintLabel_;
    QWidget      *wFinish_;

    KPageWidgetItem *wPrintPage_;
    KPageWidgetItem *wFinishPage_;

    int currPage_;

    QPrinter     *printer_;

    CalFormatter *formatter_;

    CalPainter   *painter_;

    QMap<int, KUrl>  months_;

    KIPI::Interface* interface_;

    KIPIPlugins::KPAboutData    *m_about;

    void print();

private slots:

    void slotPageSelected(KPageWidgetItem *current, KPageWidgetItem *before);
    void slotHelp();
    void printComplete();
    void paintNextPage();
};

}  // NameSpace KIPICalendarPlugin

#endif // __CALWIZARD_H__
