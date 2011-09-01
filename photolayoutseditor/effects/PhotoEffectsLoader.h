#ifndef ABSTRACTPHOTOEFFECTS_H
#define ABSTRACTPHOTOEFFECTS_H

#include <QImage>
#include <QObject>
#include <QSharedPointer>
#include <QUndoCommand>
#include <QSemaphore>
#include <QMap>
#include <QList>

class QtProperty;
class QtAbstractPropertyBrowser;

namespace KIPIPhotoLayoutsEditor
{
    class AbstractPhoto;
    class PhotoEffectsGroup;
    class AbstractPhotoEffectFactory;
    class AbstractPhotoEffectProperty;
    class AbstractPhotoEffectInterface;

    class PhotoEffectsLoader : public QObject
    {
            Q_OBJECT

            class OpacityUndoCommand;

            static QString m_effect_name;
            QString m_name;

            // Semafore (for multi-thread safety)
            QSemaphore sem;

            static QMap<QString, AbstractPhotoEffectFactory*> registeredEffects;

            static PhotoEffectsLoader * m_instance;
            explicit PhotoEffectsLoader(QObject * parent);

        public:

            static PhotoEffectsLoader * instance(QObject * parent = 0);
            PhotoEffectsGroup * group() const;
            AbstractPhoto * photo() const;

          /**
            * Name propetry
            */
            Q_PROPERTY(QString m_name READ name WRITE setName)
            virtual QString name() const
            {
                return m_name;
            }
            virtual void setName(const QString & name)
            {
                m_name = name;
            }

          /** Registers new effect using it's factory object.
            * \arg effectFactory - this object should be allocated on heap usong \fn operator new(),
            * this class takes over the parenthood of this factory and it will delete it if it'll no longer needed.
            */
            static bool registerEffect(AbstractPhotoEffectFactory * effectFactory);

          /** Returns registered effects names
            * This implementation returns \class QStringList object with effects names obtained by calling \fn effectName()
            * method of its factory object.
            */
            static QStringList registeredEffectsNames();

          /** Returns factory object for the given name
            */
            static AbstractPhotoEffectFactory * getFactoryByName(const QString & name);

          /** Return an instance of effect using its name.
            */
            static AbstractPhotoEffectInterface * getEffectByName(const QString & name);

          /** Returns property browser for effect.
            * \arg effect is the object of \class AbstractPhotoEffectInterface base type which represents effect with set of properties to configure.
            */
            static QtAbstractPropertyBrowser * propertyBrowser(AbstractPhotoEffectInterface * effect);

        protected:

            AbstractPhotoEffectInterface * m_effect;
            QList<AbstractPhotoEffectProperty*> m_effect_edited_properties;

          /// Use this function before/after modifying or creating new QUndoCommand for your effect
            void beginUndoCommandChange()
            {
                sem.acquire();
            }
            void endUndoCommandChange()
            {
                sem.release();
            }

        protected slots:

          /** This function if called when any QtProperty has changed.
            * If you are reimplementing this function remember to call yours parent's version to process it's
            * properties if \arg property is not your own.
            * \note Remember to create/modify your version of QUndoCommand here.
            * \warning Also remember to connect this function to all of your QtPropertyManager's to
            * recieve all property change events.
            */
            void propertyChanged(QtProperty * property);

          /** Call this slot to post your QUndoCommand.
            * \note It is good to connect this slot to \fn editingFinished() signal of your QtEditorFacroty objects.
            */
            void postEffectChangedEvent();

        signals:

          /** Effect changed signal.
            * Emit this signal to notify all listeners that your effects state has changed
            * (i.e. opacity has changed and repaint is needed)
            */
            void effectChanged(PhotoEffectsLoader * effect);

        private:

            void setEffectPropertyValue(AbstractPhotoEffectProperty * effectProperty, QtProperty * property);

        friend class AbstractPhotoEffectFactory;
    };
}

#endif // ABSTRACTPHOTOEFFECTS_H
