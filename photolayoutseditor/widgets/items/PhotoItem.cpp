/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#include "PhotoItem.h"
#include "PhotoEffectsGroup.h"
#include "PhotoEffectsLoader.h"
#include "ImageFileDialog.h"
#include "BordersGroup.h"
#include "global.h"
#include "PLEConfigSkeleton.h"
#include "photolayoutseditor.h"
#include "ImageLoadingThread.h"
#include "ProgressEvent.h"

#include <QBuffer>
#include <QStyleOptionGraphicsItem>
#include <QFile>
#include <QImageReader>
#include <QGraphicsScene>
#include <QMimeData>

#include <kapplication.h>
#include <kmessagebox.h>
#include <klocalizedstring.h>
#include <kstandarddirs.h>

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::PhotoItemPixmapChangeCommand : public QUndoCommand
{
    QImage m_image;
    PhotoItem * m_item;
public:
    PhotoItemPixmapChangeCommand(const QImage & image, PhotoItem * item, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Image change"), parent),
        m_image(image),
        m_item(item)
    {}
    PhotoItemPixmapChangeCommand(const QPixmap & pixmap, PhotoItem * item, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Image change"), parent),
        m_image(pixmap.toImage()),
        m_item(item)
    {}
    virtual void redo()
    {
        QImage temp = m_item->image();
        m_item->d->setImage(m_image);
        m_image = temp;
    }
    virtual void undo()
    {
        QImage temp = m_item->image();
        m_item->d->setImage(m_image);
        m_image = temp;
        m_item->update();
    }
};
class KIPIPhotoLayoutsEditor::PhotoItemUrlChangeCommand : public QUndoCommand
{
    KUrl m_url;
    PhotoItem * m_item;
public:
    PhotoItemUrlChangeCommand(const KUrl & url, PhotoItem * item, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Image path change"), parent),
        m_url(url),
        m_item(item)
    {}
    virtual void redo()
    {
        this->run();
    }
    virtual void undo()
    {
        this->run();
    }
    void run()
    {
        KUrl temp = m_item->d->fileUrl();
        m_item->d->setFileUrl(m_url);
        m_url = temp;
    }
};
class KIPIPhotoLayoutsEditor::PhotoItemImagePathChangeCommand : public QUndoCommand
{
    PhotoItem * m_item;
    QPainterPath m_image_path;
    CropShapeChangeCommand * command;
public:
    PhotoItemImagePathChangeCommand(PhotoItem * item, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Image shape change"), parent),
        m_item(item),
        m_image_path(m_item->m_image_path)
    {}
    virtual void redo()
    {
        m_item->m_image_path = QPainterPath();
        m_item->m_image_path.addRect(m_item->m_pixmap.rect());
        m_item->recalcShape();
        m_item->update();
    }
    virtual void undo()
    {
        m_item->m_image_path = m_image_path;
        m_item->recalcShape();
        m_item->update();
    }
};

QString PhotoItem::PhotoItemPrivate::locateFile(const QString & filePath)
{
    QString resultPath = filePath;
    if (!resultPath.isEmpty())
    {
        // Try to open existing file
        if (!QFile::exists(resultPath))
        {
            int result = KMessageBox::questionYesNo(0,
                                                    i18n("Can't find image file in this location:\n %s\n\n"
                                                         "Would you like to set new location of this file?\n"
                                                         "If not this image will be removed from the composition.").append(resultPath.toAscii()),
                                                    i18n("File reading error"));
            if (result != KMessageBox::Yes)
                resultPath = QString();
            else
            {
                KUrl fileUrl(filePath);
                ImageFileDialog dialog(fileUrl);
                result = dialog.exec();
                resultPath = dialog.selectedFile();
                if (result != ImageFileDialog::Accepted || !QFile::exists(resultPath))
                    resultPath = QString();
            }
        }
    }
    return resultPath;
}
void PhotoItem::PhotoItemPrivate::setImage(const QImage & image)
{
    if (image.isNull() || image == m_image)
        return;
    m_image = image;
    m_item->refresh();
}
QImage & PhotoItem::PhotoItemPrivate::image()
{
    return m_image;
}
void PhotoItem::PhotoItemPrivate::setFileUrl(const KUrl & url)
{
    this->m_file_path = url;
}
KUrl & PhotoItem::PhotoItemPrivate::fileUrl()
{
    return this->m_file_path;
}

PhotoItem::PhotoItem(const QImage & photo, const QString & name, Scene * scene) :
    AbstractPhoto((name.isEmpty() ? i18n("New image") : name), scene),
    m_highlight(false),
    d(new PhotoItemPrivate(this))
{
    this->setupItem(photo);
}

PhotoItem::PhotoItem(const QString & name, Scene * scene) :
    AbstractPhoto((name.isEmpty() ? i18n("New image") : name), scene),
    m_highlight(false),
    d(new PhotoItemPrivate(this))
{
    this->setupItem(QImage());
}

PhotoItem::~PhotoItem()
{
    delete d;
}

QDomElement PhotoItem::toSvg(QDomDocument & document) const
{
    QDomElement result = AbstractPhoto::toSvg(document);
    result.setAttribute("class", "PhotoItem");

    // 'defs' tag
    QDomElement defs = document.createElement("defs");
    defs.setAttribute("class", "data");
    result.appendChild(defs);

    // 'defs'-> pfe:'data'
    QDomElement appNS = document.createElementNS(KIPIPhotoLayoutsEditor::uri(), "data");
    appNS.setPrefix(KIPIPhotoLayoutsEditor::name());
    defs.appendChild(appNS);

    if (!m_image_path.isEmpty())
    {
        // 'defs'-> pfe:'data' ->'path'
        QDomElement path = KIPIPhotoLayoutsEditor::pathToSvg(m_image_path, document);
        path.setAttribute("class", "m_image_path");
        path.setPrefix(KIPIPhotoLayoutsEditor::name());
        appNS.appendChild(path);
    }

    QDomElement image = document.createElementNS(KIPIPhotoLayoutsEditor::uri(), "image");
    appNS.appendChild(image);
    // Saving image data
    if (!PLEConfigSkeleton::embedImagesData())
    {
        int result = KMessageBox::questionYesNo(0,
                                                i18n("Do you want to embed images data?\n"
                                                        "Remember that when you move or rename image files on your disk or the storage device become unavailable, those images become unavailable for %1 "
                                                        "and this layout might become broken.", KApplication::applicationName()),
                                                i18n("Saving: %1", this->name()),
                                                KStandardGuiItem::yes(),
                                                KStandardGuiItem::no(),
                                                PLEConfigSkeleton::self()->config()->name());
        if (result == KMessageBox::Yes)
            PLEConfigSkeleton::setEmbedImagesData(true);
    }

    if ( (PLEConfigSkeleton::embedImagesData() && !d->image().isNull()) || !d->fileUrl().isValid())
    {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        d->image().save(&buffer, "PNG");
        image.appendChild( document.createTextNode( QString(byteArray.toBase64()) ) );
        image.setAttribute("width",QString::number(d->image().width()));
        image.setAttribute("height",QString::number(d->image().height()));
    }

    // Saving image path
    if (d->fileUrl().isValid())
        image.setAttribute("src", d->fileUrl().url());

    return result;
}

PhotoItem * PhotoItem::fromSvg(QDomElement & element)
{
    PhotoItem * item = new PhotoItem();
    if (item->AbstractPhoto::fromSvg(element))
    {
        // Gets data field
        QDomElement defs = element.firstChildElement("defs");
        while (!defs.isNull() && defs.attribute("class") != "data")
            defs = defs.nextSiblingElement("defs");
        if (defs.isNull())
            goto _delete;
        QDomElement data = defs.firstChildElement("data");
        if (data.isNull())
            goto _delete;

        // m_image_path
        QDomElement path = data.firstChildElement("path");
        if (path.isNull())
            goto _delete;
        item->m_image_path = KIPIPhotoLayoutsEditor::pathFromSvg(path);
        if (item->m_image_path.isEmpty())
            goto _delete;

        // m_pixmap_original
        QDomElement image = data.firstChildElement("image");
        QString imageAttribute;
        QImage img;
        // Fullsize image is embedded in SVG file!
        if (!(imageAttribute = image.text()).isEmpty())
        {
            img = QImage::fromData( QByteArray::fromBase64(imageAttribute.toAscii()) );
            if (img.isNull())
                goto _delete;
        }
        // Try to find file from path attribute
        else if ( !(imageAttribute = PhotoItemPrivate::locateFile( image.attribute("xlink:href") )).isEmpty() )
        {
            QImageReader reader(imageAttribute);
            if (!reader.canRead())
                goto _delete;
            reader.setAutoDetectImageFormat(true);
            img = QImage(reader.size(), QImage::Format_ARGB32_Premultiplied);
            if (!reader.read(&img))
                goto _delete;
        }
        else
        {
            goto _delete;
        }
        item->d->setImage(img);

        return item;
    }
_delete:
    delete item;
    return 0;
}

QDomElement PhotoItem::svgVisibleArea(QDomDocument & document) const
{
    // 'defs'->'image'
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    m_pixmap.save(&buffer, "PNG");
    QDomElement img = document.createElement("image");
    img.setAttribute("width",m_pixmap.width());
    img.setAttribute("height",m_pixmap.height());
    img.setAttribute("xlink:href",QString("data:image/png;base64,")+byteArray.toBase64());
    return img;
}

void PhotoItem::dragEnterEvent(QGraphicsSceneDragDropEvent * event)
{
    const QMimeData * mimeData = event->mimeData();
    if ( PhotoLayoutsEditor::instance()->hasInterface() &&
            mimeData->hasFormat("digikam/item-ids"))
    {
        KUrl::List urls;
        QByteArray ba = mimeData->data("digikam/item-ids");
        QDataStream ds(&ba, QIODevice::ReadOnly);
        ds >> urls;
        event->setAccepted( (urls.count() == 1) );
        if (urls.count() == 1)
            event->setDropAction( Qt::CopyAction );
        else
            event->setDropAction( Qt::IgnoreAction );
    }
    else if (mimeData->hasFormat("text/uri-list"))
    {
        QList<QUrl> urls = mimeData->urls();
        event->setAccepted( (urls.count() == 1) );
        if (urls.count() == 1)
            event->setDropAction( Qt::CopyAction );
        else
            event->setDropAction( Qt::IgnoreAction );
    }
    this->setHighlightItem( event->isAccepted() );
}

void PhotoItem::dragLeaveEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
    this->setHighlightItem(false);
}

void PhotoItem::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    const QMimeData * mimeData = event->mimeData();
    if ( PhotoLayoutsEditor::instance()->hasInterface() &&
            mimeData->hasFormat("digikam/item-ids"))
    {
        KUrl::List urls;
        QByteArray ba = mimeData->data("digikam/item-ids");
        QDataStream ds(&ba, QIODevice::ReadOnly);
        ds >> urls;
        event->setAccepted( (urls.count() == 1) );
        if (urls.count() == 1)
            event->setDropAction( Qt::CopyAction );
        else
            event->setDropAction( Qt::IgnoreAction );
    }
    else if (mimeData->hasFormat("text/uri-list"))
    {
        QList<QUrl> urls = mimeData->urls();
        event->setAccepted( (urls.count() == 1) );
        if (urls.count() == 1)
            event->setDropAction( Qt::CopyAction );
        else
            event->setDropAction( Qt::IgnoreAction );
    }
    this->setHighlightItem( event->isAccepted() );
}

void PhotoItem::dropEvent(QGraphicsSceneDragDropEvent * event)
{
    QImage img;
    const QMimeData * mimeData = event->mimeData();
    if ( PhotoLayoutsEditor::instance()->hasInterface() &&
            mimeData->hasFormat("digikam/item-ids"))
    {
        KUrl::List urls;
        QByteArray ba = mimeData->data("digikam/item-ids");
        QDataStream ds(&ba, QIODevice::ReadOnly);
        ds >> urls;
        if (urls.count() == 1)
            this->setImageUrl(urls.at(0));
    }
    else if (mimeData->hasFormat("text/uri-list"))
    {
        QList<QUrl> urls = mimeData->urls();
        if (urls.count() == 1)
            this->setImageUrl(urls.at(0));
    }

    this->setHighlightItem(false);
    event->setAccepted( !img.isNull() );
}

QImage & PhotoItem::image()
{
    return d->m_image;
}

const QImage & PhotoItem::image() const
{
    return d->m_image;
}

void PhotoItem::setImage(const QImage & image)
{
    qDebug() << "setImage();";
    if (image.isNull())
        return;
    PhotoLayoutsEditor::instance()->beginUndoCommandGroup(i18n("Image change"));
    PLE_PostUndoCommand(new PhotoItemPixmapChangeCommand(image, this));
    if (this->cropShape().isEmpty())
        this->setCropShape( m_image_path );
    PLE_PostUndoCommand(new PhotoItemImagePathChangeCommand(this));
    PhotoLayoutsEditor::instance()->endUndoCommandGroup();
}

void PhotoItem::imageLoaded(const KUrl & url, const QImage & image)
{
    if (image.isNull())
        return;

    PhotoLayoutsEditor::instance()->beginUndoCommandGroup(i18n("Image change"));
    PLE_PostUndoCommand(new PhotoItemPixmapChangeCommand(image, this));
    if (this->cropShape().isEmpty())
        this->setCropShape( m_image_path );
    PLE_PostUndoCommand(new PhotoItemImagePathChangeCommand(this));
    PLE_PostUndoCommand(new PhotoItemUrlChangeCommand(url, this));
    PhotoLayoutsEditor::instance()->endUndoCommandGroup();
}

void PhotoItem::setImageUrl(const KUrl & url)
{
    ImageLoadingThread * ilt = new ImageLoadingThread(this);
    ilt->setImagesUrls(url);
    connect(ilt, SIGNAL(imageLoaded(KUrl,QImage)), this, SLOT(imageLoaded(KUrl,QImage)));
    ilt->start();
}

void PhotoItem::updateIcon()
{
    QPixmap temp(m_pixmap.size());
    if (m_pixmap.isNull())
        temp = QPixmap(48,48);
    temp.fill(Qt::transparent);

    QPainter p(&temp);
    if (!m_pixmap.isNull())
    {
        p.fillPath(itemOpaqueArea(), QBrush(this->m_pixmap));
        p.end();
        temp = temp.scaled(48,48,Qt::KeepAspectRatio);
        p.begin(&temp);
    }
    QPen pen(Qt::gray,1);
    pen.setCosmetic(true);
    p.setPen(pen);
    p.drawRect( QRect(QPoint(0,0), temp.size()-QSize(1,1)) );
    p.end();
    this->setIcon(QIcon(temp));
}

void PhotoItem::fitToRect(const QRect & rect)
{
    // Scaling if to big
    QSize s = d->image().size();
    QRect r = d->image().rect();
    if (rect.isValid() && (rect.width()<s.width() || rect.height()<s.height()))
    {
        s.scale(rect.size()*0.8, Qt::KeepAspectRatio);
        r.setSize(s);
    }

    QPainterPath p;
    p.addRect(r);
    m_image_path = p;
    this->m_image_path = m_image_path.simplified();
    this->recalcShape();

    // Create effective pixmap
    this->refresh();
}

void PhotoItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    if (!m_pixmap.isNull())
    {
        painter->save();
        QBrush b(this->m_pixmap);
        painter->fillPath(itemOpaqueArea(), b);
        painter->restore();
    }
    AbstractPhoto::paint(painter, option, widget);

    // Highlight item
    if (m_highlight)
    {
        painter->fillPath(this->shape(), QColor(255,0,0,100));
    }
}

void PhotoItem::refreshItem()
{
    if (d->image().isNull())
        return;
    this->m_pixmap = QPixmap::fromImage(effectsGroup()->apply( d->image().scaled(this->m_image_path.boundingRect().size().toSize(),
                                                                     Qt::KeepAspectRatioByExpanding,
                                                                     Qt::SmoothTransformation))
                                        );

    this->updateIcon();
    this->recalcShape();
    this->update();
}

QtAbstractPropertyBrowser * PhotoItem::propertyBrowser()
{
    return 0; /// TODO
}

void PhotoItem::setupItem(const QImage & image)
{
    if (image.isNull())
        return;

    d->setImage(image);

    // Scaling if to big
    if (scene())
        fitToRect(scene()->sceneRect().toRect());
    else
        fitToRect(image.rect());

    // Create effective pixmap
    this->refresh();

    this->setFlag(QGraphicsItem::ItemIsSelectable);
}

void PhotoItem::recalcShape()
{
    m_complete_path = m_image_path;
}

bool PhotoItem::highlightItem()
{
    return m_highlight;
}

void PhotoItem::setHighlightItem(bool isHighlighted)
{
    if (m_highlight == isHighlighted)
        return;
    m_highlight = isHighlighted;
    this->update();
}
