#ifndef SEPIAPHOTOEFFECT_H
#define SEPIAPHOTOEFFECT_H

#include "SepiaPhotoEffect_global.h"
#include "AbstractPhotoEffectFactory.h"

using namespace KIPIPhotoLayoutsEditor;

class SEPIAPHOTOEFFECTSHARED_EXPORT SepiaPhotoEffectFactory : public AbstractPhotoEffectFactory
{
        Q_OBJECT
        Q_INTERFACES(KIPIPhotoLayoutsEditor::AbstractPhotoEffectFactory)

    public:

        SepiaPhotoEffectFactory(QObject * parent, const QVariantList&);
        virtual AbstractPhotoEffectInterface * getEffectInstance();
        virtual QString effectName() const;

    protected:

        virtual void writeToSvg(AbstractPhotoEffectInterface * effect, QDomElement & effectElement);
        virtual AbstractPhotoEffectInterface * readFromSvg(QDomElement & element);
};

#endif // SEPIAPHOTOEFFECT_H
