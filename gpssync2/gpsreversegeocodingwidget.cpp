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
#include <QHBoxLayout>
#include <QCheckBox>

// KDE includes

#include <kaction.h>
#include <kconfiggroup.h>
#include <khtml_part.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kvbox.h>
#include <kcombobox.h>
#include <kseparator.h>

//local includes

#include "../worldmapwidget2/lib/worldmapwidget2_primitives.h"
#include "../worldmapwidget2/lib/html_widget.h"
#include "gpssyncdialog.h"
#include "kipiimagemodel.h"
#include "gpsimageitem.h"
#include "gpsreversegeocodingwidget.h"
#include "backend-rg.h"
#include "backend-google-rg.h"
#include "backend-geonames-rg.h"
#include "backend-osm-rg.h"

namespace KIPIGPSSyncPlugin
{

class GPSReverseGeocodingWidgetPrivate
{
public:
    GPSReverseGeocodingWidgetPrivate()
    {
    }

    bool hideOptions;
    QLabel *label;
    KipiImageModel* imageModel;
    QItemSelectionModel* selectionModel;
    QPushButton* buttonRGSelected;
    QString wantedLanguage;

    
    KComboBox* serviceComboBox;
    QLineEdit *languageEdit;
    QList<RGInfo> photoList;
    QList<RGBackend*> backendRGList;
    int requestedRGCount;
    int receivedRGCount;
    QCheckBox *country, *state, *county, *city, *district, *street, *streetNumber;
    QLineEdit* baseTagEdit;
    QPushButton* buttonHideOptions;
    QCheckBox *autoTag;
    QCheckBox *iptc, *xmpLoc, *xmpKey;
    QWidget* UGridContainer;
    QWidget* LGridContainer;
};


GPSReverseGeocodingWidget::GPSReverseGeocodingWidget(KipiImageModel* const imageModel, QItemSelectionModel* const selectionModel, QWidget *const parent)
: QWidget(parent), d(new GPSReverseGeocodingWidgetPrivate())
{

    d->imageModel = imageModel;
    d->selectionModel = selectionModel;

    d->wantedLanguage = "EN";

    KVBox* const vbox = new KVBox(this);
    vbox->resize(350,400);
    vbox->layout()->setSpacing(0);
    vbox->layout()->setMargin(0);
    
    d->UGridContainer = new QWidget(vbox);
    d->UGridContainer->resize(350,400);
    
    QGridLayout* const gridLayout = new QGridLayout(d->UGridContainer);

    QLabel* languageLabel = new QLabel(i18n("Select language:"), d->UGridContainer);
    d->languageEdit = new QLineEdit(d->UGridContainer);

    QLabel* serviceLabel = new QLabel(i18n("Select service:"), d->UGridContainer);
    d->serviceComboBox = new KComboBox(d->UGridContainer);

    d->serviceComboBox->addItem(i18n("Geonames.org"));
    d->serviceComboBox->addItem(i18n("Open Street Map"));

    QLabel* addressElemLabel = new QLabel(i18n("Select address elements:"), d->UGridContainer);
    QWidget* addressElemContainer = new QWidget(d->UGridContainer);
    QGridLayout* addressElemLayout = new QGridLayout(addressElemContainer);
    
    d->country = new QCheckBox( i18n("Country"),addressElemContainer );
    d->state = new QCheckBox( i18n("State"),addressElemContainer );
    d->county = new QCheckBox( i18n("County"),addressElemContainer );
    d->city = new QCheckBox( i18n("City"),addressElemContainer );
    d->district = new QCheckBox( i18n("District"),addressElemContainer );
    d->street = new QCheckBox( i18n("Street"),addressElemContainer );
    d->streetNumber = new QCheckBox( i18n("Street Nb."),addressElemContainer );
   
    addressElemLayout->addWidget(d->country, 0,0,1,1);
    addressElemLayout->addWidget(d->state, 0,1,1,1);
    addressElemLayout->addWidget(d->county, 0,2,1,1);
    addressElemLayout->addWidget(d->city, 0,3,1,1);
    addressElemLayout->addWidget(d->district, 1,0,1,1);
    addressElemLayout->addWidget(d->street, 1,1,1,1);
    addressElemLayout->addWidget(d->streetNumber, 1,2,1,1);

    addressElemContainer->setLayout(addressElemLayout);    

    QLabel* baseTagLabel = new QLabel(i18n("Select base tag:"), d->UGridContainer);
    d->baseTagEdit = new QLineEdit(d->UGridContainer);



    int row = 0;
    gridLayout->addWidget(serviceLabel,row,0,1,1);
    gridLayout->addWidget(d->serviceComboBox,row,1,1,1); 
    row++;
    gridLayout->addWidget(addressElemLabel,row,0,1,2);
    row++;
    gridLayout->addWidget(addressElemContainer, row,0,1,2);
    row++;
    gridLayout->addWidget(languageLabel,row,0,1,1);
    gridLayout->addWidget(d->languageEdit,row,1,1,1);
    row++;
    gridLayout->addWidget(baseTagLabel,row,0,1,2);
    row++;
    gridLayout->addWidget(d->baseTagEdit, row, 0,1,2);

    d->UGridContainer->setLayout(gridLayout);

    KSeparator* const separator = new KSeparator(Qt::Horizontal, vbox);
    d->buttonHideOptions = new QPushButton(i18n("More options/Less options"), vbox);
    d->hideOptions = true;


    d->LGridContainer = new QWidget(vbox);
    d->LGridContainer->resize(350,400);
    QGridLayout* LGridLayout = new QGridLayout(d->LGridContainer);
       
    d->autoTag = new QCheckBox("Tag automatically when coordinates are changed", d->LGridContainer);

    QLabel* metadataLabel = new QLabel( i18n("Write tags to:"),d->LGridContainer); 

    d->iptc = new QCheckBox( i18n("IPTC"), d->LGridContainer);
    d->xmpLoc = new QCheckBox( i18n("XMP location"), d->LGridContainer);
    d->xmpKey = new QCheckBox( i18n("XMP keywords"), d->LGridContainer);

    row = 0;
    LGridLayout->addWidget(d->autoTag, row,0,1,3);
    row++;
    LGridLayout->addWidget(metadataLabel, row,0,1,3);
    row++;
    LGridLayout->addWidget(d->iptc,row,0,1,3);
    row++;
    LGridLayout->addWidget(d->xmpLoc,row,0,1,3);
    row++;
    LGridLayout->addWidget(d->xmpKey, row,0,1,3);

    d->LGridContainer->setLayout(LGridLayout);

    d->buttonRGSelected = new QPushButton(i18n("Apply reverse geocoding"), vbox);
    
    if(!d->selectionModel->hasSelection()){

        d->buttonRGSelected->setEnabled(false);

    }

    dynamic_cast<QVBoxLayout*>(vbox->layout())->addStretch(300); 

    //d->backendRGList.append(new BackendGoogleRG(this));
    d->backendRGList.append(new BackendGeonamesRG(this));
    d->backendRGList.append(new BackendOsmRG(this));

    connect(d->buttonRGSelected, SIGNAL(clicked()),
            this, SLOT(slotButtonRGSelected()));

    connect(d->buttonHideOptions, SIGNAL(clicked()),
            this, SLOT(slotHideOptions()));

    connect(d->selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection)),
            this, SLOT(updateUIState()));

    for (int i=0; i<d->backendRGList.count(); ++i)
    {
        connect(d->backendRGList[i], SIGNAL(signalRGReady(QList<RGInfo> &)),
                this, SLOT(slotRGReady(QList<RGInfo>&)));
    }
}

void GPSReverseGeocodingWidget::updateUIState()
{

    if(d->selectionModel->hasSelection()){
        d->buttonRGSelected->setEnabled(true);

    }
    else{
        d->buttonRGSelected->setEnabled(false);
    }

}

GPSReverseGeocodingWidget::~GPSReverseGeocodingWidget()
{
    delete d;
}


void GPSReverseGeocodingWidget::slotButtonRGSelected()
{
    // get the selected image:
    const QModelIndexList selectedItems = d->selectionModel->selectedRows();
    int backendIndex = d->serviceComboBox->currentIndex(); 


    QList<RGInfo> photoList;
    d->wantedLanguage = d->languageEdit->displayText();

    for( int i = 0; i < selectedItems.count(); ++i){

        const QPersistentModelIndex itemIndex = selectedItems.at(i);
        const GPSImageItem* const selectedItem = static_cast<GPSImageItem*>(d->imageModel->itemFromIndex(itemIndex));


        const GPSDataContainer gpsData = selectedItem->gpsData();
        if (!gpsData.m_hasFlags.testFlag(GPSDataContainer::HasCoordinates))
            return;

        const qreal latitude = gpsData.m_coordinates.lat();
        const qreal longitude = gpsData.m_coordinates.lon();
     
        RGInfo photoObj;
        photoObj.id = QVariant::fromValue(itemIndex);
        photoObj.coordinates = WMW2::WMWGeoCoordinate(latitude, longitude);

        photoList << photoObj;
    }

    if (!photoList.isEmpty())
    {
        d->receivedRGCount = 0;
        d->requestedRGCount = photoList.count();
        emit(signalProgressSetup(d->requestedRGCount, i18n("Retrieving RG info - %p%")));
        emit(signalSetUIEnabled(false));


        d->backendRGList[backendIndex]->callRGBackend(photoList, d->wantedLanguage);
    }
}

void GPSReverseGeocodingWidget::slotHideOptions()
{

    if(d->hideOptions){
        d->LGridContainer->hide();
        d->hideOptions = false;
    }
    else{
        d->LGridContainer->show();
        d->hideOptions = true;
    }

}

void GPSReverseGeocodingWidget::slotRGReady(QList<RGInfo>& returnedRGList)
{

    //TODO: filter the results using checkboxes from UI

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

    d->receivedRGCount+=returnedRGList.count();
    if (d->receivedRGCount>=d->requestedRGCount)
    {
        emit(signalSetUIEnabled(true));
    }
    else
    {
        emit(signalProgressChanged(d->receivedRGCount));
    }
} 

void GPSReverseGeocodingWidget::setUIEnabled(const bool state)
{
    d->buttonRGSelected->setEnabled(state);
}

} /* KIPIGPSSyncPlugin  */


