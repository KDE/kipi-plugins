#ifndef COLORIZEPHOTOEFFECT_H
#define COLORIZEPHOTOEFFECT_H

#include "ColorizePhotoEffect_global.h"
#include "AbstractPhotoEffectFactory.h"

using namespace KIPIPhotoLayoutsEditor;

class COLORIZEPHOTOEFFECTSHARED_EXPORT ColorizePhotoEffectFactory : public AbstractPhotoEffectFactory
{
        Q_OBJECT
        Q_INTERFACES(KIPIPhotoLayoutsEditor::AbstractPhotoEffectFactory)

    public:

        ColorizePhotoEffectFactory(QObject * parent, const QVariantList& args);
        virtual AbstractPhotoEffectInterface * getEffectInstance();
        virtual QString effectName() const;

    protected:

        virtual void writeToSvg(AbstractPhotoEffectInterface * effect, QDomElement & effectElement);
        virtual AbstractPhotoEffectInterface * readFromSvg(QDomElement & element);
};

#endif // COLORIZEPHOTOEFFECT_H
