#ifndef CAMERADRAGOBJECT_H
#define CAMERADRAGOBJECT_H

#include <qdragobject.h>

class QMimeSource;
class QWidget;
class CameraType;

class CameraDragObject : public QStoredDrag {
	
public:
    CameraDragObject(const CameraType* ctype, QWidget* dragSource=0);
    ~CameraDragObject();
    static bool canDecode(const QMimeSource* e);
    static bool decode(const QMimeSource* e, CameraType& ctype);

private:
    void setCameraType(const CameraType* ctype);
};

#endif

