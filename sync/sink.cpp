/* ============================================================
 * File  : sink.cpp
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2007-01-22
 *
 * Copyright 2007 Colin Guthrie <kde@colin.guthr.ie>
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

#include "sink.h"
#include "sinkfactory.h"
#include "libkipi2/collection.h"

namespace KIPISyncPlugin
{

Sink::Sink(unsigned int sinkId, QString name, KConfig*, KWallet::Wallet*)
 : mSinkId(sinkId),
   mName(name)
{
}

unsigned int Sink::SinkId()
{
  return mSinkId;
}

QString Sink::Name()
{
  return mName;
}
/*
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
*/

// Pretend sink (just for testin the structure - will be removed!)
Sink2::Sink2(unsigned int sinkId, QString name, KConfig* pConfig, KWallet::Wallet* pWallet)
 : Sink(sinkId, name, pConfig, pWallet)
{
  mName = "Sink2";
}

QString Sink2::Type() { return "Sink2"; }
QString Sink2::TypeDescription() { return "Sink2"; }

KIPI2::CollectionList* Sink2::getCollections()
{
  //KIPI2::Collection* tmp = new KIPI2::Collection;
  return (KIPI2::CollectionList*) NULL;
}

void Sink2::Save(KConfig*, KWallet::Wallet*)
{
  
}

void test()
{
  SinkFactory::Register("Sink2", SinkCreator<Sink2>);
  
  Sink* p_tmp = SinkFactory::Create("Sink2", 1, "Something", NULL, NULL);
  p_tmp = p_tmp;
}
}

//#include "sink.moc"
