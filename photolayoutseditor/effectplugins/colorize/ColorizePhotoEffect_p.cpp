#include "ColorizePhotoEffect_p.h"
#include "ColorizePhotoEffect.h"

#include <klocalizedstring.h>

QColor ColorizePhotoEffect::m_last_color = QColor(255,255,255,0);

ColorizePhotoEffect::ColorizePhotoEffect(ColorizePhotoEffectFactory * factory, QObject * parent) :
    AbstractPhotoEffectInterface(factory, parent)
{
    AbstractPhotoEffectProperty  * color = new AbstractPhotoEffectProperty(COLOR_PROPERTY);
    color->value = m_last_color;
    m_properties.push_back(color);
}

QImage ColorizePhotoEffect::apply(const QImage & image) const
{
    QColor tempColor = color();
    if (!strength() || !tempColor.alpha())
        return image;
    QImage result = image;
    QPainter p(&result);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.drawImage(0,0,AbstractPhotoEffectInterface::apply(colorized(image, tempColor)));
    return result;
}

QString ColorizePhotoEffect::effectName() const
{
    return factory()->effectName();
}

QString ColorizePhotoEffect::toString() const
{
    return factory()->effectName() + " [" + color().name() + "]";
}

ColorizePhotoEffect::operator QString() const
{
    return toString();
}
