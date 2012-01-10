/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#ifndef COLOROPTIONSDIALOG_H
#define COLOROPTIONSDIALOG_H

// Qt includes

#include <QString>

// KDE includes

#include <kdialog.h>

class KComboBox;
class KIntNumInput;

namespace KIPIBatchProcessImagesPlugin
{

class ColorOptionsDialog : public KDialog
{
    Q_OBJECT

public:

    explicit ColorOptionsDialog(QWidget *parent = 0, int ColorType = 0);
    ~ColorOptionsDialog();

    KComboBox       *m_depthValue;

    KIntNumInput    *m_fuzzDistance;

    KIntNumInput    *m_segmentCluster;
    KIntNumInput    *m_segmentSmooth;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // COLOROPTIONSDIALOG_H
