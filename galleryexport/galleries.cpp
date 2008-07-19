/* ============================================================
 * File  : galleries.cpp
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2006-09-04
 * Copyright 2006 by Colin Guthrie <kde@colin.guthr.ie>
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

// KDE includes
#include <QString>
#include <QWidget>
//Added by qt3to4:
#include <KDebug>
#include <KConfig>
#include <KConfigGroup>
#include <KLocale>
#include <KApplication>
#include <kwallet.h>

// local includes
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


Q3ListViewItem* Gallery::asQListViewItem(Q3ListView* pParent)
{
  Q3ListViewItem* p_lvi = (Q3ListViewItem*) new GalleryQListViewItem(this, pParent);
  return p_lvi;
}



GalleryQListViewItem::GalleryQListViewItem(Gallery* pGallery, Q3ListView* pParent)
  : Q3ListViewItem(pParent, pGallery->name(), pGallery->url(), pGallery->username()),
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
  mpWallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),
                                         kapp->activeWindow()->winId(),
                                         KWallet::Wallet::Synchronous);
  if (!mpWallet)
  {
    kWarning() << "Failed to open kwallet" << endl;
  }
  else
  {
    if (!mpWallet->hasFolder("KIPIGallerySyncPlugin"))
    {
      if (!mpWallet->createFolder("KIPIGallerySyncPlugin"))
        kWarning() << "Failed to create kwallet folder" << endl;
    }

    if (!mpWallet->setFolder("KIPIGallerySyncPlugin"))
      kWarning() << "Failed to set kwallet folder" << endl;
    else
      bln_use_wallet = true;
  }

    // TODO: system this. I really don't know if this should be good..

    // read config
    KConfig config("kipirc");
    KConfigGroup group = config.group("GallerySync Settings");
//    showPage(group.readEntry("GallerySync Galleries", 0));

    for (int i = 0; i < mGalleries.size(); i++)
    {
        QString name, url, username, password = "";
        int version;

        name = group.readEntry("Name%1", config.groupList() ).at(i);
        url = group.readEntry("URL%1", config.groupList() ).at(i);
        username = group.readEntry("Username%1", config.groupList() ).at(i);
        password = group.readEntry("Password%1", config.groupList() ).at(i);
        version = group.readEntry("Version%1", config.groupList() ).at(i).toInt();

        if (bln_use_wallet)
            mpWallet->readPassword(QString("Password%1").arg(i), password);

        Gallery gallery = Gallery(name, url, username, password, version, i);
        mGalleries.append(gallery);
    }
}


void Galleries::Add(Gallery& rGallery)
{
  mGalleries.append(rGallery);
}

void Galleries::Remove(Gallery& rGallery)
{
// TODO: system this
//  mGalleries.removeOne(rGallery);

  // Slight cosmetic thing for gallery numbering.
  if (mGalleries.isEmpty())
    mMaxGalleryId = 0;
}


void Galleries::Save()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("GallerySync Galleries");

    bool bln_use_wallet = false;
    if (mpWallet)
    {
        if (mpWallet->hasFolder("KIPIGallerySyncPlugin"))
        {
            if (!mpWallet->removeFolder("KIPIGallerySyncPlugin"))
            kWarning() << "Failed to clear kwallet folder" << endl;
        }
        if (!mpWallet->createFolder("KIPIGallerySyncPlugin"))
            kWarning() << "Failed to create kwallet folder" << endl;

        if (!mpWallet->setFolder("KIPIGallerySyncPlugin"))
            kWarning() << "Failed to set kwallet folder" << endl;
        else
            bln_use_wallet = true;
    }

    for (int i = 0; i < mGalleries.size(); i++)
    {
        group.writeEntry(QString("Name%1").arg(i), config.groupList() );
        group.writeEntry(QString("URL%1").arg(i), config.groupList() );
        group.writeEntry(QString("Username%1").arg(i), config.groupList() );
        group.writeEntry(QString("Version%1").arg(i), config.groupList() );
        
        if (bln_use_wallet)
            mpWallet->writePassword(QString("Password%1").arg(i), config.groupList().at(i) );
    
        group.writeEntry("Galleries", i);
    }
}

Q3ListView* Galleries::asQListView(QWidget* pParent)
{
  Load();

// TODO: system this..

//   Q3ListView* p_lv = new Q3ListView (pParent);
//   p_lv->addColumn(i18n("Gallery Name"));
//   p_lv->addColumn(i18n("URL"));
//   p_lv->addColumn(i18n("User"));
//   p_lv->setAllColumnsShowFocus(true);
// 
//   for (GalleryPtrList::iterator it = mGalleries.begin(); it != mGalleries.end(); ++it)
//   {
//     (*it)->asQListViewItem(p_lv);
//   }
// 
//   return p_lv;

// workaround
    Q3ListView *nude = new Q3ListView;
    return nude;
}

}
