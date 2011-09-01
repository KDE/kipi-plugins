#include "GrayscalePhotoEffect.h"
#include "GrayscalePhotoEffect_p.h"

#include <kpluginfactory.h>
#include <klocalizedstring.h>

K_PLUGIN_FACTORY( GrayscalePhotoEffectFactoryLoader, registerPlugin<GrayscalePhotoEffectFactory>(); )
K_EXPORT_PLUGIN ( GrayscalePhotoEffectFactoryLoader("photolayoutseditoreffectplugin_grayscale") )

GrayscalePhotoEffectFactory::GrayscalePhotoEffectFactory(QObject * parent, const QVariantList&) :
    AbstractPhotoEffectFactory(parent)
{
}

AbstractPhotoEffectInterface * GrayscalePhotoEffectFactory::getEffectInstance()
{
    return new GrayscalePhotoEffect(this, this);
}

QString GrayscalePhotoEffectFactory::effectName() const
{
    return i18n("Grayscale effect");
}

void GrayscalePhotoEffectFactory::writeToSvg(AbstractPhotoEffectInterface * /*effect*/, QDomElement & /*effectElement*/)
{}

AbstractPhotoEffectInterface * GrayscalePhotoEffectFactory::readFromSvg(QDomElement & /*element*/)
{
    GrayscalePhotoEffect * effect = (GrayscalePhotoEffect*) this->getEffectInstance();
    return effect;
}
