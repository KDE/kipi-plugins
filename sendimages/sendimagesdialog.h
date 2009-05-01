/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SENDIMAGESDIALOG_H
#define SENDIMAGESDIALOG_H

// KDE includes

#include <kpagedialog.h>
#include <kurl.h>

// Local includes

#include "emailsettingscontainer.h"

class QCloseEvent;

namespace KIPI
{
    class Interface;
}

namespace KIPISendimagesPlugin
{

class SendImagesDialogPrivate;

class SendImagesDialog : public KPageDialog
{
    Q_OBJECT

public:

    SendImagesDialog(QWidget* parent, KIPI::Interface *iface, const KUrl::List& urls);
    ~SendImagesDialog();

    EmailSettingsContainer emailSettings();

protected:

    void closeEvent(QCloseEvent *);

private Q_SLOTS:

    void slotOk();
    void slotHelp();
    void slotCancel();

private:

    void readSettings();
    void saveSettings();

    int  activePageIndex();
    void showPage(int page);

private:

    SendImagesDialogPrivate* const d;
};

}  // namespace KIPISendimagesPlugin

#endif /* SENDIMAGESDIALOG_H */
