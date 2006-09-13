/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2003-10-22
 * Description : Raw converter single dialog
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

#ifndef SINGLEDIALOG_H
#define SINGLEDIALOG_H

// Qt includes.

#include <qstring.h>

// KDE includes.

#include <kdialogbase.h>

class QCloseEvent;
class QCustomEvent;
class QTimer;

namespace KIPIRawConverterPlugin
{

class PreviewWidget;
class ActionThread;
class DcrawSettingsWidget;
class SaveSettingsWidget;

class SingleDialog : public KDialogBase
{
    Q_OBJECT

public:

    SingleDialog(const QString& file, QWidget *parent);
    ~SingleDialog();

protected:

    void customEvent(QCustomEvent *event);
    void closeEvent(QCloseEvent *e);
    
private:

    void readSettings();
    void saveSettings();

    void busy(bool busy);

    void identified(const QString&, const QString& identity);

    void previewing(const QString&);
    void previewed(const QString&, const QString& tmpFile_);
    void previewFailed(const QString&);

    void processing(const QString&);
    void processed(const QString&, const QString& tmpFile_);
    void processingFailed(const QString&);

private slots:

    void slotHelp();
    void slotUser1();
    void slotUser2();
    void slotUser3();
    void slotClose();

    void slotIdentify();

    void slotPreviewBlinkTimerDone();
    void slotConvertBlinkTimerDone();
    
private:

    bool                 previewBlink_;
    bool                 convertBlink_;

    QString              inputFile_;
    QString              inputFileName_;
    
    QTimer              *blinkPreviewTimer_;
    QTimer              *blinkConvertTimer_;

    PreviewWidget       *previewWidget_;

    ActionThread        *m_thread;

    DcrawSettingsWidget *m_decodingSettingsBox;

    SaveSettingsWidget  *m_saveSettingsBox;
};

} // NameSpace KIPIRawConverterPlugin

#endif // SINGLEDIALOG_H
