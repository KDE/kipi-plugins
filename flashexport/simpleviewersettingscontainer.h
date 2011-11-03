/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-21
 * Description : simple viewer export settings container.
 *
 * Copyright (C) 2008-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011 by Veaceslav Munteanu <slavuttici@gmail.com>
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

    enum PluginType
    {
        SIMPLE = 0,
        AUTO,
        TILT,
        POSTCARD
    };

public:

    SimpleViewerSettingsContainer()
    {
    };

    ~SimpleViewerSettingsContainer(){};

    QString pluginType() const
    {
    	switch(plugType)
    	{
    		case SIMPLE:
    			return QString("SimpleViewer");
    			break;

    		case AUTO:
    			return QString("AutoViewer");
    			break;

    		case TILT:
    			return QString("TiltViewer");
    			break;

    		case POSTCARD:
    			return QString("PostcardViewer");
    	}
    };

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

    PluginType		             plugType;

    //Settings for all plug-ins
    KUrl                         exportUrl;

    bool                         enableRightClickOpen;
    bool                         resizeExportImages;
    bool                         showComments;
    bool                         fixOrientation;
    bool                         openInKonqueror;
    bool                         showKeywords;

    QString                      title;


    QColor                       backgroundColor;
    QColor                       frameColor;

    int                          thumbnailColumns; //columns
    int                          thumbnailRows;    // rows

    int                          imagesExportSize;
    int                          frameWidth;

    //simpleViewer settings
    QColor                       textColor;
    int                          stagePadding;
    int                          maxImageDimension;

    ThumbPosition                thumbnailPosition;

    // Navdir is deprecated and must be removed
    NavDir                       navDirection;

    // Autoviewer settings
    int                          imagePadding;
    int                          displayTime;

    // Tiltviewer options
    bool                         showFlipButton;
    bool                         useReloadButton;
    QColor						 backColor;
    QColor                       bkgndInnerColor;
    QColor                       bkgndOuterColor;

    // postcard options
    int                          cellDimension;
    int                          zoomOutPerc;
    int                          zoomInPerc;

    QList<KIPI::ImageCollection> collections;
};

}  // namespace KIPIFlashExportPlugin

#endif  // SIMPLEVIEWERSETTINGSCONTAINER_H
