/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-09
 * Description : a class to resize image in a separate thread.
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QThread>
#include <QString>

// KDE includes

#include <kurl.h>

// Local includes

#include "emailsettingscontainer.h"

namespace KIPISendimagesPlugin
{

class ImageResizePriv;

class ImageResize : public QThread
{
    Q_OBJECT

public:

    ImageResize(QObject *parent);
    ~ImageResize();

    void resize(const EmailSettingsContainer& settings);
    void cancel();

private:

    void run();
    bool imageResize(const EmailSettingsContainer& settings, 
                     const KUrl& orgUrl, const QString& destName, QString& err);

Q_SIGNALS:

    void startingResize(const KUrl &orgUrl);
    void finishedResize(const KUrl &orgUrl, const KUrl& emailUrl, int percent);
    void failedResize(const KUrl &orgUrl, const QString &errString, int percent);
    void completeResize();

private:

    ImageResizePriv* const d;
};

}  // namespace KIPISendimagesPlugin

#endif /* IMAGERESIZE_H */
