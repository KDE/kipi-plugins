#ifndef GPCONTROLLER_H
#define GPCONTROLLER_H

#include <qobject.h>
#include <qthread.h>
#include <qmutex.h>

#include "mtqueue.h"
#include "gpcommand.h"
#include "cameratype.h"

class QString;
class QImage;
class GPCamera;

class GPController : public QObject, public QThread {
    Q_OBJECT

public:

    GPController(QObject *parent, const CameraType& ctype);
    ~GPController();

    void requestInitialize();
    void requestGetSubFolders(const QString& folder);
    void requestMakeFolder(const QString& folder, const QString& newFolder);
    void requestDeleteFolder(const QString& folder);
    void requestGetItemsInfo(const QString& folder);
    void requestGetAllItemsInfo(const QString& folder);
    void requestGetThumbnail(const QString& folder, const QString& imageName);
    void requestDownloadItem(const QString& folder, const QString& itemName, const QString& saveFile);
    void requestDeleteItem(const QString& folder, const QString& itemName);
    void requestUploadItem(const QString& folder, const QString& localFile, const QString& uploadName);
    void requestOpenItem(const QString& folder, const QString& itemName, const QString& saveFile);
    void requestOpenItemWithService(const QString& folder, const QString& itemName, const QString& saveFile, const QString& serviceName);
    void cancel();
    void getInformation(QString& summary, QString& manual, QString& about);

protected:

    void run();

private:

    void initialize();
    void getSubFolders(const QString& folder);
    void makeFolder(const QString& folder,
                    const QString& newFolder);
    void deleteFolder(const QString& folder);
    void getItemsInfo(const QString& folder);
    void getAllItemsInfo(const QString& folder);
    void getThumbnail(const QString& folder,
                      const QString& imageName);
    void downloadItem(const QString& folder,
                      const QString& itemName,
                      const QString& saveFile);
    void deleteItem(const QString& folder,
                    const QString& itemName);
    void uploadItem(const QString& folder,
                    const QString& uploadName,
                    const QString& localFile);
    void openItem(const QString& folder,
                  const QString& itemName,
                  const QString& saveFile);
    void openItemWithService(const QString& folder,
                             const QString& itemName,
                             const QString& saveFile,
                             const QString& serviceName);
    void exifInfo(const QString& folder,
                  const QString& itemName);
    void error(const QString& errorMsg);

    void scaleHighlightThumbnail(QImage& thumbnail);
    void showBusy(bool val);
    

    QObject  *parent_;
    GPCamera *camera_;
    QMutex    mutex_;
    MTQueue<GPCommand> cmdQueue_;
    bool      close_;

private slots:

    void slotStatusMsg(const QString& msg);
    void slotProgressVal(int val);
    void slotErrorMsg(const QString& msg);
};

#endif /* GPCONTROLLER_H */
