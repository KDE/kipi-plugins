/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-09-09
 * Description : scanner dialog
 *
 * Copyright (C) 2007-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SCANDIALOG_H
#define SCANDIALOG_H

// Qt includes

#include <QCloseEvent>

// local includes

#include "kptooldialog.h"

class QWidget;

namespace KSaneIface
{
    class KSaneWidget;
}

using namespace KIPIPlugins;
using namespace KSaneIface;

namespace KIPIAcquireImagesPlugin
{

class ScanDialogAboutData;

class ScanDialog : public KPToolDialog
{
    Q_OBJECT

public:

    ScanDialog(KSaneWidget* const saneWidget, QWidget* const parent, ScanDialogAboutData* const about);
    ~ScanDialog();

protected:

    void closeEvent(QCloseEvent*);

private Q_SLOTS:

    void slotSaveImage(QByteArray&, int, int, int, int);
    void slotThreadDone(const KUrl&, bool);
    void slotCloseClicked();

private:

    void readSettings();
    void saveSettings();

private:

    class Private;
    Private* const d;
};

}  // namespace KIPIAcquireImagesPlugin

#endif // SCANDIALOG_H
