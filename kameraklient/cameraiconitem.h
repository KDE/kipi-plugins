#ifndef CAMERAICONITEM_H
#define CAMERAICONITEM_H

#include "thumbitem.h"

class QString;
class QPixmap;
class QImage;
class QPainter;
class QColorGroup;

class GPFileItemInfo;
class ThumbView;

class CameraIconItem : public ThumbItem {
    friend class CameraIconView;
    
public:
    CameraIconItem(ThumbView* parent, const GPFileItemInfo* fileInfo, const QPixmap& pixmap);
    ~CameraIconItem();
    const GPFileItemInfo* fileInfo();
    void setPixmap(const QImage& thumb);
 
private:
    void loadNewEmblem();
    const GPFileItemInfo* fileInfo_;
    int pixWidth_, pixHeight_;
    static QPixmap* newEmblem;
    static const char* new_xpm[];
};

#endif 

