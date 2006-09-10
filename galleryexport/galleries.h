/* ============================================================
 * File  : galleries.h
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2006-09-04
 * Copyright 2006 by Colin Guthrie
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

#include <qptrlist.h>
#include <qlistview.h>

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

    QListViewItem* asQListViewItem(QListView* pParent);

private:

  QString mName;
  QString mUrl;
  QString mUsername;
  QString mPassword;
  unsigned int mVersion;

  unsigned int mGalleryId;
};


/* Simple Stub Class to allow easy access to Galleries from GUI elements */
class GalleryQListViewItem : public QListViewItem
{
public:
  GalleryQListViewItem(Gallery* pGallery, QListView* pParent);

  Gallery* GetGallery();
  void Refresh();
private:
  Gallery* mpGallery;
};


typedef QPtrList<Gallery> GalleryPtrList;

/* Container class for all Galleries */
class Galleries
{
public:
  Galleries();
  ~Galleries();

  void Add(Gallery* pGallery);
  void Remove(Gallery* pGallery);
  void Save();
  QListView* asQListView(QWidget* pParent);

private:
  void Load();

  KWallet::Wallet* mpWallet;

  GalleryPtrList mGalleries;
  unsigned int mMaxGalleryId;
};


}

#endif /* GALLERIES_H */
