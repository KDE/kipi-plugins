
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
#include "gpssync2_common.h"

//Qt includes
#include <QDomDocument>
#include <QMap>
#include <QString>
#include <qtimer.h>

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
    QString language;
};


class BackendOsmRGPrivate
{

public:
    
    BackendOsmRGPrivate()
    : jobs()
    {
    }
    
    QList<OsmInternalJobs> jobs;
    QString errorMessage;
};

BackendOsmRG::BackendOsmRG(QObject* const parent)
: RGBackend(parent), d(new BackendOsmRGPrivate())
{
    
}

BackendOsmRG::~BackendOsmRG()
{
    delete d;
}

void BackendOsmRG::nextPhoto()
{

    KUrl jobUrl("http://nominatim.openstreetmap.org/reverse");
    jobUrl.addQueryItem("format", "xml");
    jobUrl.addQueryItem("lat", d->jobs.first().request.first().coordinates.latString());
    jobUrl.addQueryItem("lon", d->jobs.first().request.first().coordinates.lonString());
    jobUrl.addQueryItem("zoom", "18");
    jobUrl.addQueryItem("addressdetails", "1");
    jobUrl.addQueryItem("accept-language", d->jobs.first().language);

    d->jobs.first().kioJob = KIO::get(jobUrl, KIO::NoReload, KIO::HideProgressInfo);

    d->jobs.first().kioJob->addMetaData("User-Agent", getKipiUserAgentName());

    connect(d->jobs.first().kioJob, SIGNAL(data(KIO::Job*, const QByteArray&)), 
            this, SLOT(dataIsHere(KIO::Job*,const QByteArray &)));
    connect(d->jobs.first().kioJob, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));    
        

}

void BackendOsmRG::callRGBackend(QList<RGInfo> rgList, QString language)
{

    kDebug()<<"Entering OSM backend";

    d->errorMessage.clear();

    for( int i = 0; i < rgList.count(); i++){


        bool foundIt = false;
        for( int j=0; j < d->jobs.count(); j++){

            if(d->jobs[j].request.first().coordinates.sameLonLatAs(rgList[i].coordinates)){

                d->jobs[j].request << rgList[i];
                d->jobs[j].language = language;
                foundIt = true;
                break;

             }


        }

        if(!foundIt){

        OsmInternalJobs newJob;
        newJob.request << rgList.at(i);
        newJob.language = language;
        d->jobs<<newJob;
        }

    }
    
    if(!d->jobs.empty()){
    
        nextPhoto();

    }

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

            if( (e.tagName().compare(QString("country")) == 0) ||
                (e.tagName().compare(QString("city")) == 0) ||
                (e.tagName().compare(QString("town")) == 0) ||
                (e.tagName().compare(QString("village")) == 0) ||
                (e.tagName().compare(QString("hamlet")) == 0) ||
                (e.tagName().compare(QString("place")) == 0))
            {
            
            mappedData.insert(e.tagName(), e.text());
            resultString.append(e.tagName() + ":" + e.text() + "\n");

            }
        }

        n = n.nextSibling();

    }

    return mappedData;

}

QString BackendOsmRG::getErrorMessage()
{

    return d->errorMessage;
}

void BackendOsmRG::slotResult(KJob* kJob)
{
    KIO::Job* kioJob = qobject_cast<KIO::Job*>(kJob);

    if(kioJob->error())
    {

        d->errorMessage = kioJob->errorString();
        emit(signalRGReady(d->jobs.first().request));
        d->jobs.clear();
        
        return;

    }
 

    for(int i = 0; i < d->jobs.count(); ++i){

        if(d->jobs.at(i).kioJob == kioJob){

            QString dataString;
            dataString = QString::fromUtf8(d->jobs[i].data.constData(),qstrlen(d->jobs[i].data.constData()));

            int pos = dataString.indexOf("<reversegeocode");
            dataString.remove(0,pos);

            QMap<QString, QString> resultMap = makeQMapFromXML(dataString);

            for(int j = 0; j < d->jobs[i].request.count(); ++j){

                d->jobs[i].request[j].rgData = resultMap; 
            
            }
           emit(signalRGReady(d->jobs[i].request));
 
            d->jobs.removeAt(i);

            if(!d->jobs.empty()){    
                QTimer::singleShot(500, this, SLOT(nextPhoto()));
            }

            break;
        }
    }
}



} //KIPIGPSSyncPlugin
