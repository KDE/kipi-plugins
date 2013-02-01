/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-12-09
 * Description : raw converter plugin action descriptions
 *
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef ACTIONS_H
#define ACTIONS_H

// Qt includes

#include <QString>
#include <QImage>
#include <QMetaType>

// KDE includes

#include <kurl.h>

namespace KIPIRawConverterPlugin
{

enum Action
{
    NONE = 0,
    IDENTIFY,
    IDENTIFY_FULL,
    THUMBNAIL,
    PREVIEW,
    PROCESS
};

class ActionData
{

public:

    ActionData()
    {
        starting = false;
        success  = false;
        action   = NONE;
    }

    bool    starting;
    bool    success;

    QString destPath;
    QString message;

    QImage  image;

    KUrl    fileUrl;

    Action  action;
};

}  // namespace KIPIRawConverterPlugin

Q_DECLARE_METATYPE(KIPIRawConverterPlugin::ActionData)

#endif /* ACTIONS_H */
