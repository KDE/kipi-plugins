/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-31-01
 * Description : a kipi plugin to print images
 *
 * Copyright 2003      by Todd Shoemaker <todd@theshoemakers.net>
 * Copyright 2007-2012 by Angelo Naselli <anaselli at linux dot it>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef UTILS_H
#define UTILS_H

// Qt includes

#include <QString>

class QWidget;
class QStringList;

namespace KIPIPrintImagesPlugin
{

int  NINT(double n);
bool launchExternalApp(const QString& program, const QStringList& args);
bool checkTempPath(QWidget* const parent, const QString& tempPath);

}  // NameSpace KIPIPrintImagesPlugin

#endif // UTILS_H
