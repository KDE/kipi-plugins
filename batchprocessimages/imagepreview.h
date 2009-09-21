/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <kdialog.h>

// Local includes

#include "ui_imagepreview.h"


namespace KIPIPlugins
{
class KPAboutData;
}


namespace KIPIBatchProcessImagesPlugin
{

class ImagePreview : public KDialog, private Ui_ImagePreview
{
    Q_OBJECT

public:

    ImagePreview(const QString &fileOrig, const QString &fileDest, const QString &tmpPath,
                 bool cropActionOrig, bool cropActionDest, const QString &EffectName,
                 const QString &FileName, QWidget *parent = 0);
    ~ImagePreview();

private Q_SLOTS:

    void slotZoomFactorValueChanged(int ZoomFactorValue);
    void slotWheelChanged(int delta);
    void slotHelp(void);

private:

    KIPIPlugins::KPAboutData    *m_about;

    int INIT_ZOOM_FACTOR; // FIXME: Ugly name
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // IMAGEPREVIEW_H
