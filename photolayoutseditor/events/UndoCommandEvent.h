#ifndef UNDOCOMMANDEVENT_H
#define UNDOCOMMANDEVENT_H

#include <QEvent>
#include <QUndoCommand>

namespace KIPIPhotoLayoutsEditor
{
    class UndoCommandEvent : public QEvent
    {
            QUndoCommand * m_command;

        public:

            static Type registeredEventType()
            {
                static Type myType = static_cast<QEvent::Type>(QEvent::registerEventType());
                return myType;
            }

            explicit UndoCommandEvent();

            void setUndoCommand(QUndoCommand * command)
            {
                m_command = command;
            }
            QUndoCommand * undoCommand()
            {
                return m_command;
            }

        signals:

        public slots:

    };
}

#endif // UNDOCOMMANDEVENT_H
