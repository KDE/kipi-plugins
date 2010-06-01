
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

    QList<RGInfo> request;
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

    KUrl jobUrl("http://nominatim.openstreetmap.org/reverse");
    jobUrl.addQueryItem("format", "xml");
    jobUrl.addQueryItem("lat", d->jobs[d->itemCounter].request.first().coordinates.latString());
    jobUrl.addQueryItem("lon", d->jobs[d->itemCounter].request.first().coordinates.lonString());
    jobUrl.addQueryItem("zoom", "18");
    jobUrl.addQueryItem("addressdetails", "1");
    jobUrl.addQueryItem("accept-language", d->wantedLanguage);

    d->jobs[d->itemCounter].kioJob = KIO::get(jobUrl, KIO::NoReload, KIO::HideProgressInfo);

    d->jobs[d->itemCounter].kioJob->addMetaData("User-Agent", "kde-imaging@kde.org");
        
    connect(d->jobs[d->itemCounter].kioJob, SIGNAL(data(KIO::Job*, const QByteArray&)), 
            this, SLOT(dataIsHere(KIO::Job*,const QByteArray &)));
    connect(d->jobs[d->itemCounter].kioJob, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));    
        

}

void BackendOsmRG::callRGBackend(QList<RGInfo> rgList, QString language)
{

    //TODO: Remove dublicates from rgList to mergedQuery
    kDebug()<<"Entering OSM backend";
    d->itemCounter = 0;
    d->jobs.clear();
    d->wantedLanguage = language;

    for( int i = 0; i < rgList.count(); i++){


        bool foundIt = false;
        for( int j=0; j < d->jobs.count(); j++){

            if(d->jobs[j].request.first().coordinates.sameLonLatAs(rgList[i].coordinates)){

                d->jobs[j].request << rgList[i];
                foundIt = true;
                break;

             }


        }

        if(!foundIt){

        OsmInternalJobs newJob;
        newJob.request << rgList.at(i);
        d->jobs<<newJob;
        }

    }
    
    
    d->itemCount = d->jobs.count();
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


            QString dataString;
            dataString = QString::fromUtf8(d->jobs[i].data.constData(),qstrlen(d->jobs[i].data.constData()));


            int pos = dataString.indexOf("<reversegeocode");
            dataString.remove(0,pos);

            QMap<QString, QString> resultMap = makeQMapFromXML(dataString);

            for(int j = 0; j < d->jobs[i].request.count(); ++j){

                d->jobs[i].request[j].rgData = resultMap; 

                emit(signalRGReady(QList<RGInfo>()<<d->jobs.at(i).request.at(j)));
            
            }
            //d->jobs.removeAt(i);

            d->itemCounter++;
            if(d->itemCounter < d->itemCount){    
                nextPhoto();
            }

            break;
        }
    }
}



} //KIPIGPSSyncPlugin
