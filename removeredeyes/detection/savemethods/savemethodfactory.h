/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-01-18
 * Description : factory to create save method objects
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

#ifndef SAVEMETHODFACTORY_H
#define SAVEMETHODFACTORY_H

// Local includes

#include "savemethods.h"

namespace KIPIRemoveRedEyesPlugin
{

class SaveMethod;

class SaveMethodFactory
{

public:

    static SaveMethod* create(int type);

private:

    SaveMethodFactory() {};
    virtual ~SaveMethodFactory() {};
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* SAVEMETHODFACTORY_H */
