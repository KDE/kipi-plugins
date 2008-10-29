/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-10-29
 * Description : Twain interface
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef TWAINDIALOG_H
#define TWAINDIALOG_H

// Qt includes.

#include <QCloseEvent>

// KDE includes.

#include <kdialog.h>

class QWidget;

namespace KIPI
{
    class Interface;
}

namespace KIPIAcquireImagesPlugin
{

class TwainDialogPriv;

class TwainDialog : public KDialog
{
    Q_OBJECT

public:

    TwainDialog(KIPI::Interface* interface=0, QWidget *parent=0);
    ~TwainDialog();

    void showEvent(QShowEvent*);

protected:

    bool winEvent(MSG* pMsg, long *result);
    void closeEvent(QCloseEvent* e);

private slots:

    void slotInit();
    void slotImageAcquired(const QImage& img);
    void slotSaveImage();
    void slotClose();
    void slotHelp();

private:

    void readSettings();
    void saveSettings();

private:

    TwainDialogPriv* d;
};

}  // namespace KIPIAcquireImagesPlugin

#endif // TWAINDIALOG_H
