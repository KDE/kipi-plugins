#ifndef BLURPHOTOEFFECT_H
#define BLURPHOTOEFFECT_H

#include "BlurPhotoEffect_global.h"
#include "AbstractPhotoEffectFactory.h"

using namespace KIPIPhotoLayoutsEditor;

class BLURPHOTOEFFECTSHARED_EXPORT BlurPhotoEffectFactory : public AbstractPhotoEffectFactory
{
        Q_OBJECT
        Q_INTERFACES(KIPIPhotoLayoutsEditor::AbstractPhotoEffectFactory)

    public:

        BlurPhotoEffectFactory(QObject * parent, const QVariantList& args);
        virtual AbstractPhotoEffectInterface * getEffectInstance();
        virtual QString effectName() const;

    protected:

        virtual void writeToSvg(AbstractPhotoEffectInterface * effect, QDomElement & effectElement);
        virtual AbstractPhotoEffectInterface * readFromSvg(QDomElement & element);
};

#endif // BLURPHOTOEFFECT_H
