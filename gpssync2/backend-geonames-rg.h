
#ifndef BACKEND_GEONAMES_RG_H
#define BACKEND_GEONAMES_RG_H

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

//local includes
#include "gpsreversegeocodingwidget.h"
#include "backend-rg.h"

namespace KIO { class Job; }
class KJob;

namespace KIPIGPSSyncPlugin
{

class BackendGeonamesRGPrivate;

class BackendGeonamesRG : public RGBackend
{

    Q_OBJECT

public:

    BackendGeonamesRG(QObject* const parent);
    virtual ~BackendGeonamesRG();

    virtual void runRGScript(QList <RGInfo>, QString);

private Q_SLOTS:

    void nextPhoto(); 
    void dataIsHere(KIO::Job*, const QByteArray &); 
    void slotResult(KJob*);
private:
    BackendGeonamesRGPrivate *d;

};


} //KIPIGISSyncPlugin

#endif
