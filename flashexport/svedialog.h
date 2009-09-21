/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SVEDIALOG_H
#define SVEDIALOG_H

// Include files for KDE

#include <kpagedialog.h>
#include <kurl.h>

// Include files for KIPI

#include <libkipi/interface.h>

// Local includes

#include "simpleviewersettingscontainer.h"

namespace KIPIFlashExportPlugin
{
class SVEDialogPriv;

class SVEDialog : public KPageDialog
{
    Q_OBJECT

public:

    SVEDialog(KIPI::Interface *interface, QWidget *parent=0);
    ~SVEDialog();

    SimpleViewerSettingsContainer settings() const;

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

    SVEDialogPriv* const d;
};

} // namespace KIPIFlashExportPlugin

#endif /* SVEDIALOG_H */
