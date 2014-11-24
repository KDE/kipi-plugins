/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-13
 * Description : a plugin to export to flash
 *
 * Copyright (C) 2011 by Veaceslav Munteanu <slavuttici at gmail dot com>
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

#ifndef FIRSTRUNPAGE_H
#define FIRSTRUNPAGE_H

// Include files for KDE

#include <kdialog.h>
#include <kurl.h>

// Include files for KIPI

#include <libkipi/interface.h>

// Local includes

#include "kpwizardpage.h"

using namespace KIPIPlugins;

namespace KIPIFlashExportPlugin
{

/**
 * To avoid licensing problems with some distributions, the SimpleViewer
 * Flash cannot be shipped with the plugin. During the first run of the
 * plugin, the user has to download SimpleViewer from its homepage and point
 * the plugin to that archive to install it. This is done by this dialog.
 */
class FirstRunPage : public KPWizardPage
{
    Q_OBJECT

public:

    explicit FirstRunPage(KAssistantDialog* const dlg);
    ~FirstRunPage();

    /**
     * Returns the URL, where the SimpleViewer package is stored
     */
    KUrl getUrl() const;

Q_SIGNALS:

    void signalUrlObtained();

private Q_SLOTS:

    /**
     * Opens the browser with the SimpleViewer download page
     */
    void slotDownload(const QString& url);

    /**
     * Starts the installation of SimpleViewer
     */
    void slotUrlSelected(const KUrl& url);

private:

    class FirstRunPagePriv;
    FirstRunPagePriv* const d;
};

}   // namespace KIPIFlashexport

#endif /* FIRSTRUNPAGE_H */
