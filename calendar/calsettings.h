/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-11-03
 * Description : calendar parameters.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
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

    QPrinter::PageSize pageSize;
    int                paperWidth;
    int                paperHeight;
    int                width;
    int                height;
    bool               drawLines;
    float              ratio;
    ImagePosition      imgPos;
    QFont              baseFont;
    int                year;
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
    void addSpecial(const QDate &date, const Day &info);
    void loadSpecial(const KUrl &url, const QColor &color);
    bool isSpecial(int month, int day) const;

    QColor getDayColor(int month, int day) const;
    QString getDayDescr(int month, int day) const;

    static CalSettings* instance();

    ~CalSettings();

public:

    CalParams params;

Q_SIGNALS:

    void settingsChanged();

public Q_SLOTS:

    void setPaperSize(const QString &paperSize);
    void setImagePos(int pos);
    void setDrawLines(bool draw);
    void setRatio(int ratio);
    void setFont(const QString &font);

protected:

    bool isPrayDay(const QDate &date) const;

private:

    CalSettings();
    CalSettings(CalSettings const &);
    CalSettings &operator=(CalSettings const &);

private:

    static QPointer<CalSettings> instance_;

    QMap<int, KUrl>              monthMap_;
    QMap<QDate, Day>             special;
};

}  // NameSpace KIPICalendarPlugin

#endif // CALSETTINGS_H
