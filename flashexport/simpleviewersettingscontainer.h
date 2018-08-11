/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2008-02-21
 * Description : simple viewer export settings container.
 *
 * Copyright (C) 2008-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011      by Veaceslav Munteanu <slavuttici@gmail.com>
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

#include <QUrl>

// Libkipi includes

#include <KIPI/ImageCollection>

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

    enum PluginType
    {
        SIMPLE = 0,
        AUTO,
        TILT,
        POSTCARD
    };

    enum ImageGetOption
    {
        COLLECTION = 0,
        IMAGEDIALOG
    };

public:

    SimpleViewerSettingsContainer()
    {
        // All items must be initialized in constructor to avoid warnings
        plugType             = SIMPLE;
        imgGetOption         = COLLECTION;
        exportPath           = QString();

        enableRightClickOpen = false;
        resizeExportImages   = false;
        showComments         = false;
        fixOrientation       = false;
        openInBrowser        = false;
        showKeywords         = false;

        title                = QString();

        backgroundColor      = QColor();
        frameColor           = QColor();

        thumbnailColumns     = 0; // columns
        thumbnailRows        = 0; // rows

        imagesExportSize     = 0;
        frameWidth           = 0;

        //simpleViewer settings
        textColor            = QColor();
        stagePadding         = 0;
        maxImageDimension    = 0;

        thumbnailPosition    = RIGHT;

        // Autoviewer settings
        imagePadding         = 0;
        displayTime          = 0 ;

        // Tiltviewer options
        showFlipButton       = false;
        useReloadButton      = false;
        backColor            = QColor();
        bkgndInnerColor      = QColor();
        bkgndOuterColor      = QColor();

        // postcard options
        cellDimension        = 0;
        zoomOutPerc          = 0;
        zoomInPerc           = 0;

        collections          = QList<KIPI::ImageCollection>();
        imageDialogList      = QList<QUrl>();
    };

    ~SimpleViewerSettingsContainer()
    {
    };

    QString imageGetOptions() const
    {
        switch(imgGetOption)
        {
            case COLLECTION:
                return QLatin1String("Collections");
                break;
            case IMAGEDIALOG:
                return QLatin1String("Image Dialog");
        }
    };

    QString pluginType() const
    {
        switch(plugType)
        {
            case SIMPLE:
                return QLatin1String("SimpleViewer");
                break;

            case AUTO:
                return QLatin1String("AutoViewer");
                break;

            case TILT:
                return QLatin1String("TiltViewer");
                break;

            case POSTCARD:
                return QLatin1String("PostcardViewer");
        }
    };

    QString thumbPosition() const
    {
        switch(thumbnailPosition)
        {
            case RIGHT:
                return QLatin1String("right");
                break;

            case LEFT:
                return QLatin1String("left");
                break;

            case TOP:
                return QLatin1String("top");
                break;

            default: // BOTTOM
                return QLatin1String("bottom");
                break;
        }
    };

    QString enableRightClickToOpen() const
    {
        if (enableRightClickOpen)
            return QLatin1String("true");

        return QLatin1String("false");
    };

public:

    PluginType                   plugType;

    // Choose between getting images with collections or with imagedialog
    ImageGetOption               imgGetOption;


    //Settings for all plug-ins
    QString                      exportPath;

    bool                         enableRightClickOpen;
    bool                         resizeExportImages;
    bool                         showComments;
    bool                         fixOrientation;
    bool                         openInBrowser;
    bool                         showKeywords;

    QString                      title;


    QColor                       backgroundColor;
    QColor                       frameColor;

    int                          thumbnailColumns; // columns
    int                          thumbnailRows;    // rows

    int                          imagesExportSize;
    int                          frameWidth;

    //simpleViewer settings
    QColor                       textColor;
    int                          stagePadding;
    int                          maxImageDimension;

    ThumbPosition                thumbnailPosition;

    // Autoviewer settings
    int                          imagePadding;
    int                          displayTime;

    // Tiltviewer options
    bool                         showFlipButton;
    bool                         useReloadButton;
    QColor                       backColor;
    QColor                       bkgndInnerColor;
    QColor                       bkgndOuterColor;

    // postcard options
    int                          cellDimension;
    int                          zoomOutPerc;
    int                          zoomInPerc;

    QList<KIPI::ImageCollection> collections;
    QList<QUrl>                  imageDialogList;
};

}  // namespace KIPIFlashExportPlugin

#endif  // SIMPLEVIEWERSETTINGSCONTAINER_H
