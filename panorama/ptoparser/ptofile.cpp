/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-04
 * Description : a plugin to create panorama by fusion of several images.
 *               This parser is based on pto file format described here:
 *               http://hugin.sourceforge.net/docs/nona/nona.txt, and
 *               on pto files produced by Hugin's tools.
 *
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

// C++ includes

#include <string>
#include <iostream>

#include <mcheck.h>

// Qt includes

#include <QFile>

// KDE includes

#include <kdebug.h>

// Local includes

#include "ptofile.h"
extern "C" {
    #include "tparser.h"
    #include "tparsergetters.h"
}

namespace KIPIPanoramaPlugin
{

struct PTOFile::PTOFilePriv {
    pt_script* script;
};

PTOFile::PTOFile()
{
    d = new PTOFilePriv();
    d->script = NULL;
}

PTOFile::~PTOFile()
{
    if (d->script != NULL)
    {
        panoScriptFree(d->script);
        delete d->script;
    }
}

bool PTOFile::openFile(const QString& path)
{
    mtrace();
    char* tmp = path.toUtf8().data();

    if (d->script != NULL)
    {
        panoScriptFree(d->script);
        delete d->script;
        d->script = NULL;
    }

    d->script = new pt_script();
    if (!panoScriptParse(tmp, d->script))
    {
        return false;
    }

    muntrace();

    return true;
}


} // namespace KIPIPanoramaPlugin
