/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "PhotoItemLoader.moc"
#include "PhotoItem.h"
#include "ImageLoadingThread.h"
#include "ProgressObserver.h"
#include "global.h"

#include <QBuffer>

#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

PhotoItemLoader::PhotoItemLoader(PhotoItem * item, QDomElement & element, QObject * parent) :
    AbstractPhotoItemLoader(item, element, parent)
{
}

void PhotoItemLoader::run()
{
    PhotoItem * item = (PhotoItem*) this->item();
    ProgressObserver * observer = this->observer();
    AbstractPhotoItemLoader::run();

    QDomElement e = this->element();

    // Gets data field
    QDomElement defs = e.firstChildElement("defs");
    while (!defs.isNull() && defs.attribute("class") != "data")
        defs = defs.nextSiblingElement("defs");
    if (defs.isNull())
        this->exit(1);
    QDomElement data = defs.firstChildElement("data");
    if (data.isNull())
        this->exit(1);

    // m_image_path
    if (observer)
    {
        observer->progresChanged(0.5);
        observer->progresName(i18n("Loading shape..."));
    }
    QDomElement path = data.firstChildElement("path");
    if (path.isNull())
        this->exit(1);
    item->m_image_path = KIPIPhotoLayoutsEditor::pathFromSvg(path);
    if (item->m_image_path.isEmpty())
        this->exit(1);

    // transform
    QDomElement transform = path.nextSiblingElement("transform");
    if (!transform.isNull())
    {
        item->d->m_brush_transform = QTransform();
        QRegExp rot("matrix\\([-0-9.]+,[-0-9.]+,[-0-9.]+,[-0-9.]+,[-0-9.]+,[-0-9.]+\\)");
        if (rot.indexIn(transform.attribute("matrix")) >= 0)
        {
            QStringList list = rot.capturedTexts();
            QString matrix = list.at(0);
            matrix.remove(matrix.length()-1,1).remove(0,7);
            list = matrix.split(',');
            QString m11 = list.at(0);
            QString m12 = list.at(1);
            QString m21 = list.at(2);
            QString m22 = list.at(3);
            QString m31 = list.at(4);
            QString m32 = list.at(5);
            item->d->m_brush_transform *= QTransform(m11.toDouble(), m12.toDouble(), 0,
                                                     m21.toDouble(), m22.toDouble(), 0,
                                                     m31.toDouble(), m32.toDouble(), 1);
        }
        qDebug() << item->d->m_brush_transform;
    }
    qDebug() << item->d->m_brush_transform;

    // m_pixmap_original
    if (observer)
    {
        observer->progresChanged(0.6);
        observer->progresName(i18n("Loading image..."));
    }
    QDomElement imageElement = data.firstChildElement("image");
    QString imageAttribute;
    // Fullsize image is embedded in SVG file!
    if (!(imageAttribute = imageElement.text()).isEmpty())
    {
        item->d->m_image = QImage::fromData( QByteArray::fromBase64(imageAttribute.toAscii()) );
        //if (item->d->m_image.isNull())
        //    this->exit(1);
    }
    // Try to find file from path attribute
    else if ( !(imageAttribute = PhotoItem::PhotoItemPrivate::locateFile( imageElement.attribute("xlink:href") )).isEmpty() )
    {
        ImageLoadingThread * loader = new ImageLoadingThread(this);
        loader->setImageUrl(KUrl(imageAttribute));
        loader->start();
        loader->wait();
    }
    else
        this->exit(1);

    if (observer)
    {
        observer->progresChanged(1);
        observer->progresName(i18n("Finishing..."));
    }

    this->exit(0);
}

void PhotoItemLoader::imageLoaded(const KUrl & /*url*/, const QImage & image)
{
    if (image.isNull())
        this->exit(1);
    PhotoItem * item = (PhotoItem*) this->item();
    item->d->m_image = image;
}
