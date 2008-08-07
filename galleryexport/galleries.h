/* ============================================================
 * File  : galleries.h
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2006-09-04
 * Copyright 2006 by Colin Guthrie <kde@colin.guthr.ie>
 *
 *
 * Modified by : Andrea Diamantini <adjam7@gmail.com>
 * Date        : 2008-07-11
 * Copyright 2008 by Andrea Diamantini <adjam7@gmail.com>
 *
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
 * ============================================================ */

#ifndef GALLERIES_H
#define GALLERIES_H

// Qt includes
#include <QList>
#include <QTreeWidget>

namespace KWallet
{
class Wallet;
}

namespace KIPIGalleryExportPlugin
{

class Gallery
{
public:

    Gallery();
    ~Gallery();

    QString name()           const;
    QString url()            const;
    QString username()       const;
    QString password()       const;
    unsigned int version()   const;
    unsigned int galleryId() const;

    void setName(QString name);
    void setUrl(QString url);
    void setUsername(QString username);
    void setPassword(QString password);
    void setVersion(unsigned int version);
    void setGalleryId(unsigned int galleryId);

private:

    QString mName;
    QString mUrl;
    QString mUsername;
    QString mPassword;
    unsigned int mVersion;
    unsigned int mGalleryId;

public:

    void save();

private:

    void load();
    KWallet::Wallet* mpWallet;

};

}

#endif /* GALLERIES_H */
