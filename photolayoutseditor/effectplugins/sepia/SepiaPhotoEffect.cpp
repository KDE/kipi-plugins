#include "SepiaPhotoEffect.h"
#include "SepiaPhotoEffect_p.h"

#include <kpluginfactory.h>
#include <klocalizedstring.h>

K_PLUGIN_FACTORY( SepiaPhotoEffectFactoryLoader, registerPlugin<SepiaPhotoEffectFactory>(); )
K_EXPORT_PLUGIN ( SepiaPhotoEffectFactoryLoader("photolayoutseditoreffectplugin_sepia") )

SepiaPhotoEffectFactory::SepiaPhotoEffectFactory(QObject * parent, const QVariantList&) :
    AbstractPhotoEffectFactory(parent)
{
}

AbstractPhotoEffectInterface * SepiaPhotoEffectFactory::getEffectInstance()
{
    return new SepiaPhotoEffect(this, this);
}

QString SepiaPhotoEffectFactory::effectName() const
{
    return i18n("Sepia effect");
}

void SepiaPhotoEffectFactory::writeToSvg(AbstractPhotoEffectInterface * /*effect*/, QDomElement & /*effectElement*/)
{
}

AbstractPhotoEffectInterface * SepiaPhotoEffectFactory::readFromSvg(QDomElement & /*element*/)
{
    SepiaPhotoEffect * effect = (SepiaPhotoEffect*) this->getEffectInstance();
    return effect;
}

