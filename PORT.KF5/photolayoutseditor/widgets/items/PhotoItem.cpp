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

#include "PhotoItem.moc"

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

#include "PhotoEffectsGroup.h"
#include "PhotoEffectsLoader.h"
#include "ImageFileDialog.h"
#include "BordersGroup.h"
#include "global.h"
#include "PLEConfigSkeleton.h"
#include "photolayoutseditor.h"
#include "ImageLoadingThread.h"
#include "ProgressEvent.h"

#define EMPTY_FILL_COLOR QColor(255, 0, 0, 120)

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::PhotoItemPixmapChangeCommand : public QUndoCommand
{
    QImage     m_image;
    PhotoItem* m_item;

public:

    PhotoItemPixmapChangeCommand(const QImage & image, PhotoItem * item, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Image Change"), parent),
        m_image(image),
        m_item(item)
    {}

    PhotoItemPixmapChangeCommand(const QPixmap & pixmap, PhotoItem * item, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Image Change"), parent),
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
    KUrl       m_url;
    PhotoItem* m_item;

public:

    PhotoItemUrlChangeCommand(const KUrl & url, PhotoItem * item, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Image Path Change"), parent),
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
    PhotoItem*              m_item;
    QPainterPath            m_image_path;
//    CropShapeChangeCommand* command;

public:

    PhotoItemImagePathChangeCommand(PhotoItem * item, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Image Shape Change"), parent),
        m_item(item),
        m_image_path(m_item->m_image_path)
//        , command(0)
    {}
    virtual void redo()
    {
        m_item->m_image_path = QPainterPath();
        m_item->m_image_path.addRect(m_item->m_temp_image.rect());
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

class KIPIPhotoLayoutsEditor::PhotoItemImageMovedCommand : public QUndoCommand
{
    PhotoItem* m_item;
    QPointF    translation;
    bool       done;

    static PhotoItemImageMovedCommand* m_instance;

    PhotoItemImageMovedCommand(PhotoItem * item, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Image Position Change"), parent),
        m_item(item),
        done(true)
    {}

public:

    static PhotoItemImageMovedCommand * instance(PhotoItem * item)
    {
        if (!m_instance)
            m_instance = new PhotoItemImageMovedCommand(item);
        return m_instance;
    }

    void translate(const QPointF & translation)
    {
        this->translation += translation;
    }

    virtual void redo()
    {
        if (done)
            return;
        m_item->d->m_brush_transform.translate(translation.x(), translation.y());
        m_item->d->m_complete_path_transform.translate(translation.x(), translation.y());
        m_item->m_complete_path.translate(translation);
        m_item->update();
        done = !done;
    }

    virtual void undo()
    {
        if (!done)
            return;
        m_item->d->m_brush_transform.translate(-translation.x(), -translation.y());
        m_item->d->m_complete_path_transform.translate(-translation.x(), -translation.y());
        m_item->m_complete_path.translate(-translation);
        m_item->update();
        done = !done;
    }

    static void post()
    {
        if (!m_instance)
            return;
        PLE_PostUndoCommand(m_instance);
        m_instance = 0;
    }
};

PhotoItemImageMovedCommand * PhotoItemImageMovedCommand::m_instance = 0;

QString PhotoItem::PhotoItemPrivate::locateFile(const QString & filePath)
{
    QString resultPath = filePath;
    if (!resultPath.isEmpty())
    {
        // Try to open existing file
        if (!QFile::exists(resultPath))
        {
            int result = KMessageBox::questionYesNo(0,
                                                    i18n("Can't find image file in this location:"
                                                         "\n%1"
                                                         "\n"
                                                         "\nWould you like to set new location of this file?"
                                                         "\nIf not this image will be removed from the composition.", resultPath),
                                                    i18n("File reading error"));
            if (result != KMessageBox::Yes)
                resultPath.clear();
            else
            {
                KUrl fileUrl(filePath);
                ImageFileDialog dialog(fileUrl);
                result = dialog.exec();
                resultPath = dialog.selectedFile();
                if (result != ImageFileDialog::Accepted || !QFile::exists(resultPath))
                    resultPath.clear();
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

PhotoItem::PhotoItem(const QPainterPath & shape, const QString & name, Scene * scene) :
    AbstractPhoto((name.isEmpty() ? i18n("New image") : name), scene),
    m_highlight(false),
    d(new PhotoItemPrivate(this))
{
    m_image_path = shape;
    this->refresh();
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

QDomDocument PhotoItem::toSvg() const
{
    QDomDocument document = AbstractPhoto::toSvg();
    QDomElement itemElement = document.firstChildElement();
    itemElement.setAttribute("class", "PhotoItem");

    // 'defs' tag
    QDomElement defs = document.createElement("defs");
    defs.setAttribute("class", "data");
    itemElement.appendChild(defs);

    // 'defs'-> ple:'data'
    QDomElement appNS = document.createElementNS(KIPIPhotoLayoutsEditor::uri(), "data");
    appNS.setPrefix(KIPIPhotoLayoutsEditor::name());
    defs.appendChild(appNS);

    if (!m_image_path.isEmpty())
    {
        // 'defs'-> ple:'data' ->'path'
        QDomDocument document = KIPIPhotoLayoutsEditor::pathToSvg(m_image_path);
        QDomElement path = document.firstChildElement("path");
        path.setAttribute("class", "m_image_path");
        path.setPrefix(KIPIPhotoLayoutsEditor::name());
        appNS.appendChild(document.documentElement());
    }

    // 'defs'-> ple:'data' ->'transform'
    QDomElement transform = document.createElement("transform");
    transform.setPrefix(KIPIPhotoLayoutsEditor::name());
    QString matrix = "matrix("+
                     QString::number(this->transform().m11())+
                     ','+
                     QString::number(this->transform().m12())+
                     ','+
                     QString::number(this->transform().m21())+
                     ','+
                     QString::number(this->transform().m22())+
                     ','+
                     QString::number(this->transform().m31())+
                     ','+
                     QString::number(this->transform().m32())+
                     ')';
    transform.setAttribute("matrix", matrix);
    appNS.appendChild(transform);

    if (!this->isEmpty())
    {
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
    }
    else
    {
        itemElement.setAttribute("visibility", "hidden");
    }

    return document;
}

QDomDocument PhotoItem::toTemplateSvg() const
{
    QDomDocument document = AbstractPhoto::toTemplateSvg();
    QDomElement itemElement = document.firstChildElement();
    itemElement.setAttribute("class", "PhotoItem");

    // 'defs' tag
    QDomElement defs = document.createElement("defs");
    defs.setAttribute("class", "data");
    itemElement.appendChild(defs);

    // 'defs'-> ple:'data'
    QDomElement appNS = document.createElementNS(KIPIPhotoLayoutsEditor::uri(), "data");
    appNS.setPrefix(KIPIPhotoLayoutsEditor::name());
    defs.appendChild(appNS);

    if (!m_image_path.isEmpty())
    {
        // 'defs'-> ple:'data' ->'path'
        QDomDocument document = KIPIPhotoLayoutsEditor::pathToSvg(m_image_path);
        QDomElement path = document.firstChildElement("path");
        path.setAttribute("class", "m_image_path");
        path.setPrefix(KIPIPhotoLayoutsEditor::name());
        appNS.appendChild(document.documentElement());
    }

    // 'defs'-> ple:'data' ->'transform'
    QDomElement transform = document.createElement("transform");
    transform.setPrefix(KIPIPhotoLayoutsEditor::name());
    QString matrix = "matrix("+
                     QString::number(this->transform().m11())+
                     ','+
                     QString::number(this->transform().m12())+
                     ','+
                     QString::number(this->transform().m21())+
                     ','+
                     QString::number(this->transform().m22())+
                     ','+
                     QString::number(this->transform().m31())+
                     ','+
                     QString::number(this->transform().m32())+
                     ')';
    transform.setAttribute("matrix", matrix);
    appNS.appendChild(transform);

    return document;
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

QDomDocument PhotoItem::svgVisibleArea() const
{
    QDomDocument document;
    if (!this->isEmpty())
    {
        // 'defs' -> 'g'
        QDomElement g = document.createElement("g");
        document.appendChild(g);
        QTransform transform = d->m_brush_transform;
        QString translate = "translate("+
                            QString::number(this->pos().x())+
                            ','+
                            QString::number(this->pos().y())+
                            ')';
        QString matrix = "matrix("+
                         QString::number(transform.m11())+
                         ','+
                         QString::number(transform.m12())+
                         ','+
                         QString::number(transform.m21())+
                         ','+
                         QString::number(transform.m22())+
                         ','+
                         QString::number(transform.m31())+
                         ','+
                         QString::number(transform.m32())+
                         ')';
        g.setAttribute("transform", translate + ' ' + matrix);

        // 'defs' -> 'g' -> 'image'
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        m_temp_image.save(&buffer, "PNG");
        QDomElement img = document.createElement("image");
        img.setAttribute("width",m_temp_image.width());
        img.setAttribute("height",m_temp_image.height());
        img.setAttribute("xlink:href",QString("data:image/png;base64,")+byteArray.toBase64());
        g.appendChild(img);
    }
    return document;
}

QDomDocument PhotoItem::svgTemplateArea() const
{
    QDomDocument document;
    if (!this->isEmpty())
    {
        // 'defs' -> 'g'
        QDomElement g = document.createElement("g");
        document.appendChild(g);
        QTransform transform = d->m_brush_transform;
        QString translate = "translate("+
                            QString::number(this->pos().x())+
                            ','+
                            QString::number(this->pos().y())+
                            ')';
        QString matrix = "matrix("+
                         QString::number(transform.m11())+
                         ','+
                         QString::number(transform.m12())+
                         ','+
                         QString::number(transform.m21())+
                         ','+
                         QString::number(transform.m22())+
                         ','+
                         QString::number(transform.m31())+
                         ','+
                         QString::number(transform.m32())+
                         ')';
        g.setAttribute("transform", translate + ' ' + matrix);

        // 'defs' -> 'g' -> 'path'
        QDomDocument document = KIPIPhotoLayoutsEditor::pathToSvg(m_image_path);
        QDomElement path = document.firstChildElement("path");
        path.setAttribute("opacity", 100);
        path.setAttribute("fill", "#ff0000");
        g.appendChild(path);
    }
    return document;
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

void PhotoItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if ((event->modifiers() & Qt::ControlModifier) && (event->buttons() & Qt::LeftButton))
        d->m_image_moving = true;
    else
        AbstractPhoto::mousePressEvent(event);
}

void PhotoItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    event->setAccepted(false);
    if (d->m_image_moving)
    {
        if ((event->modifiers() & Qt::ControlModifier) && (event->buttons() & Qt::LeftButton))
        {
            QPointF p = event->pos() - event->lastPos();
            d->m_brush_transform.translate(p.x(), p.y());
            d->m_complete_path_transform.translate(p.x(), p.y());
            m_complete_path.translate(p);
            PhotoItemImageMovedCommand::instance(this)->translate(p);
            this->update();
        }
        else
            PhotoItemImageMovedCommand::post();
        event->setAccepted(true);
    }
    else
    {
        AbstractPhoto::mouseMoveEvent(event);
    }
}

void PhotoItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    if (d->m_image_moving)
    {
        PhotoItemImageMovedCommand::post();
        d->m_image_moving = false;
    }
    else
        AbstractPhoto::mouseReleaseEvent(event);
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
    PhotoLayoutsEditor::instance()->beginUndoCommandGroup(i18n("Image Change"));
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

    PhotoLayoutsEditor::instance()->beginUndoCommandGroup(i18n("Image Change"));
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
    QPixmap temp(m_temp_image.size());
    if (m_temp_image.isNull())
        temp = QPixmap(48,48);
    temp.fill(Qt::transparent);

    QPainter p(&temp);
    if (!m_temp_image.isNull())
    {
        p.fillPath(itemOpaqueArea(), QBrush(this->m_temp_image));
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
    painter->fillPath(itemOpaqueArea(), EMPTY_FILL_COLOR);
    if (!m_temp_image.isNull())
    {
        QBrush b(this->m_temp_image);
        b.setTransform(d->m_brush_transform);
        painter->fillPath(itemOpaqueArea() & m_complete_path, b);
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
    this->m_temp_image = effectsGroup()->apply( d->image().scaled(this->m_image_path.boundingRect().size().toSize(),
                                                              Qt::KeepAspectRatioByExpanding,
                                                              Qt::SmoothTransformation));

    this->updateIcon();
    this->recalcShape();
    this->update();
}

QtAbstractPropertyBrowser * PhotoItem::propertyBrowser()
{
    return 0; /// TODO
}

bool PhotoItem::isEmpty() const
{
    return d->m_image.isNull();
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
    d->m_brush_transform = QTransform();
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
