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

// local includes:

#include "kipitest-debug.h"


KipiImageInfoShared::KipiImageInfoShared( KIPI::Interface* interface, const KUrl& url)
    : KIPI::ImageInfoShared(interface, url)
{

}

KipiImageInfoShared::~KipiImageInfoShared()
{

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
