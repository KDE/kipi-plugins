#ifndef BORDERDRAWERFACTORY_H
#define BORDERDRAWERFACTORY_H

#include <QObject>

namespace KIPIPhotoLayoutsEditor
{
    class BorderDrawerInterface;
    class BorderDrawerFactoryInterface : public QObject
    {
        public:

            explicit BorderDrawerFactoryInterface(QObject * parent = 0) :
                QObject(parent)
            {}
            virtual ~BorderDrawerFactoryInterface()
            {}

            virtual QString drawerName() const = 0;

            virtual BorderDrawerInterface * getDrawerInstance(QObject * parent = 0) = 0;
    };
}

Q_DECLARE_INTERFACE(KIPIPhotoLayoutsEditor::BorderDrawerFactoryInterface,"pl.coder89.pfe.BorderDrawerFactoryInterface/1.0")

#endif // BORDERDRAWERFACTORY_H
