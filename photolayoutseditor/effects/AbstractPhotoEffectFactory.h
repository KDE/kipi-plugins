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

#ifndef ABSTRACTPHOTOEFFECTFACTORY_H
#define ABSTRACTPHOTOEFFECTFACTORY_H

#include <QObject>
#include <QDomDocument>

#include "AbstractPhotoEffectInterface.h"

namespace KIPIPhotoLayoutsEditor
{
    class KDE_EXPORT AbstractPhotoEffectFactory : public QObject
    {
        public:

            AbstractPhotoEffectFactory(QObject * parent = 0) :
                QObject(parent)
            {}
            virtual ~AbstractPhotoEffectFactory()
            {}

            /** Returns effects instance.
            * \arg browser - as this argument you can set \class QtAbstractPropertyBrowser recieved from virtual
            * \fn propertyBrowser() method of this object.
            */
            virtual AbstractPhotoEffectInterface * getEffectInstance() = 0;

          /** Returns effect name.
            * This name is used be the user to identify effect. This name should be unique becouse effects are identified by this name.
            * Moreover, this name is used in UI to name effects.
            */
            virtual QString effectName() const = 0;

          /** Returns DOM node which contains effects attributes
            */
            QDomElement toSvg(AbstractPhotoEffectInterface * effect, QDomDocument & document)
            {
                QDomElement element = document.createElement("effect");
                element.setAttribute("name", effectName());
                element.setAttribute(STRENGTH_PROPERTY, effect->strength());
                this->writeToSvg(effect, element);
                return element;
            }

          /** Reads node attributes from DOM node and returns ready effect object.
            */
            AbstractPhotoEffectInterface * fromSvg(QDomElement & element)
            {
                if (element.tagName() != "effect" || element.attribute("name") != effectName())
                    return 0;
                AbstractPhotoEffectInterface * result = this->readFromSvg(element);
                if (result)
                    result->setStrength( element.attribute(STRENGTH_PROPERTY).toInt() );
                return result;
            }

        protected:

          /** Writes effect specific attributes to DOM effect element.
            * This is a pure virtual method which need to be implemented to write any effect specific attributes.
            * You should save all data which are needed by your \fn readFromSvg() method to create valid copy of this effect.
            */
            virtual void writeToSvg(AbstractPhotoEffectInterface * effect, QDomElement & effectElement) = 0;

          /** Reads effect attributes from DOM node and returns ready effect object.
            * This method should create a valid effect object connected with apropriate factory object.
            * It should also set any specific effect's attributes saved by \fn writeToSvg() method.
            */
            virtual AbstractPhotoEffectInterface * readFromSvg(QDomElement & element) = 0;
    };
}

Q_DECLARE_INTERFACE(KIPIPhotoLayoutsEditor::AbstractPhotoEffectFactory,"pl.coder89.pfe.AbstractPhotoEffectFactory/1.0")

#endif // ABSTRACTPHOTOEFFECTFACTORY_H
