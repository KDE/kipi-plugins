/* ============================================================
 * File  : calsettings.h
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Orgad Shaneh <orgads@gmail.com>
 * Date  : 2008-11-13
 * Description: calendar parameters
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

#ifndef __CALSETTINGS_H__
#define __CALSETTINGS_H__

// Qt includes.

#include <QMap>
#include <QFont>
#include <QObject>
#include <QString>
#include <QPointer>
#include <QPrinter>

// KDE includes.

#include <KUrl>

namespace KIPICalendarPlugin
{

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
};

class CalSettings : public QObject
{
    Q_OBJECT

public:

    void    setYear(int year);
    int     year() const;
    void    setImage(int month, const KUrl& url);
    KUrl    image(int month) const;

    CalParams params;

    static CalSettings* instance();

    ~CalSettings();

signals:

    void settingsChanged();

public slots:

    void setPaperSize(const QString &paperSize);
    void setImagePos(int pos);
    void setDrawLines(bool draw);
    void setRatio(int ratio);
    void setFont(const QString &font);

private:

    CalSettings();
    CalSettings(CalSettings const &);
    CalSettings &operator=(CalSettings const &);

    static QPointer<CalSettings> instance_;

    QMap<int, KUrl>     monthMap_;
    int                 year_;

};

}  // NameSpace KIPICalendarPlugin

#endif // __CALSETTINGS_H__
