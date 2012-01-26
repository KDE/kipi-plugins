/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-01-26
 * Description : a progress bar with progress information dispatched to kipi host
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PROGRESS_WIDGET_H
#define PROGRESS_WIDGET_H

// Qt includes

#include <QProgressBar>

// Local includes

#include "kipiplugins_export.h"

namespace KIPI
{
    class Interface;
}

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT ProgressWidget : public QProgressBar
{
    Q_OBJECT

public:

    ProgressWidget(KIPI::Interface* iface, QWidget* parent);
    ~ProgressWidget();

    void progressScheduled(const QString& title, bool canBeCanceled, bool hasThumb);
    void progresssThumbnailChanged(const QPixmap& thumb);
    void progressStatusChanged(const QString& status);
    void progressCompleted();

Q_SIGNALS:

    void signalProgressCanceled();

private Q_SLOTS:

    void slotValueChanged(int);
    void slotProgressCanceled(const QString& id);

private:

    class ProgressWidgetPriv;
    ProgressWidgetPriv* const d;
};

} // namespace KIPIPlugins

#endif /* PROGRESS_WIDGET_H */
