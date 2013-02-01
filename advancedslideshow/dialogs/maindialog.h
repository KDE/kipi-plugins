/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-09
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
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

#include <QPixmap>

// KDE includes

#include "kio/previewjob.h"
#include "kurl.h"

// Local includes

#include "ui_maindialog.h"

class QTreeWidgetItem;

namespace KIPIPlugins
{
    class KPImagesList;
}

namespace KIPIAdvancedSlideshowPlugin
{

class SharedContainer;

class MainDialog : public QWidget, Ui::MainDialog
{
    Q_OBJECT

public:

    MainDialog(QWidget* const parent, SharedContainer* const sharedData);
    ~MainDialog();

    void readSettings();
    void saveSettings();
    bool updateUrlList();

Q_SIGNALS :

    void signalTotalTimeChanged(const QTime&);


private Q_SLOTS:

    void slotOpenGLToggled();
    void slotEffectChanged();
    void slotDelayChanged(int);
    void slotPrintCommentsToggled();
    void slotSelection();
    void slotUseMillisecondsToggled();
    void slotThumbnail(const KUrl&, const QPixmap&);
    void slotImageListChanged();

    void slotPortfolioDurationChanged(int);
    void slotImagesFilesSelected(QTreeWidgetItem* item);

private:

    void setupConnections();
    void loadEffectNames();
    void loadEffectNamesGL();
    void showNumberImages();
    void addItems(const KUrl::List& fileList);

private:

    SharedContainer*           m_sharedData;
    KIO::PreviewJob*           m_thumbJob;
    QTime                      m_totalTime;
    QPixmap                    m_noPreviewPixmap;
    KIPIPlugins::KPImagesList* m_ImagesFilesListBox;
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif // MAINDIALOG_H
