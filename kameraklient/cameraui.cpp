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

// Qt includes.

#include <qcombobox.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qstringlist.h>
#include <qtooltip.h>
#include <qvaluelist.h>
#include <qvbox.h>
#include <qpushbutton.h>

// KDE includes

#include <kaccel.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdirlister.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <klocale.h>
#include <kedittoolbar.h>
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
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// Include files for KIPI

#include <libkipi/version.h>

// Local includes.

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

CameraUI::CameraUI() : QWidget() 
{
    setWFlags(Qt::WDestructiveClose);
    resize(700, 440);
    setMinimumSize(600, 400);
    mCameraList = new CameraList(this, locateLocal("data", "kipi/cameras.xml"));
    mCameraType =  new CameraType();
    QHBoxLayout* mMainBoxLayout = new QHBoxLayout(this, 0, 4);
    mMainBoxLayout->setResizeMode(QLayout::FreeResize);
    QVBoxLayout* mLeftBoxLayout = new QVBoxLayout(mMainBoxLayout, 0);
    QVBoxLayout* mBtnBoxLayout = new QVBoxLayout(mMainBoxLayout, 4);
    mBtnBoxLayout->setMargin(2);
    
    // create Button Box ----------------------------------------------------------------------
    
    mCameraSetupBtn = new QPushButton(i18n("Setup"), this);
    mCameraSetupBtn->setMinimumSize(QSize(100, 0));
    mBtnBoxLayout->addWidget(mCameraSetupBtn);
    mCameraStopBtn = new QPushButton(i18n("Stop"), this);
    mCameraStopBtn->setMinimumSize(QSize(100, 0));
    mBtnBoxLayout->addWidget(mCameraStopBtn);
    mCameraDownloadBtn = new QPushButton(i18n("Download"), this);
    mCameraDownloadBtn->setMinimumSize(QSize(100, 0));
    mBtnBoxLayout->addWidget(mCameraDownloadBtn);
    mCameraUploadBtn = new QPushButton(i18n("Upload"), this);
    mCameraUploadBtn->setMinimumSize(QSize(100, 0));
    mBtnBoxLayout->addWidget(mCameraUploadBtn);
    mCameraDeleteBtn = new QPushButton(i18n("Delete"), this);
    mCameraDeleteBtn->setMinimumSize(QSize(100, 0));
    mBtnBoxLayout->addWidget(mCameraDeleteBtn);
    QSpacerItem* mBtnSpacer = new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mBtnBoxLayout->addItem(mBtnSpacer);
    mDialogCloseBtn = new QPushButton(i18n("Close"), this);
    mDialogCloseBtn->setMinimumSize(QSize(100, 0));
    mBtnBoxLayout->addWidget(mDialogCloseBtn);

    // About data and help button ---------------------------------------------------
    
    mhelpButton = new QPushButton(i18n("&Help"), this);
    mhelpButton->setMinimumSize(QSize(100, 0));
    mBtnBoxLayout->addWidget(mhelpButton); 
    
    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("KameraKlient"), 
                                       kipi_version,
                                       I18N_NOOP("An Digital camera interface Kipi plugin"),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2004, Renchi Raju\n"
                                       "(c) 2004, Tudor Calin", 
                                       0,
                                       "http://extragear.kde.org/apps/kipi.php");
    
    about->addAuthor("Renchi Raju", I18N_NOOP("Original author from Digikam project"),
                     "renchi@pooh.tam.uiuc.edu");

    about->addAuthor("Tudor Calin", I18N_NOOP("Porting the Digikam GPhoto2 interface to Kipi. Maintainer"),
                     "tudor@1xtech.com");

    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("KameraKlient handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    mhelpButton->setPopup( helpMenu->menu() );
    
    // create Camera Box-----------------------------------------------------------------------
    
    QHBoxLayout* mCameraBoxLayout = new QHBoxLayout(mLeftBoxLayout, 4);
    mCameraBoxLayout->setMargin(4);
    mCameraConnectBtn = new QPushButton(i18n("Connect"), this);
    mCameraBoxLayout->addWidget(mCameraConnectBtn);
    mCameraComboBox = new QComboBox(this, "camera");
    mCameraComboBox->setInsertionPolicy(QComboBox::AtBottom);
    mCameraComboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    mCameraBoxLayout->addWidget(mCameraComboBox); 
    
    // create Download Directory Camera Box ---------------------------------------------------
    
    QHBoxLayout *mDownloadDirectoryBoxLayout = new QHBoxLayout(mLeftBoxLayout, 4);
    mDownloadDirectoryBoxLayout->setMargin(4);
    QLabel* mDownloadDirectoryLabel = new QLabel(i18n("Download to: "), this);
    mDownloadDirectoryBoxLayout->addWidget(mDownloadDirectoryLabel);
    mDownloadDirectoryEdit = new QLineEdit(this);
    mDownloadDirectoryEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    mDownloadDirectoryEdit->setReadOnly("true");
    mDownloadDirectoryBoxLayout->addWidget(mDownloadDirectoryEdit);
    mChangeDownloadDirectoryBtn = new QPushButton(i18n("&Change"), this);
    mDownloadDirectoryBoxLayout->addWidget(mChangeDownloadDirectoryBtn); // -------------------------
    mSplitter = new QSplitter(this);
    mLeftBoxLayout->addWidget(mSplitter);
    mSplitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mFolderView   = new CameraFolderView(mSplitter);
    mIconView   = new CameraIconView(mSplitter);
    mSplitter->setOpaqueResize(true);
    mSplitter->setResizeMode(mFolderView, QSplitter::Stretch);
    mSplitter->setResizeMode(mIconView, QSplitter::Stretch);
    container_  = new GPFileItemContainer(this, mFolderView, mIconView);
    efilter_    = new GPEventFilter(this);
    controller_ = new GPController(this, *mCameraType);
    controller_->start();
    cameraConnected_ = false;
    
    // create Status Bar -----------------------------------------------------------------------------
    
    mStatusBar =  new KStatusBar(this);
    mLeftBoxLayout->addWidget(mStatusBar);
    mStatusLabel = new QLabel(mStatusBar);
    mStatusLabel->setText(i18n("Ready"));
    mStatusBar->addWidget(mStatusLabel, 7, true);
    mProgressBar = new KProgress(mStatusBar);
    mProgressBar->setTotalSteps(100);
    mStatusBar->addWidget(mProgressBar, 5, true); 
    
    // ------------------------------------------------- 
    
    setupAccel();
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


void CameraUI::slotHelp()
{
    KApplication::kApplication()->invokeHelp("kameraklient",
                                             "kipi-plugins");
} 

const CameraType* CameraUI::cameraType() {
    return mCameraType;
}

void CameraUI::setupAccel() {
    mCameraUIAccel = new KAccel(this);
    mCameraUIAccel->insert("Select All", i18n("Select All"), 
	    		i18n("Select all the images from the camera."), 
			CTRL+Key_A, this, SLOT(slotSelectAll()));
    mCameraUIAccel->insert("Select None", i18n("Select None"), 
	    		i18n("Deselect all the images from the camera."), 
			CTRL+Key_U, this, SLOT(slotSelectNone()));
    mCameraUIAccel->insert("Invert selection", i18n("Invert selection"), 
	    		i18n("Invert the selection."), 
			CTRL+Key_Asterisk, this, SLOT(slotSelectInvert()));
    mCameraUIAccel->insert("Select New", i18n("Select New Items"), 
	    		i18n("Select all the that were not previously downloaded."), 
			CTRL+Key_Slash, this, SLOT(slotSelectNew()));
    setCameraConnected(false);
}

void CameraUI::setupConnections() {
    connect(this, SIGNAL(signalStatusMsg(const QString&)), this, SLOT(slotSetStatusMsg(const QString&)));
    connect(this, SIGNAL(signalProgressVal(int)), this, SLOT(slotSetProgressVal(int)));
    connect(this, SIGNAL(signalBusy(bool)), this, SLOT(slotBusy(bool)));
    connect(efilter_, SIGNAL(signalStatusMsg(const QString&)), this, SIGNAL(signalStatusMsg(const QString&)));
    connect(efilter_, SIGNAL(signalProgressVal(int)), this, SIGNAL(signalProgressVal(int)));
    connect(efilter_, SIGNAL(signalBusy(bool)), this, SIGNAL(signalBusy(bool))); 
    connect(mFolderView, SIGNAL(signalFolderChanged(CameraFolderItem*)), this, SLOT(slotFolderSelected(CameraFolderItem*)));
    connect(mIconView, SIGNAL(signalDownloadSelectedItems()), this, SLOT(slotCameraDownloadSelected()));
    connect(mIconView, SIGNAL(signalDeleteSelectedItems()), this, SLOT(slotCameraDeleteSelected()));
    connect(mChangeDownloadDirectoryBtn, SIGNAL(pressed()), this, SLOT(slotChangeDownloadDirectory()));
    connect(mCameraList, SIGNAL(signalCameraListChanged()), this, SLOT(slotSyncCameraComboBox()));
    connect(mCameraConnectBtn, SIGNAL(pressed()), this, SLOT(slotCameraConnectToggle()));
    connect(mCameraSetupBtn, SIGNAL(pressed()), this, SLOT(slotSetupCamera()));
    connect(mCameraStopBtn, SIGNAL(pressed()), this, SLOT(slotCameraCancel()));
    connect(mCameraDownloadBtn, SIGNAL(pressed()), this, SLOT(slotCameraDownloadSelected()));
    connect(mCameraUploadBtn, SIGNAL(pressed()), this, SLOT(slotCameraUpload()));
    connect(mCameraDeleteBtn, SIGNAL(pressed()), this, SLOT(slotCameraDeleteSelected()));
    connect(mDialogCloseBtn, SIGNAL(pressed()), this, SLOT(close()));
}

void CameraUI::setCameraConnected(bool val) {
    mCameraDownloadBtn->setEnabled(val);
    mCameraUploadBtn->setEnabled(val);
    mCameraDeleteBtn->setEnabled(val);
    if(val) {
	mStatusLabel->setText(i18n("Connected"));
	mCameraConnectBtn->setText(i18n("Disconnect"));
    } else {
	mStatusLabel->setText(i18n("Disconnected"));
	mCameraConnectBtn->setText(i18n("Connect"));
    }
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
    mCameraStopBtn->setEnabled(val);
}

void CameraUI::slotSetupCamera() {
    SetupCamera *mSetupDialog = new SetupCamera(this, "camerasetup");
    mSetupDialog->exec();
}

void CameraUI::slotSyncCameraComboBox() {
    mCameraComboBox->clear();
    QPtrList<CameraType>* mCameraTypeList = mCameraList->cameraList();
    for(mCameraTypeList->first(); mCameraTypeList->current(); mCameraTypeList->next()) {
	mCameraComboBox->insertItem(mCameraTypeList->current()->model());
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
        container_->addVirtualFolder(mCameraType->model());
        container_->addRootFolder("/");
        controller_->requestGetSubFolders("/");
        controller_->requestGetAllItemsInfo("/");
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
    if (mCameraComboBox->count() == 0) {
	KMessageBox::error(0, i18n("There is no configured camera!"));
	return;
    }
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
        controller_->requestGetAllItemsInfo("/");
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
    mConfig->writeEntry("DialogSize", frameSize());
    mConfig->writeEntry("DialogXPos", x());
    mConfig->writeEntry("DialogYPos", y());
    mConfig->writeEntry("SplitterSizes", mSplitter->sizes());
    mConfig->sync();
    delete mConfig;
}

void CameraUI::readSettings() {
    mConfig = new KConfig("kipirc");
    mConfig->setGroup("KameraKlient Settings");
    mDownloadDirectoryEdit->setText(mConfig->readPathEntry("DownloadDirectory", "$HOME"));
    resize(mConfig->readSizeEntry("DialogSize"));
    move(mConfig->readNumEntry("DialogXPos"), mConfig->readNumEntry("DialogYPos"));
    mSplitter->setSizes(mConfig->readIntListEntry("SplitterSizes"));
    delete mConfig;
}

#include "cameraui.moc"
