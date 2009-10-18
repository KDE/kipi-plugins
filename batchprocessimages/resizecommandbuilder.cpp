/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#include "resizecommandbuilder.h"
#include "resizecommandbuilder.moc"

// KDE includes

#include <kdebug.h>

namespace KIPIBatchProcessImagesPlugin
{

const unsigned int ResizeCommandBuilder::MAX_QUALITY = 100;
const unsigned int ResizeCommandBuilder::MIN_SIZE = 10;

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

ResizeCommandBuilder::ResizeCommandBuilder(QObject *parent) :
    QObject(parent), m_quality(75)
{
}

ResizeCommandBuilder::~ResizeCommandBuilder()
{
}

void ResizeCommandBuilder::setQuality(unsigned int quality)
{

    if (quality > MAX_QUALITY)
    {
        kWarning(51000) << "Got quality > " << MAX_QUALITY << ": " << quality
                        << ", truncating it to " << MAX_QUALITY;
        m_quality = MAX_QUALITY;
    }
    else
    {
        m_quality = quality;
    }

}

void ResizeCommandBuilder::setFilterName(QString filterName)
{

    if (!getAllowedFilters().contains(filterName))
    {
        kWarning(51000) << "Unknown filter with name" << filterName <<". Using default";
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

OneDimResizeCommandBuilder::OneDimResizeCommandBuilder(QObject *parent) :
    ResizeCommandBuilder(parent), m_size(MIN_SIZE)
{

}

OneDimResizeCommandBuilder::~OneDimResizeCommandBuilder()
{
}

void OneDimResizeCommandBuilder::buildCommand(KProcess *proc,
                BatchProcessImagesItem *item, const QString& albumDest)
{

    // Proportional (1 dim.)
    *proc << "convert";

    *proc << "-resize";
    *proc << QString::number(m_size) + "x" + QString::number(m_size);

    appendQualityAndFilter(proc);

    *proc << "-verbose";
    *proc << item->pathSrc() + "[0]";
    *proc << albumDest + "/" + item->nameDest();

}

void OneDimResizeCommandBuilder::setSize(unsigned int size)
{

    if (size < MIN_SIZE)
    {
        kWarning(51000) << "Got size beneath minimum " << MIN_SIZE << ": "
                        << size << ". Truncating it to " << MIN_SIZE;
        m_size = MIN_SIZE;
    }
    else
    {
        m_size = size;
    }

}

// --- two dim ---

TwoDimResizeCommandBuilder::TwoDimResizeCommandBuilder(QObject *parent) :
    ResizeCommandBuilder(parent), m_width(MIN_SIZE), m_height(MIN_SIZE),
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

    QString targetBackgroundSize = QString::number(m_width) + "x" + QString::number(m_height);

    *proc << "-verbose";

    *proc << item->pathSrc() + "[0]";

    // resize original image
    *proc << "-resize";
    *proc << QString::number(m_width) + "x" + QString::number(m_height);

    // fill image if desired
    if (m_fill)
    {
        *proc << "-bordercolor"
              << "rgb(" + QString::number(m_fillColor.red()) + ","
                        + QString::number(m_fillColor.green()) + ","
                        + QString::number(m_fillColor.blue()) + ")";
        *proc << "-border" << QString::number(m_width) + "x" + QString::number(m_height);

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
    *proc << albumDest + "/" + item->nameDest();

}

void TwoDimResizeCommandBuilder::setWidth(unsigned int width)
{

    if (width < MIN_SIZE)
    {
        kWarning(51000) << "Got width beneath minimum " << MIN_SIZE << ": "
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
        kWarning(51000) << "Got height beneath minimum " << MIN_SIZE << ": "
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
    *proc << QString::number(m_width) + "x" + QString::number(m_height) + "!";

    appendQualityAndFilter(proc);

    *proc << "-verbose";
    *proc << item->pathSrc() + "[0]";
    *proc << albumDest + "/" + item->nameDest();

}

void NonProportionalResizeCommandBuilder::setWidth(unsigned int width)
{

    if (width < MIN_SIZE)
    {
        kWarning(51000) << "Got width beneath minimum " << MIN_SIZE << ": "
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
        kWarning(51000) << "Got height beneath minimum " << MIN_SIZE << ": "
                        << height << ". Truncating it to " << MIN_SIZE;
        m_height = MIN_SIZE;
    }
    else
    {
        m_height = height;
    }

}

// --- print-prepare ---

PrintPrepareResizeCommandBuilder::PrintPrepareResizeCommandBuilder(
                QObject *parent) :
    ResizeCommandBuilder(parent), m_paperWidth(10), m_paperHeight(10),
                    m_dpi(75), m_margin(10), m_backgroundColor(
                                    QColor(Qt::white))
{

}

PrintPrepareResizeCommandBuilder::~PrintPrepareResizeCommandBuilder()
{
}

void PrintPrepareResizeCommandBuilder::buildCommand(KProcess *proc,
                BatchProcessImagesItem *item, const QString& albumDest)
{

    kDebug(51000) << "resizing for settings: paperWidth = " << m_paperWidth
                    << ", paperHeight = " << m_paperHeight << ", dpi = "
                    << m_dpi;

    // Get image information.
    QImage img;
    img.load(item->pathSrc());
    unsigned int w = img.width();
    unsigned int h = img.height();

    const float oneInchInMM = 25.4;

    unsigned int marginInPixels = (int)((float)(m_margin * m_dpi) / oneInchInMM);
    kDebug(51000) << "marginInPixels = " << marginInPixels;

    // calculate needed image size as paper size in pixels for the given
    // resolution and rotate the canvas if needed
    unsigned int paperWidthInPixels = 0;
    unsigned int paperHeightInPixels = 0;
	if (w < h)
	{
		// (w < h) because all paper dimensions are given in landscape format
		paperWidthInPixels  = (int)((float)(m_paperHeight * m_dpi) / oneInchInMM);
		paperHeightInPixels = (int)((float)(m_paperWidth * m_dpi) / oneInchInMM);
	}
	else
	{
		paperHeightInPixels = (int)((float)(m_paperHeight * m_dpi) / oneInchInMM);
		paperWidthInPixels  = (int)((float)(m_paperWidth * m_dpi) / oneInchInMM);
	}

	kDebug(51000) << "paper size in pixel: " << paperWidthInPixels << "x"
                    << paperHeightInPixels;

	*proc << "composite";

	// Get the target image resizing dimensions with using the target paper size.
    int ResizeCoeff = 0;
    float RFactor = 0;
    if (paperWidthInPixels < paperHeightInPixels)
    {
        RFactor = (float) paperWidthInPixels / (float) w;
        if (RFactor > 1.0)
        {
            RFactor = (float) paperHeightInPixels / (float) h;
        }
        ResizeCoeff = (int) ((float) h * RFactor);
    }
    else
    {
        RFactor = (float) paperHeightInPixels / (float) h;
        if (RFactor > 1.0)
            RFactor = (float) paperWidthInPixels / (float) w;
        ResizeCoeff = (int) ((float) w * RFactor);
    }

	resizeImage(w, h, ResizeCoeff - marginInPixels);

	*proc << "-verbose" << "-gravity" << "Center";

	*proc << "-resize";
	QString Temp, Temp2;
	Temp2 = Temp.setNum(w) + "x";
	Temp2.append(Temp.setNum(h));
	*proc << Temp2;

	appendQualityAndFilter(proc);

	*proc << item->pathSrc();

	Temp2 = "xc:rgb(" + Temp.setNum(m_backgroundColor.red()) + ",";
	Temp2.append(Temp.setNum(m_backgroundColor.green()) + ",");
	Temp2.append(Temp.setNum(m_backgroundColor.blue()) + ")");
	*proc << Temp2;

	// ImageMagick composite program do not preserve exif data from original.
	// Need to use "-profile" option for that.

	*proc << "-profile" << item->pathSrc();

	QString targetBackgroundSize = QString::number(paperWidthInPixels) + "x" + QString::number(paperHeightInPixels);
	*proc << "-resize" << targetBackgroundSize + "!";

	appendQualityAndFilter(proc);

	*proc << albumDest + "/" + item->nameDest();

}

bool PrintPrepareResizeCommandBuilder::resizeImage(unsigned int &w, unsigned int &h, unsigned int sizeFactor)
{
    bool valRet;

    if (w > h)
    {
        h = (int)((double)(h * sizeFactor) / w);

        if (h == 0) h = 1;

        if (w < sizeFactor) valRet = true;
        else valRet = false;

        w = sizeFactor;
    }
    else
    {
        w = (int)((double)(w * sizeFactor) / h);

        if (w == 0) w = 1;

        if (h < sizeFactor) valRet = true;
        else valRet = false;

        h = sizeFactor;
    }

    kDebug(51000) << "calculated new image size width = " << w
                    << ", new height = " << h << ", image is increased = "
                    << valRet;

    return (valRet);  // Return true if image increased, else false.
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

void PrintPrepareResizeCommandBuilder::setMargin(unsigned int margin)
{
	m_margin = margin;
}

void PrintPrepareResizeCommandBuilder::setBackgroundColor(QColor backgroundColor)
{
	m_backgroundColor = backgroundColor;
}

}
