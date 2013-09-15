/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-09-04
 * Description : a tool to blend bracketed images/create HDR images.
 *
 * Copyright (C) 2013 by Soumajyoti Sarkar <ergy dot ergy at gmail dot com>
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

#ifndef HDRIMAGEDLG_H
#define HDRIMAGEDLG_H

// Qt includes

#include <QString>
#include <QPixmap>

// KDE includes

#include <kurl.h>

// Local includes

#include "actions.h"
#include "kptooldialog.h"

class QCloseEvent;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

class Manager;
class ActionData;

class HdrImageDlg : public KPToolDialog
{
    Q_OBJECT

public:

    explicit HdrImageDlg(Manager* const mngr, QWidget* const parent=0);
    ~HdrImageDlg();

    void loadItems(const KUrl::List& urls);

private:

    void closeEvent(QCloseEvent*);

    void readSettings();
    void saveSettings();

    void busy(bool busy);
    void saveItem(const KUrl& temp, const EnfuseSettings& settings);

    void setIdentity(const KUrl& url, const QString& identity);

private Q_SLOTS:

    void slotDefault();
    void slotClose();
    void slotPreview();
    void slotProcess();
    void slotAbort();

    void slotLoadProcessed(const KUrl&);
    void slotAction(const KIPIExpoBlendingPlugin::ActionData&);
    void slotAddItems(const KUrl::List& urls);

    void slotPreviewButtonClicked();
    void slotFileFormatChanged();

private:

    class HdrImageDlgPriv;
    HdrImageDlgPriv* const d;
};

} // namespace KIPIExpoBlendingPlugin

#endif //  HDRIMAGEDLG_H
