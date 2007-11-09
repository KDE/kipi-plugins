/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-02-25
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006 by Tom Albers <tomalbers at kde dot nl>
 * Copyright (C) 2006 by Michael Hoechstetter <michael dot hoechstetter at gmx dot de>
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

// Qt includes.

#include <qtimer.h>

// KDE includes

#include <klocale.h>

// Local includes.

#include "actions.h"
#include "emailsettingscontainer.h"
#include "sendimages.h"
#include "sendimages.moc"

namespace KIPISendimagesPlugin
{

class SendImagesPriv
{
public:

    SendImagesPriv()
    {
    }

    EmailSettingsContainer settings;
};

SendImages::SendImages(QObject *parent)
          : QObject(parent)
{
    d = new SendImagesPriv;
}

SendImages::~SendImages()
{
    delete d;
}

void SendImages::setSettings(const EmailSettingsContainer& settings)
{
    d->settings = settings;
}

}  // NameSpace KIPISendimagesPlugin
