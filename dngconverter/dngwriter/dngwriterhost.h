/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
 *
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DNGWRITTERHOST_H
#define DNGWRITTERHOST_H

// Local includes

#include "dngwriter_p.h"

namespace DNGIface
{

class DNGWriterHost : public dng_host
{

public:

    DNGWriterHost(DNGWriter::DNGWriterPrivate* priv, dng_memory_allocator* allocator=0);
    ~DNGWriterHost();

private:

    void SniffForAbort();

private:

    DNGWriter::DNGWriterPrivate* const m_priv;
};

}  // namespace DNGIface

#endif /* DNGWRITTERHOST_H */
