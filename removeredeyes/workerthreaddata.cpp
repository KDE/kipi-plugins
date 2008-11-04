/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : data that will be sent when calculation is finished
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "workerthreaddata.h"

// KDE includes.

#include <kurl.h>

namespace KIPIRemoveRedEyesPlugin
{

WorkerThreadData::WorkerThreadData(const KUrl& url, int current, int eyes): m_url(url)
{
    m_current = current;
    m_eyes    = eyes;
}


WorkerThreadData::~WorkerThreadData()
{
}

int WorkerThreadData::current() const
{
    return m_current;
}

int WorkerThreadData::eyes() const
{
    return m_eyes;
}

const KUrl& WorkerThreadData::url() const
{
    return m_url;
}

} // namespace KIPIRemoveRedEyesPlugin
