
#include "backend-osm-rg.moc"


//KDE includes
#include <kio/scheduler.h>
#include <kurl.h>
#include <kio/jobclasses.h>
#include <kdebug.h>
#include <QMessageBox>
#include <kio/job.h>
#include <klocale.h>

//local includes
#include "backend-osm-rg.h"
#include "gpsreversegeocodingwidget.h"
#include "backend-rg.h"

//Qt includes
#include <QDomDocument>
#include <QMap>
#include <QString>

namespace KIPIGPSSyncPlugin
{


class OsmInternalJobs {

public:

    OsmInternalJobs()
    : request(),
      data(),
      kioJob(0)
    {
    }

    RGInfo request;
    QByteArray data;
    KIO::Job* kioJob;
};


class BackendOsmRGPrivate
{

public:
    
    BackendOsmRGPrivate()
    : jobs()
    {
    }
    
    int counter;
    int itemCounter;
    int itemCount;
    QString wantedLanguage;
    QList<OsmInternalJobs> jobs;

};

BackendOsmRG::BackendOsmRG(QObject* const parent)
: RGBackend(parent), d(new BackendOsmRGPrivate())
{
    d->wantedLanguage = "en";
    
}

BackendOsmRG::~BackendOsmRG()
{
    delete d;
}

void BackendOsmRG::nextPhoto()
{
    int endValue;

    if(d->itemCounter + 5 > d->itemCount){
        endValue = d->itemCount % 5;
    }
    else{
        endValue = 5;
    }

    for(int i = 0; i < endValue; ++i){

        KUrl jobUrl("http://nominatim.openstreetmap.org/reverse");
        jobUrl.addQueryItem("format", "xml");
        jobUrl.addQueryItem("lat", d->jobs[d->itemCounter + i].request.coordinates.latString());
        jobUrl.addQueryItem("lon", d->jobs[d->itemCounter + i].request.coordinates.lonString());
        jobUrl.addQueryItem("zoom", "18");
        jobUrl.addQueryItem("addressdetails", "1");
        jobUrl.addQueryItem("accept-language", d->wantedLanguage);

        d->jobs[d->itemCounter + i].kioJob = KIO::get(jobUrl, KIO::NoReload, KIO::HideProgressInfo);

        
        connect(d->jobs[d->itemCounter + i].kioJob, SIGNAL(data(KIO::Job*, const QByteArray&)), 
                this, SLOT(dataIsHere(KIO::Job*,const QByteArray &)));
        connect(d->jobs[d->itemCounter + i].kioJob, SIGNAL(result(KJob*)),
                this, SLOT(slotResult(KJob*)));    
        
    }

}

void BackendOsmRG::callRGBackend(QList<RGInfo> rgList, QString language)
{

    //TODO: Remove dublicates from rgList to mergedQuery
    kDebug()<<"Entering OSM backend";
    QList<RGInfo> mergedQuery = rgList;
    d->itemCount = mergedQuery.count();
    d->itemCounter = 0;
    d->wantedLanguage = language;

    for( int i = 0; i < mergedQuery.count(); i++){

        OsmInternalJobs newJob;
        newJob.request = mergedQuery.at(i);

        d->jobs<<newJob;
        d->jobs[i].kioJob = newJob.kioJob;


    }
    nextPhoto();

}

void BackendOsmRG::dataIsHere(KIO::Job* job, const QByteArray & data)
{

    
    for(int i = 0; i < d->jobs.count(); ++i){

        if(d->jobs.at(i).kioJob == job){

            
            d->jobs[i].data.append(data);
            break;

        }
        

    }

}


QMap<QString,QString> BackendOsmRG::makeQMapFromXML(QString xmlData)
{

    QString resultString;
    QMap<QString, QString> mappedData;
    QDomDocument doc;
    doc.setContent(xmlData);

    QDomElement docElem =  doc.documentElement();

    QDomNode n = docElem.lastChild().firstChild();

    while(!n.isNull()){

        QDomElement e = n.toElement();
        if(!e.isNull()){

            mappedData.insert(e.tagName(), e.text()); 
            resultString.append(e.tagName() + ":" + e.text() + "\n");

        }

        n = n.nextSibling();

    }

    return mappedData;

}


void BackendOsmRG::slotResult(KJob* kJob)
{
    KIO::Job* kioJob = qobject_cast<KIO::Job*>(kJob);

    for(int i = 0; i < d->jobs.count(); ++i){

        if(d->jobs.at(i).kioJob == kioJob){

            QString dataString(d->jobs.at(i).data);

            int pos = dataString.indexOf("<reversegeocode");
            dataString.remove(0,pos);

            d->jobs[i].request.rgData = makeQMapFromXML(dataString);

            emit(signalRGReady(QList<RGInfo>()<<d->jobs.at(i).request));

            //d->jobs.removeAt(i);

            d->itemCounter++;
            if((d->itemCounter % 5 == 0) && (d->itemCounter < d->itemCount)){
                nextPhoto();
            }
            else if(d->itemCounter == d->itemCount){
                d->jobs.clear();
            }


            break;
        }
    }
}



} //KIPIGPSSyncPlugin
