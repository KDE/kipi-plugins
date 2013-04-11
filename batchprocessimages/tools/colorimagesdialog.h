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

#ifndef COLORIMAGESDIALOG_H
#define COLORIMAGESDIALOG_H

// Local includes

#include "batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class ColorImagesDialog : public BatchProcessImagesDialog
{
    Q_OBJECT

public:

    explicit ColorImagesDialog(const KUrl::List& images, QWidget* parent = 0);
    ~ColorImagesDialog();

private Q_SLOTS:

    void slotOptionsClicked();
    void slotTypeChanged(int type);

protected:

    QString m_depthValue;
    int     m_fuzzDistance;
    int     m_segmentCluster;
    int     m_segmentSmooth;

protected:

    void initProcess(KProcess* proc, BatchProcessImagesItem* item,
                     const QString& albumDest, bool previewMode);

    void readSettings();
    void saveSettings();
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // COLORIMAGESDIALOG_H
