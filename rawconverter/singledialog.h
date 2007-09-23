/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-22
 * Description : Raw converter single dialog
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SINGLEDIALOG_H
#define SINGLEDIALOG_H

// Qt includes.

#include <QString>

// KDE includes.

#include <kdialog.h>

// Local includes

#include "kpaboutdata.h"

class QCloseEvent;
class QEvent;

namespace KIPIRawConverterPlugin
{

class SingleDialogPriv;

class SingleDialog : public KDialog
{
    Q_OBJECT

public:

    SingleDialog(const QString& file, QWidget *parent);
    ~SingleDialog();

protected:

    void customEvent(QEvent *event);
    void closeEvent(QCloseEvent *e);
    
private:

    void readSettings();
    void saveSettings();

    void busy(bool busy);

    void identified(const QString&, const QString& identity, const QPixmap& preview);

    void previewing(const QString&);
    void previewed(const QString&, const QString& tmpFile);
    void previewFailed(const QString&);

    void processing(const QString&);
    void processed(const QString&, const QString& tmpFile);
    void processingFailed(const QString&);

private slots:
    
    void slotDefault();
    void slotClose();
    void slotHelp();
    void slotUser1();
    void slotUser2();
    void slotUser3();
    
    void slotIdentify();

    void slotPreviewBlinkTimerDone();
    void slotConvertBlinkTimerDone();
    
private:

    SingleDialogPriv* d;
};

} // NameSpace KIPIRawConverterPlugin

#endif // SINGLEDIALOG_H
