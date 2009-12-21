/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef EXPOBLENDINGDLG_H
#define EXPOBLENDINGDLG_H

// Qt includes

#include <QString>
#include <QPixmap>

// KDE includes

#include <kdialog.h>
#include <kurl.h>

// Local includes

#include "actions.h"

class QCloseEvent;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIExpoBlendingPlugin
{

class Manager;
class ActionData;
class ExpoBlendingAboutData;
class ExpoBlendingDlgPriv;

class ExpoBlendingDlg : public KDialog
{
    Q_OBJECT

public:

    ExpoBlendingDlg(Manager* mngr, QWidget* parent=0);
    ~ExpoBlendingDlg();

    void loadCurrentSelection();
    void loadItems(const KUrl::List& urls);

private:

    void closeEvent(QCloseEvent*);

    void readSettings();
    void saveSettings();

    void busy(bool busy);

    void setIdentity(const KUrl& url, const QString& identity);

    void processing(const KUrl& url);
    void processed(const KUrl& url, const KUrl& tmpFile);
    void processingFailed(const KUrl& url);

    void clearEnfusedTmpFile();

private Q_SLOTS:

    void slotDefault();
    void slotClose();
    void slotHelp();
    void slotUser1();
    void slotUser2();
    void slotUser3();

    void slotAction(const KIPIExpoBlendingPlugin::ActionData&);
    void slotAddItems(const KUrl::List& urls);

private:

    ExpoBlendingDlgPriv* const d;
};

} // namespace KIPIExpoBlendingPlugin

#endif // EXPOBLENDINGDLG_H
