#ifndef GPEVENTFILTER_H
#define GPEVENTFILTER_H

#include <qobject.h>

#include "gpfileiteminfo.h"

class QEvent;
class QImage;
class QString;

class CameraUI;

class GPEventFilter : public QObject {
    Q_OBJECT
    
public:
    GPEventFilter(QObject *parent);
    ~GPEventFilter();

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private:
    CameraUI *view_;
    
signals:
    void signalCameraError(const QString&);
    void signalStatusMsg(const QString&);
    void signalProgressVal(int);
    void signalBusy(bool);
    
};

#endif

