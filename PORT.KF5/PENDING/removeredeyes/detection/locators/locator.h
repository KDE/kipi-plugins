/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-02-12
 * Description : locator abstract class
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#ifndef LOCATOR_H
#define LOCATOR_H

// Qt includes

#include <QString>
#include <QWidget>
#include <QObject>

namespace KIPIRemoveRedEyesPlugin
{

class Locator: public QObject
{

public:

    enum SaveResult
    {
        Final = 0,
        OriginalPreview,
        CorrectedPreview,
        MaskPreview
    };

public:

    Locator() {};
    virtual ~Locator() {};

    virtual int      startCorrection(const QString& src, const QString& dest) = 0;
    virtual int      startTestrun(const QString& src) = 0;
    virtual int      startPreview(const QString& src) = 0;
    virtual QWidget* settingsWidget() = 0;

    virtual void readSettings()  = 0;
    virtual void writeSettings() = 0;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* LOCATOR_H */
