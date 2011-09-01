#include "PhotoItem.h"
#include "PhotoEffectsGroup.h"
#include "PhotoEffectsLoader.h"
#include "ImageFileDialog.h"
#include "BordersGroup.h"
#include "global.h"
#include "PLEConfigSkeleton.h"
#include "photolayoutseditor.h"

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

class KIPIPhotoLayoutsEditor::PhotoItemPrivate
{
    PhotoItemPrivate(PhotoItem * item) :
        m_item(item)
    {}

    static QString locateFile(const QString & filePath);

    PhotoItem * m_item;

    // Pixmap
    void setPixmap(const QPixmap & pixmap);
    inline QPixmap & pixmap();
    QPixmap m_pixmap_original;

    // Pixmap's url
    void setFileUrl(const KUrl & url);
    inline KUrl & fileUrl();
    KUrl m_file_path;

    friend class PhotoItem;
    friend class PhotoItemPixmapChangeCommand;
    friend class PhotoItemUrlChangeCommand;
};

class KIPIPhotoLayoutsEditor::PhotoItemPixmapChangeCommand : public QUndoCommand
{
    QPixmap m_pixmap;
    PhotoItem * m_item;
public:
    PhotoItemPixmapChangeCommand(const QImage & image, PhotoItem * item, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Image change"), parent),
        m_pixmap(QPixmap::fromImage(image)),
        m_item(item)
    {}
    PhotoItemPixmapChangeCommand(const QPixmap & pixmap, PhotoItem * item, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Image change"), parent),
        m_pixmap(pixmap),
        m_item(item)
    {}
    virtual void redo()
    {
        QPixmap temp = m_item->pixmap();
        m_item->d->setPixmap(m_pixmap);
        m_pixmap = temp;
    }
    virtual void undo()
    {
        QPixmap temp = m_item->pixmap();
        m_item->d->setPixmap(m_pixmap);
        m_pixmap = temp;
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

QString PhotoItemPrivate::locateFile(const QString & filePath)
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
void PhotoItemPrivate::setPixmap(const QPixmap & pixmap)
{
    if (pixmap.isNull() || &pixmap == &m_pixmap_original)
        return;
    m_pixmap_original = pixmap;
    m_item->refresh();
}
QPixmap & PhotoItemPrivate::pixmap()
{
    return m_pixmap_original;
}
void PhotoItemPrivate::setFileUrl(const KUrl & url)
{
    this->m_file_path = url;
}
KUrl & PhotoItemPrivate::fileUrl()
{
    return this->m_file_path;
}

PhotoItem::PhotoItem(const QImage & photo, const QString & name, Scene * scene) :
    AbstractPhoto((name.isEmpty() ? i18n("New image") : name), scene),
    d(new PhotoItemPrivate(this))
{
    this->setHighlightItem(false);
    this->setupItem(QPixmap::fromImage(photo));
}

PhotoItem * PhotoItem::fromUrl(const KUrl & imageUrl, Scene * scene)
{
    QImage img;
    if (PhotoLayoutsEditor::instance()->hasInterface())
    {
        KIPI::ImageInfo info = PhotoLayoutsEditor::instance()->interface()->info(imageUrl);
        QImageReader ir (info.path().toLocalFile());
        if (!ir.read(&img))
            return 0;
    }
    else if (imageUrl.isValid())
    {
        QImageReader ir (imageUrl.toLocalFile());
        if (!ir.read(&img))
            return 0;
    }

    if (img.isNull())
        return 0;

    PhotoItem * result = new PhotoItem(img, imageUrl.fileName(), scene);
    result->d->setFileUrl(imageUrl);
    return result;
}

PhotoItem::PhotoItem(const QString & name, Scene * scene) :
    AbstractPhoto((name.isEmpty() ? i18n("New image") : name), scene),
    d(new PhotoItemPrivate(this))
{
    this->setHighlightItem(false);
    this->setupItem(QPixmap());
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

    if ( (PLEConfigSkeleton::embedImagesData() && !d->pixmap().isNull()) || !d->fileUrl().isValid())
    {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        d->pixmap().save(&buffer, "PNG");
        image.appendChild( document.createTextNode( QString(byteArray.toBase64()) ) );
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
        item->d->setPixmap(QPixmap::fromImage(img));

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

QPixmap & PhotoItem::pixmap()
{
    return d->m_pixmap_original;
}

const QPixmap & PhotoItem::pixmap() const
{
    return d->m_pixmap_original;
}

void PhotoItem::setPixmap(const QPixmap & pixmap)
{
    if (pixmap.isNull())
        return;
    PhotoLayoutsEditor::instance()->beginUndoCommandGroup(i18n("Image change"));
    PLE_PostUndoCommand(new PhotoItemPixmapChangeCommand(pixmap, this));
    if (this->cropShape().isEmpty())
        this->setCropShape( m_image_path );
    PLE_PostUndoCommand(new PhotoItemImagePathChangeCommand(this));
    PhotoLayoutsEditor::instance()->endUndoCommandGroup();
}

void PhotoItem::setImageUrl(const KUrl & url)
{
    QImage img;
    if (PhotoLayoutsEditor::instance()->hasInterface())
    {
        KIPI::ImageInfo info = PhotoLayoutsEditor::instance()->interface()->info(url);
        QImageReader ir (info.path().toLocalFile());
        if (ir.read(&img))
        {
            PhotoLayoutsEditor::instance()->beginUndoCommandGroup(i18n("Image change"));
            this->setPixmap( QPixmap::fromImage(img) );
            PLE_PostUndoCommand(new PhotoItemUrlChangeCommand(info.path(), this));
            PhotoLayoutsEditor::instance()->endUndoCommandGroup();
        }
    }
    else if (url.isValid())
    {
        QImageReader ir (url.toLocalFile());
        if (ir.read(&img))
        {
            PhotoLayoutsEditor::instance()->beginUndoCommandGroup(i18n("Image change"));
            this->setPixmap( QPixmap::fromImage(img) );
            PLE_PostUndoCommand(new PhotoItemUrlChangeCommand(url, this));
            PhotoLayoutsEditor::instance()->endUndoCommandGroup();
        }
    }
}

void PhotoItem::updateIcon()
{
    QPixmap temp(m_pixmap.size());
    temp.fill(Qt::transparent);
    QPainter p(&temp);
    p.fillPath(itemOpaqueArea(), QBrush(this->m_pixmap));
    p.end();
    temp = temp.scaled(48,48,Qt::KeepAspectRatio);
    p.begin(&temp);
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
    QSize s = d->pixmap().size();
    QRect r = d->pixmap().rect();
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
    if (d->pixmap().isNull())
        return;
    this->m_pixmap = effectsGroup()->apply( d->pixmap().scaled(this->m_image_path.boundingRect().size().toSize(),
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

void PhotoItem::setupItem(const QPixmap & photo)
{
    if (photo.isNull())
        return;

    d->setPixmap(photo);

    // Scaling if to big
    if (scene())
        fitToRect(scene()->sceneRect().toRect());
    else
        fitToRect(photo.rect());

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
