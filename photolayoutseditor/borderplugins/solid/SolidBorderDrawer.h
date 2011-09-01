#ifndef SOLIDBORDERDRAWER_H
#define SOLIDBORDERDRAWER_H

#include "SolidBorderDrawer_global.h"
#include "BorderDrawerFactoryInterface.h"

#include <QVariantList>

using namespace KIPIPhotoLayoutsEditor;

class SOLIDBORDERDRAWERSHARED_EXPORT SolidBorderDrawerFactory : public BorderDrawerFactoryInterface
{
        Q_OBJECT
        Q_INTERFACES(KIPIPhotoLayoutsEditor::BorderDrawerFactoryInterface)

    public:

        SolidBorderDrawerFactory(QObject * parent, const QVariantList&);

        virtual QString drawerName() const;

        virtual BorderDrawerInterface * getDrawerInstance(QObject * parent = 0);
};

#endif // SOLIDBORDERDRAWER_H
