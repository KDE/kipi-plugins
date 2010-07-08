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

//#include <QItemSelectionModel>
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
#include <QAbstractItemModel>
#include <QTreeView>
#include <QContextMenuEvent>
#include <QInputDialog>

// KDE includes

#include <kaction.h>
#include <kconfiggroup.h>
#include <khtml_part.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kvbox.h>
#include <kcombobox.h>
#include <kseparator.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kmenu.h>

// Libkmap includes

#include <libkmap/html_widget.h>
#include <libkmap/kmap_primitives.h>

//local includes

#include "gpssyncdialog.h"
#include "kipiimagemodel.h"
#include "gpsimageitem.h"
#include "gpsreversegeocodingwidget.h"
#include "backend-rg.h"
#include "backend-geonames-rg.h"
#include "backend-osm-rg.h"
#include "backend-geonamesUS-rg.h"
#include "parseTagString.h"
#include "rgtagmodel.h"
#include "tests/simpletreemodel/simpletreemodel.h"

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>
#include <libkipi/version.h>

#ifdef GPSSYNC_MODELTEST
#include <modeltest.h>
#endif /* GPSSYNC_MODELTEST */

namespace KIPIGPSSyncPlugin
{

class GPSReverseGeocodingWidgetPrivate
{
public:
    GPSReverseGeocodingWidgetPrivate()
    {
    }

    bool hideOptions;
    bool UIEnabled;
    QLabel *label;
    KipiImageModel* imageModel;
    QItemSelectionModel* selectionModel;
    QPushButton* buttonRGSelected;

    KComboBox* serviceComboBox;
    KComboBox *languageEdit;
    QList<RGInfo> photoList;
    QList<RGBackend*> backendRGList;
    RGBackend* currentBackend;
    int requestedRGCount;
    int receivedRGCount;
    QLineEdit* baseTagEdit;
    QPushButton* buttonHideOptions;
    QCheckBox *autoTag;
    QCheckBox *iptc, *xmpLoc, *xmpKey;
    QWidget* UGridContainer;
    QWidget* LGridContainer;
    QLabel* baseTagLabel;
    QLabel* serviceLabel;
    QLabel* metadataLabel;
    QLabel* languageLabel;
    int backendIndex;

    QAbstractItemModel* externTagModel;
    RGTagModel* tagModel;
    QTreeView *tagTreeView;

    QItemSelectionModel* tagSelectionModel; 
    KAction* actionAddCountry;
    KAction* actionAddCounty;
    KAction* actionAddCity;
    KAction* actionAddStreet;
    KAction* actionAddPlace;
    KAction* actionAddLAU2;
    KAction* actionAddLAU1;
    KAction* actionAddCustomizedSpacer;
    KAction* actionRemoveTag;
    KAction* actionRemoveAllNewTags;
    KAction* actionReaddNewTags;

    GPSUndoCommand* undoCommand;
};


GPSReverseGeocodingWidget::GPSReverseGeocodingWidget(KIPI::Interface* interface, KipiImageModel* const imageModel, QItemSelectionModel* const selectionModel, QWidget *const parent)
: QWidget(parent), d(new GPSReverseGeocodingWidgetPrivate())
{

    d->imageModel = imageModel;
    d->selectionModel = selectionModel;
    
    d->UIEnabled = true;
    // we need to have a main layout and add KVBox to it or derive from KVBox
    // - or is there an easier way to use KVBox?
    QVBoxLayout* vBoxLayout = new QVBoxLayout(this);

    KVBox* const vbox = new KVBox(this);
    vBoxLayout->addWidget(vbox);
    vbox->layout()->setSpacing(0);
    vbox->layout()->setMargin(0);
    

    d->UGridContainer = new QWidget(vbox);
    d->tagTreeView = new QTreeView(vbox);
    Q_ASSERT(d->tagTreeView!=0);

    // TODO: workaround until the new libkipi hits the streets
#if KIPI_VERSION>=0x010109
    d->externTagModel = interface->getTagTree();
#endif

    if (!d->externTagModel)
    {
        // we currently require a backend model, even if it is empty
        d->externTagModel = new SimpleTreeModel(1, this);
    }

    if (d->externTagModel)
    {
        d->tagModel = new RGTagModel(d->externTagModel, this);
        d->tagTreeView->setModel(d->tagModel);

#ifdef GPSSYNC_MODELTEST
         new ModelTest(d->externTagModel, d->tagTreeView);
         new ModelTest(d->tagModel, d->tagTreeView);
#endif /* GPSSYNC_MODELTEST */
    }

    d->tagSelectionModel = new QItemSelectionModel(d->tagModel);
    d->tagTreeView->setSelectionModel(d->tagSelectionModel);

    d->actionAddCountry = new KAction(i18n("Add country tag"), this);
    d->actionAddCounty = new KAction(i18n("Add county tag"), this);
    d->actionAddCity = new KAction(i18n("Add city tag"), this);
    d->actionAddStreet = new KAction(i18n("Add street"), this);
    d->actionAddPlace = new KAction(i18n("Add place"), this);
    d->actionAddLAU2 = new KAction(i18n("Add LAU2"), this);
    d->actionAddLAU1 = new KAction(i18n("Add LAU1"), this);
    d->actionAddCustomizedSpacer = new KAction(i18n("Add new tag"), this);
    d->actionRemoveTag = new KAction(i18n("Remove selected tag"), this);
    d->actionRemoveAllNewTags = new KAction(i18n("Remove all new tags"), this);
    d->actionReaddNewTags = new KAction(i18n("Readd new tags"), this);

    QGridLayout* const gridLayout = new QGridLayout(d->UGridContainer);

    d->languageLabel = new QLabel(i18n("Select language:"), d->UGridContainer);
    d->languageEdit = new KComboBox(d->UGridContainer);

    d->languageEdit->addItem(i18n("English"),"en");
    d->languageEdit->addItem(i18n("German"), "de");
    d->languageEdit->addItem(i18n("Romanian"), "ro");
    d->languageEdit->addItem(i18n("Chinese"), "zh");
    d->languageEdit->addItem(i18n("Arabic"), "ar");
    d->languageEdit->addItem(i18n("Morroco"), "ma");
    d->languageEdit->addItem(i18n("Egiptian"), "eg");

    d->serviceLabel = new QLabel(i18n("Select service:"), d->UGridContainer);
    d->serviceComboBox = new KComboBox(d->UGridContainer);

    d->serviceComboBox->addItem(i18n("Geonames.org place name (non-US)"));
    d->serviceComboBox->addItem(i18n("Geonames.org full address (US only)"));
    d->serviceComboBox->addItem(i18n("Open Street Map"));

    d->baseTagLabel = new QLabel(i18n("Select base tag:"), d->UGridContainer);
    d->baseTagEdit = new QLineEdit("My Tags/{Country}/{City}", d->UGridContainer);

    int row = 0;
    gridLayout->addWidget(d->serviceLabel,row,0,1,2);
    row++;
    gridLayout->addWidget(d->serviceComboBox,row,0,1,2); 
    row++;
    gridLayout->addWidget(d->languageLabel,row,0,1,1);
    gridLayout->addWidget(d->languageEdit,row,1,1,1);
    row++;
    gridLayout->addWidget(d->baseTagLabel,row,0,1,2);
    row++;
    gridLayout->addWidget(d->baseTagEdit, row, 0,1,2);

    d->UGridContainer->setLayout(gridLayout);

    new KSeparator(Qt::Horizontal, vbox);
    d->buttonHideOptions = new QPushButton(i18n("Less options"), vbox);
    d->hideOptions = true;


    d->LGridContainer = new QWidget(vbox);
    QGridLayout* LGridLayout = new QGridLayout(d->LGridContainer);

    d->autoTag = new QCheckBox("Tag automatically when coordinates are changed", d->LGridContainer);

    d->metadataLabel = new QLabel( i18n("Write tags to:"),d->LGridContainer); 

    d->iptc = new QCheckBox( i18n("IPTC"), d->LGridContainer);
    d->xmpLoc = new QCheckBox( i18n("XMP location"), d->LGridContainer);
    d->xmpKey = new QCheckBox( i18n("XMP keywords"), d->LGridContainer);

    row = 0;
    LGridLayout->addWidget(d->autoTag, row,0,1,3);
    row++;
    LGridLayout->addWidget(d->metadataLabel, row,0,1,3);
    row++;
    LGridLayout->addWidget(d->iptc,row,0,1,3);
    row++;
    LGridLayout->addWidget(d->xmpLoc,row,0,1,3);
    row++;
    LGridLayout->addWidget(d->xmpKey, row,0,1,3);

    d->LGridContainer->setLayout(LGridLayout);

    d->buttonRGSelected = new QPushButton(i18n("Apply reverse geocoding"), vbox);

    //d->undoCommand = new GPSUndoCommand();

    dynamic_cast<QVBoxLayout*>(vbox->layout())->addStretch(300); 

    //d->backendRGList.append(new BackendGoogleRG(this));
    d->backendRGList.append(new BackendGeonamesRG(this));
    d->backendRGList.append(new BackendGeonamesUSRG(this));
    d->backendRGList.append(new BackendOsmRG(this));
   
    d->tagTreeView->installEventFilter(this);
 
    updateUIState();

    connect(d->buttonRGSelected, SIGNAL(clicked()),
            this, SLOT(slotButtonRGSelected()));

    connect(d->buttonHideOptions, SIGNAL(clicked()),
            this, SLOT(slotHideOptions()));

    connect(d->selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection)),
            this, SLOT(updateUIState()));

    connect(d->tagTreeView, SIGNAL( clicked(const QModelIndex &)), 
            this, SLOT( treeItemClicked(const QModelIndex &)));    

    connect(d->serviceComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT( slotServiceSelectionChanged(int)));

    connect(d->actionAddCountry, SIGNAL(triggered(bool)),
            this, SLOT(slotAddCountry()));

    connect(d->actionAddCounty, SIGNAL(triggered(bool)),
            this, SLOT(slotAddCounty()));
    
    connect(d->actionAddCity, SIGNAL(triggered(bool)),
            this, SLOT(slotAddCity()));
    connect(d->actionAddStreet, SIGNAL(triggered(bool)),
            this, SLOT(slotAddStreet()));

    connect(d->actionAddPlace, SIGNAL(triggered(bool)),
            this, SLOT(slotAddPlace()));

    connect(d->actionAddLAU2, SIGNAL(triggered(bool)),
            this, SLOT(slotAddLAU2()));

    connect(d->actionAddLAU1, SIGNAL(triggered(bool)),
            this, SLOT(slotAddLAU1()));

    connect(d->actionAddCustomizedSpacer, SIGNAL(triggered(bool)),
            this, SLOT(slotAddCustomizedSpacer()));

    connect(d->imageModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
            this, SLOT(slotRegenerateNewTags()));

    connect(d->actionRemoveTag, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveTag()));
    connect(d->actionRemoveAllNewTags, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveAllNewTags()));
    connect(d->actionReaddNewTags, SIGNAL(triggered(bool)),
            this, SLOT(slotReaddNewTags()));

    for (int i=0; i<d->backendRGList.count(); ++i)
    {
        connect(d->backendRGList[i], SIGNAL(signalRGReady(QList<RGInfo> &)),
                this, SLOT(slotRGReady(QList<RGInfo>&)));
    }

    
    d->backendIndex = d->serviceComboBox->currentIndex(); 
    d->currentBackend = d->backendRGList[d->backendIndex];


}

void GPSReverseGeocodingWidget::updateUIState()
{
    const bool haveSelection = d->selectionModel->hasSelection();

    d->buttonRGSelected->setEnabled(d->UIEnabled && haveSelection);
    d->serviceLabel->setEnabled(d->UIEnabled);
    d->serviceComboBox->setEnabled(d->UIEnabled);
    d->languageLabel->setEnabled(d->UIEnabled);
    d->languageEdit->setEnabled(d->UIEnabled);

    d->baseTagLabel->setEnabled(d->UIEnabled);
    d->baseTagEdit->setEnabled(d->UIEnabled);

    d->buttonHideOptions->setEnabled(d->UIEnabled);
    d->autoTag->setEnabled(d->UIEnabled);
    d->metadataLabel->setEnabled(d->UIEnabled);
    d->iptc->setEnabled(d->UIEnabled);
    d->xmpLoc->setEnabled(d->UIEnabled);
    d->xmpKey->setEnabled(d->UIEnabled);
}

GPSReverseGeocodingWidget::~GPSReverseGeocodingWidget()
{
    delete d;
}

void GPSReverseGeocodingWidget::slotButtonRGSelected()
{
    // get the selected image:
    const QModelIndexList selectedItems = d->selectionModel->selectedRows();
    d->backendIndex = d->serviceComboBox->currentIndex(); 
    d->currentBackend = d->backendRGList[d->backendIndex];
    
    d->undoCommand = new GPSUndoCommand();

    QList<RGInfo> photoList;

    QString wantedLanguage = d->languageEdit->itemData(d->languageEdit->currentIndex()).toString(); 


    QList<QList<TagData> > returnedSpacers = d->tagModel->getSpacers();

    for( int i = 0; i < selectedItems.count(); ++i)
    {

        const QPersistentModelIndex itemIndex = selectedItems.at(i);
        const GPSImageItem* const selectedItem = static_cast<GPSImageItem*>(d->imageModel->itemFromIndex(itemIndex));

        const GPSDataContainer gpsData = selectedItem->gpsData();
         if (!gpsData.hasCoordinates())
            continue;

        const qreal latitude = gpsData.getCoordinates().lat();
        const qreal longitude = gpsData.getCoordinates().lon();

        RGInfo photoObj;
        photoObj.id = itemIndex;
        photoObj.coordinates = KMapIface::WMWGeoCoordinate(latitude, longitude);

        photoList << photoObj;
    }

    if (!photoList.isEmpty())
    {
        d->receivedRGCount = 0;
        d->requestedRGCount = photoList.count();
        emit(signalProgressSetup(d->requestedRGCount, i18n("Retrieving RG info - %p%")));
        emit(signalSetUIEnabled(false));

        d->currentBackend->callRGBackend(photoList, wantedLanguage);
    }
}

void GPSReverseGeocodingWidget::slotHideOptions()
{

    if(d->hideOptions)
    {
        d->LGridContainer->hide();
        d->hideOptions = false;
        d->buttonHideOptions->setText("More options");
    }
    else{
        d->LGridContainer->show();
        d->hideOptions = true;
        d->buttonHideOptions->setText("Less options");
    }

}

void GPSReverseGeocodingWidget::slotRGReady(QList<RGInfo>& returnedRGList)
{

    const QString errorString = d->currentBackend->getErrorMessage();
    
    if(!errorString.isEmpty())
    {

        KMessageBox::error(this, errorString);
        
        d->receivedRGCount+=returnedRGList.count();
        emit(signalSetUIEnabled(true));
        return;
    } 

    QString address;
    for(int i = 0; i < returnedRGList.count(); ++i)
    {

        QPersistentModelIndex currentImageIndex = returnedRGList[i].id;

        if(!returnedRGList[i].rgData.empty())
        {
            //QString result = makeTagString(returnedRGList[i], d->baseTagEdit->displayText(), d->currentBackend->backendName());

            kDebug()<<"RETURNED RG LIST:"<<returnedRGList[i].rgData;

            QString addressElementsWantedFormat;
            

            if(d->currentBackend->backendName() == QString("Geonames"))
                addressElementsWantedFormat.append("/{Country}/{Place}");
            else if(d->currentBackend->backendName() == QString("GeonamesUS"))
                addressElementsWantedFormat.append("/{LAU2}/{LAU1}/{City}");
            else     
                addressElementsWantedFormat.append("/{Country}/{County}/{City}/{Street}");
            

            QStringList combinedResult = makeTagString(returnedRGList[i], addressElementsWantedFormat, d->currentBackend->backendName());

            kDebug()<<"ADDRESS ELEMENTS:"<<combinedResult;

            QString addressFormat = combinedResult[0];
            QString addressElements = combinedResult[1];

            //removes first "/" from tag addresses
            addressFormat.remove(0,1);
            addressElements.remove(0,1);
            addressElementsWantedFormat.remove(0,1);

            QStringList listAddressElementsWantedFormat = addressElementsWantedFormat.split("/");
            QStringList listAddressElements = addressElements.split("/");
            QStringList listAddressFormat = addressFormat.split("/");

            QStringList elements, resultedData;

            for(int i=0; i<listAddressElementsWantedFormat.count(); ++i)
            {
                QString currentAddressFormat = listAddressElementsWantedFormat.at(i);

                int currentIndexFormat = listAddressFormat.indexOf(currentAddressFormat,0);
                if(currentIndexFormat != -1)
                {
                    elements<<currentAddressFormat;
                    resultedData<<listAddressElements.at(currentIndexFormat);
                }
            
            }

            kDebug()<<"ELEMENTS:"<<elements;
            kDebug()<<"RESULTED DATA:"<<resultedData;              

            QList<QList<TagData> > returnedTags = d->tagModel->addNewData(elements, resultedData);   
            
            //kDebug()<<"Returned tags:"<<returnedTags;
            //returnedTags.removeDuplicates();

            GPSImageItem* currentItem = static_cast<GPSImageItem*>(d->imageModel->itemFromIndex(currentImageIndex));

            GPSUndoCommand::UndoInfo undoInfo(currentImageIndex);
            undoInfo.readOldDataFromItem(currentItem);            

            currentItem->setTagList(returnedTags);

            undoInfo.readNewDataFromItem(currentItem);
            d->undoCommand->addUndoInfo(undoInfo);
         
        }
    }

    d->receivedRGCount+=returnedRGList.count();
    if (d->receivedRGCount>=d->requestedRGCount)
    {
        d->undoCommand->setText(i18n("Image tags are changed."));
        emit(signalUndoCommand(d->undoCommand));

        emit(signalSetUIEnabled(true));
    }
    else
    {
        emit(signalProgressChanged(d->receivedRGCount));
    }
}

void GPSReverseGeocodingWidget::slotServiceSelectionChanged(int index)
{
    d->backendIndex = index;
    //d->currentBackend = 
} 

void GPSReverseGeocodingWidget::setUIEnabled(const bool state)
{
    d->UIEnabled = state;
    updateUIState();
}

void GPSReverseGeocodingWidget::treeItemClicked( const QModelIndex& index)
{

}

bool GPSReverseGeocodingWidget::eventFilter(QObject* watched, QEvent* event)
{
    if(watched == d->tagTreeView)
    {
        if((event->type()==QEvent::ContextMenu) && d->tagSelectionModel->hasSelection() && d->UIEnabled)
        {
            KMenu * const menu = new KMenu(d->tagTreeView);
           
            d->currentBackend = d->backendRGList[d->backendIndex]; 
            QString backendName = d->currentBackend->backendName();

            if(backendName.compare(QString("OSM")) == 0)
            {
            
                menu->addAction(d->actionAddCountry);
                menu->addAction(d->actionAddCounty);
                menu->addAction(d->actionAddCity);
                menu->addAction(d->actionAddStreet);
                menu->addAction(d->actionAddCustomizedSpacer);
                menu->addAction(d->actionRemoveTag);
                menu->addAction(d->actionRemoveAllNewTags);
                menu->addAction(d->actionReaddNewTags);  
         
            }
            else if(d->currentBackend->backendName() == QString("Geonames"))
            {
                menu->addAction(d->actionAddCountry);
                menu->addAction(d->actionAddPlace);
            }
            else if(d->currentBackend->backendName() == QString("GeonamesUS"))
            {
                menu->addAction(d->actionAddLAU2);
                menu->addAction(d->actionAddLAU1);
                menu->addAction(d->actionAddCity); 
            }
            QContextMenuEvent * const e = static_cast<QContextMenuEvent*>(event);
            menu->exec(e->globalPos());

        }

    }
    return QObject::eventFilter(watched, event);
}

void GPSReverseGeocodingWidget::saveSettingsToGroup(KConfigGroup* const group)
{
    
    group->writeEntry("RG Backend", d->serviceComboBox->currentIndex()); 
    group->writeEntry("Language", d->languageEdit->currentIndex());

    group->writeEntry("Hide options", d->hideOptions); 
    group->writeEntry("IPTC", d->iptc->isChecked());
    group->writeEntry("XMP location", d->xmpLoc->isChecked());
    group->writeEntry("XMP keywords", d->xmpKey->isChecked());

}

void GPSReverseGeocodingWidget::readSettingsFromGroup(const KConfigGroup* const group)
{

    d->serviceComboBox->setCurrentIndex(group->readEntry("RG Backend", 0));
    d->languageEdit->setCurrentIndex(group->readEntry("Language", 0));

    d->hideOptions = !(group->readEntry("Hide options", false));
    slotHideOptions();

    d->iptc->setChecked(group->readEntry("IPTC", false));
    d->xmpLoc->setChecked(group->readEntry("XMP location", false));
    d->xmpKey->setChecked(group->readEntry("XMP keywords", false));

}

void GPSReverseGeocodingWidget::slotAddCountry()
{
    const QModelIndex baseIndex = d->tagSelectionModel->currentIndex();
    d->tagModel->addSpacerTag(baseIndex, "{Country}");

}

void GPSReverseGeocodingWidget::slotAddCounty()
{
    const QModelIndex baseIndex = d->tagSelectionModel->currentIndex();
    d->tagModel->addSpacerTag(baseIndex, "{County}");
}

void GPSReverseGeocodingWidget::slotAddCity()
{
    const QModelIndex baseIndex = d->tagSelectionModel->currentIndex();
    d->tagModel->addSpacerTag(baseIndex, "{City}");
}

void GPSReverseGeocodingWidget::slotAddStreet()
{
    const QModelIndex baseIndex = d->tagSelectionModel->currentIndex();
    d->tagModel->addSpacerTag(baseIndex, "{Street}");
}

void GPSReverseGeocodingWidget::slotAddPlace()
{
    const QModelIndex baseIndex = d->tagSelectionModel->currentIndex();
    d->tagModel->addSpacerTag(baseIndex, "{Place}");
}

void GPSReverseGeocodingWidget::slotAddLAU1()
{
    const QModelIndex baseIndex = d->tagSelectionModel->currentIndex();
    d->tagModel->addSpacerTag(baseIndex, "{LAU1}");
}

void GPSReverseGeocodingWidget::slotAddLAU2()
{
    const QModelIndex baseIndex = d->tagSelectionModel->currentIndex();
    d->tagModel->addSpacerTag(baseIndex, "{LAU2}");
}

void GPSReverseGeocodingWidget::slotAddCustomizedSpacer()
{
    const QModelIndex baseIndex = d->tagSelectionModel->currentIndex();
    bool ok;
    QString textString;
    
    textString = QInputDialog::getText(this,QString("%1").arg("Add new tag:"), QString("%1").arg("Select a name for the new tag:"), QLineEdit::Normal, QString::null, &ok);
    if( ok && !textString.isEmpty())
    {
        d->tagModel->addSpacerTag(baseIndex, textString);
    }
    
    
}

void GPSReverseGeocodingWidget::slotRemoveTag()
{
    const QModelIndex baseIndex = d->tagSelectionModel->currentIndex();
    d->tagModel->deleteTag(baseIndex);
}

void GPSReverseGeocodingWidget::slotRemoveAllNewTags()
{
    d->tagModel->deleteAllNewTags();
}

void GPSReverseGeocodingWidget::slotReaddNewTags()
{

    for( int row=0; row<d->imageModel->rowCount(); ++row)
    {

        GPSImageItem* currentItem = static_cast<GPSImageItem*>(d->imageModel->itemFromIndex(d->imageModel->index(row,0)));

        QList<QList<TagData> > tagAddresses = currentItem->getTagList();
        if(!tagAddresses.isEmpty())
            d->tagModel->readdNewTags(tagAddresses);

    }

}

void GPSReverseGeocodingWidget::slotRegenerateNewTags()
{
    kDebug()<<"REGENERATES TAG TREE:";

   slotRemoveAllNewTags();

    kDebug()<<"All new tags are removed.";

   slotReaddNewTags(); 
}

} /* KIPIGPSSyncPlugin  */


