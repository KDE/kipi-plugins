
#include "backend-google-rg.moc"

//Qt includes

#include <QTimer>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMessageBox>

// Libkmap includes

#include <libkmap/html_widget.h>

//local includes

#include "gpsreversegeocodingwidget.h"

//kde includes

#include <kurl.h>


namespace KIPIGPSSyncPlugin
{

class RGInternal {

    public:

    RGInternal()
    :request(),
    requestId(){   }

    RGInfo request;
    int requestId;
};


class BackendGoogleRGPrivate
{

public:

    BackendGoogleRGPrivate()
    : htmlWidget(0),
      counter(0)
    {
    }

    KMap::HTMLWidget* htmlWidget;
    QList<RGInternal> internalList;
    int counter;
    QString language;
    QString errorMessage;
};

BackendGoogleRG::BackendGoogleRG(QObject* const parent)
: RGBackend(parent), d(new BackendGoogleRGPrivate())
{  

    d->htmlWidget = new KMap::HTMLWidget();
    KUrl htmlUrl = KStandardDirs::locate("data", "gpssync/rg-google-maps-v3.html");
    d->htmlWidget->openUrl(htmlUrl);
    d->language = "EN";

    connect(d->htmlWidget, SIGNAL(signalJavaScriptReady()),
            this, SLOT(slotHTMLInitialized()));

    connect(d->htmlWidget, SIGNAL(signalHTMLEvents(const QStringList&)),
                this, SLOT(slotHTMLEvents(const QStringList&)));


}

BackendGoogleRG::~BackendGoogleRG()
{

    delete d;

}

QMap<QString,QString> BackendGoogleRG::makeQMap(QString str)
{


    QMap<QString, QString> map;

    QStringList listAddress;
    listAddress = str.split("\n");
    listAddress.removeLast();

    for(int i = 0; i < listAddress.size(); ++i)
    {

        QStringList listKeyValue;
        QString element,name;

        QString addressElement = listAddress.at(i);

        listKeyValue = addressElement.split(":");
        map.insert(listKeyValue.at(0), listKeyValue.at(1));

    }

    return map;
}


void BackendGoogleRG::slotHTMLInitialized()
{


}

void BackendGoogleRG::slotHTMLEvents( const QStringList& events)
{

    int foundId;
    QList<RGInfo> returnedRGList;

    

    for( int i = 0; i < events.count(); i++)
    {

        //saves the current address from address lists
        QString currentAddress = events.at(i);

        //separates the id from address
        int positionId = currentAddress.indexOf("~");
        QString stringId = currentAddress.left(positionId);
        currentAddress.remove(0,positionId + 1);
        int photoId = stringId.toInt();


        //finds the RGInfo object inside RGinteralList
        for ( int j = 0; j < d->internalList.count(); ++j)
        {

            if(d->internalList[j].requestId == photoId)
            {
                foundId = j;
                break;
            }
        }

        //inserts the RGInfo object inside a list to put addresses inside photos
        RGInternal photoObj = d->internalList.at(foundId);

        photoObj.request.rgData = makeQMap(currentAddress);

        returnedRGList.insert(i, photoObj.request);

    }
    if(d->counter < d->internalList.count())
    {
      QTimer::singleShot( 500 , this, SLOT(nextPhoto()));
    }

    emit(signalRGReady(returnedRGList));
}


void BackendGoogleRG::nextPhoto()
{

    RGInfo photoDetails = d->internalList[d->counter].request;
    


    d->htmlWidget->runScript(QString("reverseGeocoding(%1,'%2',%3,%4);").arg(d->internalList[d->counter].requestId).arg(d->language).arg(photoDetails.coordinates.latString()).arg(photoDetails.coordinates.lonString()));
    d->counter++;

}

QString BackendGoogleRG::getErrorMessage()
{

    return d->errorMessage;

}

void BackendGoogleRG::callRGBackend(QList<RGInfo> rgList, QString language)
{

    d->internalList.clear();
    d->counter = 0;
    d->language = language;

    for( int i = 0; i < rgList.count(); ++i)
    {

        RGInternal internalObj;
        internalObj.request = rgList.at(i);
        internalObj.requestId = i;

        d->internalList.insert(i, internalObj);

    }
        nextPhoto();
    
}

 
} //KIPIGPSSyncPlugin
