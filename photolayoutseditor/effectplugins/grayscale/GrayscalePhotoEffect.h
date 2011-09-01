#ifndef GRAYSCALEPHOTOEFFECT_H
#define GRAYSCALEPHOTOEFFECT_H

#include "GrayscalePhotoEffect_global.h"
#include "AbstractPhotoEffectFactory.h"

using namespace KIPIPhotoLayoutsEditor;

class GRAYSCALEPHOTOEFFECTSHARED_EXPORT GrayscalePhotoEffectFactory : public AbstractPhotoEffectFactory
{
        Q_OBJECT
        Q_INTERFACES(KIPIPhotoLayoutsEditor::AbstractPhotoEffectFactory)

    public:

        GrayscalePhotoEffectFactory(QObject * parent, const QVariantList&);
        virtual AbstractPhotoEffectInterface * getEffectInstance();
        virtual QString effectName() const;

    protected:

        virtual void writeToSvg(AbstractPhotoEffectInterface * effect, QDomElement & effectElement);
        virtual AbstractPhotoEffectInterface * readFromSvg(QDomElement & element);
};

#endif // GRAYSCALEPHOTOEFFECT_H
