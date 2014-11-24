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

#ifndef FILTERIMAGESDIALOG_H
#define FILTERIMAGESDIALOG_H

// Local includes

#include "batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class FilterImagesDialog : public BatchProcessImagesDialog
{
    Q_OBJECT

public:

    explicit FilterImagesDialog(const KUrl::List& images, QWidget* parent = 0);
    ~FilterImagesDialog();

private Q_SLOTS:

    void slotOptionsClicked();
    void slotTypeChanged(int type);

protected:

    void initProcess(KProcess* proc, BatchProcessImagesItem *item,
                     const QString& albumDest, bool previewMode);

    void readSettings();
    void saveSettings();

protected:

    QString m_noiseType;
    int     m_blurRadius;
    int     m_blurDeviation;
    int     m_medianRadius;
    int     m_noiseRadius;
    int     m_sharpenRadius;
    int     m_sharpenDeviation;
    int     m_unsharpenRadius;
    int     m_unsharpenDeviation;
    int     m_unsharpenPercent;
    int     m_unsharpenThreshold;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // FILTERIMAGESDIALOG_H
