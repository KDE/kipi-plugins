//////////////////////////////////////////////////////////////////////////////
//
//    Copyright (C) 2006 Jesper Pedersen <blackie at kde.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#include "actions.h"

/**
 * \brief Send a status message to the main thread.
 * The worker thread communicates status messages to the main thread by posting events to it, just as it should.
 * There is just one problem: When the main thread gets a message it needs to update a listbox, which is
 * rather expensive, The worker thread thus easily floods the main thread.
 * The result of this flooding is that the GUI doesn't update very well, and as a consequence looks crappy.
 *
 * To overcome this problem, we will only send status messages at most every 50 msec.
 */
void KIPIFindDupplicateImagesPlugin::sendMessage( QObject* receiver, const Action& action, const QString & fileName,
                                                  int total, bool starting, bool succes )
{
    static QTime time;
    static int count = 0;
    if (starting)
        ++count;

    if ( time.elapsed() > 50 || action == KIPIFindDupplicateImagesPlugin::Progress ||
         (!starting && !succes ) /*error messages*/ ) {
        KIPIFindDupplicateImagesPlugin::EventData *d = new KIPIFindDupplicateImagesPlugin::EventData;
        d->action = action;
        d->fileName = fileName;
        d->total = total;
        d->count = count;
        d->starting = starting;
        d->success = succes;
        QApplication::postEvent( receiver, new QCustomEvent(QEvent::User, d));
        time.restart();
    }
}
