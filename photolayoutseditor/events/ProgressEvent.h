#ifndef PROGRESSEVENT_H
#define PROGRESSEVENT_H

#include <QEvent>
#include <QVariant>

namespace KIPIPhotoLayoutsEditor
{
    class ProgressEvent : public QEvent
    {
        public:

            enum Type
            {
                Unknown,
                Init,
                Update,
                Finish,
            };

            explicit ProgressEvent() :
                QEvent(ProgressEvent::registeredEventType()),
                m_type(Unknown)
            {
            }

            void setData(ProgressEvent::Type type, QVariant data)
            {
                this->m_type = type;
                this->m_data = data;
            }

            ProgressEvent::Type type() const
            {
                return this->m_type;
            }

            QVariant data() const
            {
                return this->m_data;
            }

            static QEvent::Type registeredEventType()
            {
                static QEvent::Type myType = static_cast<QEvent::Type>(QEvent::registerEventType());
                return myType;
            }

        private:

            ProgressEvent::Type m_type;
            QVariant m_data;

            Q_DISABLE_COPY(ProgressEvent)
    };
}

#endif // PROGRESSEVENT_H
