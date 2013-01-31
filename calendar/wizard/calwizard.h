/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description : main dialog.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Tom Albers <tomalbers@kde.nl>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
 * Copyright (C) 2012      by Angelo Naselli <anaselli at linux dot it>
 * Copyright (C) 2012-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef CALWIZARD_H
#define CALWIZARD_H

// Qt includes

#include <QMap>

// UI includes

#include "ui_calevents.h"
#include "ui_calprogress.h"

// Local includes

#include "kptooldialog.h"

class QLabel;
class QPrinter;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPICalendarPlugin
{

class CalEvents;
class CalPrinter;
class CalSettings;
class CalTemplate;

class CalWizard : public KPWizardDialog
{
    Q_OBJECT

public:

    explicit CalWizard(QWidget* const parent=0);
    virtual ~CalWizard();

private:

    void print();

private Q_SLOTS:

    void slotPageSelected(KPageWidgetItem* current, KPageWidgetItem* before);
    void printComplete();
    void updatePage(int page);

private:

    CalSettings*     cSettings_;
    CalTemplate*     wTemplate_;
    QWidget*         wEvents_;
    Ui::CalEvents    calEventsUI;
    Ui::CalProgress  calProgressUI;

    QLabel*          wPrintLabel_;
    QWidget*         wFinish_;

    KPageWidgetItem* wPrintPage_;
    KPageWidgetItem* wFinishPage_;

    QPrinter*        printer_;

    CalPrinter*      printThread_;

    QMap<int, KUrl>  months_;
};

}  // NameSpace KIPICalendarPlugin

#endif // CALWIZARD_H
