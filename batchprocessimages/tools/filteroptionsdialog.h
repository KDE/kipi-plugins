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

#ifndef FILTEROPTIONSDIALOG_H
#define FILTEROPTIONSDIALOG_H

// Qt includes

#include <QString>

// KDE includes

#include <kdialog.h>

class KComboBox;
class KIntNumInput;

namespace KIPIBatchProcessImagesPlugin
{

class FilterOptionsDialog : public KDialog
{
    Q_OBJECT

public:

    explicit FilterOptionsDialog(QWidget *parent = 0, int FilterType = 0);
    ~FilterOptionsDialog();

    KComboBox       *m_noiseType;

    KIntNumInput    *m_blurRadius;
    KIntNumInput    *m_blurDeviation;

    KIntNumInput    *m_medianRadius;

    KIntNumInput    *m_noiseRadius;

    KIntNumInput    *m_sharpenRadius;
    KIntNumInput    *m_sharpenDeviation;

    KIntNumInput    *m_unsharpenRadius;
    KIntNumInput    *m_unsharpenDeviation;
    KIntNumInput    *m_unsharpenPercent;
    KIntNumInput    *m_unsharpenThreshold;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // FILTEROPTIONSDIALOG_H
