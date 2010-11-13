/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
 *
 * Copyright (C) 2008-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <kdebug.h>

// Local includes

#include "dngwriterhost.h"

namespace DNGIface
{

DNGWriterHost::DNGWriterHost(DNGWriter::DNGWriterPrivate* priv, dng_memory_allocator* allocator)
             : dng_host(allocator), m_priv(priv)
{
}

DNGWriterHost::~DNGWriterHost()
{
}

void DNGWriterHost::SniffForAbort()
{
    if (m_priv->cancel)
    {
        kDebug() << "DNGWriter: Canceled by user..." ;
        m_priv->cleanup();
        ThrowUserCanceled();
    }
}

}  // namespace DNGIface
