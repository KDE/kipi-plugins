/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2012-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SOUNDTRACK_PAGE_H
#define SOUNDTRACK_PAGE_H

// Qt includes

#include <QTime>
#include <QMutex>
#include <QUrl>
#include <QDialog>

// Local includes

#include "ui_soundtrackpage.h"
#include "playbackwidget.h"
#include "listsounditems.h"

namespace KIPIAdvancedSlideshowPlugin
{

class PlaybackWidget;
class SharedContainer;

class SoundtrackPreview : public QDialog
{

public :

    SoundtrackPreview(QWidget* const, const QList<QUrl>&, SharedContainer* const);
    ~SoundtrackPreview();

private :

    PlaybackWidget* m_playbackWidget;
};

// ----------------------------------------------------------------------

class SoundtrackPage : public QWidget, public Ui::SoundtrackPage
{
    Q_OBJECT

public:

    SoundtrackPage(QWidget* const parent, SharedContainer* const sharedData);
    ~SoundtrackPage();

    void readSettings();
    void saveSettings();

private:

    void addItems(const QList<QUrl>& fileList);
    void updateTracksNumber();
    void updateFileList();
    void compareTimes();

private Q_SLOTS:

    void slotAddDropItems(const QList<QUrl>& filesUrl);
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
    void slotAddNewTime(const QUrl&, const QTime&);

private:

    class Private;
    Private* const d;
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif // SOUNDTRACK_PAGE_H
