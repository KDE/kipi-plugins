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

#ifndef EFFECTIMAGESDIALOG_H
#define EFFECTIMAGESDIALOG_H

// Local includes

#include "kpaboutdata.h"
#include "batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class EffectImagesDialog : public BatchProcessImagesDialog
{
    Q_OBJECT

public:

    EffectImagesDialog(const KUrl::List& images, KIPI::Interface* interface, QWidget *parent = 0);
    ~EffectImagesDialog();

private Q_SLOTS:

    void slotHelp();
    void slotOptionsClicked();

protected:

    void initProcess(KProcess* proc, BatchProcessImagesItem *item,
                     const QString& albumDest, bool previewMode);

    void readSettings();
    void saveSettings();

protected:

    int  m_latWidth;
    int  m_latHeight;
    int  m_latOffset;
    int  m_charcoalRadius;
    int  m_charcoalDeviation;
    int  m_edgeRadius;
    int  m_embossRadius;
    int  m_embossDeviation;
    int  m_implodeFactor;
    int  m_paintRadius;
    int  m_shadeAzimuth;
    int  m_shadeElevation;
    int  m_solarizeFactor;
    int  m_spreadRadius;
    int  m_swirlDegrees;
    int  m_waveAmplitude;
    int  m_waveLenght;

private:

    KIPIPlugins::KPAboutData *m_about;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // EFFECTIMAGESDIALOG_H
