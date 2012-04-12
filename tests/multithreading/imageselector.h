/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : test for implementation of threadWeaver api
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

// KDE includes

#include <kurl.h>

// Local includes

#include "kpaboutdata.h"
#include "kptooldialog.h"

using namespace KIPIPlugins;

class ImageSelector : public KPToolDialog
{
    Q_OBJECT

public:

    ImageSelector(KPAboutData* const about);
    ~ImageSelector();

private Q_SLOTS:

    void slotStart();
    void slotStarting(const KUrl&, int);
    void slotFinished(const KUrl&, int);
    void slotFailed(const KUrl&, int, const QString&);

private:

    class ImageSelectorPriv;
    ImageSelectorPriv* const d;
};

#endif // IMAGESELECTOR_H
