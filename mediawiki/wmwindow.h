/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to WikiMedia web service
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Parthasarathy Gopavarapu <gparthasarathy93 at gmail dot com>
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

#ifndef WMWINDOW_H
#define WMWINDOW_H

// MediaWiki includes

#include <libmediawiki/login.h>
#include <libmediawiki/mediawiki.h>

// Local includes

#include "kptooldialog.h"

class QCloseEvent;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KIPIPlugins;
using namespace mediawiki;

namespace KIPIWikiMediaPlugin
{

class WmWidget;
class WmLogin;
class WikiMediaJob;

class WMWindow : public KPToolDialog
{
    Q_OBJECT

public:

    WMWindow(Interface* const interface, const QString& tmpFolder, QWidget* const parent);
    ~WMWindow();

    void reactivate();

private Q_SLOTS:

    void slotClose();
    void slotStartTransfer();
    void slotChangeUserClicked();
    void slotDoLogin(const QString& login, const QString& pass, const QUrl& wiki);
    void slotEndUpload();
    int  slotLoginHandle(KJob* loginJob);

private:

    void closeEvent(QCloseEvent*);
    void readSettings();
    void saveSettings();

private:

    QString       m_tmpDir;
    QString       m_tmpPath;
    QString       m_login;
    QString       m_pass;
    QUrl          m_wiki;

    WmWidget*     m_widget;
    MediaWiki*    m_mediawiki;

    Interface*    m_interface;
    WmLogin*      m_dlgLoginExport;
    WikiMediaJob* m_uploadJob;
};

} // namespace KIPIWikiMediaPlugin

#endif // WMWINDOW_H
