#include "GrayscalePhotoEffect_p.h"
#include "GrayscalePhotoEffect.h"

#include <klocalizedstring.h>

GrayscalePhotoEffect::GrayscalePhotoEffect(GrayscalePhotoEffectFactory * factory, QObject * parent) :
    AbstractPhotoEffectInterface(factory, parent)
{
}

QImage GrayscalePhotoEffect::apply(const QImage & image) const
{
    if (!strength())
        return image;
    QImage result = image;
    QPainter p(&result);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.drawImage(0,0,AbstractPhotoEffectInterface::apply(greyscaled(image)));
    return result;
}

QString GrayscalePhotoEffect::effectName() const
{
    return factory()->effectName();
}

QString GrayscalePhotoEffect::toString() const
{
    return effectName();
}

GrayscalePhotoEffect::operator QString() const
{
    return toString();
}
