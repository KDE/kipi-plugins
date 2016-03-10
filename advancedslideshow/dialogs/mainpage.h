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

#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

// Qt includes

#include <QPixmap>

// Local includes

#include "ui_mainpage.h"
#include "kpsvgpixmaprenderer.h"

class QTreeWidgetItem;

namespace KIPIPlugins
{
    class KPImagesList;
}

namespace KIPIAdvancedSlideshowPlugin
{

class PresentationContainer;

class MainPage : public QWidget, Ui::MainPage
{
    Q_OBJECT

public:

    MainPage(QWidget* const parent, PresentationContainer* const sharedData);
    ~MainPage();

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
    void slotThumbnail(const QUrl&, const QPixmap&);
    void slotImageListChanged();

    void slotPortfolioDurationChanged(int);
    void slotImagesFilesSelected(QTreeWidgetItem* item);

private:

    void setupConnections();
    void loadEffectNames();
    void loadEffectNamesGL();
    void showNumberImages();
    void addItems(const QList<QUrl>& fileList);

private:

    class Private;
    Private* const d;
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif // MAIN_PAGE_H
