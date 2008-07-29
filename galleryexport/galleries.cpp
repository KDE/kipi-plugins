/* ============================================================
 * File  : galleries.cpp
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

// Qt includes
#include <QString>
#include <QWidget>

// KDE includes
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

QString Gallery::name() const
{
    return mName;
}

QString Gallery::url() const
{
    return mUrl;
}

QString Gallery::username() const
{
    return mUsername;
}

QString Gallery::password() const
{
    return mPassword;
}

unsigned int Gallery::version() const
{
    return mVersion;
}

unsigned int Gallery::galleryId() const
{
    return mGalleryId;
}

// -------------------------------------

void Gallery::setName(QString name)
{
    mName = name;
}

void Gallery::setUrl(QString url)
{
    mUrl = url;
}

void Gallery::setUsername(QString username)
{
    mUsername = username;
}

void Gallery::setPassword(QString password)
{
    mPassword = password;
}

void Gallery::setVersion(unsigned int version)
{
    mVersion = version;
}

void Gallery::setGalleryId(unsigned int galleryId)
{
    mGalleryId = galleryId;
}

// --------------------------------------------------------

QTreeWidgetItem* Gallery::asQTreeWidgetItem(QTreeWidget* pParent)
{
    QTreeWidgetItem* p_lvi = (QTreeWidgetItem*) new GalleryQTreeWidgetItem(this, pParent);
    return p_lvi;
}



GalleryQTreeWidgetItem::GalleryQTreeWidgetItem(Gallery* pGallery, QTreeWidget* pParent)
        : QTreeWidgetItem(pParent),
        mpGallery(pGallery)
{
}

Gallery* GalleryQTreeWidgetItem::getGallery()
{
    return mpGallery;
}

void GalleryQTreeWidgetItem::refresh()
{
    setText(0, mpGallery->name());
    setText(1, mpGallery->url());
    setText(2, mpGallery->username());
}



Galleries::Galleries()
        : mpWallet(0),
        mMaxGalleryId(0)
{
	load();		// se non ci metto questo, non funzioneràmai...
}

Galleries::~Galleries()
{
    if (mpWallet)
        delete mpWallet;

    // Todo: clear up mGalleries
}

void Galleries::load()
{
    static bool bln_loaded = false;
    if (bln_loaded) return;
    bln_loaded = true;

    bool bln_use_wallet = false;
    mpWallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),
                                           kapp->activeWindow()->winId(),
                                           KWallet::Wallet::Synchronous);
    if (!mpWallet) {
        kWarning() << "Failed to open kwallet" << endl;
    } else {
        if (!mpWallet->hasFolder("KIPIGallerySyncPlugin")) {
            if (!mpWallet->createFolder("KIPIGallerySyncPlugin"))
                kWarning() << "Failed to create kwallet folder" << endl;
        }

        if (!mpWallet->setFolder("KIPIGallerySyncPlugin"))
            kWarning() << "Failed to set kwallet folder" << endl;
        else
            bln_use_wallet = true;
    }

    // read config
    KConfig config("kipirc");
    KConfigGroup group = config.group("Gallery Settings");

    for (int i = 0; i < mGalleries.size(); i++) {
        QString name, url, username, password = "";
        int version;

        name = group.readEntry("Name%1", config.groupList()).at(i);
        url = group.readEntry("URL%1", config.groupList()).at(i);
        username = group.readEntry("Username%1", config.groupList()).at(i);
        password = group.readEntry("Password%1", config.groupList()).at(i);
        version = group.readEntry("Version%1", config.groupList()).at(i).toInt();

        if (bln_use_wallet)
            mpWallet->readPassword(QString("Password%1").arg(i), password);

        Gallery gallery = Gallery(name, url, username, password, version, i);
        mGalleries.append(gallery);
    }
}


void Galleries::add(Gallery& rGallery)
{
    mGalleries.append(rGallery);
}

// FIXME remove gallery NOT index!!
void Galleries::remove(Gallery& rGallery)
{
    mGalleries.removeAt(0);

    // Slight cosmetic thing for gallery numbering.
    if (mGalleries.isEmpty())
        mMaxGalleryId = 0;
}


void Galleries::save()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Gallery Settings");

    bool bln_use_wallet = false;
    if (mpWallet) {
        if (mpWallet->hasFolder("KIPIGallerySyncPlugin")) {
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

    for (int i = 0; i < mGalleries.size(); i++) {
        group.writeEntry(QString("Name"), mGalleries.at(i).name());
        group.writeEntry(QString("URL"), mGalleries.at(i).url());
        group.writeEntry(QString("Username"), mGalleries.at(i).username());
        group.writeEntry(QString("Version"), mGalleries.at(i).version());

        if (bln_use_wallet)
            mpWallet->writePassword(QString("Password") , mGalleries.at(i).password());

        config.sync();
    }
}

QTreeWidget* Galleries::asQTreeWidget(QWidget* pParent)
{
    load();

    QTreeWidget* p_lv = new QTreeWidget(pParent);
    QStringList header;
    header << i18n("Gallery Name") << i18n("URL") << i18n("User") ;
    p_lv->setHeaderLabels(header);
//   p_lv->addItem(i18n("Gallery Name"));
//   p_lv->addItem(i18n("URL"));
//   p_lv->addItem(i18n("User"));

//  FIXME p_lv->setAllColumnsShowFocus(true);

// FIXME : full QTreeWidget
    for (int i = 0; i < mGalleries.size() ; ++i) {
        Gallery item = mGalleries.at(i);
        item.asQTreeWidgetItem(p_lv);
    }

//    for (GalleryPtrList::iterator it = mGalleries.begin(); it != mGalleries.end(); ++it)
//    {
//      it.asQTreeWidgetItem(p_lv);
//    }

    return p_lv;

}

}
