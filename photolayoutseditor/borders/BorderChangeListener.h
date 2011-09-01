#ifndef BORDERCHANGELISTENERS_H
#define BORDERCHANGELISTENERS_H

#include <QObject>
#include <QtProperty>
#include "BorderDrawerInterface.h"

namespace KIPIPhotoLayoutsEditor
{
    class BorderChangeCommand;
    class BorderChangeListener : public QObject
    {
            Q_OBJECT

            BorderDrawerInterface * drawer;
            BorderChangeCommand * command;
            bool createCommands;

        public:

            explicit BorderChangeListener(BorderDrawerInterface * drawer, QObject * parent, bool createCommands);

        public slots:

            void propertyChanged(QtProperty * property);
            void editingFinished();
    };
}

#endif // BORDERCHANGELISTENERS_H
