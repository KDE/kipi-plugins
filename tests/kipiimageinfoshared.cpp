/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-11-21
 * @brief  kipi host test application
 *
 * @author Copyright (C) 2009-2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 * @author Copyright (C) 2011-2012 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
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

#include "kipiimageinfoshared.h"

// Qt includes

#include <QFileInfo>

// local includes:

#include "kipitest-debug.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;

class KipiImageInfoShared::KipiImageInfoSharedPrivate
{
public:

    KipiImageInfoSharedPrivate()
    {
    }

    QDateTime dateTime;
};

KipiImageInfoShared::KipiImageInfoShared(Interface* const interface, const KUrl& url)
    : ImageInfoShared(interface, url),
      d(new KipiImageInfoSharedPrivate())
{
}

KipiImageInfoShared::~KipiImageInfoShared()
{
    delete d;
}

QMap<QString, QVariant> KipiImageInfoShared::attributes()
{
    kipiDebug("QMap<QString,QVariant> attributes()");

    QMap<QString, QVariant> res;

    // Comment attribute
    res["comment"] = QString("Image located at \"%1\"").arg(_url.url());

    // Date attribute
    if (!d->dateTime.isValid())
    {
        if ( ! _url.isLocalFile() )
        {
            kFatal() << "KIPI::ImageInfoShared::time does not yet support non local files, please fix\n";
            d->dateTime = QDateTime();
        }
        else
        {
            KPMetadata meta;
            meta.load(_url.path());
            d->dateTime = meta.getImageDateTime();

            if (!d->dateTime.isValid())
            {
                d->dateTime = QFileInfo( _url.toLocalFile() ).lastModified();
            }
        }
    }
    res["date"] = d->dateTime;

    return res;
}
void KipiImageInfoShared::clearAttributes()
{
    kipiDebug("void KipiImageInfoShared::clearAttributes()");
}

void KipiImageInfoShared::addAttributes(const QMap<QString, QVariant>& attributes)
{
    kipiDebug("void KipiImageInfoShared::addAttributes()");

    QMap<QString, QVariant>::const_iterator it = attributes.constBegin();
    while (it != attributes.constEnd())
    {
        QString key = it.key();
        QString val = it.value().toString();
        kipiDebug(QString("attribute( \"%1\" ), value( \"%2\" )").arg(key).arg(val));
        ++it;
    }
}

void KipiImageInfoShared::delAttributes(const QStringList& attributes)
{
    kipiDebug("void KipiImageInfoShared::delAttributes()");
    kipiDebug(QString("attributes : \"%1\"").arg(attributes.join(", ")));
}
