/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description : calendar parameters.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
 * Copyright (C) 2011      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2012      by Angelo Naselli <anaselli at linux dot it>
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

#ifndef CALSETTINGS_H
#define CALSETTINGS_H

// Qt includes

#include <QMap>
#include <QDate>
#include <QFont>
#include <QPair>
#include <QColor>
#include <QObject>
#include <QString>
#include <QPointer>
#include <QPrinter>

// KDE includes

#include <kurl.h>

namespace KIPI
{
    class Interface;
}

namespace KIPICalendarPlugin
{

typedef QPair<QColor, QString> Day;

struct CalParams
{
public:

    enum ImagePosition
    {
        Top = 0,
        Left,
        Right
    };

    QPrinter::PageSize    pageSize;
    QPrinter::PrinterMode printResolution;
    int                   paperWidth;
    int                   paperHeight;
    int                   width;
    int                   height;
    bool                  drawLines;
    float                 ratio;
    ImagePosition         imgPos;
    QFont                 baseFont;
    int                   year;

    // adding interface so who needs it can get it from settings
    KIPI::Interface*      interface;
};

// ---------------------------------------------------------------------------

class CalSettings : public QObject
{
    Q_OBJECT

public:

    void setYear(int year);
    int  year() const;
    void setImage(int month, const KUrl& url);
    KUrl image(int month) const;
    void clearSpecial();
    void addSpecial(const QDate& date, const Day& info);
    void loadSpecial(const KUrl& url, const QColor& color);
    bool isSpecial(int month, int day) const;

    QColor getDayColor(int month, int day) const;
    QString getDayDescr(int month, int day) const;
    QPrinter::PrinterMode resolution() const;

    void setKipiInterface(KIPI::Interface* const interface);
    KIPI::Interface* kipiInterface() const;

    static CalSettings* instance(QObject* const parent = 0);

    ~CalSettings();

public:

    CalParams params;

Q_SIGNALS:

    void settingsChanged();

public Q_SLOTS:

    void setPaperSize(const QString& paperSize);
    void setResolution(const QString& resolution);
    void setImagePos(int pos);
    void setDrawLines(bool draw);
    void setRatio(int ratio);
    void setFont(const QString& font);

protected:

    bool isPrayDay(const QDate& date) const;

private:

    CalSettings(QObject* const parent);
    CalSettings(CalSettings const&);
    CalSettings& operator=(CalSettings const&);

private:

    static QPointer<CalSettings> s_instance;

    QMap<int, KUrl>              m_monthMap;
    QMap<QDate, Day>             m_special;
};

}  // NameSpace KIPICalendarPlugin

#endif // CALSETTINGS_H
