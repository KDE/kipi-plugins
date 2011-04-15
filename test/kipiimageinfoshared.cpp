/* ============================================================
 *
 * Copyright (C) 2009,2010 by Michael G. Hansen <mike at mghansen dot de>
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

#include "kipiimageinfoshared.h"

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// local includes:

#include "kipitest-debug.h"

class KipiImageInfoSharedPrivate
{
public:
    KipiImageInfoSharedPrivate()
    {
    }

    QDateTime dateTime;
};

KipiImageInfoShared::KipiImageInfoShared( KIPI::Interface* interface, const KUrl& url)
    : KIPI::ImageInfoShared(interface, url), d(new KipiImageInfoSharedPrivate())
{

}

KipiImageInfoShared::~KipiImageInfoShared()
{
    delete d;
}

QString KipiImageInfoShared::description()
{
    kipiDebug("QString KipiImageInfoShared::description()");
    return QString("Image located at \"%1\"").arg(path().url());
}

void KipiImageInfoShared::setDescription( const QString& newDescription)
{
    kipiDebug(QString("void KipiImageInfoShared::setDescription( \"%1\" )").arg(newDescription));
}

QMap<QString,QVariant> KipiImageInfoShared::attributes()
{
    kipiDebug("QMap<QString,QVariant> attributes()");
    return QMap<QString,QVariant>();
}
void KipiImageInfoShared::clearAttributes()
{
    kipiDebug("void KipiImageInfoShared::clearAttributes()");
}

void KipiImageInfoShared::addAttributes( const QMap<QString,QVariant>& attributesToAdd )
{
    Q_UNUSED(attributesToAdd);
    kipiDebug("void KipiImageInfoShared::addAttributes( const QMap<QString,QVariant>& attributesToAdd )");
}

void KipiImageInfoShared::delAttributes( const QStringList& attributesToDelete )
{
    Q_UNUSED(attributesToDelete);
    kipiDebug("void KipiImageInfoShared::delAttributes( const QStringList& attributesToDelete )");
}

QDateTime KipiImageInfoShared::time(KIPI::TimeSpec timeSpec)
{
    if (d->dateTime.isValid())
        return d->dateTime;

    if ( ! _url.isLocalFile() )
    {
        kFatal() << "KIPI::ImageInfoShared::time does not yet support non local files, please fix\n";
        return QDateTime();
    }
    else
    {
        KExiv2Iface::KExiv2* const exiv2Iface = new KExiv2Iface::KExiv2;
        exiv2Iface->load(_url.path());
        d->dateTime = exiv2Iface->getImageDateTime();
        delete exiv2Iface;

        if (!d->dateTime.isValid())
        {
            d->dateTime = ImageInfoShared::time(timeSpec);
        }

        return d->dateTime;
    }
}
