/* ============================================================
 * File  : cameraui.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-01-21
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju
 * Copyright 2004 by Tudor Calin

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef CAMERAUI_H
#define CAMERAUI_H

// Qt
#include <qdict.h>
#include <qstring.h>
// KDE
#include <kfileitem.h>
#include <kdialogbase.h>
// Local
#include "gpfileiteminfo.h"
#include "cameratype.h"

class QComboBox;
class QImage;
class QLabel;
class QLineEdit;
class QPushButton;

class KAction;
class KComboBox;
class KDirLister;
class KProgress;
class KStatusBar;
class KToggleAction;

namespace KIO {
	class Job;
}

namespace KIPIKameraKlientPlugin
{

class CameraList;
class CameraType;
class GPEventFilter;
class GPController;
class GPFileItemContainer;
class CameraIconItem;
class CameraIconView;
class CameraFolderItem;
class CameraFolderView;

class CameraUI : public KDialogBase {
    Q_OBJECT
	
public:
    CameraUI();
    ~CameraUI();
    void setCameraConnected(bool val);
    static CameraUI* getInstance();
    const CameraType* cameraType();
    void setCameraType(const CameraType& ctype);
    void cameraInitialized(bool val);
    void cameraSubFolder(const QString& folder, const QString& subFolder);
    void cameraNewItems(const QString& folder, const GPFileItemInfoList& infoList);
    void cameraNewItems(const GPFileItemInfoList& infoList);
    void cameraNewThumbnail(const QString& folder, const QString& itemName, const QImage&  thumbnail);
    void cameraDownloadedItem(const QString& folder, const QString& itemName);
    void cameraDeletedItem(const QString& folder, const QString& itemName);
    void cameraErrorMsg(const QString& msg);
   
private:
    void setupActions();
    void setupConnections();
    bool cameraReadyForUpload(QString& reason);
    void downloadOneItem(const QString& item, const QString& folder, const QString& downloadDir, bool& proceedFurther, bool& overwriteAll);
    
signals:
    void signalStatusMsg(const QString&);
    void signalProgressVal(int);
    void signalBusy(bool);

public slots:
    void slotCameraConnectToggle();
    void slotCameraDownloadSelected();
    void slotCameraDeleteSelected();
    void slotCameraUpload();
    void slotCameraCancel();
    void slotSelectAll();
    void slotSelectNone();
    void slotSelectInvert();
    void slotSelectNew();
   
private slots:
	void slotClose();
    void slotSetStatusMsg(const QString& msg);
    void slotSetProgressVal(int val);
    void slotResetStatusBar();
    void slotBusy(bool val);
    void slotSetupCamera();
    void slotSyncCameraComboBox();
    void slotFolderSelected(CameraFolderItem *item);
	void slotChangeDownloadDirectory();
	void writeSettings();
	void readSettings();
    
private:
    QLabel              *mStatusLabel;
    KProgress           *mProgressBar;
    QComboBox           *mCameraComboBox;
    QPushButton         *mCameraConnectButton;
    QPushButton         *mCameraSetupButton;
	QPushButton         *mCameraCancelButton;
    KAction             *mSelectAllAction;
    KAction             *mSelectNoneAction;
    KAction             *mSelectInvertAction;
    KAction             *mSelectNewAction;
    KToggleAction       *mCameraConnectAction;
	CameraType          *mCameraType;
    CameraList       	*mCameraList;
	KStatusBar        	*mStatusBar;
	KConfig             *mConfig; 
    GPEventFilter       *efilter_;
    GPController        *controller_;
    GPFileItemContainer *container_;
    CameraFolderView    *mFolderView;
    CameraIconView      *mIconView;
    QString             cameraPath_;
    bool                cameraConnected_;
	QPushButton         *mChangeDownloadDirectoryButton;
	QLineEdit           *mDownloadDirectoryEdit;
};

}  // NameSpace KIPIKameraKlientPlugin

#endif 
