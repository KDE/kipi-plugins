/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-09-19
 * @brief  Correlator for tracks and images
 *
 * @author Copyright (C) 2006-2015 by Gilles Caulier
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

#include "track_correlator.h"

// KDE includes

#include <klocalizedstring.h>

// local includes

#include "kipiplugins_debug.h"
#include "track_correlator_thread.h"

namespace KIPIGeolocatorPlugin
{

class TrackCorrelator::Private
{
public:

    Private()
      : trackManager(0),
        thread(0)
    {

    }

    KGeoMap::TrackManager* trackManager;
    TrackCorrelatorThread* thread;
};

TrackCorrelator::TrackCorrelator(KGeoMap::TrackManager* const trackManager, QObject* const parent)
    : QObject(parent), d(new Private())
{
    d->trackManager = trackManager;

    qRegisterMetaType<KIPIGeolocatorPlugin::TrackCorrelator::Correlation::List>("KIPIGeolocatorPlugin::TrackCorrelator::Correlation::List");
}

TrackCorrelator::~TrackCorrelator()
{

}

/**
 * @brief GPS-correlate items
 */
void TrackCorrelator::correlate(const Correlation::List& itemsToCorrelate, const CorrelationOptions& options)
{
    d->thread                   = new TrackCorrelatorThread(this);
    d->thread->options          = options;
    d->thread->fileList         = d->trackManager->getTrackList();
    d->thread->itemsToCorrelate = itemsToCorrelate;

    connect(d->thread, SIGNAL(signalItemsCorrelated(KIPIGeolocatorPlugin::TrackCorrelator::Correlation::List)),
            this, SLOT(slotThreadItemsCorrelated(KIPIGeolocatorPlugin::TrackCorrelator::Correlation::List)), Qt::QueuedConnection);

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(slotThreadFinished()), Qt::QueuedConnection);

    d->thread->start();
}

void TrackCorrelator::slotThreadItemsCorrelated(const Correlation::List& correlatedItems)
{
    emit(signalItemsCorrelated(correlatedItems));
}

void TrackCorrelator::slotThreadFinished()
{
    const bool threadCanceled = d->thread->canceled;
    delete d->thread;
    d->thread                 = 0;

    if (threadCanceled)
    {
        emit(signalCorrelationCanceled());
    }
    else
    {
        emit(signalAllItemsCorrelated());
    }
}

void TrackCorrelator::cancelCorrelation()
{
    if (d->thread)
    {
        d->thread->doCancel = true;
    }
}

} // namespace KIPIGeolocatorPlugin
