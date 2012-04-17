/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-17-04
 * Description : time adjust images list.
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef MYIMAGELIST_H
#define MYIMAGELIST_H

// Local includes

#include "kpimageslist.h"

using namespace KIPIPlugins;

namespace KIPITimeAdjustPlugin
{

class MyImageList : public KPImagesList
{
    Q_OBJECT

public:

    MyImageList(QWidget* const parent);
    ~MyImageList();
};

}  // namespace KIPITimeAdjustPlugin

#endif // MYIMAGELIST_H
