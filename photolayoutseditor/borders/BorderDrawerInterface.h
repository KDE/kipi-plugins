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
