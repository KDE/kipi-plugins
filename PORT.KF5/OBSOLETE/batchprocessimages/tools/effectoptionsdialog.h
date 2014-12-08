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

#ifndef EFFECTOPTIONSDIALOG_H
#define EFFECTOPTIONSDIALOG_H

// Qt includes

#include <QString>

// KDE includes

#include <kdialog.h>

class KIntNumInput;

namespace KIPIBatchProcessImagesPlugin
{

class EffectOptionsDialog : public KDialog
{
    Q_OBJECT

public:

    explicit EffectOptionsDialog(QWidget *parent = 0, int EffectType = 0);
    ~EffectOptionsDialog();

    KIntNumInput    *m_latWidth;
    KIntNumInput    *m_latHeight;
    KIntNumInput    *m_latOffset;

    KIntNumInput    *m_charcoalRadius;
    KIntNumInput    *m_charcoalDeviation;

    KIntNumInput    *m_edgeRadius;

    KIntNumInput    *m_embossRadius;
    KIntNumInput    *m_embossDeviation;

    KIntNumInput    *m_implodeFactor;

    KIntNumInput    *m_paintRadius;

    KIntNumInput    *m_shadeAzimuth;
    KIntNumInput    *m_shadeElevation;

    KIntNumInput    *m_solarizeFactor;

    KIntNumInput    *m_spreadRadius;

    KIntNumInput    *m_swirlDegrees;

    KIntNumInput    *m_waveAmplitude;
    KIntNumInput    *m_waveLength;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // EFFECTOPTIONSDIALOG_H
