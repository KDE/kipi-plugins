/* ============================================================
 * File  : convert2grayscale.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-10-14
 * Description : batch images grayscale conversion
 *
 * Copyright 2003 by Gilles Caulier

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

#ifndef convert2grayscale_H
#define convert2grayscale_H

class QString;
class QImage;

namespace KIPIJPEGLossLessPlugin
{
bool image2GrayScale(const QString& src, const QString& TmpFolder,
                     QString& err);
bool image2GrayScaleJPEG(const QString& src, const QString& dest,
                         QString& err);
bool image2GrayScaleQImage(const QString& src, const QString& dest,
                           QString& err);

}  // NameSpace KIPIJPEGLossLessPlugin

#endif /* convert2grayscale_H */
