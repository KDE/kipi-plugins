#ifndef CAMERAFOLDERVIEW_H
#define CAMERAFOLDERVIEW_H

#include <klistview.h>
#include <qstring.h>

class CameraFolderItem;

class CameraFolderView : public KListView {
    Q_OBJECT

public:
    CameraFolderView(QWidget* parent);
    ~CameraFolderView();

    void addVirtualFolder(const QString& name);
    void addRootFolder(const QString& folder);
    CameraFolderItem* addFolder(const QString& folder, const QString& subFolder);

    CameraFolderItem* findFolder(const QString& folderPath);

    CameraFolderItem* virtualFolder();
    CameraFolderItem* rootFolder();

    virtual void clear();
    
private:
    QString cameraName_;
    CameraFolderItem *virtualFolder_;
    CameraFolderItem *rootFolder_;

private:
    void setupConnections();
    
private slots:
    void slotSelectionChanged(QListViewItem* item);

signals:
    void signalFolderChanged(CameraFolderItem*);
    void signalCleared();
};

#endif

