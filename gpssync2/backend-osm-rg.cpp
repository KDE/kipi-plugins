
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
    :jobs()
    {
    }
    

    QList<OsmInternalJobs> jobs;

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

}

void BackendOsmRG::callRGBackend(QList<RGInfo> rgList, QString language)
{

    //TODO: Remove dublicates from rgList to mergedQuery
    QList<RGInfo> mergedQuery = rgList;
    d->jobs.clear();


    for( int i = 0; i < mergedQuery.count(); i++){

        OsmInternalJobs newJob;
        newJob.request = mergedQuery.at(i);
//        newJob.data.append("");    
  
        KUrl jobUrl("http://nominatim.openstreetmap.org/reverse");
        jobUrl.addQueryItem("format", "xml");
        jobUrl.addQueryItem("lat", mergedQuery.at(i).coordinates.latString());
        jobUrl.addQueryItem("lon", mergedQuery.at(i).coordinates.lonString());
        jobUrl.addQueryItem("zoom", "18");
        jobUrl.addQueryItem("addressdetails", "1");

        newJob.kioJob = KIO::get(jobUrl, KIO::NoReload, KIO::HideProgressInfo);
        d->jobs<<newJob;
        d->jobs[i].kioJob = newJob.kioJob;

        kDebug()<<d->jobs[0].kioJob;

        connect(newJob.kioJob, SIGNAL(data(KIO::Job*, const QByteArray&)), 
                this, SLOT(dataIsHere(KIO::Job*,const QByteArray &)));
        connect(newJob.kioJob, SIGNAL(result(KJob*)),
                this, SLOT(slotResult(KJob*)));    
    

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


void BackendOsmRG::makeDOMFromXML(QString xmlData)
{

    QString resultString;

    QDomDocument doc;
    doc.setContent(xmlData);

    QDomElement docElem =  doc.documentElement();

    QDomNode n = docElem.firstChild();

   while(!n.isNull()){

        QDomElement e = n.toElement();
        if(!e.isNull()){

            QDomText t = n.toText();

            resultString.append(e.tagName() + ":" + e.text() + "\n");

        }

        n = n.nextSibling();

    }

    kDebug()<<resultString;

}


void BackendOsmRG::slotResult(KJob* kJob)
{



    KIO::Job* kioJob = qobject_cast<KIO::Job*>(kJob);


    for(int i = 0;i < d->jobs.count(); ++i){

        if(d->jobs.at(i).kioJob == kioJob){

            //kDebug()<<d->jobs.at(i).data;
           
            QString dataString(d->jobs.at(i).data);
            dataString.remove(0,40);
//            dataString.chop(1);

            //kDebug()<<dataString;

             makeDOMFromXML(dataString);

 
            break;
        }

    }
    

}



} //KIPIGPSSyncPlugin
