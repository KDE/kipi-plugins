/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-05-21
 * Description : a widget to display the imagelist
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#ifndef MYIMAGESLIST_H
#define MYIMAGESLIST_H

// Local includes

#include "kpimageslist.h"

using namespace KIPIPlugins;

namespace KIPIRemoveRedEyesPlugin
{

class MyImagesList : public KPImagesList
{
    Q_OBJECT

public:

    explicit MyImagesList(QWidget* const parent = 0);
    ~MyImagesList();

    bool hasUnprocessedImages();
    void removeUnprocessedImages();
    void resetEyeCounterColumn();

public Q_SLOTS:

    void addEyeCounterByUrl(const KUrl&, int);
};

}  // namespace KIPIRemoveRedEyesPlugin

#endif // MYIMAGESLIST_H
