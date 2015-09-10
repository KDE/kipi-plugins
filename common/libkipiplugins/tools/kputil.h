/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-09-10
 * Description : Kipi-Plugins shared library.
 *
 * Copyright (C) 2015 by Alexander Potashev <aspotashev@gmail.com>
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

#ifndef KPUTIL_H
#define KPUTIL_H

#include "kipiplugins_export.h"

#include <QtCore/QString>
#include <QtCore/QDir>

namespace KIPIPlugins
{

QDir KIPIPLUGINS_EXPORT makeTemporaryDir(const QString& prefix);

} // namespace KIPIPlugins

#endif // KPUTIL_H
