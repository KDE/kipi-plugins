#include "StarndardEffectsFactory.h"

#include "BlurPhotoEffect.h"

using namespace KIPIPhotoLayoutsEditor;

StarndardEffectsFactory::StarndardEffectsFactory(QObject * parent) :
    AbstractPhotoEffectFactory(parent)
{}

AbstractPhotoEffectInterface * StarndardEffectsFactory::getEffectInstance(const QString & name)
{
    if (name == i18n("Blur effect"))
        return new BlurPhotoEffect(this, this);
    return 0;
}

QString StarndardEffectsFactory::effectName() const
{
    return i18n("Blur effect") + QString(";");
}
