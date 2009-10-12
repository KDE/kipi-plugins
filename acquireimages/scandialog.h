/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-09-09
 * Description : scanner dialog
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <kdialog.h>

class QWidget;

namespace KIPI
{
    class Interface;
}

namespace KSaneIface
{
    class KSaneWidget;
}

namespace KIPIAcquireImagesPlugin
{

class ScanDialogAboutData;
class ScanDialogPriv;

class ScanDialog : public KDialog
{
    Q_OBJECT

public:

    ScanDialog(KIPI::Interface* interface, KSaneIface::KSaneWidget* saneWidget,
               QWidget* parent, ScanDialogAboutData *about);
    ~ScanDialog();

protected:

    void closeEvent(QCloseEvent*);

private Q_SLOTS:

    void slotSaveImage(QByteArray&, int, int, int, int);
    void slotClose();
    void slotHelp();
    void slotThreadDone(const KUrl&, bool);

private:

    void readSettings();
    void saveSettings();

private:

    ScanDialogPriv* const d;
};

}  // namespace KIPIAcquireImagesPlugin

#endif // SCANDIALOG_H
