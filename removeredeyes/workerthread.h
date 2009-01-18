/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : the calculation thread for red-eye removal
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

// Qt includes.

#include <QThread>

// KDE includes.

#include <kurl.h>

namespace KIPIRemoveRedEyesPlugin
{

struct WorkerThreadPriv;
class WorkerThreadData;
class RemovalSettings;
class SaveMethodAbstract;

class WorkerThread : public QThread
{

    Q_OBJECT

public:

    enum RunType
    {
        Testrun = 0,
        Correction,
        Preview
    };

    enum ImageType
    {
        OriginalImage = 0,
        CorrectedImage,
        MaskImage
    };

signals:

    void calculationFinished(WorkerThreadData*);

public:

    WorkerThread(QObject* parent);
    ~WorkerThread();

    void setRunType(int);
    int runType() const;

    void setSaveMethod(SaveMethodAbstract* method);

    void loadSettings(RemovalSettings);
    void setImagesList(const KUrl::List&);
    void setTempFile(const QString&, ImageType);

    void cancel();
    void run();

private:

    WorkerThreadPriv* const d;
};
} // namespace KIPIRemoveRedEyesPlugin

#endif
