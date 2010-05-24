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
#include "backend-rg.h"
#include "backend-google-rg.h"

namespace KIPIGPSSyncPlugin
{

class GPSReverseGeocodingWidgetPrivate
{
public:
    GPSReverseGeocodingWidgetPrivate()
    {
    }

    QLabel *label;
    KipiImageModel* imageModel;
    QItemSelectionModel* selectionModel;
    QPushButton* buttonRGSelected;


    QLineEdit *textEdit;
    QList<RGInfo> photoList;
    RGBackend* backendRG;
};


GPSReverseGeocodingWidget::GPSReverseGeocodingWidget(KipiImageModel* const imageModel, QItemSelectionModel* const selectionModel, QWidget *const parent)
: QWidget(parent), d(new GPSReverseGeocodingWidgetPrivate())
{

    d->imageModel = imageModel;
    d->selectionModel = selectionModel;

    QSplitter *splitter = new QSplitter(Qt::Vertical,this);
    splitter->resize(300,100);


    KVBox* const vbox = new KVBox(splitter);
    splitter->addWidget(vbox);

    d->textEdit = new QLineEdit(vbox);
    
    d->buttonRGSelected = new QPushButton(i18n("RG selected image"), vbox);
    

    d->backendRG = new BackendGoogleRG(this);    

    connect(d->buttonRGSelected, SIGNAL(clicked()),
            this, SLOT(slotButtonRGSelected()));

    connect(d->backendRG, SIGNAL(signalRGReady(QList<RGInfo> &)), 
            this, SLOT(slotRGReady(QList<RGInfo>&)));	
}



GPSReverseGeocodingWidget::~GPSReverseGeocodingWidget()
{
    delete d;
}


void GPSReverseGeocodingWidget::slotButtonRGSelected()
{
    // get the selected image:
    const QModelIndexList selectedItems = d->selectionModel->selectedRows();
    
    QList<RGInfo> photoList;
    const QString wanted_language = d->textEdit->displayText();
   

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

    d->backendRG->runRGScript(photoList, wanted_language);

}


void GPSReverseGeocodingWidget::slotRGReady(QList<RGInfo>& returnedRGList)
{

    QString address;
  //  QMessageBox msg;
    for(int i = 0; i < returnedRGList.count(); ++i){

        address = "";
    
        QMap<QString, QString>::const_iterator it = returnedRGList[i].rgData.constBegin();
    
        while( it != returnedRGList[i].rgData.constEnd() ){
        
            address.append(it.key() + ":" + it.value() + "\n");
            ++it;

        }
	    kDebug()<<"Address "<<returnedRGList[i].id<<" coord:"<<returnedRGList[i].coordinates.latString()<<"    "<<address;
    //    msg.setText(QString("Photo ID: %1 , Address: %2").arg(returnedRGList[i].id.toInt()).arg(address));
   //     msg.exec();
    

    }

} 

} /* KIPIGPSSyncPlugin  */


