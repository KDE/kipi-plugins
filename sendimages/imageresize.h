/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-09
 * Description : batch image resize
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGERESIZE_H
#define IMAGERESIZE_H

// Qt includes.

#include <QString>

// KDE includes.

#include <kurl.h>

// Local includes.

#include "emailsettingscontainer.h"
#include "actions.h"

namespace KIPISendimagesPlugin
{

class ImageResize
{

public:

    ImageResize(const EmailSettingsContainer& settings);
    ~ImageResize();

    bool resize(const KUrl& src, const QString destName, QString& err);

private :

    EmailSettingsContainer m_settings;

};

}  // NameSpace KIPISendimagesPlugin

#endif /* IMAGERESIZE_H */
