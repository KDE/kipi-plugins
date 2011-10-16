#ifndef STANDARDBORDERSFACTORY_H
#define STANDARDBORDERSFACTORY_H

#include "BorderDrawerFactoryInterface.h"

namespace KIPIPhotoLayoutsEditor
{
    class StandardBordersFactory : public BorderDrawerFactoryInterface
    {
        public:

            explicit StandardBordersFactory(QObject *parent = 0);

            virtual QString drawersNames() const;

            virtual BorderDrawerInterface * getDrawerInstance(const QString & name);

    };
}

#endif // STANDARDBORDERSFACTORY_H
