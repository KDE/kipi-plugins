/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
 *
 * ============================================================ */

#ifndef ACTIONS_H
#define ACTIONS_H

// Qt includes

#include <QString>
#include <QImage>
#include <QMetaType>
#include <QMap>

// KDE includes

#include <kurl.h>

// Local includes

#include "enfusesettings.h"
#include "pfshdrsettings.h"

namespace KIPIExpoBlendingPlugin
{

enum Action
{
    NONE = 0,
    IDENTIFY,
    PREPROCESSING,
    ENFUSEPREVIEW,
    ENFUSEFINAL,
    LOAD,
    HDRGEN,
    CAMERARESPONSE,
    HDRCALIBRATEPREVIEW,
    HDRCALIBRATEFINAL
};

class ItemPreprocessedUrls
{
public:

    ItemPreprocessedUrls(){};

    ItemPreprocessedUrls(const KUrl& preprocessed, const KUrl& preview)
    {
        preprocessedUrl = preprocessed;
        previewUrl      = preview;
    };

    virtual ~ItemPreprocessedUrls(){};

    KUrl preprocessedUrl;              // Can be original file or aligned version, depending of user choice.
    KUrl previewUrl;                   // The JPEG preview version, accordingly of preprocessedUrl constent.
};

struct EvValueUrls
{
  EvValueUrls() {};
  EvValueUrls(const KUrl& outUrl, const QString& message)
  {
      outputUrl = outUrl;
      exposure_value = message;
  };
  
  virtual ~EvValueUrls() {};
  
  KUrl        outputUrl;
  QString     exposure_value;
  
};  
  
typedef QMap<KUrl, ItemPreprocessedUrls> ItemUrlsMap;   // Map between original Url and processed temp Urls.
typedef QMap<KUrl, EvValueUrls> EvUrlsMap;   // Map between original Url and EV processed Urls.

class ActionData
{

public:

    ActionData()
    {
        starting = false;
        success  = false;
        action = NONE;
	id = 0;
    }

    bool           starting;
    bool           success;

    QString        message;

    QImage         image;

    QString        dirName;
    KUrl::List     inUrls;
    KUrl::List     outUrls;

    EnfuseSettings enfuseSettings;
    PfsHdrSettings pfshdrSettings;

    ItemUrlsMap    preProcessedUrlsMap;
    
    int            id;
    int            option;

    Action         action;
};

}  // namespace KIPIExpoBlendingPlugin

Q_DECLARE_METATYPE(KIPIExpoBlendingPlugin::ActionData)
Q_DECLARE_METATYPE(KIPIExpoBlendingPlugin::ItemPreprocessedUrls)
Q_DECLARE_METATYPE(KIPIExpoBlendingPlugin::EvValueUrls)

#endif /* ACTIONS_H */
