/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-01-18
 * Description : different save methods for red eye correction
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

#ifndef SAVEMETHODS_H
#define SAVEMETHODS_H

// Qt includes

#include <QString>

namespace KIPIRemoveRedEyesPlugin
{

class SaveMethod
{
public:

    SaveMethod() {};
    virtual ~SaveMethod() {};

    virtual QString savePath(const QString& path, const QString& extra) const = 0;
};

// -----------------------------------------------

class SaveSubfolder : public SaveMethod
{
public:

    SaveSubfolder() {};
    ~SaveSubfolder() {};
    QString savePath(const QString& path, const QString& extra) const;
};

// -----------------------------------------------

class SavePrefix : public SaveMethod
{
public:

    SavePrefix() {};
    ~SavePrefix() {};
    QString savePath(const QString& path, const QString& extra) const;
};

// -----------------------------------------------

class SaveSuffix : public SaveMethod
{
public:

    SaveSuffix() {};
    ~SaveSuffix() {};
    QString savePath(const QString& path, const QString& extra) const;
};

// -----------------------------------------------

class SaveOverwrite : public SaveMethod
{
public:

    SaveOverwrite() {};
    ~SaveOverwrite() {};
    QString savePath(const QString& path, const QString& extra) const;
};

// -----------------------------------------------

} // namespace KIPIRemoveRedEyesPlugin

#endif /* SAVEMETHODS_H */
