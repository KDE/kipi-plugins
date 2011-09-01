#include "PhotoEffectsLoader.h"
#include "PhotoEffectsGroup.h"
#include "AbstractPhoto.h"
#include "UndoCommandEvent.h"
#include "KEditFactory.h"
#include "global.h"
#include "AbstractPhotoEffectFactory.h"
#include "AbstractPhotoEffectInterface.h"

#include <QtAbstractPropertyBrowser>
#include <QtTreePropertyBrowser>
#include <QtIntPropertyManager>
#include <QtVariantPropertyManager>

#include <kapplication.h>
#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

PhotoEffectsLoader * PhotoEffectsLoader::m_instance = 0;
QMap<QString, AbstractPhotoEffectFactory*> PhotoEffectsLoader::registeredEffects;

PhotoEffectsLoader::PhotoEffectsLoader(QObject * parent) :
    QObject(parent),
    sem(1),
    m_effect(0)
{
}

PhotoEffectsLoader * PhotoEffectsLoader::instance(QObject * parent)
{
    if (m_instance)
    {
        if (parent)
            m_instance->setParent(parent);
        return m_instance;
    }
    return (m_instance = new PhotoEffectsLoader(parent));
}

PhotoEffectsGroup * PhotoEffectsLoader::group() const
{
    return qobject_cast<PhotoEffectsGroup*>(this->parent());
}

AbstractPhoto * PhotoEffectsLoader::photo() const
{
    PhotoEffectsGroup * tempGroup = this->group();
    if (tempGroup)
        return tempGroup->photo();
    else
        return 0;
}

bool PhotoEffectsLoader::registerEffect(AbstractPhotoEffectFactory * effectFactory)
{
    QString effectName = effectFactory->effectName();
    return registeredEffects.insert(effectName, effectFactory) != registeredEffects.end();
}

QStringList PhotoEffectsLoader::registeredEffectsNames()
{
    return registeredEffects.keys();
}

AbstractPhotoEffectFactory * PhotoEffectsLoader::getFactoryByName(const QString & name)
{
    return registeredEffects.value(name, 0);
}

AbstractPhotoEffectInterface * PhotoEffectsLoader::getEffectByName(const QString & effectName)
{
    AbstractPhotoEffectFactory * factory = PhotoEffectsLoader::registeredEffects[effectName];
    if (factory)
        return factory->getEffectInstance();
    return 0;
}

QtAbstractPropertyBrowser * PhotoEffectsLoader::propertyBrowser(AbstractPhotoEffectInterface * effect)
{
    PhotoEffectsLoader * loader = instance();
    loader->postEffectChangedEvent();

    QtAbstractPropertyBrowser * browser = new QtTreePropertyBrowser();

    loader->m_effect = effect;
    if (effect)
    {
        // QVariant::Int
        QtIntPropertyManager * intManager = new QtIntPropertyManager(browser);
        KSliderEditFactory * sliderEditFactory = new KSliderEditFactory(browser);
        browser->setFactoryForManager(intManager, sliderEditFactory);
        // QVariant::Color
        QtColorPropertyManager * colorManager = 0;
        KColorEditorFactory * colorFactory = 0;
        // QVariant others....
        QtVariantPropertyManager * variantManager = 0;
        KVariantEditorFactory * variantFactory = 0;
        QList<AbstractPhotoEffectProperty*> effectProperties = effect->effectProperties();
        foreach (AbstractPhotoEffectProperty * property, effectProperties)
        {
            QtProperty * qtproperty;
            switch(property->value.type())
            {
                case QVariant::Int:
                    qtproperty = intManager->addProperty(property->id);
                    intManager->setValue(qtproperty,property->value.toInt());
                    intManager->setMaximum(qtproperty,property->data[AbstractPhotoEffectProperty::Maximum].toInt());
                    intManager->setMinimum(qtproperty,property->data[AbstractPhotoEffectProperty::Minimum].toInt());
                    browser->addProperty(qtproperty);
                    break;
                case QVariant::Color:
                    if (!colorManager || !colorFactory)
                    {
                        colorManager = new QtColorPropertyManager(browser);
                        colorFactory = new KColorEditorFactory(browser);
                        browser->setFactoryForManager(colorManager,colorFactory);
                    }
                    qtproperty = colorManager->addProperty(property->id);
                    colorManager->setValue(qtproperty,property->value.value<QColor>());
                    browser->addProperty(qtproperty);
                    break;
                default:
                    if (!variantManager || !variantFactory)
                    {
                        variantManager = new QtVariantPropertyManager(browser);
                        variantFactory = new KVariantEditorFactory(browser);
                        browser->setFactoryForManager(variantManager,variantFactory);
                    }
                    break;
            }
        }
        connect(intManager,SIGNAL(propertyChanged(QtProperty*)),loader,SLOT(propertyChanged(QtProperty*)));
        connect(sliderEditFactory,SIGNAL(editingFinished()),loader,SLOT(postEffectChangedEvent()));
        if (colorManager && colorFactory)
        {
            connect(colorManager,SIGNAL(propertyChanged(QtProperty*)),loader,SLOT(propertyChanged(QtProperty*)));
            connect(colorFactory,SIGNAL(editingFinished()),loader,SLOT(postEffectChangedEvent()));
        }
        if (variantManager && variantFactory)
        {
            connect(variantManager,SIGNAL(propertyChanged(QtProperty*)),loader,SLOT(propertyChanged(QtProperty*)));
            connect(variantFactory,SIGNAL(editingFinished()),loader,SLOT(postEffectChangedEvent()));
        }
    }
    else
        browser->setEnabled(false);
    return browser;
}

void PhotoEffectsLoader::propertyChanged(QtProperty * property)
{
    sem.acquire();
    AbstractPhotoEffectProperty * tempProperty;
    foreach (AbstractPhotoEffectProperty * currentProperty, m_effect_edited_properties)
    {
        if (currentProperty->id == property->propertyName())
        {
            setEffectPropertyValue(currentProperty, property);
            goto _return;
        }
    }
    foreach (AbstractPhotoEffectProperty * currentProperty, m_effect->effectProperties())
    {
        if (currentProperty->id == property->propertyName())
        {
            tempProperty = new AbstractPhotoEffectProperty(currentProperty->id);
            tempProperty->value = currentProperty->value;
            tempProperty->data = currentProperty->data;
            setEffectPropertyValue(tempProperty, property);
            m_effect_edited_properties.append(tempProperty);
            goto _return;
        }
    }

_return:
    sem.release();
}

void PhotoEffectsLoader::postEffectChangedEvent()
{
    QUndoCommand * command;
    sem.acquire();

    // If no effect there is nothing to do...
    if (!m_effect)
        goto _return;

    // Try to create effect command
    command = m_effect->createChangeCommand(m_effect_edited_properties);

    // If command has been created
    if (command)
    {
#ifdef QT_DEBUG
        qDebug() << "Command posted!";
#endif
        PLE_PostUndoCommand(command);
    }
    // If command can't be created just explicitly save effect properties to save effect state!
    else
        m_effect->setEffectProperties(m_effect_edited_properties);

_return:
    m_effect_edited_properties.clear();
    sem.release();
}

void PhotoEffectsLoader::setEffectPropertyValue(AbstractPhotoEffectProperty * effectProperty, QtProperty * property)
{
    switch (effectProperty->value.type())
    {
        case QVariant::Int:
            effectProperty->value = qobject_cast<QtIntPropertyManager*>(property->propertyManager())->value(property);
            break;
        case QVariant::Color:
            effectProperty->value = qobject_cast<QtColorPropertyManager*>(property->propertyManager())->value(property);
            break;
        default:
            effectProperty->value = qobject_cast<QtVariantPropertyManager*>(property->propertyManager())->value(property);
            break;
    }
}
