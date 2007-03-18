/* ============================================================
 * File  : sink.h
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2007-01-22
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

#ifndef SINK_H
#define SINK_H

#include <qstring.h>
#include "libkipi2/interface.h"

#include <kconfig.h>
#include <kdeversion.h>
#if KDE_IS_VERSION(3,2,0)
#include <kwallet.h>
#endif

namespace KIPI2
{
class Collection;
} 

namespace KIPISyncPlugin
{

// Foreard definition
/// @todo Convert this to an Interface/Factory type setup.
class Sink : public KIPI2::Interface
{
public:
  Sink(unsigned int sinkId, QString name, KConfig* pConfig, KWallet::Wallet* pWallet);

  unsigned int SinkId();

  /// The Name of the Sink as chosen by the user.
  QString Name();
  
  /// An internal, unique type as specifed by the sink.
  virtual QString Type() = 0;
  
  /// Some information about the sink type for the benefit of the user.
  virtual QString TypeDescription() = 0;
  
  virtual const KIPI2::CollectionList* getCollections() = 0;

  virtual void Save(KConfig* pConfig, KWallet::Wallet* pWallet) = 0;
  
  // As some Sinks are remote, we have the concept of connect/disconnect
  virtual bool Connect() { return true; };
  
  virtual void Disconnect() { };
  
  //virtual void asQListViewItem(QListView* pParent) = 0;

protected:

  unsigned int mSinkId;
  QString mName;


};

// Play at creating a sink (this will be removed)
class Sink2 : Sink
{
public:
  Sink2(unsigned int sinkId, QString name, KConfig* pConfig, KWallet::Wallet* pWallet);
  QString Type();
  QString TypeDescription();
  KIPI2::CollectionList* getCollections();
  void Save(KConfig* pConfig, KWallet::Wallet* pWallet);
};


}
#endif /* SINK_H */
