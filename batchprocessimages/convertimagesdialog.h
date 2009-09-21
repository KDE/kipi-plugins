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

#ifndef CONVERTIMAGESDIALOG_H
#define CONVERTIMAGESDIALOG_H

// Local includes

#include "kpaboutdata.h"
#include "batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class ConvertImagesDialog : public BatchProcessImagesDialog
{
    Q_OBJECT

public:

    ConvertImagesDialog(KUrl::List images, KIPI::Interface* interface, QWidget *parent = 0);
    ~ConvertImagesDialog();

private Q_SLOTS:

    void slotHelp(void);
    void slotOptionsClicked(void);
    void slotTypeChanged(int type);

protected:

    int                    m_JPEGPNGCompression;
    bool                   m_compressLossLess;
    QString                m_TIFFCompressionAlgo;
    QString                m_TGACompressionAlgo;

    void initProcess(KProcess* proc, BatchProcessImagesItem *item,
                     const QString& albumDest, bool previewMode);

    void processDone();

    void readSettings(void);
    void saveSettings(void);

    QString oldFileName2NewFileName(QString fileName);
    QString ImageFileExt(QString Ext);

private:

    KIPIPlugins::KPAboutData *m_about;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // CONVERTIMAGESDIALOG_H
