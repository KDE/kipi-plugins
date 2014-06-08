/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-09-19
 * @brief  Thread for correlator for tracks and images
 *
 * @author Copyright (C) 2006-2013 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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

#ifndef TRACK_CORRELATOR_THREAD_H
#define TRACK_CORRELATOR_THREAD_H

// Qt includes

#include <QThread>

// local includes

#include "track_correlator.h"

namespace KIPIGPSSyncPlugin
{

class TrackCorrelatorThread : public QThread
{
    Q_OBJECT

public:

    TrackCorrelatorThread(QObject* const parent = 0);
    ~TrackCorrelatorThread();

public:

    TrackCorrelator::Correlation::List itemsToCorrelate;
    TrackCorrelator::CorrelationOptions options;
    KGeoMap::TrackManager::Track::List fileList;
    bool doCancel;
    bool canceled;

protected:

    virtual void run();

Q_SIGNALS:

    void signalItemsCorrelated(const KIPIGPSSyncPlugin::TrackCorrelator::Correlation::List& correlatedItems);
};

} // namespace KIPIGPSSyncPlugin

#endif  // TRACK_CORRELATOR_THREAD_H
