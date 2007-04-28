/* ============================================================
 * File  : sinkfactory.cpp
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2007-01-22
 *
 * Copyright 2007 by Colin Guthrie <kde@colin.guthr.ie>
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

#include "sinkfactory.h"

namespace KIPISyncPlugin
{

// Define the static stack variables
SinkMap SinkFactory::mSinkProxies;

// Define the register method
bool SinkFactory::Register(QString type, SinkProxy* pSinkProxy)
{
  if (mSinkProxies.contains(type))
    return false;
  
  mSinkProxies[type] = pSinkProxy;
  return true;
}

// The main creation method
Sink* SinkFactory::Create(QString type, unsigned int sinkId, QString name, KConfig* pConfig, KWallet::Wallet* pWallet)
{
  if (!mSinkProxies.contains(type))
    return NULL;
  
  return (*(mSinkProxies[type]))(sinkId, name, pConfig, pWallet);
}

}
