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

#ifndef KIPIIMAGEINFOSHARED_H
#define KIPIIMAGEINFOSHARED_H

// libkipi includes

#include <libkipi/imageinfoshared.h>

class KipiImageInfoShared : public KIPI::ImageInfoShared
{
public:
    KipiImageInfoShared( KIPI::Interface* interface, const KUrl& url );
    ~KipiImageInfoShared();

    QString description();
    void setDescription( const QString& );

    QMap<QString,QVariant> attributes();
    void clearAttributes();
    void addAttributes( const QMap<QString,QVariant>& attributesToAdd );
    void delAttributes( const QStringList& attributesToDelete );

};

#endif // KIPIIMAGEINFOSHARED_H

