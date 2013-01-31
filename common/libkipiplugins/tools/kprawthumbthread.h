/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-01
 * Description : Raw thumbnail thread.
 *
 * Copyright (C) 2004-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPRAWTHUMBTHREAD_H
#define KPRAWTHUMBTHREAD_H

// Qt includes

#include <QThread>
#include <QImage>

// KDE includes

#include <kurl.h>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPRawThumbThread : public QThread
{
    Q_OBJECT

public:

    explicit KPRawThumbThread(QObject* const parent, int size=256);
    ~KPRawThumbThread();

    void getRawThumb(const KUrl& url);
    void cancel();

Q_SIGNALS:

    /** Emmited when Raw thumb is extracted. Send a null image if failed
     */
    void signalRawThumb(const KUrl&, const QImage& img);

private:

    void run();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIPlugins

#endif /* KPRAWTHUMBTHREAD_H */
