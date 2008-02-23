/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-21
 * Description : simple viewer export settings container.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SIMPLEVIEWERSETTINGSCONTAINER_H
#define SIMPLEVIEWERSETTINGSCONTAINER_H

// Qt includes.

#include <QList>
#include <QString>
#include <QColor>

// KDE includes.

#include <kurl.h>

// Include files for KIPI

#include <libkipi/imagecollection.h>

namespace KIPISimpleViewerExportPlugin
{

class SimpleViewerSettingsContainer
{

public:

    enum ThumbPosition 
    {
        RIGHT = 0,
        LEFT,
        TOP,
        BOTTOM
    };

    enum NavDir 
    {
        LEFT2RIGHT = 0,
        RIGHT2LEFT
    };

public:
    
    SimpleViewerSettingsContainer()
    {
    };
    
    ~SimpleViewerSettingsContainer(){};

public:

    ThumbPosition                thumbnailPosition;
    NavDir                       navDirection;

    bool                         resizeExportImages;
    bool                         showExifComments;

    int                          imagesExportSize;
    int                          maxImageDimension;
    int                          frameWidth;
    int                          stagePadding;
    int                          thumbnailColumns;
    int                          thumbnailRows;

    QString                      title;

    QColor                       textColor;
    QColor                       backgroundColor;
    QColor                       frameColor;

    KUrl                         exportUrl;  

    QList<KIPI::ImageCollection> collections;
};

}  // namespace KIPISimpleViewerExportPlugin

#endif  // SIMPLEVIEWERSETTINGSCONTAINER_H
