/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date  : 2003-10-24
 * Description : Raw converter batch dialog
 *
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006 by Gilles Caulier
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

#ifndef BATCHDIALOG_H
#define BATCHDIALOG_H

// Qt includes.

#include <qstringlist.h>
#include <qdict.h>

// KDE includes.

#include <kdialogbase.h>

class QTimer;
class QWidget;
class QCustomEvent;
class QCloseEvent;

class KListView;
class KProgress;

namespace KIPIRawConverterPlugin
{

class  ActionThread;
class  DcrawSettingsWidget;
class  SaveSettingsWidget;
struct RawItem;

class BatchDialog : public KDialogBase
{

Q_OBJECT

public:

    BatchDialog(QWidget *parent);
    ~BatchDialog();

    void addItems(const QStringList& itemList);

protected:

    void customEvent(QCustomEvent *event);
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

    void slotClose();
    void slotHelp();
    void slotUser1();
    void slotUser2();
    void slotAborted();

    void slotSaveFormatChanged();
    void slotConvertBlinkTimerDone();

private:

    bool                 m_convertBlink;

    QTimer              *m_blinkConvertTimer;

    QWidget             *m_page;

    QDict<RawItem>       m_itemDict;

    QStringList          m_fileList;

    KProgress           *m_progressBar;

    KListView           *m_listView;

    RawItem             *m_currentConvertItem;
   
    ActionThread        *m_thread;

    DcrawSettingsWidget *m_decodingSettingsBox;

    SaveSettingsWidget  *m_saveSettingsBox;
};

} // NameSpace KIPIRawConverterPlugin

#endif /* BATCHDIALOG_H */
