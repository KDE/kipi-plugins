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

#include <qdialog.h>
#include <qstring.h>

class QFrame;
class QCheckBox;
class QPushButton;
class QVButtonGroup;
class QRadioButton;
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

    SingleDialog(const QString& file, QWidget *parent);
    ~SingleDialog();

protected:

    void closeEvent(QCloseEvent *e);
    
private:

    void readSettings();
    void saveSettings();

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
    
private:

    QCheckBox         *cameraWBCheckBox_;
    QCheckBox         *fourColorCheckBox_;
    
    QPushButton       *helpButton_;
    QPushButton       *previewButton_;
    QPushButton       *processButton_;
    QPushButton       *closeButton_;
    QPushButton       *abortButton_;

    QRadioButton      *jpegButton_;
    QRadioButton      *tiffButton_;
    QRadioButton      *ppmButton_;
    
    QVButtonGroup     *saveButtonGroup_;

    QString            inputFile_;
    QString            inputFileName_;
    
    CSpinBox          *brightnessSpinBox_;
    CSpinBox          *redSpinBox_;
    CSpinBox          *blueSpinBox_;

    PreviewWidget     *previewWidget_;

    ProcessController *controller_;
};

} // NameSpace KIPIRawConverterPlugin

#endif // SINGLEDIALOG_H
