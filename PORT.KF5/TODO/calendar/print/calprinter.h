/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-11-13
 * Description : printer thread.
 *
 * Copyright (C) 2008 by Orgad Shaneh <orgads at gmail dot com>
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

#ifndef CALPRINTER_H
#define CALPRINTER_H

// Qt includes

#include <QMap>
#include <QThread>

// KDE includes

#include <kurl.h>

class QPrinter;

namespace KIPI
{
class Interface;
}

using namespace KIPI;

namespace KIPICalendarPlugin
{

class CalPainter;

class CalPrinter : public QThread
{
    Q_OBJECT

public:

    CalPrinter(QPrinter* const printer,
               QMap<int, KUrl>& months,
               Interface* const interface,
               QObject* const parent);

    virtual ~CalPrinter();

protected:

    void run();

Q_SIGNALS:

    void pageChanged(int page);
    void totalBlocks(int total);
    void blocksFinished(int finished);

public Q_SLOTS:

    void cancel();

private:

    bool             cancelled_;

    QMap<int, KUrl>  months_;
    QPrinter*        printer_;

    CalPainter*      painter_;

    Interface*       interface_;
};

}  // NameSpace KIPICalendarPlugin

#endif // CALPRINTER_H
