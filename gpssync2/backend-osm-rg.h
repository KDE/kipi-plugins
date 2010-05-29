
#ifndef BACKEND_OSM_RG_H
#define BACKEND_OSM_RG_H

//kde includes
#include <kmainwindow.h>
#include <kio/scheduler.h>
#include <kurl.h>
#include <kio/jobclasses.h>
#include <kio/job.h>
#include <klocale.h>

//Qt includes
#include <QWidget>
#include <QList>
#include <QMap>

//local includes
#include "gpsreversegeocodingwidget.h"
#include "backend-rg.h"

namespace KIO { class Job; }
class KJob;

namespace KIPIGPSSyncPlugin
{

class BackendOsmRGPrivate;

class BackendOsmRG : public RGBackend
{

    Q_OBJECT

public:

    BackendOsmRG(QObject* const parent);
    virtual ~BackendOsmRG();
    QMap<QString,QString> makeQMapFromXML(QString);

    virtual void callRGBackend(QList <RGInfo>, QString);

private Q_SLOTS:

    void nextPhoto(); 
    void dataIsHere(KIO::Job*, const QByteArray &); 
    void slotResult(KJob*);

private:
    BackendOsmRGPrivate *d;

};


} //KIPIGISSyncPlugin

#endif
