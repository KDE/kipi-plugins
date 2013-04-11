/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2012-07-01
 * @brief  actions
 *
 * @author Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef ACTIONS_H
#define ACTIONS_H

// Qt includes

#include <QString>
#include <QMetaType>

// KDE includes

#include <kurl.h>

namespace KIPIVideoSlideShowPlugin
{

enum Action
{
    TYPE_NONE = 0,
    TYPE_TRANSITION,
    TYPE_IMAGE
};

// -----------------------------------------------------

class ActionData
{

public:

    ActionData()
    {
        totalFrames = 0;
        action      = TYPE_NONE;
    }

    KUrl   fileUrl;

    Action action;

    int    totalFrames;
};

}  // namespace KIPIVideoSlideShowPlugin

Q_DECLARE_METATYPE(KIPIVideoSlideShowPlugin::ActionData)

#endif /* ACTIONS_H */

