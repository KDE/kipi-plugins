/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <kdialog.h>
#include <kurl.h>

// Local includes

#include "emailsettings.h"

class QCloseEvent;

namespace KIPISendimagesPlugin
{

class SendImagesDialog : public KDialog
{
    Q_OBJECT

public:

    SendImagesDialog(QWidget* const parent, const KUrl::List& urls);
    ~SendImagesDialog();

    EmailSettings emailSettings() const;

protected:

    void closeEvent(QCloseEvent*);

private Q_SLOTS:

    void slotOk();
    void slotHelp();
    void slotCancel();
    void slotImagesCountChanged();

private:

    void readSettings();
    void saveSettings();

private:

    class SendImagesDialogPrivate;
    SendImagesDialogPrivate* const d;
};

}  // namespace KIPISendimagesPlugin

#endif /* SENDIMAGESDIALOG_H */
