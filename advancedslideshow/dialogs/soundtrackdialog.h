/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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

// Qt includes

#include <QTime>
#include <QMutex>

// KDE includes

#include <kurl.h>
#include <kdialog.h>

// Local includes

#include "ui_soundtrackdialog.h"
#include "playbackwidget.h"
#include "listsounditems.h"

namespace KIPIAdvancedSlideshowPlugin
{

class PlaybackWidget;

class SharedContainer;

class SoundtrackPreview : public KDialog
{

public :

    SoundtrackPreview(QWidget* const, KUrl::List&, SharedContainer* const);
    ~SoundtrackPreview();

private :

    PlaybackWidget* m_playbackWidget;
};

// ----------------------------------------------------------------------

class SoundtrackDialog : public QWidget, public Ui::SoundtrackDialog
{
    Q_OBJECT

public:

    SoundtrackDialog(QWidget* const parent, SharedContainer* const sharedData);
    ~SoundtrackDialog();

    void readSettings();
    void saveSettings();

private:

    void addItems(const KUrl::List& fileList);
    void updateTracksNumber();
    void updateFileList();
    void compareTimes();

private Q_SLOTS:

    void slotAddDropItems(const KUrl::List& filesUrl);
    void slotSoundFilesButtonAdd();
    void slotSoundFilesButtonDelete();
    void slotSoundFilesButtonUp();
    void slotSoundFilesButtonDown();
    void slotSoundFilesButtonLoad();
    void slotSoundFilesButtonSave();
    void slotSoundFilesButtonReset();
    void slotSoundFilesSelected(int);
    void slotPreviewButtonClicked();
    void slotImageTotalTimeChanged(const QTime&);
    void slotAddNewTime(const KUrl&, const QTime&);

private:

    KUrl::List              m_urlList;
    SharedContainer*        m_sharedData;
    QTime                   m_totalTime;
    QTime                   m_imageTime;
    QMap<KUrl, QTime>*      m_tracksTime;
    QMap<KUrl, SoundItem*>* m_soundItems;
    QMutex*                 m_timeMutex;
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif // SOUNDTRACKDIALOG_H
