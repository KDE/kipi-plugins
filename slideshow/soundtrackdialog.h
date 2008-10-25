/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-14
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

#ifndef SOUNDTRACKDIALOG_H
#define SOUNDTRACKDIALOG_H

// Qt includes.
#include <QTime>
#include <QMutex>

// KDE includes.
#include <kurl.h>
#include <kdialog.h>

// Local includes.
#include "ui_soundtrackdialog.h"
#include "common.h"
#include "playbackwidget.h"
#include "listsounditems.h"

namespace KIPISlideShowPlugin
{

class PlaybackWidget;

class SharedData;

class SoundtrackPreview : public KDialog
{

public :
    SoundtrackPreview(QWidget*, KUrl::List&, SharedData*);
    ~SoundtrackPreview();

private :
    PlaybackWidget* m_playbackWidget;
};

class SoundtrackDialog : public QWidget, public Ui::SoundtrackDialog
{
    Q_OBJECT

public:
    SoundtrackDialog( QWidget* parent, SharedData* sharedData );
    ~SoundtrackDialog();

    void readSettings();
    void saveSettings();

private:
    void addItems(const KUrl::List& fileList);
    void updateTracksNumber();
    void compareTimes();

private slots:
    void slotAddDropItems(KUrl::List filesUrl);
    void slotSoundFilesButtonAdd( void );
    void slotSoundFilesButtonDelete( void );
    void slotSoundFilesButtonUp( void );
    void slotSoundFilesButtonDown( void );
    void slotSoundFilesSelected( int );
    void slotPreviewButtonClicked( void );
    void slotImageTotalTimeChanged( QTime );
    void slotAddNewTime(KUrl, QTime);

private:
    KUrl::List              m_urlList;
    int                     m_currIndex;
    SharedData*             m_sharedData;
    QTime                   m_totalTime;
    QTime                   m_imageTime;
    QMap<KUrl, QTime>*       m_tracksTime;
    QMap<KUrl, SoundItem*>*  m_soundItems;
    QMutex*                 m_timeMutex;
};

} // namespace KIPISlideShowPlugin

#endif // SOUNDTRACKDIALOG_H
