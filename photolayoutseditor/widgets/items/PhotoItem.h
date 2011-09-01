#ifndef PHOTOITEM_H
#define PHOTOITEM_H

#include "AbstractPhoto.h"

#include <kurl.h>

namespace KIPIPhotoLayoutsEditor
{
    class PhotoItemPrivate;
    class PhotoItemPixmapChangeCommand;
    class PhotoItemUrlChangeCommand;
    class PhotoItemImagePathChangeCommand;

    class PhotoItem : public AbstractPhoto
    {
            Q_OBJECT

        public:

            PhotoItem(const QImage & photo, const QString & name = QString(), Scene * scene = 0);
            static PhotoItem * fromUrl(const KUrl & imageUrl, Scene * scene = 0);
            virtual ~PhotoItem();

            /// Convert photo item to SVG format
            virtual QDomElement toSvg(QDomDocument & document) const;

            /// Create Photo item from SVG format code
            static PhotoItem * fromSvg(QDomElement & element);

            /// Pixmap data
            Q_PROPERTY(QPixmap m_pixmap_original READ pixmap WRITE setPixmap)
            QPixmap & pixmap();
            const QPixmap & pixmap() const;
            void setPixmap(const QPixmap & pixmap);

            /// Pixmap and pixmap's url
            void setImageUrl(const KUrl & url);

            /// Scales image to fit scenes rect
            void fitToRect(const QRect & rect);

            /// Reimplemented from QGraphicsItem
            virtual bool contains(const QPointF & point) const
            {
                return m_complete_path.contains(point);
            }

            /// Reimplemented from AbstractPhoto
            virtual QPainterPath itemShape() const
            {
                if (this->cropShape().isEmpty())
                    return m_image_path;
                else
                    return m_image_path & this->cropShape();
            }

            /// Reimplemented from AbstractPhoto
            virtual QPainterPath itemOpaqueArea() const
            {
                if (this->cropShape().isEmpty())
                    return m_image_path;
                else
                    return m_image_path & this->cropShape();
            }

            /// Reimplemented from AbstractPhoto
            virtual QPainterPath itemDrawArea() const
            {
                return m_image_path;
            }

            /// Returns item's property browser
            virtual QtAbstractPropertyBrowser * propertyBrowser();

        protected:

            PhotoItem(const QString & name = QString(), Scene * scene = 0);

            /// Converts item data to SVG format
            virtual QDomElement svgVisibleArea(QDomDocument & document) const;

            virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
            virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
            virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
            virtual void dropEvent(QGraphicsSceneDragDropEvent * event);

            /// Updates item icon
            virtual void updateIcon();

            /// Reimplemented from AbstractPhoto
            void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

        private:

            // Refreshes items data
            virtual void refreshItem();

            // Setups items
            void setupItem(const QPixmap & photo);

            // Recalculates item shape
            void recalcShape();

            // Highlight item
            Q_PROPERTY(bool m_highlight READ highlightItem WRITE setHighlightItem)
            bool highlightItem();
            void setHighlightItem(bool isHighlighted);
            bool m_highlight;

            PhotoItemPrivate * d;

            QPixmap m_pixmap;

            // Widget path
            QPainterPath m_complete_path;
            QPainterPath m_image_path;

        friend class Scene;
        friend class PhotoItemPrivate;
        friend class PhotoItemPixmapChangeCommand;
        friend class PhotoItemUrlChangeCommand;
        friend class PhotoItemImagePathChangeCommand;
    };
}

#endif // PHOTOITEM_H
