/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-09
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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

#ifndef MAINDIALOG_H
#define MAINDIALOG_H

// Qt includes
#include <QSvgRenderer>

// KDE includes.
#include "kio/previewjob.h"
#include "kurl.h"

// Local includes.
#include "common.h"
#include "ui_maindialog.h"

namespace KIPISlideShowPlugin
{

class SharedData;

class MainDialog : public QWidget, Ui::MainDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget* parent, SharedData* sharedData);
    ~MainDialog();

    void  readSettings();
    void  saveSettings();
    bool  updateUrlList();

private slots:
    void slotOpenGLToggled( void );
    void slotEffectChanged( void );
    void slotDelayChanged( void );
    void slotPrintCommentsToggled( void );
    void slotSelection( void );
    void slotUseMillisecondsToggled( void );
    void slotThumbnail(const KUrl&, const QPixmap&);
    
    void SlotPortfolioDurationChanged ( int );
    void slotImagesFilesSelected( int );
    void slotAddDropItems(KUrl::List filesUrl);
    void slotImagesFilesButtonAdd( void );
    void slotImagesFilesButtonDelete( void );
    void slotImagesFilesButtonUp( void );
    void slotImagesFilesButtonDown( void );

signals :
    void totalTimeChanged( QTime );

private:

    void loadEffectNames();
    void loadEffectNamesGL();

    void ShowNumberImages( int Number );
    void addItems(const KUrl::List& fileList);

private:
    SharedData*       m_sharedData;
    KIO::PreviewJob*  m_thumbJob;
    QTime             m_totalTime;
    QPixmap           m_noPreviewPixmap;
};

} // namespace KIPISlideShowPlugin

#endif // MAINDIALOG_H
