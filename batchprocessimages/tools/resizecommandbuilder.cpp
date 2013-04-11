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

#include "resizecommandbuilder.moc"

// Qt includes

#include <qglobal.h>

// KDE includes

#include <kdebug.h>

namespace KIPIBatchProcessImagesPlugin
{

const unsigned int ResizeCommandBuilder::MAX_QUALITY = 100;
const unsigned int ResizeCommandBuilder::MIN_SIZE    = 10;

QStringList ResizeCommandBuilder::getAllowedFilters()
{
    QStringList filters;

    filters << "Bessel";
    filters << "Blackman";
    filters << "Box";
    filters << "Catrom";
    filters << "Cubic";
    filters << "Gaussian";
    filters << "Hermite";
    filters << "Hanning";
    filters << "Hamming";
    filters << "Lanczos";
    filters << "Mitchell";
    filters << "Point";
    filters << "Quadratic";
    filters << "Sinc";
    filters << "Triangle";

    return filters;
}

ResizeCommandBuilder::ResizeCommandBuilder(QObject* const parent)
					: QObject(parent), m_quality(75)
{
}

ResizeCommandBuilder::~ResizeCommandBuilder()
{
}

void ResizeCommandBuilder::setQuality(unsigned int quality)
{
    if (quality > MAX_QUALITY)
    {
        kWarning() << "Got quality > " << MAX_QUALITY << ": " << quality
                        << ", truncating it to " << MAX_QUALITY;
        m_quality = MAX_QUALITY;
    }
    else
    {
        m_quality = quality;
    }
}

void ResizeCommandBuilder::setFilterName(const QString& filterName)
{
    if (!getAllowedFilters().contains(filterName))
    {
        kWarning() << "Unknown filter with name" << filterName <<". Using default";
        m_filterName = "";
    }
    else
    {
        m_filterName = filterName;
    }
}

void ResizeCommandBuilder::appendQualityAndFilter(KProcess *proc)
{
    if (!m_filterName.isEmpty())
    {
        *proc << "-filter" << m_filterName;
    }

    *proc << "-quality" << QString::number(m_quality);
}

// --- one dim ---

OneDimResizeCommandBuilder::OneDimResizeCommandBuilder(QObject *parent)
                          : ResizeCommandBuilder(parent), m_size(MIN_SIZE)
{
}

OneDimResizeCommandBuilder::~OneDimResizeCommandBuilder()
{
}

void OneDimResizeCommandBuilder::buildCommand(KProcess *proc, BatchProcessImagesItem *item, const QString& albumDest)
{
    // Proportional (1 dim.)
    *proc << "convert";

    *proc << "-resize";
    *proc << QString::number(m_size) + 'x' + QString::number(m_size);

    appendQualityAndFilter(proc);

    *proc << "-verbose";
    *proc << item->pathSrc() + "[0]";
    *proc << albumDest + '/' + item->nameDest();
}

void OneDimResizeCommandBuilder::setSize(unsigned int size)
{
    if (size < MIN_SIZE)
    {
        kWarning() << "Got size beneath minimum " << MIN_SIZE << ": "
                   << size << ". Truncating it to " << MIN_SIZE;
        m_size = MIN_SIZE;
    }
    else
    {
        m_size = size;
    }
}

// --- two dim ---

TwoDimResizeCommandBuilder::TwoDimResizeCommandBuilder(QObject *parent)
                          : ResizeCommandBuilder(parent), 
						    m_width(MIN_SIZE), m_height(MIN_SIZE),
                            m_fill(false), m_fillColor(QColor(Qt::white))
{
}

TwoDimResizeCommandBuilder::~TwoDimResizeCommandBuilder()
{
}

void TwoDimResizeCommandBuilder::buildCommand(KProcess *proc,
                BatchProcessImagesItem *item, const QString& albumDest)
{
    *proc << "convert";

    QString targetBackgroundSize = QString::number(m_width) + 'x' + QString::number(m_height);

    *proc << "-verbose";

    *proc << item->pathSrc() + "[0]";

    // resize original image
    *proc << "-resize";
    *proc << QString::number(m_width) + 'x' + QString::number(m_height);

    // fill image if desired
    if (m_fill)
    {
        *proc << "-bordercolor"
              << "rgb(" + QString::number(m_fillColor.red()) + ','
                        + QString::number(m_fillColor.green()) + ','
                        + QString::number(m_fillColor.blue()) + ')';
        *proc << "-border" << QString::number(m_width) + 'x' + QString::number(m_height);

        // center resized image on canvas
        *proc << "-gravity" << "Center";
    }

    appendQualityAndFilter(proc);

    // ImageMagick composite program do not preserve exif data from original.
    // Need to use "-profile" option for that.
    *proc << "-profile" << item->pathSrc();

    if (m_fill)
    {
        // crop image to canvas size
        *proc << "-crop" << targetBackgroundSize + "+0+0";
    }

    // set destination
    *proc << albumDest + '/' + item->nameDest();
}

void TwoDimResizeCommandBuilder::setWidth(unsigned int width)
{
    if (width < MIN_SIZE)
    {
        kWarning() << "Got width beneath minimum " << MIN_SIZE << ": "
                   << width << ". Truncating it to " << MIN_SIZE;
        m_width = MIN_SIZE;
    }
    else
    {
        m_width = width;
    }
}

void TwoDimResizeCommandBuilder::setHeight(unsigned int height)
{
    if (height < MIN_SIZE)
    {
        kWarning() << "Got height beneath minimum " << MIN_SIZE << ": "
                   << height << ". Truncating it to " << MIN_SIZE;
        m_height = MIN_SIZE;
    }
    else
    {
        m_height = height;
    }
}

void TwoDimResizeCommandBuilder::setFill(bool fill)
{
    m_fill = fill;
}

void TwoDimResizeCommandBuilder::setFillColor(QColor fillColor)
{
    m_fillColor = fillColor;
}

// --- non-proportional ---

NonProportionalResizeCommandBuilder::NonProportionalResizeCommandBuilder(QObject *parent) :
    ResizeCommandBuilder(parent), m_width(MIN_SIZE), m_height(MIN_SIZE)
{
}

NonProportionalResizeCommandBuilder::~NonProportionalResizeCommandBuilder()
{
}

void NonProportionalResizeCommandBuilder::buildCommand(KProcess *proc,
                BatchProcessImagesItem *item, const QString& albumDest)
{
    *proc << "convert";

    *proc << "-resize";
    *proc << QString::number(m_width) + 'x' + QString::number(m_height) + '!';

    appendQualityAndFilter(proc);

    *proc << "-verbose";
    *proc << item->pathSrc() + "[0]";
    *proc << albumDest + '/' + item->nameDest();
}

void NonProportionalResizeCommandBuilder::setWidth(unsigned int width)
{
    if (width < MIN_SIZE)
    {
        kWarning() << "Got width beneath minimum " << MIN_SIZE << ": "
                   << width << ". Truncating it to " << MIN_SIZE;
        m_width = MIN_SIZE;
    }
    else
    {
        m_width = width;
    }
}

void NonProportionalResizeCommandBuilder::setHeight(unsigned int height)
{
    if (height < MIN_SIZE)
    {
        kWarning() << "Got height beneath minimum " << MIN_SIZE << ": "
                   << height << ". Truncating it to " << MIN_SIZE;
        m_height = MIN_SIZE;
    }
    else
    {
        m_height = height;
    }
}

// --- print-prepare ---

PrintPrepareResizeCommandBuilder::PrintPrepareResizeCommandBuilder(QObject *parent)
								: ResizeCommandBuilder(parent), 
								  m_paperWidth(10), m_paperHeight(10),
								  m_dpi(75), m_stretch(false)
{
}

PrintPrepareResizeCommandBuilder::~PrintPrepareResizeCommandBuilder()
{
}

void PrintPrepareResizeCommandBuilder::buildCommand(KProcess *proc,
                BatchProcessImagesItem *item, const QString& albumDest)
{

    kDebug() << "resizing for settings: paperWidth = " << m_paperWidth
             << ", paperHeight = " << m_paperHeight << ", dpi = "
             << m_dpi;

    // Get image information.
    QImage img;
    bool loaded = img.load(item->pathSrc());
    if (!loaded)
    {
        kError() << "Unable to load image " << item->pathSrc();
        return;
    }

    unsigned int w          = img.width();
    unsigned int h          = img.height();
    const float oneInchInMM = 25.4F;

    // calculate needed image size as paper size in pixels for the given
    // resolution and rotate the canvas if needed
    unsigned int paperWidthInPixels  = 0;
    unsigned int paperHeightInPixels = 0;
	if (w < h)
	{
		// (w < h) because all paper dimensions are given in landscape format
		paperWidthInPixels  = (int)((float)(m_paperHeight * m_dpi) / oneInchInMM);
		paperHeightInPixels = (int)((float)(m_paperWidth  * m_dpi) / oneInchInMM);
	}
	else
	{
		paperHeightInPixels = (int)((float)(m_paperHeight * m_dpi) / oneInchInMM);
		paperWidthInPixels  = (int)((float)(m_paperWidth  * m_dpi) / oneInchInMM);
	}

	kDebug() << "paper size in pixel: " << paperWidthInPixels << "x"
             << paperHeightInPixels;

	*proc << "convert" << "-verbose";

    *proc << item->pathSrc();

	// resize image
    const QString rawPaperDimensions = QString::number(paperWidthInPixels)
                    + 'x' + QString::number(paperHeightInPixels);
	if (m_stretch)
	{
	    // stretching is simple, just force paper dimensions
	    *proc << "-resize" << rawPaperDimensions + '!';
	    appendQualityAndFilter(proc);
	}
	else
    {
        // if we don't want to stretch the image, some more work is needed

        // first resize the image so that it will fit at least the whole paper
	    // but one dimension can get bigger
        *proc << "-resize" << rawPaperDimensions + '^';
        appendQualityAndFilter(proc);

        // and the crop it to the desired paper size
        *proc << "-gravity" << "center";
        *proc << "-crop" << rawPaperDimensions + "+0+0" << "+repage";
    }

	// ImageMagick composite program do not preserve exif data from original.
	// Need to use "-profile" option for that.
	*proc << "-profile" << item->pathSrc();

	*proc << albumDest + '/' + item->nameDest();
}

void PrintPrepareResizeCommandBuilder::setPaperWidth(unsigned int paperWidth)
{
	m_paperWidth = paperWidth;
}

void PrintPrepareResizeCommandBuilder::setPaperHeight(unsigned int paperHeight)
{
	m_paperHeight = paperHeight;
}

void PrintPrepareResizeCommandBuilder::setDpi(unsigned int dpi)
{
	// TODO error checks
	m_dpi = dpi;
}

void PrintPrepareResizeCommandBuilder::setStretch(bool stretch)
{
    m_stretch = stretch;
}

} // namespace KIPIBatchProcessImagesPlugin
