/* ============================================================
 * Copyright 2004 by Tudor Calin <tudor@1xtech.com>

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
#ifndef CAMERASELECTION_H
#define CAMERASELECTION_H

// Qt includes.

#include <qstring.h>
#include <qstringlist.h>

// KDE includes.

#include <kdialogbase.h>

// Local includes

#include "kpaboutdata.h"

class QComboBox;
class QListView;
class QListViewItem;
class QRadioButton;
class QVButtonGroup;
class QLabel;
class QLineEdit;
class QPushButton;

namespace KIPIKameraKlientPlugin
{

class CameraSelection : public KDialogBase 
{
    Q_OBJECT

public:
    
    CameraSelection( QWidget* parent = 0 );
    ~CameraSelection();
    
    void setCamera(const QString& model, const QString& port);
    QString currentModel();
    QString currentPortPath();

private:
    
    void getCameraList();
    void getSerialPortList();
    
    QListView*     listView_;
    
    QVButtonGroup* portButtonGroup_;
    
    QRadioButton*  usbButton_;
    QRadioButton*  serialButton_;
    
    QLabel*        portPathLabel_;
    
    QComboBox*     portPathComboBox_;
    
    QStringList    serialPortList_;
    
    QPushButton*   helpButton_;
    
    KIPIPlugins::KPAboutData*   m_about;

private slots:
    
    void slotSelectionChanged(QListViewItem *item);
    void slotPortChanged();
    void slotOkClicked();
    void slotHelp();   
    
signals:
    
    void signalOkClicked(const QString& model, const QString& port);
};

}  // NameSpace KIPIKameraKlientPlugin

#endif 
