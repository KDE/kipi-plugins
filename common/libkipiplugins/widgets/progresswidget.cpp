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

#include "progresswidget.moc"

// Qt includes

#include <QString>

// LibKIPI includes

#include <libkipi/version.h>
#include <libkipi/interface.h>

namespace KIPIPlugins
{

class ProgressWidget::ProgressWidgetPriv
{
public:

    ProgressWidgetPriv()
    {
        iface = 0;
    }

    QString          progressId;
    KIPI::Interface* iface;
};

ProgressWidget::ProgressWidget(KIPI::Interface* iface, QWidget* parent)
    : QProgressBar(parent), d(new ProgressWidgetPriv)
{
    d->iface = iface;

    connect(this, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));
}

ProgressWidget::~ProgressWidget()
{
    delete d;
}

void ProgressWidget::slotValueChanged(int)
{
#if KIPI_VERSION >= 0x010500
    if (d->iface && d->iface->hasFeature(KIPI::HostSupportsProgressBar))
    {
        float percents = ((float)value() / (float)maximum()) * 100.0;
        d->iface->progressValueChanged(d->progressId, percents);
    }
#endif // KIPI_VERSION >= 0x010500
}

void ProgressWidget::progressCompleted()
{
#if KIPI_VERSION >= 0x010500
    if (d->iface && d->iface->hasFeature(KIPI::HostSupportsProgressBar))
    {
        d->iface->progressCompleted(d->progressId);
    }
#endif // KIPI_VERSION >= 0x010500
}

void ProgressWidget::progressThumbnailChanged(const QPixmap& thumb)
{
#if KIPI_VERSION >= 0x010500
    if (d->iface && d->iface->hasFeature(KIPI::HostSupportsProgressBar))
    {
        d->iface->progressThumbnailChanged(d->progressId, thumb);
    }
#endif // KIPI_VERSION >= 0x010500
}

void ProgressWidget::progressStatusChanged(const QString& status)
{
#if KIPI_VERSION >= 0x010500
    if (d->iface && d->iface->hasFeature(KIPI::HostSupportsProgressBar))
    {
        d->iface->progressStatusChanged(d->progressId, status);
    }
#endif // KIPI_VERSION >= 0x010500
}

void ProgressWidget::progressScheduled(const QString& title, bool canBeCanceled, bool hasThumb)
{
#if KIPI_VERSION >= 0x010500
    if (d->iface && d->iface->hasFeature(KIPI::HostSupportsProgressBar))
    {
        d->progressId = d->iface->progressScheduled(title, canBeCanceled, hasThumb);

        if (canBeCanceled)
        {
            connect(d->iface, SIGNAL(progressCanceled(QString)),
                    this, SLOT(slotProgressCanceled(QString)));
        }
    }
#endif // KIPI_VERSION >= 0x010500
}

void ProgressWidget::slotProgressCanceled(const QString& id)
{
    if (d->progressId == id)
        emit signalProgressCanceled();
}

} // namespace KIPIPlugins
