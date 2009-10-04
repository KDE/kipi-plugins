/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 04.10.2009
 * Description : A tool for importing images via KIO
 *
 * Copyright (C) 2009 by Johannes Wienke <languitar at semipol dot de>
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

#ifndef KIOIMPORTWINDOW_H
#define KIOIMPORTWINDOW_H

// KDE includes

#include <kdialog.h>
#include <kjob.h>
#include <kurl.h>
#include <kio/job.h>

// Local includes

#include "KioImportWidget.h"

namespace KIPI
{
    class Interface;
}

namespace KIPIPlugins
{
    class KPAboutData;
    class ImagesList;
}

namespace KIPIKioExportPlugin
{

/**
 * Main dialog used for the kio import tool.
 */
class KioImportWindow : public KDialog
{

    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * @param parent parent widget
     */
    KioImportWindow(QWidget *parent, KIPI::Interface *interface);

    /**
     * Destructor.
     */
    virtual ~KioImportWindow();

private slots:

    /**
     * Starts importing the selected images.
     */
    void slotImport();

    /**
     * Reacts on changes in the image list or the target to update button
     * activity etc.
     */
    void slotSourceAndTargetUpdated();

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

private:

    KIPI::Interface          *m_interface;

    KioImportWidget          *m_importWidget;

    KIPIPlugins::KPAboutData *m_about;
};

} // namespace KIPIKioExportPlugin

#endif /* KIOIMPORTWINDOW_H */
