/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : e-mail settings page.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef EMAIL_PAGE_H
#define EMAIL_PAGE_H

// Qt includes.

#include <QWidget>

namespace KIPISendimagesPlugin
{

class EmailPagePriv;

class EmailPage : public QWidget
{
    Q_OBJECT

public:

    enum EmailClient 
    {
        DEFAULT = 0,
        BALSA,
        CLAWSMAIL,
        EVOLUTION,
        GMAILAGENT,
        KMAIL,
        MOZILLA,
        NETSCAPE,
        SYLPHEED,
        SYLPHEEDCLAWS,
        THUNDERBIRD
    };

    enum ImageSize 
    {
        VERYSMALL = 0,
        SMALL,
        MEDIUM,
        BIG,
        VERYBIG,
        HUGE
    };

    enum ImageFormat 
    {
        JPEG = 0,
        PNG
    };

public:

    EmailPage(QWidget* parent);
    ~EmailPage();

signals:

    void signalEnableButtonOK(bool);

private slots:

    void slotMailAgentChanged(int);
    void slotThunderbirdBinPathChanged(const QString&);

private:

    EmailPagePriv* d;
};

}  // namespace KIPISendimagesPlugin

#endif // EMAIL_PAGE_H 
