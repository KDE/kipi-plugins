/* ============================================================
 * File  : cameraui.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-01-21
 * Description :
 *
 * Copyright 2003 by Renchi Raju
 * Copyright 2004 by Tudor Calin
 *
 * Update : 08/28/2003 - Gilles Caulier <caulier.gilles@free.fr>
 *          Add standard shortcuts and toolbars KDE menu entry.
 *          Add new toolbar icons.
 *          Improve i18n messages.
 *          17/09/2003 - Gilles Caulier <caulier.gilles@free.fr>
 *          Add FullScreen mode.
 *          19/09/2003 - Gilles Caulier <caulier.gilles@free.fr>
 *          Add new default shortcuts.
 *
 *
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

// Qt
#include <qcombobox.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qstringlist.h>
#include <qtooltip.h>
#include <qvaluelist.h>
#include <qvbox.h>
// KDE
#include <kaction.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdirlister.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <klocale.h>
#include <kedittoolbar.h>
#include <kiconloader.h>
#include <kio/job.h>
#include <kkeydialog.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kprogress.h>
#include <krun.h>
#include <kservice.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include <kurl.h>
// Local
#include "camerafolderitem.h"
#include "cameraiconview.h"
#include "camerafolderview.h"
#include "cameraiconitem.h"
#include "cameralist.h"
#include "cameratype.h"
#include "cameraui.h"
#include "dmessagebox.h"
#include "gpcontroller.h"
#include "gpeventfilter.h"
#include "gpfileitemcontainer.h"
#include "gpfileiteminfo.h"
#include "savefiledialog.h"
#include "setupcamera.h"

namespace KIPIKameraKlientPlugin
{

CameraUI::CameraUI() : KDialogBase(Plain, i18n("KameraKlient"), Try | User1 | User2 | User3 | Ok | Help | Close, Close, 0, 0, false, true) {
	resize(700, 440);
	setButtonBoxOrientation(Vertical);
	setButtonText(Try, i18n("Stop"));
	mCameraCancelButton = actionButton(Try);
	setButtonText(User1, i18n("Download"));
	setButtonText(User2, i18n("Upload"));
	setButtonText(User3, i18n("Delete"));
	setButtonText(Ok, i18n("Setup"));
	mCameraSetupButton = actionButton(Ok);
    mCameraList = new CameraList(this, locateLocal("data", "kipi/cameras.xml"));
	mCameraType =  new CameraType(0, 0, 0, 0);
	QFrame* mFrame =  plainPage(); 
	QVBoxLayout* mMainBox = new QVBoxLayout(mFrame);
	QHBox* mCameraBox = new QHBox(mFrame);
	mMainBox->addWidget(mCameraBox);
	mCameraConnectButton = new QPushButton(i18n("Connect"), mCameraBox);
	mCameraComboBox = new QComboBox(mCameraBox, "camera");
	mCameraComboBox->setInsertionPolicy(QComboBox::AtBottom);
    mCameraComboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	QHBox *bottomBox = new QHBox(mFrame);
	mMainBox->addWidget(bottomBox);
    bottomBox->setFrameShape(QFrame::NoFrame);
    bottomBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
    (void) new QLabel(i18n("Download to: "), bottomBox);
    mDownloadDirectoryEdit = new QLineEdit(bottomBox);
    mDownloadDirectoryEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    mDownloadDirectoryEdit->setReadOnly("true");
    mChangeDownloadDirectoryButton = new QPushButton(i18n("&Change"), bottomBox);
    QSplitter *splitter = new QSplitter(mFrame);
	mMainBox->addWidget(splitter);
    splitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mFolderView = new CameraFolderView(splitter);
    mIconView   = new CameraIconView(splitter);
    splitter->setOpaqueResize(true);
    splitter->setResizeMode(mFolderView,  QSplitter::Stretch);
    splitter->setResizeMode(mIconView,  QSplitter::Stretch);
    QValueList<int> sizeList;
    sizeList.push_back(2);
    sizeList.push_back(5);
    splitter->setSizes (sizeList);
	container_  = new GPFileItemContainer(this, mFolderView, mIconView);
    efilter_    = new GPEventFilter(this);
    controller_ = new GPController(this, *mCameraType);
    controller_->start();
    cameraConnected_ = false;
	mIconView->setThumbnailSize();
	mStatusBar =  new KStatusBar(mFrame); 
	mMainBox->addWidget(mStatusBar);
    mStatusLabel = new QLabel(mStatusBar);
    mStatusLabel->setText(i18n("Ready"));
    mStatusBar->addWidget(mStatusLabel, 7, true);
    mProgressBar = new KProgress(mStatusBar);
    mProgressBar->setTotalSteps(100);
    mStatusBar->addWidget(mProgressBar, 5, true);
    setupActions();
    setupConnections();
    mCameraList->load();
	readSettings();
}

CameraUI::~CameraUI() {
	writeSettings();
    delete controller_;
    delete container_;
    mFolderView->clear();
    mIconView->clear();
}

const CameraType* CameraUI::cameraType() {
    return mCameraType;
}

void CameraUI::setupActions() {
	mSelectAllAction = new KAction(i18n("Select All"), 0, CTRL+Key_A,
											this, SLOT(slotSelectAll()));
    mSelectNoneAction = new KAction(i18n("Select None"), 0, CTRL+Key_U,
											this, SLOT(slotSelectNone()));
    mSelectInvertAction = new KAction(i18n("Invert Selection"), 0, CTRL+Key_Asterisk,
											this, SLOT(slotSelectInvert()));
    mSelectNewAction = new KAction(i18n("Select New Items"), 0, CTRL+Key_Slash,
											this, SLOT(slotSelectNew()));
    setCameraConnected(false);
}

void CameraUI::setupConnections() {
    connect(this, SIGNAL(signalStatusMsg(const QString&)), 
			this, SLOT(slotSetStatusMsg(const QString&)));
    connect(this, SIGNAL(signalProgressVal(int)), 
			this, SLOT(slotSetProgressVal(int)));
    connect(this, SIGNAL(signalBusy(bool)), 
			this, SLOT(slotBusy(bool)));
    connect(efilter_, SIGNAL(signalStatusMsg(const QString&)),
			this, SIGNAL(signalStatusMsg(const QString&)));
    connect(efilter_, SIGNAL(signalProgressVal(int)),
            this, SIGNAL(signalProgressVal(int)));
    connect(efilter_, SIGNAL(signalBusy(bool)),
            this, SIGNAL(signalBusy(bool))); 
    connect(mFolderView, SIGNAL(signalFolderChanged(CameraFolderItem*)),
            this, SLOT(slotFolderSelected(CameraFolderItem*)));
    connect(mIconView, SIGNAL(signalDownloadSelectedItems()),
            this, SLOT(slotCameraDownloadSelected()));
    connect(mIconView, SIGNAL(signalDeleteSelectedItems()),
            this, SLOT(slotCameraDeleteSelected()));
    connect(mChangeDownloadDirectoryButton, SIGNAL(pressed()),
			this, SLOT(slotChangeDownloadDirectory()));
    connect(mCameraList, SIGNAL(signalCameraListChanged()), 
			this, SLOT(slotSyncCameraComboBox()));
	connect(mCameraConnectButton, SIGNAL(pressed()), 
			this, SLOT(slotCameraConnectToggle()));
	connect(mCameraSetupButton, SIGNAL(pressed()), 
			this, SLOT(slotSetupCamera()));
	connect(mCameraCancelButton, SIGNAL(pressed()),
			this, SLOT(slotCameraCancel()));
    connect(this, SIGNAL(user1Clicked()),
			this, SLOT(slotCameraDownloadSelected()));
    connect(this, SIGNAL(user2Clicked()),
			this, SLOT(slotCameraUpload()));
	connect(this, SIGNAL(user3Clicked()),
			this, SLOT(slotCameraDeleteSelected()));
}

void CameraUI::setCameraConnected(bool val) {
    enableButton(User1, val);
    enableButton(User2, val);
    enableButton(User3, val);
    if(val) {
		mStatusLabel->setText(i18n("Connected"));
		mCameraConnectButton->setText(i18n("Disconnect"));
    } else {
		mStatusLabel->setText(i18n("Disconnected"));
		mCameraConnectButton->setText(i18n("Connect"));
    }
}

void CameraUI::slotClose() {
	this->~CameraUI();
}

void CameraUI::slotSetStatusMsg(const QString& msg) {
    mStatusLabel->setText(msg);
}

void CameraUI::slotSetProgressVal(int val) {
    if(val >= 0 && val <= 100) {
		mProgressBar->setProgress(val);
    }
}

void CameraUI::slotResetStatusBar() {
    mProgressBar->setTotalSteps(100);
    mProgressBar->setProgress(0);
}

void CameraUI::slotBusy(bool val) {
    if(!val) {
		slotResetStatusBar();
    }
    mCameraCancelButton->setEnabled(val);
}

void CameraUI::slotSetupCamera() {
    SetupCamera *mSetupDialog = new SetupCamera(this, "camerasetup");
    mSetupDialog->exec();
}

void CameraUI::slotSyncCameraComboBox() {
	mCameraComboBox->clear();
	QPtrList<CameraType>* mCameraTypeList = mCameraList->cameraList();
	for(mCameraTypeList->first(); mCameraTypeList->current(); mCameraTypeList->next()) {
		mCameraComboBox->insertItem(mCameraTypeList->current()->title());
	}
}

void CameraUI::setCameraType(const CameraType& ctype) {
    *mCameraType = ctype;
    delete controller_;
    controller_ = new GPController(this, *mCameraType);
    controller_->start();
}

void CameraUI::cameraInitialized(bool val) {
    if(val) {
        cameraConnected_ = true;
        setCameraConnected(true);
        container_->addVirtualFolder(mCameraType->title());
        container_->addRootFolder(mCameraType->path());
        controller_->requestGetSubFolders(mCameraType->path());
        controller_->requestGetAllItemsInfo(mCameraType->path());
        mFolderView->virtualFolder()->setSelected(true);
    }
}

void CameraUI::cameraSubFolder(const QString& folder, const QString& subFolder) {
    container_->addFolder(folder, subFolder);
}

void CameraUI::cameraNewItems(const QString& folder, const GPFileItemInfoList& infoList) {
    QListViewItem *item = mFolderView->currentItem();
    if (!item) {
		return;
    }
    CameraFolderItem *folderItem = static_cast<CameraFolderItem *>(item);
    if (folderItem->folderPath() != folder && !folderItem->isVirtualFolder()) {
        return;
    }
    container_->addFiles(folder, infoList);
    GPFileItemInfoList::const_iterator it;
    for (it = infoList.begin(); it != infoList.end(); it++) {
        if ((*it).mime.contains("image")) {
            controller_->requestGetThumbnail(folder, (*it).name);
		}
    }
}

void CameraUI::cameraNewItems(const GPFileItemInfoList& infoList) {
    QListViewItem *item = mFolderView->currentItem();
    if (!item) {
		return;
    }
    CameraFolderItem *folderItem = static_cast<CameraFolderItem *>(item);
    if (!folderItem->isVirtualFolder()){
        return;
    }
    container_->addFiles(infoList);
    GPFileItemInfoList::const_iterator it;
    for (it = infoList.begin(); it != infoList.end(); it++) {
        if ((*it).mime.contains("image")) {
            controller_->requestGetThumbnail((*it).folder, (*it).name);
		}
    }
}

void CameraUI::cameraNewThumbnail(const QString& folder, const QString& itemName, const QImage& thumbnail) {
    CameraIconItem *iconItem = container_->findItem(folder, itemName);
    if (!iconItem) {
		return;
    }
    mIconView->setThumbnail(iconItem, thumbnail);    
}

void CameraUI::cameraDownloadedItem(const QString& folder, const QString& itemName) {
    CameraIconItem *iconItem = container_->findItem(folder, itemName);
    if(!iconItem) {
		return;
	}
    mIconView->markDownloaded(iconItem);
}

void CameraUI::cameraDeletedItem(const QString& folder, const QString& itemName) {
    container_->delFile(folder, itemName);
}

void CameraUI::cameraErrorMsg(const QString& msg) {
    DMessageBox::showMsg(msg);
}

void CameraUI::slotCameraConnectToggle() {
	mCameraType = mCameraList->find(mCameraComboBox->currentText());
    setCameraType(*mCameraType);
    setCameraConnected(false);
    if(!cameraConnected_) {
        controller_->requestInitialize();
    } else {
        delete controller_;
        controller_ = new GPController(this, *mCameraType);
        controller_->start();
        cameraConnected_ = false;
        mIconView->clear();
        mFolderView->clear();
    }
}

void CameraUI::slotCameraDownloadSelected() {
    if(!cameraConnected_) {
		return;
    }
    QString dir = mDownloadDirectoryEdit->text();
    QDir qdir(dir);
    if(!qdir.exists()) {
        KMessageBox::error(0, i18n("'%1' Directory does not exist").arg(dir));
        return;
    }
    int count = 0;
    for(ThumbItem *i = mIconView->firstItem(); i; i=i->nextItem() ) {
        if (i->isSelected()) {
			++count;
		}
    }
    if(count == 0) {
		return;
	}
    bool proceed = true;
    bool overwriteAll = false;
    for(ThumbItem *i = mIconView->firstItem(); i; i=i->nextItem()) {
        if(i->isSelected()) {
            CameraIconItem *item = static_cast<CameraIconItem*>(i);
            if(!item) {
				continue;
			}
            downloadOneItem(item->fileInfo()->name, item->fileInfo()->folder, dir, proceed, overwriteAll);
            if(!proceed) {
				return;
			}
        }
    }
}

void CameraUI::slotCameraDeleteSelected() {
    if(!cameraConnected_) {
       return;
    }
	QStringList deleteList;
	for (ThumbItem *i = mIconView->firstItem(); i;
		i=i->nextItem()) {
			if(i->isSelected()) {
				CameraIconItem *item = static_cast<CameraIconItem*>(i);
				deleteList.append(item->fileInfo()->name);
        }
    }
	if (deleteList.isEmpty()) {
		return;
	}
	QString warnMsg(i18n("About to delete these Image(s)\n" "Are you sure?"));
	if(KMessageBox::warningContinueCancelList(this, warnMsg, deleteList, i18n("Warning"), i18n("Delete")) ==  KMessageBox::Continue) {
		CameraIconItem *item = static_cast<CameraIconItem*>(mIconView->firstItem());
        while(item) {
            CameraIconItem *nextItem = static_cast<CameraIconItem *>(item->nextItem());
            if (item->isSelected()) {
                controller_->requestDeleteItem(item->fileInfo()->folder, item->fileInfo()->name);
			}
            item = nextItem;
        }
   }
}

void CameraUI::slotCameraUpload() {
    QString reason;
    if (! cameraReadyForUpload(reason) ) {
        KMessageBox::error(0, reason);
        return;
    }
    CameraFolderItem *folderItem = static_cast<CameraFolderItem *>(mFolderView->selectedItem());
    QStringList list = KFileDialog::getOpenFileNames(QString::null);
    bool ok;
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        QFileInfo info(*it);
        if(!info.exists()) {
			continue;
		}
        if(info.isDir()) {
			continue;
		}
        QString uploadName = info.fileName();
        while (container_->findItem(folderItem->folderPath(), uploadName)) {
            QString msg(i18n("Camera Folder '%1' contains item '%2'\n Please, enter New Name").arg(folderItem->folderName()).arg(uploadName));
            uploadName = KLineEditDlg::getText(msg,uploadName,&ok,this);
            if(!ok){
				return;
			}
        }
        controller_->requestUploadItem(folderItem->folderPath(), info.absFilePath(), uploadName);
    }    
}

void CameraUI::slotCameraCancel() {
    controller_->cancel();    
}

void CameraUI::slotSelectAll() {
    mIconView->selectAll();    
}

void CameraUI::slotSelectNone() {
    mIconView->clearSelection();
}

void CameraUI::slotSelectInvert() {
    mIconView->invertSelection();
}

void CameraUI::slotSelectNew() {
    mIconView->clearSelection();
    for (ThumbItem *it = mIconView->firstItem(); it; it = it->nextItem()) {
        CameraIconItem *item = static_cast<CameraIconItem *>(it);
        if (item->fileInfo()->downloaded == 0) {
            item->setSelected(true, false);
		}
    }
}

void CameraUI::slotFolderSelected(CameraFolderItem *folderItem) {
    if (!folderItem) {
		return;
    }
    controller_->cancel();
    mIconView->clear();
    if (folderItem->isVirtualFolder()) {
        controller_->requestGetAllItemsInfo(mCameraType->path());
    } else {
        controller_->requestGetItemsInfo(folderItem->folderPath());
    }
}

void CameraUI::downloadOneItem(const QString& item, const QString& folder, const QString& downloadDir, bool& proceedFurther, bool& overwriteAll) {
    proceedFurther = true;
    QString saveFile(downloadDir);
    if (!downloadDir.endsWith("/")) {
        saveFile += "/";
    }
    saveFile += item;
    while (QFile::exists(saveFile) && !(overwriteAll)) {
        bool overwrite=false;
        SavefileDialog *dlg = new SavefileDialog(saveFile);
        if (dlg->exec()== QDialog::Rejected) {
            delete dlg;
            proceedFurther = false;
            return;
        }
        switch(dlg->saveFileOperation()) {
			case (SavefileDialog::Skip): {
				delete dlg;
				return;
			}
			case (SavefileDialog::SkipAll): {
				delete dlg;
				proceedFurther = false;
				return;
			}
			case (SavefileDialog::Overwrite): {
				overwrite = true;
				delete dlg;
				break;
			}
			case (SavefileDialog::OverwriteAll): {
				overwriteAll = true;
				delete dlg;
				break;
			}
			case (SavefileDialog::Rename): {
				saveFile = downloadDir+"/"+dlg->renameFile();
				delete dlg;
				break;
			}
			default:  {
				delete dlg;
				proceedFurther = false;
				return;
			}
        }
        if (overwrite) {
			break;
		}
    }
    controller_->requestDownloadItem(folder, item, saveFile);
}

bool CameraUI::cameraReadyForUpload(QString& reason) {
    bool result = false;
    if (!cameraConnected_) {
		reason = i18n("Camera Not Initialised");
		return result;
    } /*
    if (!controller_->cameraSupportsUpload()) {
	reason = i18n("Camera does not support Uploads");
	return result;
    } */
    if (!mFolderView->selectedItem() || mFolderView->selectedItem() == mFolderView->firstChild()) {
		reason = i18n("Please Select a Folder on Camera to Upload");
		return result;
    }
    result = true;
    return result;
}

void CameraUI::slotChangeDownloadDirectory() {
    QString result = KFileDialog::getExistingDirectory(mDownloadDirectoryEdit->text(), this);
    if(!((new QFileInfo(result))->isWritable())) {
		KMessageBox::sorry(0, i18n("Sorry! The directory is not writable!"));
		return;
    }
    if(!result.isEmpty()) {
		mDownloadDirectoryEdit->setText(result);
    }
}

void CameraUI::writeSettings() {
    mConfig = new KConfig("kipirc");
    mConfig->setGroup("KameraKlient Settings");
    mConfig->writePathEntry("DownloadDirectory", mDownloadDirectoryEdit->text());
	mConfig->sync();
	delete mConfig;
}

void CameraUI::readSettings() {
    mConfig = new KConfig("kipirc");
    mConfig->setGroup("KameraKlient Settings");
    mDownloadDirectoryEdit->setText(mConfig->readPathEntry("DownloadDirectory", "$HOME"));
	delete mConfig;
}

CameraUI* CameraUI::getInstance() {
	static CameraUI inst;
	return &inst;
}

}  // NameSpace KIPIKameraKlientPlugin

#include "cameraui.moc"
