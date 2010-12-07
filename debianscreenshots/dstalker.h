/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#ifndef DSTALKER_H
#define DSTALKER_H

// KDE includes

#include <KIO/Job>

namespace KIPIDebianScreenshotsPlugin
{

class DsTalker : public QObject
{
    Q_OBJECT

public:

    DsTalker(QWidget* parent);
    ~DsTalker();

    bool addScreenshot(const QString& imgPath, const QString& packageName,
                       const QString& packageVersion = QString(),
                       const QString& description = QString() );

Q_SIGNALS:

     void signalBusy(bool val);
     void signalAddScreenshotDone(int errCode, const QString& errMsg);

private Q_SLOTS:

     void data(KIO::Job* job, const QByteArray& data);
     void slotResult(KJob* job);

private:

    QWidget*        m_parent;

    QByteArray      m_buffer;

    QString         m_userAgent;
    QString         m_uploadUrl;
    KIO::Job*       m_job;
};

} // namespace KIPIDebianScreenshotsPlugin

#endif /* DSTALKER_H */
