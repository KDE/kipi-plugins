/* ============================================================
 * File  : sinks.h
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

#ifndef SINKS_H
#define SINKS_H

#include <qptrlist.h>
#include <qlistview.h>

// Some sinks may wish to make use of the Wallet to store sensitive
// information so we manage it here.
namespace KWallet
{
class Wallet;
}

namespace KIPISyncPlugin
{

/// @todo Convert this to an Interface/Factory type setup.
class Sink
{

public:

  Sink(const QString& name = i18n("New Sink"),
       const QString& url = QString("http://www.newsink.com/"),
       const QString& username = QString(),
       const QString& password = QString(),
       const unsigned int version = 2,
       const unsigned int sinkId = 0);
  ~Sink();

  QString name()           const;
  QString url()            const;
  QString username()       const;
  QString password()       const;
  unsigned int version()   const;
  unsigned int sinkId() const;

  void setName(QString name);
  void setUrl(QString url);
  void setUsername(QString username);
  void setPassword(QString password);
  void setVersion(unsigned int version);
  void setSinkId(unsigned int sinkId);

  void asQListViewItem(QListView* pParent);

private:

  QString mName;
  QString mUrl;
  QString mUsername;
  QString mPassword;
  unsigned int mVersion;

  unsigned int mSinkId;

};


/* Simple Stub Class to allow easy access to Sinks from GUI elements */
class SinkQListViewItem : public QListViewItem
{

public:
  SinkQListViewItem(Sink* pSink, QListView* pParent);

  Sink* GetSink();
  void Refresh();

private:
  Sink* mpSink;

};


typedef QPtrList<Sink> SinkPtrList;

/* Container class for all Sinks */
class Sinks
{

public:
  Sinks();
  ~Sinks();

  void Add(Sink* pSink);
  void Remove(Sink* pSink);
  void Save();
  void asQListView(QListView* pListView);

private:
  void Load();

  KWallet::Wallet* mpWallet;

  SinkPtrList mSinks;
  unsigned int mMaxSinkId;
};


}

#endif /* SINKS_H */
