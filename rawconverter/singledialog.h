/* ============================================================
 * File  : singledialog.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-10-22
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju

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

#include <qdialog.h>
#include <qstring.h>

class QFrame;
class QCheckBox;
class QPushButton;
class QVButtonGroup;
class QCloseEvent;

namespace KIPIRawConverterPlugin
{

class CSpinBox;
class PreviewWidget;
class ProcessController;

class SingleDialog : public QDialog
{
    Q_OBJECT

public:

    SingleDialog(const QString& file);
    ~SingleDialog();

protected:

    void closeEvent(QCloseEvent *e);
    
private:

    void readSettings();
    void saveSettings();
    
    PreviewWidget* previewWidget_;
    
    QCheckBox*     cameraWBCheckBox_;
    QCheckBox*     fourColorCheckBox_;
    
    CSpinBox*      gammaSpinBox_;
    CSpinBox*      brightnessSpinBox_;
    CSpinBox*      redSpinBox_;
    CSpinBox*      blueSpinBox_;
    
    QPushButton*   helpButton_;
    QPushButton*   previewButton_;
    QPushButton*   processButton_;
    QPushButton*   closeButton_;
    QPushButton*   abortButton_;
    
    QVButtonGroup* saveButtonGroup_;

    QString            inputFile_;
    QString            inputFileName_;
    
    ProcessController* controller_;


private slots:

    void slotHelp();
    void slotPreview();
    void slotProcess();
    void slotClose();
    void slotAbort();
    void slotIdentify();

    void slotBusy(bool val);
    void slotIdentified(const QString&, const QString& identity);
    void slotIdentifyFailed(const QString&, const QString& identity);
    void slotPreviewing(const QString&);
    void slotPreviewed(const QString&, const QString& tmpFile_);
    void slotPreviewFailed(const QString&);
    void slotProcessing(const QString&);
    void slotProcessed(const QString&, const QString& tmpFile_);
    void slotProcessingFailed(const QString&);

};

} // NameSpace KIPIRawConverterPlugin

#endif // SINGLEDIALOG_H
