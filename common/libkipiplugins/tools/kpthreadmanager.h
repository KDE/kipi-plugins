/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2011-12-28
 * Description : Low level threads management for batch processing on multi-core
 *
 * Copyright (C)      2014 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 * Copyright (C) 2011-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
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

#ifndef KP_THREAD_MNGR_H
#define KP_THREAD_MNGR_H

// Qt includes

#include <QThread>
#include <QRunnable>
#include <QObject>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPJob : public QObject,
                                 public QRunnable
{
    Q_OBJECT

public:

    /** Constructor which delegate deletion of QRunnable instance to KPThreadManager, not QThreadPool.
     */
    KPJob();

    /** Re-implement destructor in you implementation. Don't forget to cancel job.
     */
    virtual ~KPJob();

Q_SIGNALS:

    /** Use this signal in your implementation to inform KPThreadManager manager that job is started
     */
    void signalStarted();

    /** Use this signal in your implementation to inform KPThreadManager manager the job progress
     */
    void signalProgress(int);

    /** Use this signal in your implementation to inform KPThreadManager manager the job is done.
     */
    void signalDone();

public Q_SLOTS:

    /** Call this method to cancel job.
     */
    void cancel();

protected:

    /** You can use this boolean in your implementation to know if job must be canceled.
     */
    bool m_cancel;
};

/** Define a map of job/priority to process by KPThreadManager manager.
 *  Priority value can be used to control the run queue's order of execution.
 *  Zero priority want mean to process job with higher priority.
 */
typedef QMap<KPJob*, int> KPJobCollection;

// -------------------------------------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT KPThreadManager : public QThread
{
    Q_OBJECT

public:

    KPThreadManager(QObject* const parent=0);
    virtual ~KPThreadManager();

    /** Adjust maximum number of threads used to parallelize collection of job processing.
     */
    void setMaximumNumberOfThreads(int n);

    /** Return the maximum number of threads used to parallelize collection of job processing.
     */
    int  maximumNumberOfThreads() const;

    /** Reset maximum number of threads used to parallelize collection of job processing to max core detected on computer.
     *  This method is called in contructor.
     */
    void defaultMaximumNumberOfThreads();

    /** Cancel processing of current jobs under progress.
     */
    void cancel();

protected:

    /** Main thread loop used to process jobs in todo list.
     */
    void run() Q_DECL_OVERRIDE;

    /** Append a collection of jobs to process into QThreadPool.
     *  Jobs are add to pending lists and will be deleted by KPThreadManager, not QThreadPool.
     */
    void appendJobs(const KPJobCollection& jobs);

    /** Return true if list of pending jobs to process is empty.
     */
    bool isEmpty() const;

protected Q_SLOTS:

    void slotJobFinished();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIPlugins

#endif // KP_THREAD_MNGR_H
