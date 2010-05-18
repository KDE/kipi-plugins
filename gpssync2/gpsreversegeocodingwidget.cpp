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
// KDE includes

#include <kaction.h>
#include <kconfiggroup.h>
#include <khtml_part.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kvbox.h>
//local includes
#include "../worldmapwidget2/lib/html_widget.h"
#include "gpssyncdialog.h"
#include "kipiimagemodel.h"
#include "gpsimageitem.h"



namespace KIPIGPSSyncPlugin
{

class GPSReverseGeocodingWidgetPrivate
{
public:
    GPSReverseGeocodingWidgetPrivate()
    : htmlWidget(0)
    {
    }

    WMW2::HTMLWidget* htmlWidget;
    QLabel *label;
    KipiImageModel* imageModel;
    QItemSelectionModel* selectionModel;
    QPushButton* buttonRGSelected;
};


GPSReverseGeocodingWidget::GPSReverseGeocodingWidget(KipiImageModel* const imageModel, QItemSelectionModel* const selectionModel, QWidget *const parent)
: QWidget(parent), d(new GPSReverseGeocodingWidgetPrivate())
{

    d->imageModel = imageModel;
    d->selectionModel = selectionModel;

    QSplitter *splitter = new QSplitter(Qt::Vertical,this);
    splitter->resize(300,300);


    QWidget* dummy = new QWidget(splitter);
    dummy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    splitter->addWidget(dummy);
    
    d->htmlWidget = new WMW2::HTMLWidget(dummy);
    dummy->resize(200,200);

    // KUrl htmlUrl = KUrl("/home/gabi/Downloads/kipi-plugins-1.2.0/gpssync2/rg-google-maps-v3.html");

    KUrl htmlUrl = KStandardDirs::locate("data", "gpssync2/rg-google-maps-v3.html");

    d->htmlWidget->openUrl(htmlUrl);

    KVBox* const vbox = new KVBox(splitter);
    splitter->addWidget(vbox);

    QString ss = "Here will be the result";

    d->label = new QLabel(vbox);
    d->label->setText(ss);

    d->buttonRGSelected = new QPushButton(i18n("RG selected image"), vbox);
    


    connect(d->htmlWidget, SIGNAL(signalJavaScriptReady()),
            this, SLOT(slotHTMLInitialized()));

    connect(d->htmlWidget, SIGNAL(signalHTMLEvents(const QStringList&)),
                this, SLOT(slotHTMLEvents(const QStringList&)));

    connect(d->buttonRGSelected, SIGNAL(clicked()),
            this, SLOT(slotButtonRGSelected()));
}



GPSReverseGeocodingWidget::~GPSReverseGeocodingWidget()
{
    delete d;
}


void GPSReverseGeocodingWidget::slotHTMLEvents( const QStringList& events)
{


    for (QStringList::const_iterator it = events.constBegin(); it!=events.constEnd(); ++it){

    d->label->setText(*it);
    

    }


}


void GPSReverseGeocodingWidget::slotHTMLInitialized()
{

   d->htmlWidget->runScript(QString("reverseGeocoding(%1,%2);").arg("21").arg("42"));

}

void GPSReverseGeocodingWidget::slotButtonRGSelected()
{
    // get the selected image:
    const QModelIndexList selectedItems = d->selectionModel->selectedRows();

    // TODO: work on more than one image
    if (selectedItems.count()!=1)
        return;

    const GPSImageItem* const selectedItem = static_cast<GPSImageItem*>(d->imageModel->itemFromIndex(selectedItems.first()));

    const GPSDataContainer gpsData = selectedItem->gpsData();
    if (!gpsData.m_hasFlags.testFlag(GPSDataContainer::HasCoordinates))
        return;

    const qreal latitude = gpsData.m_coordinates.lat();
    const qreal longitude = gpsData.m_coordinates.lon();

    // now call the reverse geocoding function:
    d->htmlWidget->runScript(QString("reverseGeocoding(%1,%2);").arg(latitude).arg(longitude));
}

} /* KIPIGPSSyncPlugin  */


