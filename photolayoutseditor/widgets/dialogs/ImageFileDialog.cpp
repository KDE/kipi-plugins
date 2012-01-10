/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "ImageFileDialog.moc"

#include <kmessagebox.h>
#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

QString ImageFileDialog::FILTER =   "*.bpm|(*.bmp) Windows Bitmap\n"
                                    "*.jpg|(*.jpg) JPG\n"
                                    "*.jpeg|(*.jpeg) JPEG\n"
                                    "*.png|(*.png) Portable Network Graphics\n"
                                    "*.ppm|(*.ppm) Portable Pixmap\n"
                                    "*.tif|(*.tif) Tagged Image File Format\n"
                                    "*.xbm|(*.xbm) X11 Bitmap\n"
                                    "*.xpm|(*.xpm) X11 Pixmap";

ImageFileDialog::ImageFileDialog(const KUrl & startDir, QWidget * parent, QWidget * widget) :
    KFileDialog(startDir, FILTER, parent, widget),
    m_format(0)
{
    setMode(KFile::File);
    setKeepLocation(true);
}

int ImageFileDialog::exec()
{
    int result = KFileDialog::exec();
    QString filter = this->currentFilter();
    if (filter == "*.bpm")
        m_format = "BMP";
    else if (filter == "*.jpg")
        m_format = "JPG";
    else if (filter == "*.jpeg")
        m_format = "JPEG";
    else if (filter == "*.png")
        m_format = "PNG";
    else if (filter == "*.ppm")
        m_format = "PPM";
    else if (filter == "*.tif")
        m_format = "TIFF";
    else if (filter == "*.xbm")
        m_format = "XBM";
    else if (filter == "*.xpm")
        m_format = "XPM";
    else
    {
        m_format = 0;
        KMessageBox::error(this,
                           i18n("Currently this file type (%1) is unsupported.\nPlease notify the author and ask for it in the next versions of the application.", filter.toAscii().constData()),
                           i18n("The image can't be saved!"));
        return KFileDialog::Rejected;
    }

    return result;
}

const char * ImageFileDialog::format()
{
    return m_format;
}
