#ifndef CAMERAICONVIEW_H
#define CAMERAICONVIEW_H

#include "thumbview.h"

class QString;
class QPainter;
class QImage;
class QPixmap;
class QPoint;

class GPFileItemInfo;
class ThumbItem;
class CameraIconItem;
class CameraIconViewPrivate;

class CameraIconView : public ThumbView {
    Q_OBJECT

public:
    CameraIconView(QWidget *parent);
    ~CameraIconView();
    void setThumbnailSize();
    CameraIconItem* addItem(const GPFileItemInfo* fileInfo);
    void setThumbnail(CameraIconItem* iconItem, const QImage& thumbnail);
    void markDownloaded(CameraIconItem* iconItem);
    virtual void clear();

protected:
    void startDrag();

private:
    void createPixmap(QPixmap& pix, const QString& icon, double scale);

private:
    CameraIconViewPrivate *d;
    
signals:
    void signalCleared();
    
signals:
    void signalDownloadSelectedItems();
    void signalDeleteSelectedItems();
};

#endif 
