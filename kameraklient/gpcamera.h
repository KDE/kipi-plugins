#ifndef GPCAMERA_H
#define GPCAMERA_H

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>

#include "gpfileiteminfo.h"

class QImage;

class GPCameraPrivate;
class GPStatus;

class GPCamera {

public:
    enum {
        GPError=0,
        GPInit,
        GPSetup,
        GPSuccess
    } Errors;
    

    GPCamera(const QString& model, const QString& port, const QString& path);
    ~GPCamera();

    bool thumbnailSupport();
    bool deleteSupport();
    bool uploadSupport();
    bool mkDirSupport();
    bool delDirSupport();

    
    int  initialize();

    void cancel();

    int getSubFolders(const QString& folder, QValueList<QString>& subFolderList);

    void getAllItemsInfo(const QString& folder, GPFileItemInfoList& infoList);
    int getItemsInfo(const QString& folder, GPFileItemInfoList& infoList);
    int getThumbnail(const QString& folder, const QString& imageName, QImage& thumbnail);
    int downloadItem(const QString& folder, const QString& itemName, const QString& saveFile);
    int deleteItem(const QString& folder, const QString& itemName);

    // recursively delete all items
    int deleteAllItems(const QString& folder);

    int uploadItem(const QString& folder, const QString& itemName, const QString& localFile);

    void cameraSummary(QString& summary);
    void cameraManual(QString& manual);
    void cameraAbout(QString& about);

    
    // Static Functions
    static void getSupportedCameras(int& count, QStringList& clist);
    static void getSupportedPorts(QStringList& plist);
    static void getCameraSupportedPorts(const QString& model, QStringList& plist);
    static int  autoDetect(QString& model, QString& port);
    
private:
    int  setup();
    GPCameraPrivate *d;
    GPStatus        *status;
};

#endif

