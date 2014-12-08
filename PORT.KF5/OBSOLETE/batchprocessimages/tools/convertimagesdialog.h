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

#ifndef CONVERTIMAGESDIALOG_H
#define CONVERTIMAGESDIALOG_H

// Local includes

#include "batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class ConvertImagesDialog : public BatchProcessImagesDialog
{
    Q_OBJECT

public:

    explicit ConvertImagesDialog(const KUrl::List& images, QWidget* parent = 0);
    ~ConvertImagesDialog();

private Q_SLOTS:

    void slotOptionsClicked();
    void slotTypeChanged(int type);

protected:

    void initProcess(KProcess* proc, BatchProcessImagesItem* item,
                     const QString& albumDest, bool previewMode);

    void processDone();

    void readSettings();
    void saveSettings();

    QString oldFileName2NewFileName(const QString& fileName);
    QString ImageFileExt(const QString& Ext);

protected:

    int     m_JPEGPNGCompression;
    bool    m_compressLossLess;
    QString m_TIFFCompressionAlgo;
    QString m_TGACompressionAlgo;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // CONVERTIMAGESDIALOG_H
