#include "SepiaPhotoEffect_p.h"
#include "SepiaPhotoEffect.h"

#include <klocalizedstring.h>

SepiaPhotoEffect::SepiaPhotoEffect(SepiaPhotoEffectFactory * factory, QObject * parent) :
    AbstractPhotoEffectInterface(factory, parent)
{
}

QImage SepiaPhotoEffect::apply(const QImage & image) const
{
    if (!strength())
        return image;
    QImage result = image;
    QPainter p(&result);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.drawImage(0,0,AbstractPhotoEffectInterface::apply(sepia_converted(image)));
    return result;
}

QString SepiaPhotoEffect::effectName() const
{
    return factory()->effectName();
}

QString SepiaPhotoEffect::toString() const
{
    return effectName();
}

SepiaPhotoEffect::operator QString() const
{
    return toString();
}
