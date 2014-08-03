/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-01-26
 * Description : a progress bar with progress information dispatched to kipi host
 *
 * Copyright (C) 2012-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPPROGRESS_WIDGET_H
#define KPPROGRESS_WIDGET_H

// Qt includes

#include <QProgressBar>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPProgressWidget : public QProgressBar
{
    Q_OBJECT

public:

    KPProgressWidget(QWidget* const parent);
    ~KPProgressWidget();

    /** Call this method to start a new instance of progress notification into KIPI host application
     *  You can pass tite string to name progress item, and set it as cancelable. In this case,
     *  signalProgressCanceled() is fired when user press cancel button from KIPI host application.
     *  This item can also accept a thumbnail that you can change through progresssThumbnailChanged().
     *  For more details about Progress management through KIPI host application, look into Interface class.
     */
    void progressScheduled(const QString& title, bool canBeCanceled, bool hasThumb);

    /** Change thumbnail in progress notifier from KIPI host application
     */
    void progressThumbnailChanged(const QPixmap& thumb);

    /** Change status string in progress notifier from KIPI host application
     */
    void progressStatusChanged(const QString& status);

    /** Call this method to query KIPI host application that process is done.
     */
    void progressCompleted();

Q_SIGNALS:

    /** Fired when user press cancel button from KIPI host application progress notifier.
     */
    void signalProgressCanceled();

private Q_SLOTS:

    void slotValueChanged(int);
    void slotProgressCanceled(const QString& id);

private:

    class Private;
    Private* const d;
};

} // namespace KIPIPlugins

#endif /* KPPROGRESS_WIDGET_H */
