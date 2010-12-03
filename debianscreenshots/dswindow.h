/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-11-29
 * Description : a kipi plugin to export images to Debian Screenshots
 *
 * Copyright (C) 2010 by Pau Garcia i Quiles <pgquiles at elpauer dot org>
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

#ifndef DSWINDOW_H
#define DSWINDOW_H

// KDE includes
#include <KDialog>

// LibKIPI includes
#include <libkipi/interface.h>

class QCloseEvent;

class KUrl;

namespace KIPI
{
    class Interface;
}

namespace KIPIPlugins
{
    class KPAboutData;
}

namespace KIPIDebianScreenshotsPlugin
{

class DsTalker;
class DsWidget;

class DsWindow : public KDialog
{
    Q_OBJECT

    Q_ENUMS(MassageKind)

public:

    DsWindow(KIPI::Interface *interface, const QString& tmpFolder, QWidget *parent);
    ~DsWindow();

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void reactivate();

private Q_SLOTS:

    void slotHelp();
    void slotStartTransfer();
    void slotMaybeEnableUser1();
    void slotButtonClicked(int button);
    void slotRequiredPackageInfoAvailableReceived(bool enabled);
    void slotAddScreenshotDone(int errCode, const QString& errMsg);

private:

    enum MassageType { None = 0, ImageIsRaw, ResizeRequired, NotPNG };

    bool prepareImageForUpload(const QString& imgPath, MassageType massage);
    void uploadNextPhoto();
    void buttonStateChange(bool state);
    void closeEvent(QCloseEvent*);

private:

    unsigned int m_imagesCount;
    unsigned int m_imagesTotal;
    QString m_tmpDir;
    QString m_tmpPath;
    KUrl::List m_transferQueue;
    DsTalker *m_talker;
    DsWidget *m_widget;
    KIPI::Interface *m_interface;
    KIPIPlugins::KPAboutData *m_about;
    bool m_uploadEnabled;
};

} // namespace KIPIDebianScreenshotsPlugin

#endif /* DSWINDOW_H */
