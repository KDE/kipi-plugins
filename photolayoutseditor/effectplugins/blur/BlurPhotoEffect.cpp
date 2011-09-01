#include "BlurPhotoEffect.h"
#include "BlurPhotoEffect_p.h"

#include <kpluginfactory.h>
#include <klocalizedstring.h>

K_PLUGIN_FACTORY( BlurPhotoEffectFactoryLoader, registerPlugin<BlurPhotoEffectFactory>(); )
K_EXPORT_PLUGIN ( BlurPhotoEffectFactoryLoader("photolayoutseditoreffectplugin_blur") )

BlurPhotoEffectFactory::BlurPhotoEffectFactory(QObject * parent, const QVariantList&) :
    AbstractPhotoEffectFactory(parent)
{
}

AbstractPhotoEffectInterface * BlurPhotoEffectFactory::getEffectInstance()
{
    return new BlurPhotoEffect(this, this);
}

QString BlurPhotoEffectFactory::effectName() const
{
    return i18n("Blur effect");
}

void BlurPhotoEffectFactory::writeToSvg(AbstractPhotoEffectInterface * effect, QDomElement & effectElement)
{
    BlurPhotoEffect * blurEffect = dynamic_cast<BlurPhotoEffect*>(effect);
    if (blurEffect)
        effectElement.setAttribute(RADIUS_PROPERTY, blurEffect->radius());
}

AbstractPhotoEffectInterface * BlurPhotoEffectFactory::readFromSvg(QDomElement & element)
{
    BlurPhotoEffect * effect = (BlurPhotoEffect*) this->getEffectInstance();
    effect->setRadius( element.attribute(RADIUS_PROPERTY).toInt() );
    return effect;
}

