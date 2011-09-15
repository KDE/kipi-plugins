#include "AbstractPhoto_p.h"
#include "AbstractPhoto.h"

using namespace KIPIPhotoLayoutsEditor;

AbstractPhotoPrivate::AbstractPhotoPrivate(AbstractPhoto * item) :
    m_item(item),
    m_visible(true)
{}

void AbstractPhotoPrivate::setCropShape(const QPainterPath & cropShape)
{
    m_crop_shape = cropShape;
    m_item->refresh();
}

QPainterPath & AbstractPhotoPrivate::cropShape()
{
    return m_crop_shape;
}

void AbstractPhotoPrivate::setName(const QString & name)
{
    if (name.isEmpty())
        return;
    this->m_name = name;
}

QString AbstractPhotoPrivate::name()
{
    return this->m_name;
}
