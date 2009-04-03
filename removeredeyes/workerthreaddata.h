/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : data that will be sent when calculation is finished
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifndef WORKERTHREADDATA_H
#define WORKERTHREADDATA_H

class KUrl;

namespace KIPIRemoveRedEyesPlugin
{

class WorkerThreadData
{

public:

    WorkerThreadData(const KUrl& url, int current = 0, int eyes = 0);
    ~WorkerThreadData();

    int         current;
    int         eyes;
    const KUrl& urls;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif // WORKERTHREADDATA_H
