/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#include "BlurPhotoEffect.h"
#include "KEditFactory.h"

#include <QtTreePropertyBrowser>
#include <QtIntPropertyManager>
#include <QtSpinBoxFactory>

#include <klocalizedstring.h>

using namespace KIPIPhotoFramesEditor;

const QString BlurPhotoEffect::RADIUS_STRING = i18n("Radius");

class BlurPhotoEffect::BlurUndoCommand : public QUndoCommand
{
        BlurPhotoEffect * m_effect;
        int m_radius;
    public:
        BlurUndoCommand(BlurPhotoEffect * effect, int radius);
        virtual void redo();
        virtual void undo();
        void setRadius(int radius);
    private:
        void runCommand()
        {
            int temp = m_effect->radius();
            if (temp != m_radius)
            {
                m_effect->setRadius(m_radius);
                m_radius = temp;
            }
        }
};

BlurPhotoEffect::BlurUndoCommand::BlurUndoCommand(BlurPhotoEffect * effect, int radius) :
    m_effect(effect),
    m_radius(radius)
{}

void BlurPhotoEffect::BlurUndoCommand::redo()
{
    runCommand();
}

void BlurPhotoEffect::BlurUndoCommand::undo()
{
    runCommand();
}

void BlurPhotoEffect::BlurUndoCommand::setRadius(int radius)
{
    m_radius = radius;
}

BlurPhotoEffect::BlurPhotoEffect(int radius, QObject * parent) :
    PhotoEffectsLoader(parent),
    m_radius(radius)
{
}

QImage BlurPhotoEffect::apply(const QImage & image)
{
    QImage result = image;
    QPainter p(&result);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.drawImage(0,0,PhotoEffectsLoader::apply(blurred(image, image.rect(), m_radius)));
    return result;
}

QtAbstractPropertyBrowser * BlurPhotoEffect::propertyBrowser() const
{
    QtAbstractPropertyBrowser * browser = PhotoEffectsLoader::propertyBrowser();
    QtIntPropertyManager * intManager = new QtIntPropertyManager(browser);
    KSliderEditFactory * sliderFactory = new KSliderEditFactory(browser);
    browser->setFactoryForManager(intManager,sliderFactory);

    // Radius property
    QtProperty * radius = intManager->addProperty(RADIUS_STRING);
    intManager->setMaximum(radius,200);
    intManager->setMinimum(radius,0);
    browser->addProperty(radius);

    intManager->setValue(radius,m_radius);
    connect(intManager,SIGNAL(propertyChanged(QtProperty*)),this,SLOT(propertyChanged(QtProperty*)));
    connect(sliderFactory,SIGNAL(editingFinished()),this,SLOT(postEffectChangedEvent()));

    return browser;
}

QString BlurPhotoEffect::toString() const
{
    return this->name() + " [" + RADIUS_STRING + '=' + QString::number(m_radius) + ']' ;
}

QString BlurPhotoEffect::effectName() const
{
    return i18n("Blur effect");
}

void BlurPhotoEffect::propertyChanged(QtProperty * property)
{
    QtIntPropertyManager * manager = qobject_cast<QtIntPropertyManager*>(property->propertyManager());
    int radius = m_radius;

    if (property->propertyName() == RADIUS_STRING)
        radius = manager->value(property);
    else
    {
        PhotoEffectsLoader::propertyChanged(property);
        return;
    }

    beginUndoCommandChange();
    if (m_undo_command)
    {
        BlurUndoCommand * undo = dynamic_cast<BlurUndoCommand*>(m_undo_command);
        undo->setRadius(radius);
    }
    else
        m_undo_command = new BlurUndoCommand(this,radius);
    endUndoCommandChange();
}
