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

#include "kpprogresswidget.moc"

// Qt includes

#include <QString>

// LibKIPI includes

#include <libkipi/version.h>
#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

using namespace KIPI;

namespace KIPIPlugins
{

class KPProgressWidget::Private
{
public:

    Private()
        :iface(0)
    {
        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }
    }

    QString    progressId;
    Interface* iface;
};

KPProgressWidget::KPProgressWidget(QWidget* const parent)
    : QProgressBar(parent), d(new Private)
{
    connect(this, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));
}

KPProgressWidget::~KPProgressWidget()
{
    delete d;
}

void KPProgressWidget::slotValueChanged(int)
{
#if KIPI_VERSION >= 0x010500
    if (d->iface && d->iface->hasFeature(HostSupportsProgressBar))
    {
        float percents = ((float)value() / (float)maximum()) * 100.0;
        d->iface->progressValueChanged(d->progressId, percents);
    }
#endif // KIPI_VERSION >= 0x010500
}

void KPProgressWidget::progressCompleted()
{
#if KIPI_VERSION >= 0x010500
    if (d->iface && d->iface->hasFeature(HostSupportsProgressBar))
    {
        d->iface->progressCompleted(d->progressId);
    }
#endif // KIPI_VERSION >= 0x010500
}

void KPProgressWidget::progressThumbnailChanged(const QPixmap& thumb)
{
#if KIPI_VERSION >= 0x010500
    if (d->iface && d->iface->hasFeature(HostSupportsProgressBar))
    {
        d->iface->progressThumbnailChanged(d->progressId, thumb);
    }
#endif // KIPI_VERSION >= 0x010500
}

void KPProgressWidget::progressStatusChanged(const QString& status)
{
#if KIPI_VERSION >= 0x010500
    if (d->iface && d->iface->hasFeature(HostSupportsProgressBar))
    {
        d->iface->progressStatusChanged(d->progressId, status);
    }
#endif // KIPI_VERSION >= 0x010500
}

void KPProgressWidget::progressScheduled(const QString& title, bool canBeCanceled, bool hasThumb)
{
#if KIPI_VERSION >= 0x010500
    if (d->iface && d->iface->hasFeature(HostSupportsProgressBar))
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

void KPProgressWidget::slotProgressCanceled(const QString& id)
{
    if (d->progressId == id)
        emit signalProgressCanceled();
}

} // namespace KIPIPlugins
