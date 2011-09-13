#ifndef ABSTRACTPHOTO_P_H
#define ABSTRACTPHOTO_P_H

#include <QIcon>
#include <QPainterPath>

namespace KIPIPhotoLayoutsEditor
{
    class BordersGroup;
    class PhotoEffectsGroup;

    class AbstractPhoto;
    class AbstractPhotoPrivate
    {
        AbstractPhoto * m_item;

        AbstractPhotoPrivate(AbstractPhoto * item);

        // Crop shape
        void setCropShape(const QPainterPath & cropShape);
        QPainterPath & cropShape();
        QPainterPath m_crop_shape;

        void setName(const QString & name);
        QString name();
        QString m_name;

        // For loading purpose only
        bool m_visible;
        QPointF m_pos;
        QTransform m_transform;

        mutable QString m_id;
        PhotoEffectsGroup * m_effects_group;
        BordersGroup * m_borders_group;

        // Icon object
        QIcon m_icon;

        friend class AbstractPhoto;
        friend class AbstractPhotoItemLoader;
        friend class CropShapeChangeCommand;
        friend class ItemNameChangeCommand;
    };
}

#endif // ABSTRACTPHOTO_P_H
