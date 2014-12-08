/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-06-08
 * Description : the main window of the removeredeyes plugin
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef REMOVEREDEYESWINDOW_H
#define REMOVEREDEYESWINDOW_H

// Local includes

#include "kptooldialog.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIRemoveRedEyesPlugin
{

class WorkerThreadData;

class RemoveRedEyesWindow : public KPToolDialog
{
    Q_OBJECT

public:

    enum TabStyle
    {
        FileList = 0,
        Settings,
        Preview
    };

public:

    RemoveRedEyesWindow();
    ~RemoveRedEyesWindow();

Q_SIGNALS:

    void myCloseClicked();
    void locatorUpdated();

private Q_SLOTS:

    void updateSummary();
    void resetSummary();

    void closeClicked();
    void slotButtonClicked(int);

    void foundRAWImages(bool);
    void imageListChanged();
    void tabwidgetChanged(int);

    void startPreview();
    void startTestrun();
    void startCorrection();
    void cancelCorrection();
    void threadFinished();

    void locatorChanged();

public Q_SLOTS:

    void calculationFinished(WorkerThreadData*);

private:

    void readSettings();
    void writeSettings();
    void updateSettings();

    bool acceptStorageSettings();
    void handleUnprocessedImages();
    void setBusy(bool);
    void showSummary();
    void startWorkerThread(const KUrl::List& urls);
    void initProgressBar(int max);

    void loadLocator(const QString& locator);
    void unloadLocator();

    int totalImages()     const;
    int processedImages() const;
    int failedImages()    const;

private:

    class Private;
    Private* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif // REMOVEREDEYESWINDOW_H
