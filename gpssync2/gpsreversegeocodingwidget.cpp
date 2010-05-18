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

#include <QPointer>
#include <QLabel>
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

};


GPSReverseGeocodingWidget::GPSReverseGeocodingWidget(QWidget *const parent)
: QWidget(parent), d(new GPSReverseGeocodingWidgetPrivate())
{

    
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

    QString ss = "Here will be the result";

    d->label = new QLabel(splitter);
    splitter->addWidget(d->label);
    d->label->setText(ss);


    connect(d->htmlWidget, SIGNAL(signalJavaScriptReady()),
            this, SLOT(slotHTMLInitialized()));

    connect(d->htmlWidget, SIGNAL(signalHTMLEvents(const QStringList&)),
                this, SLOT(slotHTMLEvents(const QStringList&)));

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
    
} /* KIPIGPSSyncPlugin  */


