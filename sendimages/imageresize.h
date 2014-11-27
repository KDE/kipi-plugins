/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-09
 * Description : a class to resize image in a separate thread.
 *
 * Copyright (C) 2007-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QString>
#include <QMutex>

// KDE includes

#include <kurl.h>
#include <threadweaver/Job.h>

// Libkdcraw includes

#include <libkdcraw/ractionthreadbase.h>

// Local includes

#include "emailsettings.h"

using namespace KDcrawIface;
using namespace ThreadWeaver;

namespace KIPISendimagesPlugin
{

class Task : public Job
{
    Q_OBJECT

public:

    explicit Task(QObject* const parent = 0, int* count = 0);
    ~Task();

public:

    KUrl          m_orgUrl;
    QString       m_destName;
    EmailSettings m_settings;
    int*          m_count;

Q_SIGNALS:

    void startingResize(const KUrl& orgUrl);
    void finishedResize(const KUrl& orgUrl, const KUrl& emailUrl, int percent);
    void failedResize(const KUrl& orgUrl, const QString& errString, int percent);

private:

    void run();
    bool imageResize(const EmailSettings& settings,
                     const KUrl& orgUrl, const QString& destName, QString& err);

private:

    QMutex m_mutex;
};

// ----------------------------------------------------------------------------------------------------

class ImageResize : public RActionThreadBase
{
    Q_OBJECT

public:

    explicit ImageResize(QObject* const parent);
    ~ImageResize();

    void resize(const EmailSettings& settings);
    void cancel();

Q_SIGNALS:

    void startingResize(const KUrl& orgUrl);
    void finishedResize(const KUrl& orgUrl, const KUrl& emailUrl, int percent);
    void failedResize(const KUrl& orgUrl, const QString& errString, int percent);
    void completeResize();

private Q_SLOTS:

    void slotFinished();

private:

    int* m_count;    // although it is private, it's address is passed to Task
};

}  // namespace KIPISendimagesPlugin

#endif /* IMAGERESIZE_H */
