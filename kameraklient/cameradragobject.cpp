// Qt
#include <qmime.h>
#include <qstring.h>
#include <qcstring.h>
#include <qwidget.h>
#include <qdatastream.h>
// Local
#include "cameratype.h"
#include "cameradragobject.h"

CameraDragObject::CameraDragObject(const CameraType* ctype, QWidget *dragSource) : QStoredDrag("camera/unknown", dragSource) {
    setCameraType(ctype);
}

CameraDragObject::~CameraDragObject() {
}

void CameraDragObject::setCameraType(const CameraType* ctype) {
    QByteArray byteArray;
    QDataStream ds(byteArray, IO_WriteOnly);
    ds << ctype->title();
    ds << ctype->model();
    ds << ctype->port();
    ds << ctype->path();
    setEncodedData(byteArray);    
}


bool CameraDragObject::canDecode(const QMimeSource* e) {
    return e->provides("camera/unknown");
}

bool CameraDragObject::decode(const QMimeSource* e, CameraType& ctype) {
    QByteArray payload = e->encodedData("camera/unknown");
    if (payload.size()) {
        QString title, model, port, path;
        QDataStream ds(payload, IO_ReadOnly);
        ds >> title;
        ds >> model;
        ds >> port;
        ds >> path;
        ctype = CameraType(title, model, port, path);
        return true;
    } else {
        return false;
	}
}

