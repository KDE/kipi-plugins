
#include "backend-geonames-rg.moc"


//KDE includes
#include <kio/scheduler.h>
#include <kurl.h>
#include <kio/jobclasses.h>
#include <kdebug.h>
#include <kio/job.h>
#include <klocale.h>
#include <kmessagebox.h>

//local includes
#include "backend-geonames-rg.h"
#include "gpsreversegeocodingwidget.h"
#include "backend-rg.h"

//Qt includes
#include <QDomDocument>
#include <QMap>
#include <QMessageBox>
#include <QString>

namespace KIPIGPSSyncPlugin
{


class GeonamesInternalJobs {

public:

    GeonamesInternalJobs()
    : request(),
      data(),
      kioJob(0)
    {
    }


    RGInfo request;
    QByteArray data;
    KIO::Job* kioJob;
};


class BackendGeonamesRGPrivate
{

public:

    BackendGeonamesRGPrivate()
    :jobs()
    {
    }

    QList<GeonamesInternalJobs> jobs;
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

void BackendGeonamesRG::callRGBackend(QList<RGInfo> rgList, QString language)
{

    //TODO: Remove dublicates from rgList to mergedQuery
    QList<RGInfo> mergedQuery = rgList;

    for( int i = 0; i < mergedQuery.count(); i++){

        GeonamesInternalJobs newJob;
        newJob.request = mergedQuery.at(i);
//        newJob.data.append("");    
  
        KUrl jobUrl("http://ws.geonames.org/findNearbyPlaceName");
        jobUrl.addQueryItem("lat", mergedQuery.at(i).coordinates.latString());
        jobUrl.addQueryItem("lng", mergedQuery.at(i).coordinates.lonString());
        jobUrl.addQueryItem("lang", language);

        newJob.kioJob = KIO::get(jobUrl, KIO::NoReload, KIO::HideProgressInfo);
        d->jobs<<newJob;
        d->jobs[i].kioJob = newJob.kioJob;

        connect(newJob.kioJob, SIGNAL(data(KIO::Job*, const QByteArray&)), 
                this, SLOT(dataIsHere(KIO::Job*,const QByteArray &)));
        connect(newJob.kioJob, SIGNAL(result(KJob*)),
                this, SLOT(slotResult(KJob*)));    

    }

}

void BackendGeonamesRG::dataIsHere(KIO::Job* job, const QByteArray & data)
{

    
    for(int i = 0; i < d->jobs.count(); ++i){

        if(d->jobs.at(i).kioJob == job){
            
            d->jobs[i].data.append(data);
            break;

        }
        

    }

}


QMap<QString,QString> BackendGeonamesRG::makeQMapFromXML(QString xmlData)
{

    QMap<QString, QString> mappedData;
    QString resultString;
    QDomDocument doc;

    doc.setContent(xmlData);

    //const char* msg = xmlData.toLatin1();
    //KMessageBox::information(0, xmlData.fromUtf8(msg, qstrlen(msg)));

    QDomElement docElem =  doc.documentElement();

    QDomNode n = docElem.firstChild().firstChild();

    while (!n.isNull())
    {
        const QDomElement e = n.toElement();
        if (!e.isNull())
        {
            mappedData.insert(e.tagName(), e.text());
            resultString.append(e.tagName() + ":" + e.text() + "\n");
        }

        n = n.nextSibling();

    }
    
    //kDebug()<<resultString;
    return mappedData;
}


void BackendGeonamesRG::slotResult(KJob* kJob)
{


    KIO::Job* kioJob = qobject_cast<KIO::Job*>(kJob);


    for(int i = 0;i < d->jobs.count(); ++i)
    {

        if(d->jobs.at(i).kioJob == kioJob)
        {

            QString dataString(d->jobs.at(i).data);
            int pos = dataString.indexOf("<geonames");
            dataString.remove(0,pos);
            dataString.chop(1);

            d->jobs[i].request.rgData =  makeQMapFromXML(dataString);

            QMap<QString, QString>::const_iterator it = d->jobs[i].request.rgData.constBegin();


            while( it != d->jobs[i].request.rgData.constEnd() ){

                //kDebug()<<it.key()<< ":"<< it.value();
                ++it;

            }

            emit(signalRGReady(QList<RGInfo>()<<d->jobs.at(i).request));
            d->jobs.removeAt(i);

 
            break;
        }

    }
    

}



} //KIPIGPSSyncPlugin
