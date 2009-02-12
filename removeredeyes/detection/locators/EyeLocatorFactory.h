/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-01-18
 * Description : factory to create save method objects
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifndef EYELOCATORFACTORY_H
#define EYELOCATORFACTORY_H

class QString;

namespace KIPIRemoveRedEyesPlugin
{

class EyeLocatorAbstract;

class EyeLocatorFactory
{

public:

    static EyeLocatorAbstract* create(const QString& type);

private:

    EyeLocatorFactory() {};
    virtual ~EyeLocatorFactory() {};
};

}

#endif /* EYELOCATORFACTORY_H */
