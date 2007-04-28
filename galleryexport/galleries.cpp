/* ============================================================
 * File  : galleries.cpp
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2006-09-04
 * Copyright 2006 by Colin Guthrie <kde@colin.guthr.ie>
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

#include <qstring.h>

#include <qwidget.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdeversion.h>
#if KDE_IS_VERSION(3,2,0)
#include <kwallet.h>
#endif

#include "galleries.h"

namespace KIPIGalleryExportPlugin
{


Gallery::Gallery(const QString& name, const QString& url,
                 const QString& username, const QString& password,
                 const unsigned int version,
                 const unsigned int galleryId)
  : mName(name),
    mUrl(url),
    mUsername(username),
    mPassword(password),
    mVersion(version),
    mGalleryId(galleryId)
{

}

Gallery::~Gallery()
{

}

QString Gallery::name() const { return mName; }
QString Gallery::url() const { return mUrl; }
QString Gallery::username() const { return mUsername; }
QString Gallery::password() const { return mPassword; }
unsigned int Gallery::version() const { return mVersion; }
unsigned int Gallery::galleryId() const { return mGalleryId; }

void Gallery::setName(QString name) { mName = name; }
void Gallery::setUrl(QString url) { mUrl = url; }
void Gallery::setUsername(QString username) { mUsername = username; }
void Gallery::setPassword(QString password) { mPassword = password; }
void Gallery::setVersion(unsigned int version) { mVersion = version; }
void Gallery::setGalleryId(unsigned int galleryId) { mGalleryId = galleryId; }


QListViewItem* Gallery::asQListViewItem(QListView* pParent)
{
  QListViewItem* p_lvi = (QListViewItem*) new GalleryQListViewItem(this, pParent);
  return p_lvi;
}



GalleryQListViewItem::GalleryQListViewItem(Gallery* pGallery, QListView* pParent)
  : QListViewItem(pParent, pGallery->name(), pGallery->url(), pGallery->username()),
    mpGallery(pGallery)
{
}

Gallery* GalleryQListViewItem::GetGallery()
{
  return mpGallery;
}

void GalleryQListViewItem::Refresh()
{
  setText(0, mpGallery->name());
  setText(1, mpGallery->url());
  setText(2, mpGallery->username());
}



Galleries::Galleries()
 : mpWallet(0),
   mMaxGalleryId(0)
{
}

Galleries::~Galleries()
{
  if (mpWallet)
    delete mpWallet;

  // Todo: clear up mGalleries
}

void Galleries::Load()
{
  static bool bln_loaded = false;
  if (bln_loaded) return;
  bln_loaded = true;

  bool bln_use_wallet = false;
#if KDE_IS_VERSION(3,2,0)
  mpWallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),
                                         kapp->activeWindow()->winId(),
                                         KWallet::Wallet::Synchronous);
  if (!mpWallet)
  {
    kdWarning() << "Failed to open kwallet" << endl;
  }
  else
  {
    if (!mpWallet->hasFolder("KIPIGallerySyncPlugin"))
    {
      if (!mpWallet->createFolder("KIPIGallerySyncPlugin"))
        kdWarning() << "Failed to create kwallet folder" << endl;
    }

    if (!mpWallet->setFolder("KIPIGallerySyncPlugin"))
      kdWarning() << "Failed to set kwallet folder" << endl;
    else
      bln_use_wallet = true;
  }
#endif


  // read config
  KConfig config("kipirc");
  config.setGroup("GallerySync Settings");
  QValueList<int> gallery_ids = config.readIntListEntry("Galleries");

  config.setGroup("GallerySync Galleries");
  QString name, url, username, password = "";
  int version;
  for (QValueList<int>::Iterator it = gallery_ids.begin(); it != gallery_ids.end(); ++it)
  {
    unsigned int gallery_id = (*it);

    if (gallery_id > mMaxGalleryId)
      mMaxGalleryId = gallery_id;

    // Load the gallery with this id.
    name = config.readEntry(QString("Name%1").arg(gallery_id));
    url = config.readEntry(QString("URL%1").arg(gallery_id));
    username = config.readEntry(QString("Username%1").arg(gallery_id));
    version = config.readNumEntry(QString("Version%1").arg(gallery_id));
    if (bln_use_wallet)
      mpWallet->readPassword(QString("Password%1").arg(gallery_id), password);

    Gallery* p_gallery = new Gallery(name, url, username, password, version, gallery_id);
    mGalleries.append(p_gallery);
  }
}


void Galleries::Add(Gallery* pGallery)
{
  mGalleries.append(pGallery);
}

void Galleries::Remove(Gallery* pGallery)
{
  mGalleries.remove(pGallery);

  // Slight cosmetic thing for gallery numbering.
  if (mGalleries.isEmpty())
    mMaxGalleryId = 0;
}


void Galleries::Save()
{
  QValueList<int> gallery_ids;
  KConfig config("kipirc");
  config.deleteGroup("GallerySync Galleries");
  config.setGroup("GallerySync Galleries");

  bool bln_use_wallet = false;
  if (mpWallet)
  {
    if (mpWallet->hasFolder("KIPIGallerySyncPlugin"))
    {
      if (!mpWallet->removeFolder("KIPIGallerySyncPlugin"))
        kdWarning() << "Failed to clear kwallet folder" << endl;
    }
    if (!mpWallet->createFolder("KIPIGallerySyncPlugin"))
      kdWarning() << "Failed to create kwallet folder" << endl;

    if (!mpWallet->setFolder("KIPIGallerySyncPlugin"))
      kdWarning() << "Failed to set kwallet folder" << endl;
    else
      bln_use_wallet = true;
  }

  for (GalleryPtrList::iterator it = mGalleries.begin(); it != mGalleries.end(); ++it)
  {
    Gallery* p_gallery = (*it);
    if (!p_gallery->galleryId())
      p_gallery->setGalleryId(++mMaxGalleryId);
    unsigned int gallery_id = p_gallery->galleryId();
    gallery_ids.append(gallery_id);

    config.writeEntry(QString("Name%1").arg(gallery_id), p_gallery->name());
    config.writeEntry(QString("URL%1").arg(gallery_id), p_gallery->url());
    config.writeEntry(QString("Username%1").arg(gallery_id), p_gallery->username());
    config.writeEntry(QString("Version%1").arg(gallery_id), p_gallery->version());
    if (bln_use_wallet)
      mpWallet->writePassword(QString("Password%1").arg(gallery_id), p_gallery->password());
  }

  config.setGroup("GallerySync Settings");
  config.writeEntry("Galleries", gallery_ids);
}

QListView* Galleries::asQListView(QWidget* pParent)
{
  Load();

  QListView* p_lv = new QListView (pParent);
  p_lv->addColumn(i18n("Gallery Name"));
  p_lv->addColumn(i18n("URL"));
  p_lv->addColumn(i18n("User"));
  p_lv->setAllColumnsShowFocus(true);

  for (GalleryPtrList::iterator it = mGalleries.begin(); it != mGalleries.end(); ++it)
  {
    (*it)->asQListViewItem(p_lv);
  }

  return p_lv;
}

}
