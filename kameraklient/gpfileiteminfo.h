#ifndef GPFILEITEMINFO_H
#define GPFILEITEMINFO_H

#include <qstring.h>
#include <qvaluelist.h>

class GPFileItemInfo {
    
public:
    GPFileItemInfo();
    ~GPFileItemInfo();

    GPFileItemInfo(const GPFileItemInfo& info);
    GPFileItemInfo& operator=(const GPFileItemInfo& info);
    // ---------------------------------------------------------
    QString name;
    QString folder;
    // ---------------------------------------------------------
    bool    fileInfoAvailable;

    QString mime;
    QString time;
    int     size;
    int     width;
    int     height;
    int     readPermissions;
    int     writePermissions;
    int     downloaded;
    // ---------------------------------------------------------
    bool    previewInfoAvailable;

    QString previewMime;
    int     previewSize;
    int     previewWidth;
    int     previewHeight;
    int     previewDownloaded;
    // ---------------------------------------------------------
    bool    audioInfoAvailable;

    QString audioMime;
    int     audioSize;
    int     audioDownloaded;
    // ---------------------------------------------------------
    void   *viewItem;
};

// Container for GPFileItemInfo
typedef QValueList<GPFileItemInfo> GPFileItemInfoList;

#endif 

