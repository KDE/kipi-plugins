/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-29
 * Description : a kipi plugin to export images to Debian Screenshots
 *
 * Copyright (C) 2010 by Pau Garcia i Quiles <pgquiles at elpauer dot org>
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

#ifndef DEBIANSCREENSHOTS_JOB_H
#define DEBIANSCREENSHOTS_JOB_H

// KDE includes

#include <kjob.h>
#include <kurl.h>
#include <kicon.h>

namespace KIPIDebianScreenshotsPlugin
{

class DebianScreenshotsJob : public KJob
{
    Q_OBJECT

public:

    DebianScreenshotsJob(const QString& packageName, const KUrl::List& url, QObject* const parent=0);

    virtual void start();
    virtual QList< KUrl > urls() const;
    virtual KIcon icon() const;

private slots:

    void addScreenshot(int code, const QString& message);

private:

    void sendScreenshot(const QString& package);

private:

    KUrl::List m_urls;
    QString    m_packageName;
};

} // namespace KIPIDebianScreenshotsPlugin

#endif // DEBIANSCREENSHOTS_JOB_H
