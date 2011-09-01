#ifndef ABSTRACTPROTOEFFECTINTERFACE_H
#define ABSTRACTPROTOEFFECTINTERFACE_H

#include <QtPlugin>
#include <QVariant>
#include <QList>
#include <QImage>
#include <QPainter>
#include <QUndoCommand>
#include <QDebug>

#include "PhotoEffectsGroup.h"

#include <klocalizedstring.h>

#define STRENGTH_PROPERTY "Strength"

namespace KIPIPhotoLayoutsEditor
{
    struct AbstractPhotoEffectProperty
    {
        enum AttributeTypes
        {
            Maximum,
            Minimum,
            AllowedList,
        };

        AbstractPhotoEffectProperty(const QString & _id) :
            id(_id)
        {}
        const QString id;
        QVariant value;
        QMap<AttributeTypes,QVariant> data;
    };

    class AbstractPhotoEffectFactory;
    class AbstractPhotoEffectInterface : public QObject
    {
            class AbstractPhotoEffectInterfaceCommand;

            AbstractPhotoEffectFactory * m_factory;

        public:

            explicit AbstractPhotoEffectInterface(AbstractPhotoEffectFactory * factory, QObject * parent = 0) :
                QObject(parent),
                m_factory(factory)
            {
#ifdef QT_DEBUG
                if (!m_factory)
                    qDebug() << "No factory object for effect" << this->effectName() << "from:" << __FILE__ << __LINE__;
#endif
                AbstractPhotoEffectProperty * strength = new AbstractPhotoEffectProperty(STRENGTH_PROPERTY);
                strength->value = QVariant(100);
                strength->data.insert(AbstractPhotoEffectProperty::Minimum,0);
                strength->data.insert(AbstractPhotoEffectProperty::Maximum,100);
                m_properties.push_back(strength);
            }

            virtual ~AbstractPhotoEffectInterface()
            {
                qDeleteAll(m_properties);
            }

            virtual QImage apply(const QImage & image) const
            {
                int _opacity = strength();
                if (_opacity != 100)
                {
                    QImage result(image.size(),QImage::Format_ARGB32_Premultiplied);
                    QPainter p(&result);
                    p.drawImage(0,0,image);
                    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                    p.fillRect(image.rect(), QColor(0, 0, 0, _opacity*255/100));
                    return result;
                }
                return image;
            }

            virtual QString effectName() const = 0;
            virtual QString toString() const = 0;
            virtual operator QString() const = 0;

            QUndoCommand * createChangeCommand(const QList<AbstractPhotoEffectProperty*> & properties)
            {
                if (properties.count() && group() && group()->photo())
                    return new AbstractPhotoEffectInterfaceCommand(this,properties);
                else
                    return 0;
            }

            QList<AbstractPhotoEffectProperty*> effectProperties()
            {
                return m_properties;
            }

            AbstractPhotoEffectProperty * getProperty(const QString & id)
            {
                foreach (AbstractPhotoEffectProperty * currentProperty, m_properties)
                {
                    if (currentProperty->id == id)
                        return currentProperty;
                }
                return 0;
            }

            PhotoEffectsGroup * group() const
            {
                return qobject_cast<PhotoEffectsGroup*>(this->parent());
            }

            AbstractPhotoEffectFactory * factory() const
            {
                return m_factory;
            }

            int strength() const
            {
                foreach (AbstractPhotoEffectProperty * property, m_properties)
                    if (property->id == STRENGTH_PROPERTY)
                        return property->value.toInt();
                return 100;
            }

            AbstractPhotoEffectProperty * getPropertyByName(const QString & name) const
            {
                foreach (AbstractPhotoEffectProperty * property, m_properties)
                    if (property->id == name)
                        return property;
                return 0;
            }

            QVariant::Type getPropertyTypeByName(const QString & name) const
            {
                foreach (AbstractPhotoEffectProperty * property, m_properties)
                    if (property->id == name)
                        return property->value.type();
                return QVariant::Invalid;
            }

        protected:

            QList<AbstractPhotoEffectProperty*> m_properties;

        private:

            QList<AbstractPhotoEffectProperty*> setEffectProperties(const QList<AbstractPhotoEffectProperty*> & properties)
            {
                QList<AbstractPhotoEffectProperty*> oldProperties;
                foreach (AbstractPhotoEffectProperty * newProperty, properties)
                {
                    for (QList<AbstractPhotoEffectProperty*>::iterator it = m_properties.begin(); it != m_properties.end(); ++it)
                    {
                        if ((*it) != newProperty && (*it)->id == newProperty->id && newProperty->value.type() == (*it)->value.type())
                        {
                            oldProperties.append(*it);
                            *it = newProperty;
                        }
                    }
                }
                return oldProperties;
            }

            void setStrength(int strength)
            {
                foreach (AbstractPhotoEffectProperty * property, m_properties)
                    if (property->id == STRENGTH_PROPERTY)
                        property->value = strength;
            }

            class AbstractPhotoEffectInterfaceCommand : public QUndoCommand
            {
                    AbstractPhotoEffectInterface * m_effect;
                    QList<AbstractPhotoEffectProperty*> m_properties;
                public:
                    AbstractPhotoEffectInterfaceCommand(AbstractPhotoEffectInterface * effect, const QList<AbstractPhotoEffectProperty*> & properties, QUndoCommand * parent = 0) :
                        QUndoCommand(parent),
                        m_effect(effect),
                        m_properties(properties)
                    {
                        this->setText(m_effect->effectName() + i18n(" change"));
                    }
                    ~AbstractPhotoEffectInterfaceCommand()
                    {
                        qDeleteAll(m_properties);
                        m_properties.clear();
                    }
                    virtual void redo()
                    {
                        runCommand();
                    }
                    virtual void undo()
                    {
                        runCommand();
                    }
                private:
                    void runCommand()
                    {
                        QList<AbstractPhotoEffectProperty*> tempProperties = m_effect->setEffectProperties(m_properties);
                        if (tempProperties.count() != m_properties.count())
                        {
                            for (int i = m_properties.count()-1; i >= 0; --i)
                            {
                                bool found = false;
                                foreach (AbstractPhotoEffectProperty * oldProperty, tempProperties)
                                {
                                    if (m_properties[i]->id == oldProperty->id)
                                    {
                                        found = true;
                                        break;
                                    }
                                }
                                if (!found)
                                    m_properties.removeAt(i);
                            }
                        }
                        m_properties = tempProperties;
                        m_effect->group()->emitEffectsChanged(m_effect);
                    }
            };

        friend class AbstractPhotoEffectInterfaceCommand;
        friend class AbstractPhotoEffectFactory;
        friend class PhotoEffectsLoader;
    };
}

Q_DECLARE_INTERFACE(KIPIPhotoLayoutsEditor::AbstractPhotoEffectInterface, "pl.coder89.pfe.AbstractPhotoEffectInterface")

#endif // ABSTRACTPROTOEFFECTINTERFACE_H
