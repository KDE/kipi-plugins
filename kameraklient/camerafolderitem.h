#ifndef CAMERAFOLDERITEM_H
#define CAMERAFOLDERITEM_H

#include <klistview.h>
#include <qstring.h>

class CameraFolderItem : public KListViewItem {
public:
    CameraFolderItem(KListView* parent, const QString& name);
    CameraFolderItem(KListViewItem* parent, const QString& folderName, const QString& folderPath);
    ~CameraFolderItem();
    QString folderName();
    QString folderPath();
    bool    isVirtualFolder();
    void    changeCount(int val);
    void    setCount(int val);
    int     count();
    
private:
    QString folderName_;
    QString folderPath_;
    QString name_;
    bool    virtualFolder_;
    int     count_;
};

#endif 

