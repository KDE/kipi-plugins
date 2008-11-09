/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : the main window of the removeredeyes plugin
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

// KDE includes.

#include <kdialog.h>

class KDialog;

namespace KIPI
{
class Interface;
}

namespace KIPIRemoveRedEyesPlugin
{

class RemovalSettings;
class WorkerThreadData;
class RedEyesWindowPriv;

class RemoveRedEyesWindow : public KDialog
{
    Q_OBJECT

public:

    enum TabStyle
    {
        FileList = 0,
        Settings
    };

public:

    RemoveRedEyesWindow(KIPI::Interface *interface, QWidget *parent);
    ~RemoveRedEyesWindow();

signals:

    void testRunFinished();
    void myCloseClicked();

private slots:

    void closeClicked();
    void helpClicked();

    void checkForNoneCorrectedImages();
    void foundRAWImages(bool);
    void imageListChanged(bool);

    void progressBarChanged(int);
    void progressBarTimedOut();

    void startCorrection();
    void abortCorrection();
    void startTestrun();

    // reimplement this slot from KDialog to avoid the call for close(),
    // we will do this on our own here.
    void slotButtonClicked(int);

public slots:

    void calculationFinished(WorkerThreadData*);

private:

    void readSettings();
    void writeSettings();
    void updateSettings();

    void startWorkerThread(int type);
    void setBusy(bool);


private:

    RedEyesWindowPriv* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif // REMOVEREDEYESWINDOW_H
