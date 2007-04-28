/* ============================================================
 * File  : setupcamera.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-10
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#ifndef SETUPCAMERA_H
#define SETUPCAMERA_H

// KDE includes.

#include <kdialogbase.h>

// Local includes

#include "kpaboutdata.h"

class QListView;
class QListViewItem;
class QPushButton;

namespace KIPIKameraKlientPlugin
{

class SetupCamera : public KDialogBase 
{
    Q_OBJECT

public:
    
    SetupCamera(QWidget* parent = 0, const char* name = 0);
    ~SetupCamera();
    
    void applySettings();

private:
    
    QListView*   listView_;
    
    QPushButton* addButton_;
    QPushButton* removeButton_;
    QPushButton* editButton_;
    QPushButton* autoDetectButton_;
    QPushButton* helpButton_;
    
    KIPIPlugins::KPAboutData* m_about;

private slots:
        
    void slotHelp();    
    void slotSelectionChanged();
    void slotAddCamera();
    void slotRemoveCamera();
    void slotEditCamera();
    void slotAutoDetectCamera();
    void slotAddedCamera(const QString& model, const QString& port);
    void slotEditedCamera(const QString& model, const QString& port);
    void slotOkClicked();
};

}  // NameSpace KIPIKameraKlientPlugin

#endif 

