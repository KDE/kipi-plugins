#ifndef STARNDARDEFFECTSFACTORY_H
#define STARNDARDEFFECTSFACTORY_H

#include "AbstractPhotoEffectFactory.h"

namespace KIPIPhotoLayoutsEditor
{
    class StarndardEffectsFactory : public AbstractPhotoEffectFactory
    {
        public:

            explicit StarndardEffectsFactory(QObject * parent = 0);
            virtual ~StarndardEffectsFactory()
            {}

            virtual AbstractPhotoEffectInterface * getEffectInstance(const QString & name = QString());

            virtual QString effectName() const;
    };
}

#endif // STARNDARDEFFECTSFACTORY_H
