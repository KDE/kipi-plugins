#ifndef ABSTRACTBORDERINTERFACE_H
#define ABSTRACTBORDERINTERFACE_H

#include <QObject>
#include <QDomDocument>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QCoreApplication>

#include "BorderDrawerFactoryInterface.h"

namespace KIPIPhotoLayoutsEditor
{
    enum
    {
        Enum = QVariant::UserType
    };

    class BordersGroup;
    class BorderDrawerInterface : public QObject
    {
            BordersGroup * m_group;
            BorderDrawerFactoryInterface * m_factory;

        public:

            explicit BorderDrawerInterface(BorderDrawerFactoryInterface * factory, QObject * parent = 0) :
                QObject(parent),
                m_group(0),
                m_factory(factory)
            {}

            void setGroup(BordersGroup * group)
            {
                this->m_group = group;
            }
            BordersGroup * group() const
            {
                return this->m_group;
            }
            BorderDrawerFactoryInterface * factory() const
            {
                return m_factory;
            }

            virtual QPainterPath path(const QPainterPath & path) = 0;
            virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option) = 0;

            virtual QString propertyName(const QMetaProperty & property) const = 0;
            virtual QVariant propertyValue(const QString & propertyName) const = 0;
            virtual void setPropertyValue(const QString & propertyName, const QVariant & value) = 0;
            virtual QVariant stringNames(const QMetaProperty & /*property*/){ return QVariant(); }
            virtual QVariant minimumValue(const QMetaProperty & /*property*/){ return QVariant(); }
            virtual QVariant maximumValue(const QMetaProperty & /*property*/){ return QVariant(); }
            virtual QVariant stepValue(const QMetaProperty & /*property*/){ return QVariant(); }

            virtual QDomElement toSvg(QDomDocument & document) const = 0;
            virtual QString toString() const = 0;
            virtual operator QString() const = 0;

        signals:

            void changed();

        protected:

            void propertiesChanged()
            {
                emit changed();
            }
    };
}

Q_DECLARE_INTERFACE(KIPIPhotoLayoutsEditor::BorderDrawerInterface, "pl.coder89.pfe.BorderDrawerInterface/1.0")

#endif // ABSTRACTBORDERINTERFACE_H
