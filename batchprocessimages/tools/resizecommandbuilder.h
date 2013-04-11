/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 18.10.2009
 * Description : imagemagick wrappers for resizing
 *
 * Copyright (C) 2009 by Johannes Wienke <languitar at semipol dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef RESIZECOMMANDBUILDER_H
#define RESIZECOMMANDBUILDER_H

// Qt includes

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qcolor.h>

// KDE includes

#include <kprocess.h>

// Local includes

#include "batchprocessimagesitem.h"

namespace KIPIBatchProcessImagesPlugin
{

/**
 * Interface for classes that build commands for various resize options.
 */
class ResizeCommandBuilder: public QObject
{
    Q_OBJECT

public:

    /**
     * Maximum quality value.
     */
    const static unsigned int MAX_QUALITY;

    /**
     * Minimum size for each direction that is possible.
     */
    const static unsigned int MIN_SIZE;

    /**
     * Returns a list of known filter names.
     */
    static QStringList getAllowedFilters();

    explicit ResizeCommandBuilder(QObject* const parent);
    virtual ~ResizeCommandBuilder();

    /**
     * Implement this method to create the process that resizes the image.
     *
     * @param proc process to create from resize arguments
     * @param item the image item for which the command line will be built
     * @param albumDest destination file system path
     */
    virtual void buildCommand(KProcess *proc, BatchProcessImagesItem *item, const QString& albumDest) = 0;

    /**
     * Sets te desired image quality for compressed images.
     *
     * @param quality value between 0 and 100 (MAX_QUALITY)
     */
    void setQuality(unsigned int quality);

    /**
     * Sets the filter to apply while resizing.
     *
     * @param filterName an empty string uses the default filter, every other
     *                   string must be contained in getAllowedFilters
     */
    void setFilterName(const QString& filterName);

protected:

    /**
     * Appends the quality and filter settings managed by this base class to
     * the given command
     *
     * @param proc command to append arguments to
     */
    void appendQualityAndFilter(KProcess *proc);

private:

    unsigned int m_quality;
    QString m_filterName;
};

/**
 * Command builder for one dim proprtional resize.
 */
class OneDimResizeCommandBuilder : public ResizeCommandBuilder
{
    Q_OBJECT

public:

    OneDimResizeCommandBuilder(QObject *parent);
    virtual ~OneDimResizeCommandBuilder();

    void buildCommand(KProcess *proc, BatchProcessImagesItem *item, const QString& albumDest);

    void setSize(unsigned int size);

private:

    unsigned int m_size;
};

/**
 * Command builder for two dim proportional resize.
 */
class TwoDimResizeCommandBuilder : public ResizeCommandBuilder
{
    Q_OBJECT

public:

    TwoDimResizeCommandBuilder(QObject *parent);
    virtual ~TwoDimResizeCommandBuilder();

    void buildCommand(KProcess *proc, BatchProcessImagesItem *item, const QString& albumDest);

    void setWidth(unsigned int width);
    void setHeight(unsigned int height);
    void setFill(bool fill);
    void setFillColor(QColor fillColor);

private:

    unsigned int m_width;
    unsigned int m_height;
    bool         m_fill;
    QColor       m_fillColor;
};

/**
 * Command builder for non-proportional resize.
 */
class NonProportionalResizeCommandBuilder : public ResizeCommandBuilder
{
    Q_OBJECT

public:

    NonProportionalResizeCommandBuilder(QObject *parent);
    virtual ~NonProportionalResizeCommandBuilder();

    void buildCommand(KProcess *proc, BatchProcessImagesItem *item, const QString& albumDest);

    void setWidth(unsigned int width);
    void setHeight(unsigned int height);

private:

    unsigned int m_width;
    unsigned int m_height;
};

/**
 * Command builder for print prepare resize.
 */
class PrintPrepareResizeCommandBuilder : public ResizeCommandBuilder
{
    Q_OBJECT

public:
    PrintPrepareResizeCommandBuilder(QObject *parent);
    virtual ~PrintPrepareResizeCommandBuilder();

    void buildCommand(KProcess *proc, BatchProcessImagesItem *item, const QString& albumDest);

    /**
     * Sets the target paper width.
     *
     * @param paperWidth width in mm
     */
    void setPaperWidth(unsigned int paperWidth);

    /**
     * Sets the target paper height.
     *
     * @param paperHeight height in mm
     */
    void setPaperHeight(unsigned int paperHeight);

    /**
     * Sets the desired dpi.
     *
     * @param dpi dpi > 0
     */
    void setDpi(unsigned int dpi);

    /**
     * Sets whether to stretch the image if it doesn't fit the paper size or
     * crop it.
     *
     * @param stretch if true, image will be stretched to paper size, else it
     *                will be centered on the paper and cropped
     */
    void setStretch(bool stretch);

private:

    unsigned int m_paperWidth;
    unsigned int m_paperHeight;
    unsigned int m_dpi;
    bool         m_stretch;
};

} // namespace KIPIBatchProcessImagesPlugin

#endif /* RESIZECOMMANDBUILDER_H */
