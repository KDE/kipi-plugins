/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-09-28
 * Description : a tool to export image to a KIO accessible
 *               location
 *
 * Copyright (C) 2006-2009 by Johannes Wienke <languitar at semipol dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KIOEXPORTWINDOW_H
#define KIOEXPORTWINDOW_H

// KDE includes

#include <KDialog>
#include <KUrl>
#include <kio/job.h>

// Local includes

#include "KioExportWidget.h"

namespace KIPI
{
    class Interface;
}

namespace KIPIPlugins
{
    class KPAboutData;
}

namespace KIPIKioExportPlugin
{

/**
 * Main window of the KIOExport Plugin.
 */

class KioExportWindow: public KDialog
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * @param parent parent QWidget
     * @param interface pointer to the KIPI interface to use. Must not be null.
     */
    KioExportWindow(QWidget *parent, KIPI::Interface *interface);

    /**
     * Destructor.
     */
    virtual ~KioExportWindow();

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void reactivate();

private Q_SLOTS:

    /**
     * Processes changes on the image list.
     */
    void slotImageListChanged();

    /**
     * Starts uploading the selected images.
     */
    void slotUpload();

    /**
     * Processes changes in the target url.
     */
    void slotTargetUrlChanged(const KUrl & target);

    /**
     * Removes the copied image from the image list.
     */
    void slotCopyingDone(KIO::Job *job, const KUrl& from, const KUrl& to,
                         time_t mtime, bool directory, bool renamed);

    /**
     * Re-enables the dialog after the job finished and displays a warning if
     * something didn't work.
     */
    void slotCopyingFinished(KJob *job);

    /**
     * Load plugin handbook.
     */
    void slotHelp();

    /**
     * Handle Close button.
     */
    void slotClose();

protected:

    /**
     * Handle Close event from dialog title bar.
     */
    void closeEvent(QCloseEvent *e);

    /**
     * Refresh status (enabled / disabled) of the upload button according to
     * contents of the image list and the specified target.
     */
    void updateUploadButton();

    /**
     * Restores settings.
     */
    void restoreSettings();

    /**
     * Saves settings.
     */
    void saveSettings();

private:

    const static QString TARGET_URL_PROPERTY;
    const static QString CONFIG_GROUP;

private:

    KIPI::Interface          *m_interface;

    KioExportWidget          *m_exportWidget;

    KIPIPlugins::KPAboutData *m_about;
};

} // namespace KIPIKioExportPlugin

#endif /* KIOEXPORTWINDOW_H */
