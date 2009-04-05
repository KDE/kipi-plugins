/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-21
 * Description : simple viewer export settings container.
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QList>
#include <QString>
#include <QColor>

// KDE includes

#include <kurl.h>

// Include files for KIPI

#include <libkipi/imagecollection.h>

namespace KIPIFlashExportPlugin
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

    QString thumbPosition() const
    {
        switch(thumbnailPosition)
        {
            case RIGHT:
                return QString("right");
                break;

            case LEFT:
                return QString("left");
                break;

            case TOP:
                return QString("top");
                break;

            default: // BOTTOM
                return QString("bottom");
                break;
        }
    };

    QString navDir() const
    {
        switch(navDirection)
        {
            case LEFT2RIGHT:
                return QString("LTR");
                break;

            default: // RIGHT2LEFT
                return QString("RTL");
                break;
        }
    };

    QString enableRightClickToOpen() const
    {
        if (enableRightClickOpen)
            return QString("true");

        return QString("false");
    };

public:

    ThumbPosition                thumbnailPosition;
    NavDir                       navDirection;

    bool                         enableRightClickOpen;
    bool                         resizeExportImages;
    bool                         showComments;
    bool                         fixOrientation;
    bool                         openInKonqueror;

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

}  // namespace KIPIFlashExportPlugin

#endif  // SIMPLEVIEWERSETTINGSCONTAINER_H
