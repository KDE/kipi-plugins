/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

namespace KIPIPanoramaPlugin
{

enum Action
{
    NONE = 0,
    PREPROCESS_INPUT,
    CREATEPTO,
    CPFIND,
    CPCLEAN,
    OPTIMIZE,
    AUTOCROP,
    CREATEPREVIEWPTO,
    CREATEMK,
    CREATEMKPREVIEW,
    CREATEFINALPTO,
    NONAFILE,
    NONAFILEPREVIEW,
    STITCH,
    STITCHPREVIEW,
    COPY
};

typedef enum
{
    JPEG,
    TIFF,
    HDR
}
PanoramaFileType;

struct ItemPreprocessedUrls
{
    ItemPreprocessedUrls() {}

    ItemPreprocessedUrls(const KUrl& preprocessed, const KUrl& preview)
        : preprocessedUrl(preprocessed), previewUrl(preview) {}

    virtual ~ItemPreprocessedUrls() {}

    KUrl preprocessedUrl;              // Can be an original file or a converted version, depending on the original file type
    KUrl previewUrl;                   // The JPEG preview version, accordingly of preprocessedUrl constent.
};

typedef QMap<KUrl, ItemPreprocessedUrls> ItemUrlsMap;   // Map between original Url and processed temp Urls.

// ----------------------------------------------------------------------------------------------------------

struct ActionData
{
    ActionData()
        : starting(false), success(false), id(0), action(NONE) {}

    bool                starting;
    bool                success;

    QString             message;        // Usually, an error message

    int                 id;

    Action              action;
};

}  // namespace KIPIExpoBlendingPlugin

Q_DECLARE_METATYPE(KIPIPanoramaPlugin::ActionData)
Q_DECLARE_METATYPE(KIPIPanoramaPlugin::ItemPreprocessedUrls)

#endif /* ACTIONS_H */
