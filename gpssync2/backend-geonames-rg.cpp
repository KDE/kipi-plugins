
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

    QString language;
    int itemCounter;
    int itemCount;
    QList<GeonamesInternalJobs> jobs;
};

BackendGeonamesRG::BackendGeonamesRG(QObject* const parent)
: RGBackend(parent), d(new BackendGeonamesRGPrivate())
{
    d->language = "EN";
}

BackendGeonamesRG::~BackendGeonamesRG()
{
    delete d;
}

void BackendGeonamesRG::nextPhoto()
{
    int endValue;

    if(d->itemCounter + 5 > d->itemCount){
        endValue = d->itemCount % 5;
    }
    else{
        endValue = 5;
    }

    
    for(int i = 0; i < endValue; i++){
    
        KUrl jobUrl("http://ws.geonames.org/findNearbyPlaceName");
        jobUrl.addQueryItem("lat", d->jobs[d->itemCounter+i].request.coordinates.latString());
        jobUrl.addQueryItem("lng", d->jobs[d->itemCounter+i].request.coordinates.lonString());
        jobUrl.addQueryItem("lang", d->language);

        d->jobs[d->itemCounter+i].kioJob = KIO::get(jobUrl, KIO::NoReload, KIO::HideProgressInfo);

        connect(d->jobs[d->itemCounter+i].kioJob, SIGNAL(data(KIO::Job*, const QByteArray&)), 
                this, SLOT(dataIsHere(KIO::Job*,const QByteArray &)));
        connect(d->jobs[d->itemCounter+i].kioJob, SIGNAL(result(KJob*)),
                this, SLOT(slotResult(KJob*)));    


    }    

}

void BackendGeonamesRG::callRGBackend(QList<RGInfo> rgList, QString language)
{

    kDebug()<<"Entering Geonames backend";

    //TODO: Remove dublicates from rgList to mergedQuery
    QList<RGInfo> mergedQuery = rgList;
    d->language = language;
    d->itemCount = mergedQuery.count();
    d->itemCounter = 0;
    d->jobs.clear();

    for( int i = 0; i < mergedQuery.count(); ++i){

        GeonamesInternalJobs newJob;
        newJob.request = mergedQuery.at(i);
        d->jobs<<newJob;
        d->jobs[i].kioJob = newJob.kioJob;
    }
    kDebug()<<"A ajuns inainte de primul nextPhoto()";
    nextPhoto();

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
    
    return mappedData;
}


void BackendGeonamesRG::slotResult(KJob* kJob)
{


    KIO::Job* kioJob = qobject_cast<KIO::Job*>(kJob);


    for(int i = 0;i < d->jobs.count(); ++i)
    {

        if(d->jobs.at(i).kioJob == kioJob)
        {

            QString dataString;
            dataString = QString::fromUtf8(d->jobs[i].data.constData(),qstrlen(d->jobs[i].data.constData()));
            int pos = dataString.indexOf("<geonames");
            dataString.remove(0,pos);
            dataString.chop(1);


            d->jobs[i].request.rgData =  makeQMapFromXML(dataString);

            emit(signalRGReady(QList<RGInfo>()<<d->jobs.at(i).request));
            // d->jobs.removeAt(i);

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
