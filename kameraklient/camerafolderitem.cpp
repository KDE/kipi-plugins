// KDE
#include <kiconloader.h>
// Local
#include "camerafolderitem.h"

CameraFolderItem::CameraFolderItem(KListView* parent, const QString& name) : KListViewItem(parent, name) {
    setPixmap(0, SmallIcon("folder"));
    virtualFolder_ = true;
    count_  = 0;
    name_   = name;
}

CameraFolderItem::CameraFolderItem(KListViewItem* parent, const QString& folderName, const QString& folderPath) : KListViewItem(parent, folderName) {
    setPixmap(0, SmallIcon("folder"));
    folderName_ = folderName;
    folderPath_ = folderPath;
    virtualFolder_ = false;
    count_ = 0;
    name_  = folderName;
}

CameraFolderItem::~CameraFolderItem() {
}

bool CameraFolderItem::isVirtualFolder() {
    return virtualFolder_;    
}

QString CameraFolderItem::folderName() {
    return folderName_;
}

QString CameraFolderItem::folderPath() {
    return folderPath_;
}

void CameraFolderItem::changeCount(int val) {
    count_ += val;
    setText(0, name_ + " (" + QString::number(count_) + ")");    
}

void CameraFolderItem::setCount(int val) {
    count_ = val;    
    setText(0, name_ + " (" + QString::number(count_) + ")");
}

int CameraFolderItem::count() {
    return count_;    
}

