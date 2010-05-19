/* ============================================================
 *
 * Date        : 2010-05-12
 * Description : A widget to apply Reverse Geocoding
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2010 by Gabriel Voicu <ping dot gabi at gmail dot com>
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


#ifndef REVERSEGEOCODING_H
#define REVERSEGEOCODING_H

//KDE includes

#include <kurl.h>

//Qt includes

#include <QWidget>

class QItemSelectionModel;

namespace KIPIGPSSyncPlugin
{

class GPSReverseGeocodingWidgetPrivate;
class KipiImageModel;

class GPSReverseGeocodingWidget : public QWidget
{

Q_OBJECT;

public: 

    GPSReverseGeocodingWidget(KipiImageModel* const imageModel, QItemSelectionModel* const selectionModel, QWidget* const parent = 0);
    ~GPSReverseGeocodingWidget();

private:
    QMap<QString, QString> makeQMap(QString);

private Q_SLOTS:
    void slotHTMLInitialized();
    void slotHTMLEvents( const QStringList& );
    void slotButtonRGSelected();
private:

    GPSReverseGeocodingWidgetPrivate* const d;    

};


}  /* KIPIGPSSyncPlugin */

#endif /* REVERSEGEOCODING_H */


