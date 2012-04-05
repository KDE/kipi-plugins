/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

// Local includes

#include "kptooldialog.h"
#include "ui_imagepreview.h"

using namespace KIPIPlugins;

namespace KIPIBatchProcessImagesPlugin
{

class ImagePreview : public KPToolDialog, private Ui_ImagePreview
{

public:

    ImagePreview(const QString& fileOrig, 
                 const QString& fileDest,
                 bool cropActionOrig, 
                 const QString& EffectName,
                 const QString& FileName, 
                 QWidget* const parent = 0);
    ~ImagePreview();
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // IMAGEPREVIEW_H
