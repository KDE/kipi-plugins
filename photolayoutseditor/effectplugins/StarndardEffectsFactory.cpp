#include "StarndardEffectsFactory.h"

#include "BlurPhotoEffect.h"
#include "ColorizePhotoEffect.h"
#include "GrayscalePhotoEffect.h"
#include "SepiaPhotoEffect.h"

using namespace KIPIPhotoLayoutsEditor;

StarndardEffectsFactory::StarndardEffectsFactory(QObject * parent) :
    AbstractPhotoEffectFactory(parent)
{}

AbstractPhotoEffectInterface * StarndardEffectsFactory::getEffectInstance(const QString & name)
{
    if (name == i18n("Blur effect"))
        return new BlurPhotoEffect(this);
    if (name == i18n("Colorize effect"))
        return new ColorizePhotoEffect(this);
    if (name == i18n("Grayscale effect"))
        return new GrayscalePhotoEffect(this);
    if (name == i18n("Sepia effect"))
        return new SepiaPhotoEffect(this);
    return 0;
}

QString StarndardEffectsFactory::effectName() const
{
    return i18n("Blur effect") + QString(";") +
            i18n("Colorize effect") + QString(";") +
            i18n("Grayscale effect") + QString(";") +
            i18n("Sepia effect");
}
