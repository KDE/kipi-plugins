#ifndef PHOTOEFFECTCHANGELISTENER_H
#define PHOTOEFFECTCHANGELISTENER_H

#include <QObject>
#include <QtProperty>

#include "AbstractPhotoEffectInterface.h"

namespace KIPIPhotoLayoutsEditor
{
    class PhotoEffectChangeCommand;
    class PhotoEffectChangeListener : public QObject
    {
            Q_OBJECT

            AbstractPhotoEffectInterface * effect;
            PhotoEffectChangeCommand * command;
            bool createCommands;

        public:

            explicit PhotoEffectChangeListener(AbstractPhotoEffectInterface * effect, QObject * parent, bool createCommands);

        public slots:

            void propertyChanged(QtProperty * property);
            void editingFinished();
    };
}

#endif // PHOTOEFFECTCHANGELISTENER_H
