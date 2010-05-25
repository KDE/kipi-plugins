//#include "backend-geonames-rg.moc"

#include "backend-geonames-rg.moc"

#include <kio/scheduler.h>
#include <kurl.h>
#include <kio/jobclasses.h>
#include <kdebug.h>
#include <QMessageBox>

//local includes
#include "backend-geonames-rg.h"
#include "gpsreversegeocodingwidget.h"
#include "backend-rg.h"

namespace KIPIGPSSyncPlugin
{

class TransferJob;
class BackendGeonamesRGPrivate
{

public:
    
    BackendGeonamesRGPrivate(){  }
    

};

BackendGeonamesRG::BackendGeonamesRG(QObject* const parent)
: RGBackend(parent), d(new BackendGeonamesRGPrivate())
{
    
}

BackendGeonamesRG::~BackendGeonamesRG()
{
    delete d;
}

void BackendGeonamesRG::nextPhoto()
{


}

void BackendGeonamesRG::runRGScript(QList<RGInfo> rgList, QString language)
{

//OSM     
//    KIO::TransferJob *job = KIO::get(KUrl("http://nominatim.openstreetmap.org/reverse?format=xml&lat=12&lon=9&zoom=18&addressdetails=1"));

    //geonames.org    
    KIO::TransferJob *job = KIO::get(KUrl("http://ws.geonames.org/findNearbyPlaceName?lat=42&lng=9"));

    connect (job, SIGNAL(  data(KIO::Job *, const QByteArray & )), this, SLOT(dataIsHere(KIO::Job *,const QByteArray &)));
    
}

void BackendGeonamesRG::dataIsHere(KIO::Job*, const QByteArray & data)
{

    QMessageBox msg;
    msg.setText(data);
    msg.exec();

}

} //KIPIGPSSyncPlugin
