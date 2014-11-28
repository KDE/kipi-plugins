/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-02-08 
 * Description : a kipi plugin to print images
 *
 * Copyright 2009-2012 by Angelo Naselli <anaselli at linux dot it>
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

#ifndef PRINTHELPER_H
#define PRINTHELPER_H

// KDE includes

#include <kurl.h>

class QWidget;

namespace KIPIPrintImagesPlugin
{

class PrintHelper
{

public:

    PrintHelper(QWidget* const parent);
    ~PrintHelper();

    void print(const KUrl::List& fileList);

private:

    class Private;
    Private* const d;
};

} // namespace

#endif /* PRINTHELPER_H */
