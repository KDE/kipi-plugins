/* ============================================================
 * File  : sinks.cpp
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2007-01-14
 *
 * Copyright 2007 Colin Guthrie
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

#include "sinks.h"

namespace KIPISyncPlugin
{


Sink::Sink(const QString& name, const QString& url,
           const QString& username, const QString& password,
           const unsigned int version,
           const unsigned int sinkId)
  : mName(name),
    mUrl(url),
    mUsername(username),
    mPassword(password),
    mVersion(version),
    mSinkId(sinkId)
{

}

Sink::~Sink()
{

}

QString Sink::name() const { return mName; }
QString Sink::url() const { return mUrl; }
QString Sink::username() const { return mUsername; }
QString Sink::password() const { return mPassword; }
unsigned int Sink::version() const { return mVersion; }
unsigned int Sink::sinkId() const { return mSinkId; }

void Sink::setName(QString name) { mName = name; }
void Sink::setUrl(QString url) { mUrl = url; }
void Sink::setUsername(QString username) { mUsername = username; }
void Sink::setPassword(QString password) { mPassword = password; }
void Sink::setVersion(unsigned int version) { mVersion = version; }
void Sink::setSinkId(unsigned int sinkId) { mSinkId = sinkId; }


void Sink::asQListViewItem(QListView* pParent)
{
  new SinkQListViewItem(this, pParent);
}



SinkQListViewItem::SinkQListViewItem(Sink* pSink, QListView* pParent)
  : QListViewItem(pParent, pSink->name(), pSink->url(), pSink->username()),
    mpSink(pSink)
{
}

Sink* SinkQListViewItem::GetSink()
{
  return mpSink;
}

void SinkQListViewItem::Refresh()
{
  setText(0, mpSink->name());
  setText(1, mpSink->url());
  setText(2, mpSink->username());
}



Sinks::Sinks()
 : mpWallet(NULL),
   mMaxSinkId(0)
{
}


Sinks::~Sinks()
{
  if (mpWallet)
    delete mpWallet;

  // Todo: clear up mSinks
}


/// @todo Abstract this to per-sink-type load
void Sinks::Load()
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
    if (!mpWallet->hasFolder("KIPISyncPlugin"))
    {
      if (!mpWallet->createFolder("KIPISyncPlugin"))
        kdWarning() << "Failed to create kwallet folder" << endl;
    }

    if (!mpWallet->setFolder("KIPISyncPlugin"))
      kdWarning() << "Failed to set kwallet folder" << endl;
    else
      bln_use_wallet = true;
  }
#endif

  // Read config
  KConfig config("kipirc");
  config.setGroup("Sync Settings");
  QValueList<int> sink_ids = config.readIntListEntry("Sinks");

  config.setGroup("Sync Sinks");
  QString name, url, username, password = "";
  int version;
  for (QValueList<int>::Iterator it = sink_ids.begin(); it != sink_ids.end(); ++it)
  {
    unsigned int sink_id = (*it);

    if (sink_id > mMaxSinkId)
      mMaxSinkId = sink_id;

    // Partially load the sink with this id to find out what type it is.
    name = config.readEntry(QString("Name%1").arg(sink_id));
    url = config.readEntry(QString("URL%1").arg(sink_id));
    username = config.readEntry(QString("Username%1").arg(sink_id));
    version = config.readNumEntry(QString("Version%1").arg(sink_id));
    if (bln_use_wallet)
      mpWallet->readPassword(QString("Password%1").arg(sink_id), password);

    Sink* p_sink = new Sink(name, url, username, password, version, sink_id);
    mSinks.append(p_sink);
  }
}


void Sinks::Add(Sink* pSink)
{
  mSinks.append(pSink);
}

void Sinks::Remove(Sink* pSink)
{
  mSinks.remove(pSink);

  // Slight cosmetic thing for sink numbering.
  if (mSinks.isEmpty())
    mMaxSinkId = 0;
}


/// @todo Abstract this to per-sink-type save
void Sinks::Save()
{
  QValueList<int> sink_ids;
  KConfig config("kipirc");
  config.deleteGroup("Sync Sinks");
  config.setGroup("Sync Sinks");

  bool bln_use_wallet = false;
  if (mpWallet)
  {
    if (mpWallet->hasFolder("KIPISyncPlugin"))
    {
      if (!mpWallet->removeFolder("KIPISyncPlugin"))
        kdWarning() << "Failed to clear kwallet folder" << endl;
    }
    if (!mpWallet->createFolder("KIPISyncPlugin"))
      kdWarning() << "Failed to create kwallet folder" << endl;

    if (!mpWallet->setFolder("KIPISyncPlugin"))
      kdWarning() << "Failed to set kwallet folder" << endl;
    else
      bln_use_wallet = true;
  }

  for (SinkPtrList::iterator it = mSinks.begin(); it != mSinks.end(); ++it)
  {
    Sink* p_sink = (*it);

    // Has this sink been saved before?
    if (!p_sink->sinkId())
      p_sink->setSinkId(++mMaxSinkId);

    unsigned int sink_id = p_sink->sinkId();
    sink_ids.append(sink_id);

    config.writeEntry(QString("Name%1").arg(sink_id), p_sink->name());
    config.writeEntry(QString("URL%1").arg(sink_id), p_sink->url());
    config.writeEntry(QString("Username%1").arg(sink_id), p_sink->username());
    config.writeEntry(QString("Version%1").arg(sink_id), p_sink->version());
    if (bln_use_wallet)
      mpWallet->writePassword(QString("Password%1").arg(sink_id), p_sink->password());
  }

  config.setGroup("Sync Settings");
  config.writeEntry("Sinks", sink_ids);
}

void Sinks::asQListView(QListView* pListView)
{
  Load();

  pListView->clear();
  for (SinkPtrList::iterator it = mSinks.begin(); it != mSinks.end(); ++it)
  {
    (*it)->asQListViewItem(pListView);
  }
}

}
