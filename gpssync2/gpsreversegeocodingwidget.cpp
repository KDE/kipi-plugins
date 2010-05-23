/* ============================================================
 *
 * Date        : 2010-03-26
 * Description : A widget to configure the GPS correlation
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "gpsreversegeocodingwidget.moc"

// Qt includes

#include <QItemSelectionModel>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QSplitter>
#include <QString>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QLineEdit>

// KDE includes

#include <kaction.h>
#include <kconfiggroup.h>
#include <khtml_part.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kvbox.h>
//local includes
#include "../worldmapwidget2/lib/worldmapwidget2_primitives.h"
#include "../worldmapwidget2/lib/html_widget.h"
#include "gpssyncdialog.h"
#include "kipiimagemodel.h"
#include "gpsimageitem.h"
#include "gpsreversegeocodingwidget.h"


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

class GPSReverseGeocodingWidgetPrivate
{
public:
    GPSReverseGeocodingWidgetPrivate()
    : htmlWidget(0),
      counter(0)
    {
    }

    WMW2::HTMLWidget* htmlWidget;
    QLabel *label;
    KipiImageModel* imageModel;
    QItemSelectionModel* selectionModel;
    QPushButton* buttonRGSelected;

    QLineEdit *textEdit;
    QString wanted_language;
    int counter; 
    QList<RGInternal> internalList;
    QList<RGInfo> photoList;
};


GPSReverseGeocodingWidget::GPSReverseGeocodingWidget(KipiImageModel* const imageModel, QItemSelectionModel* const selectionModel, QWidget *const parent)
: QWidget(parent), d(new GPSReverseGeocodingWidgetPrivate())
{

    d->imageModel = imageModel;
    d->selectionModel = selectionModel;

    QSplitter *splitter = new QSplitter(Qt::Vertical,this);
    splitter->resize(300,100);

    d->htmlWidget = new WMW2::HTMLWidget();

    KUrl htmlUrl = KStandardDirs::locate("data", "gpssync2/rg-google-maps-v3.html");

    d->htmlWidget->openUrl(htmlUrl);
    

    KVBox* const vbox = new KVBox(splitter);
    splitter->addWidget(vbox);

//    QString ss = "Here will be the result";

//    d->label = new QLabel(vbox);
//    d->label->setText(ss);

    d->textEdit = new QLineEdit(vbox);
    
    d->buttonRGSelected = new QPushButton(i18n("RG selected image"), vbox);
    
    d->wanted_language = "EN";

    connect(d->htmlWidget, SIGNAL(signalJavaScriptReady()),
            this, SLOT(slotHTMLInitialized()));

    connect(d->htmlWidget, SIGNAL(signalHTMLEvents(const QStringList&)),
                this, SLOT(slotHTMLEvents(const QStringList&)));

    connect(d->buttonRGSelected, SIGNAL(clicked()),
            this, SLOT(slotButtonRGSelected()));

    connect(this, SIGNAL(signalRGReady(QList<RGInfo> &)), 
            this, SLOT(slotRGReady(QList<RGInfo>&)));	
}



GPSReverseGeocodingWidget::~GPSReverseGeocodingWidget()
{
    delete d;
}


QMap<QString,QString> GPSReverseGeocodingWidget::makeQMap(QString str){


    QMap<QString, QString> map;

    QStringList listAddress;
    listAddress = str.split("\n");
    listAddress.removeLast();   
 
    for(int i = 0; i < listAddress.size(); ++i){

        QStringList listKeyValue;
        QString element,name;        

        QString addressElement = listAddress.at(i);

        listKeyValue = addressElement.split(":");
        map.insert(listKeyValue.at(0), listKeyValue.at(1));

    }

    return map;
}

void GPSReverseGeocodingWidget::slotHTMLEvents( const QStringList& events)
{

    int foundId;
    QList<RGInfo> returnedRGList;


    for( int i = 0; i < events.count(); i++){

        //saves the current address from address lists
        QString currentAddress = events.at(i);

        //separates the id from address
        int positionId = currentAddress.indexOf("~");
        QString stringId = currentAddress.left(positionId);
        currentAddress.remove(0,positionId + 1);
        int photoId = stringId.toInt();
 
 
        //finds the RGInfo object inside RGinteralList
        for ( int j = 0; j < d->internalList.count(); ++j){

            if(d->internalList[j].requestId == photoId){
                foundId = j;
                break;
            }
        }
     
        //inserts the RGInfo object inside a list to put addresses inside photos
        RGInternal photoObj = d->internalList.at(foundId);
        //d->internalList.removeAt(foundId);    
 
        photoObj.request.rgData = makeQMap(currentAddress);

        returnedRGList.insert(i, photoObj.request);

    }
    //d->internalList.clear();
    if(d->counter < d->internalList.count()){
        QTimer::singleShot( 1000 * (d->counter + 1), this, SLOT(nextPhoto()));
    }

    emit(signalRGReady(returnedRGList));

}


void GPSReverseGeocodingWidget::slotHTMLInitialized()
{


}


void GPSReverseGeocodingWidget::nextPhoto()
{

    RGInfo photoDetails = d->internalList[d->counter].request;

    d->htmlWidget->runScript(QString("reverseGeocoding(%1,'%2',%3,%4);").arg(d->internalList[d->counter].requestId).arg(d->wanted_language).arg(photoDetails.coordinates.latString()).arg(photoDetails.coordinates.lonString()));
    d->counter++;

}

void GPSReverseGeocodingWidget::runRGScript(QList<RGInfo> rgList)
{

    d->internalList.clear();
    d->counter = 0;

    for( int i = 0; i < rgList.count(); ++i){

        RGInternal internalObj;
        internalObj.request = rgList.at(i);
        internalObj.requestId = i;

        d->internalList.insert(i, internalObj);

    //    QTimer::singleShot( 1000 * i, this, SLOT(nextPhoto()));

    }

    nextPhoto();

}


void GPSReverseGeocodingWidget::slotButtonRGSelected()
{
    // get the selected image:
    const QModelIndexList selectedItems = d->selectionModel->selectedRows();
    
    QList<RGInfo> photoList;
    d->wanted_language = d->textEdit->displayText();
   

    for( int i = 0; i < selectedItems.count(); ++i){

        const GPSImageItem* const selectedItem = static_cast<GPSImageItem*>(d->imageModel->itemFromIndex(selectedItems.at(i)));


        const GPSDataContainer gpsData = selectedItem->gpsData();
        if (!gpsData.m_hasFlags.testFlag(GPSDataContainer::HasCoordinates))
            return;

        const qreal latitude = gpsData.m_coordinates.lat();
        const qreal longitude = gpsData.m_coordinates.lon();
     
        RGInfo photoObj;
        photoObj.id = QVariant(i);
        photoObj.coordinates = WMW2::WMWGeoCoordinate(latitude, longitude);

        photoList.insert(i, photoObj);
 	
    }

    runRGScript(photoList);

}


void GPSReverseGeocodingWidget::slotRGReady(QList<RGInfo>& returnedRGList)
{

    QString address;

    for(int i = 0; i < returnedRGList.count(); ++i){

        address = "";
    
        QMap<QString, QString>::const_iterator it = returnedRGList[i].rgData.constBegin();
    
        while( it != returnedRGList[i].rgData.constEnd() ){
        
            address.append(it.key() + ":" + it.value() + "\n");
            ++it;

        }
	    kDebug()<<"Address "<<returnedRGList[i].id<<" coord:"<<returnedRGList[i].coordinates.latString()<<"    "<<address;
    
    }

} 

} /* KIPIGPSSyncPlugin  */


