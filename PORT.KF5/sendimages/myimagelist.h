/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-10-18
 * Description : images list settings page.
 *
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include "emailsettings.h"

using namespace KIPIPlugins;

namespace KIPISendimagesPlugin
{

class MyImageList : public KPImagesList
{
    Q_OBJECT

public:

    explicit MyImageList(QWidget* const parent);
    ~MyImageList();

    QList<EmailItem> imagesList() const;
};

}  // namespace KIPISendimagesPlugin

#endif // MYIMAGELIST_H
