/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-24
 * Description : Raw converter batch dialog
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

#ifndef BATCHDIALOG_H
#define BATCHDIALOG_H

// Qt includes.

#include <QStringList>

// KDE includes.

#include <kdialog.h>

// Local includes

#include "kpaboutdata.h"

class QEvent;
class QCloseEvent;

namespace KIPIRawConverterPlugin
{

class BatchDialogPriv;

class BatchDialog : public KDialog
{

Q_OBJECT

public:

    BatchDialog(QWidget *parent);
    ~BatchDialog();

    void addItems(const QStringList& itemList);

protected:

    void customEvent(QEvent *event);
    void closeEvent(QCloseEvent *e);

private:

    void readSettings();
    void saveSettings();

    void busy(bool busy);

    void processOne();
    void processing(const QString& file);
    void processed(const QString& file, const QString& tmpFile);
    void processingFailed(const QString& file);

private slots:

    void slotDefault();
    void slotClose();
    void slotHelp();
    void slotUser1();
    void slotUser2();
    void slotAborted();

    void slotSaveFormatChanged();
    void slotConvertBlinkTimerDone();

private:

    BatchDialogPriv* d;
};

} // NameSpace KIPIRawConverterPlugin

#endif /* BATCHDIALOG_H */
