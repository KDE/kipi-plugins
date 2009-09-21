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

#ifndef RESIZEIMAGESDIALOG_H
#define RESIZEIMAGESDIALOG_H

// Local includes

#include "batchprocessimagesdialog.h"
#include "kpaboutdata.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class ResizeImagesDialog : public BatchProcessImagesDialog
{
    Q_OBJECT

public:

    ResizeImagesDialog(KUrl::List images, KIPI::Interface* interface, QWidget *parent = 0);
    ~ResizeImagesDialog();

private Q_SLOTS:

    void slotHelp(void);
    void slotOptionsClicked(void);

protected:

    QString                m_resizeFilter;
    QString                m_paperSize;
    QString                m_printDpi;

    QColor                 m_backgroundColor;
    QColor                 m_bgColor;

    bool                   m_customSettings;

    int                    m_customXSize;
    int                    m_customYSize;
    int                    m_marging;
    int                    m_customDpi;
    int                    m_size;
    int                    m_xPixels;
    int                    m_yPixels;
    int                    m_Width;
    int                    m_Height;
    int                    m_Border;
    int                    m_fixedWidth;
    int                    m_fixedHeight;
    int                    m_quality;

    void initProcess(KProcess* proc, BatchProcessImagesItem *item,
                     const QString& albumDest, bool previewMode);

    void readSettings(void);
    void saveSettings(void);
    bool prepareStartProcess(BatchProcessImagesItem *item,
                             const QString& albumDest);

    bool ResizeImage(int &w, int &h, int SizeFactor);

private:

    KIPIPlugins::KPAboutData *m_about;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // RESIZEIMAGESDIALOG_H
