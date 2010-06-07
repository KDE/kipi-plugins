
#ifndef BACKEND_GEONAMESUS_RG_H
#define BACKEND_GEONAMESUS_RG_H

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
#include <QString>

//local includes
#include "gpsreversegeocodingwidget.h"
#include "backend-rg.h"

namespace KIO { class Job; }
class KJob;

namespace KIPIGPSSyncPlugin
{

class BackendGeonamesUSRGPrivate;

class BackendGeonamesUSRG : public RGBackend
{

    Q_OBJECT

public:

    BackendGeonamesUSRG(QObject* const parent);
    virtual ~BackendGeonamesUSRG();
    QMap<QString, QString> makeQMapFromXML(QString);

    virtual void callRGBackend(QList <RGInfo>, QString);
    virtual QString getErrorMessage();

private Q_SLOTS:

    void nextPhoto(); 
    void dataIsHere(KIO::Job*, const QByteArray &); 
    void slotResult(KJob*);
private:
    BackendGeonamesUSRGPrivate *d;

};


} //KIPIGISSyncPlugin

#endif
