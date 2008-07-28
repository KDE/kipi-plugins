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

    Gallery(const QString& name = i18n("New Gallery"),
            const QString& url = QString("http://www.newgallery.com/"),
            const QString& username = QString(),
            const QString& password = QString(),
            const unsigned int version = 2,
            const unsigned int galleryId = 0);
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

    QTreeWidgetItem* asQTreeWidgetItem(QTreeWidget* pParent);

private:

    QString mName;
    QString mUrl;
    QString mUsername;
    QString mPassword;
    unsigned int mVersion;

    unsigned int mGalleryId;
};


/* Simple Stub Class to allow easy access to Galleries from GUI elements */
class GalleryQTreeWidgetItem : public QTreeWidgetItem
{
public:
    GalleryQTreeWidgetItem(Gallery* pGallery, QTreeWidget* pParent);

    Gallery* GetGallery();
    void Refresh();
private:
    Gallery* mpGallery;
};


typedef QList<Gallery> GalleryPtrList;

/* Container class for all Galleries */
class Galleries
{
public:
    Galleries();
    ~Galleries();

    void Add(Gallery& rGallery);
    void Remove(Gallery& rGallery);
    void Save();
    QTreeWidget* asQTreeWidget(QWidget* pParent);

private:
    void Load();

    KWallet::Wallet* mpWallet;

    GalleryPtrList mGalleries;
    unsigned int mMaxGalleryId;
};


}

#endif /* GALLERIES_H */
