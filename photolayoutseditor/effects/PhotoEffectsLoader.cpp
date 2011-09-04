/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "PhotoEffectsLoader.h"
#include "PhotoEffectsGroup.h"
#include "AbstractPhoto.h"
#include "UndoCommandEvent.h"
#include "KEditFactory.h"
#include "global.h"
#include "AbstractPhotoEffectFactory.h"
#include "AbstractPhotoEffectInterface.h"
#include "PhotoEffectChangeListener.h"

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
    QStringList names = effectName.split(";", QString::SkipEmptyParts);
    bool result = true;
    foreach (QString name, names)
        result &= (registeredEffects.insert(name, effectFactory) != registeredEffects.end());
    return result;
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
        return factory->getEffectInstance(effectName);
    return 0;
}

QtAbstractPropertyBrowser * PhotoEffectsLoader::propertyBrowser(AbstractPhotoEffectInterface * effect, bool createCommands)
{
    if (!effect)
        return 0;

    QtAbstractPropertyBrowser * browser = new QtTreePropertyBrowser();
    PhotoEffectChangeListener * listener = new PhotoEffectChangeListener(effect, browser, createCommands);

    // QVariant::Int
    QtIntPropertyManager * integerManager = new QtIntPropertyManager(browser);
    KSliderEditFactory * integerFactory = new KSliderEditFactory(browser);
    browser->setFactoryForManager(integerManager, integerFactory);

    // Double type of property
    QtDoublePropertyManager * doubleManager = 0;
    KDoubleSpinBoxFactory * doubleFactory = 0;

    // QVariant::Color
    QtColorPropertyManager * colorManager = 0;
    KColorEditorFactory * colorFactory = 0;

    // QVariant others....
    QtVariantPropertyManager * variantManager = 0;
    KVariantEditorFactory * variantFactory = 0;

    const QMetaObject * meta = effect->metaObject();
    int propertiesCount = meta->propertyCount();
    for (int i = 0; i < propertiesCount; ++i)
    {
        QMetaProperty metaProperty = meta->property(i);
        QString propertyName = effect->propertyName(metaProperty);
        if (propertyName.isEmpty())
            continue;
        QtProperty * property = 0;
        switch(metaProperty.type())
        {
            case QVariant::Int:
                {
                    if (!integerManager || !integerFactory)
                    {
                        integerManager = new QtIntPropertyManager(browser);
                        integerFactory = new KSliderEditFactory(browser);
                        browser->setFactoryForManager(integerManager, integerFactory);
                    }
                    property = integerManager->addProperty(propertyName);
                    integerManager->setValue(property, metaProperty.read(effect).toInt());
                    integerManager->setMinimum(property, effect->minimumValue(metaProperty).toInt());
                    integerManager->setMaximum(property, effect->maximumValue(metaProperty).toInt());
                    integerManager->setSingleStep(property, effect->stepValue(metaProperty).toInt());
                }
                break;
            case QVariant::Double:
                {
                    if (!doubleManager || !doubleFactory)
                    {
                        doubleManager = new QtDoublePropertyManager(browser);
                        doubleFactory = new KDoubleSpinBoxFactory(browser);
                        browser->setFactoryForManager(doubleManager, doubleFactory);
                    }
                    property = doubleManager->addProperty(propertyName);
                    doubleManager->setValue(property, metaProperty.read(effect).toDouble());
                    doubleManager->setMinimum(property, effect->minimumValue(metaProperty).toDouble());
                    doubleManager->setMaximum(property, effect->maximumValue(metaProperty).toDouble());
                    integerManager->setSingleStep(property, effect->maximumValue(metaProperty).toDouble());
                }
                break;
            case QVariant::Color:
                {
                    if (!colorManager || !colorFactory)
                    {
                        colorManager = new QtColorPropertyManager(browser);
                        colorFactory = new KColorEditorFactory(browser);
                        browser->setFactoryForManager(colorManager,colorFactory);
                    }
                    property = colorManager->addProperty(propertyName);
                    colorManager->setValue(property, metaProperty.read(effect).value<QColor>());
                    browser->addProperty(property);
                }
                break;
            default:
                {
                    if (!variantManager || !variantFactory)
                    {
                        variantManager = new QtVariantPropertyManager(browser);
                        variantFactory = new KVariantEditorFactory(browser);
                        browser->setFactoryForManager(variantManager, variantFactory);
                    }
                    property = variantManager->addProperty(metaProperty.type(), propertyName);
                    variantManager->setValue(property, metaProperty.read(effect));
                }
        }
        if (property)
            browser->addProperty(property);
    }
    connect(integerManager,SIGNAL(propertyChanged(QtProperty*)),listener,SLOT(propertyChanged(QtProperty*)));
    connect(integerFactory,SIGNAL(editingFinished()),listener,SLOT(editingFinished()));
    if (doubleManager && doubleFactory)
    {
        connect(doubleManager,SIGNAL(propertyChanged(QtProperty*)),listener,SLOT(propertyChanged(QtProperty*)));
        connect(doubleFactory,SIGNAL(editingFinished()),listener,SLOT(editingFinished()));
    }
    if (colorManager && colorFactory)
    {
        connect(colorManager,SIGNAL(propertyChanged(QtProperty*)),listener,SLOT(propertyChanged(QtProperty*)));
        connect(colorFactory,SIGNAL(editingFinished()),listener,SLOT(editingFinished()));
    }
    if (variantManager && variantFactory)
    {
        connect(variantManager,SIGNAL(propertyChanged(QtProperty*)),listener,SLOT(propertyChanged(QtProperty*)));
        connect(variantFactory,SIGNAL(editingFinished()),listener,SLOT(editingFinished()));
    }

    return browser;
}
