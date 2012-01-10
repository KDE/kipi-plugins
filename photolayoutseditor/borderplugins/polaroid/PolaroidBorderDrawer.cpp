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

#include "PolaroidBorderDrawer.moc"
#include "StandardBordersFactory.h"

#include <klocalizedstring.h>

#include <QPainter>
#include <QPaintEngine>
#include <QMetaProperty>
#include <QDebug>

using namespace KIPIPhotoLayoutsEditor;

QMap<const char *,QString> PolaroidBorderDrawer::m_properties;
int PolaroidBorderDrawer::m_default_width = 20;
QString PolaroidBorderDrawer::m_default_text = i18n("Write here some text");
QColor PolaroidBorderDrawer::m_default_color = Qt::black;
QFont PolaroidBorderDrawer::m_default_font(QFont().family(), 24);

PolaroidBorderDrawer::PolaroidBorderDrawer(StandardBordersFactory * factory, QObject * parent) :
    BorderDrawerInterface(factory, parent),
    m_width(m_default_width),
    m_text(m_default_text),
    m_color(m_default_color),
    m_font(m_default_font)
{
    if (m_properties.isEmpty())
    {
        const QMetaObject * meta = this->metaObject();
        int count = meta->propertyCount();
        while (count--)
        {
            QMetaProperty property = meta->property(count);
            if (!QString("width").compare(property.name()))
                m_properties.insert(property.name(), i18n("Width"));
            else if (!QString("text").compare(property.name()))
                m_properties.insert(property.name(), i18n("Text"));
            else if (!QString("color").compare(property.name()))
                m_properties.insert(property.name(), i18n("Color"));
            else if (!QString("font").compare(property.name()))
                m_properties.insert(property.name(), i18n("Font"));
        }
    }
}

QPainterPath PolaroidBorderDrawer::path(const QPainterPath & path)
{
    QPainterPath temp;

    QRectF r = path.boundingRect();

    m_text_rect.setTop(r.bottom());

    r.setTop(r.top()-m_width);
    r.setBottom(r.bottom()+m_width*5);
    r.setLeft(r.left()-m_width);
    r.setRight(r.right()+m_width);

    m_text_rect.setBottom(r.bottom());
    m_text_rect.setLeft(r.left());
    m_text_rect.setRight(r.right());

    temp.addRect(r);
    temp -= path;

    m_path = temp;
    return m_path;
}

void PolaroidBorderDrawer::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/)
{
    if (m_path.isEmpty())
        return;
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(m_path, Qt::white);
    painter->setFont(m_font);
    painter->setPen(m_color);
    painter->drawText(m_text_rect, Qt::AlignCenter, m_text);
    painter->restore();
}

QString PolaroidBorderDrawer::propertyName(const QMetaProperty & property) const
{
    return m_properties.value(property.name());
}

QVariant PolaroidBorderDrawer::propertyValue(const QString & propertyName) const
{
    if (!m_properties.key(propertyName))
        return QVariant();
    const QMetaObject * meta = this->metaObject();
    int index = meta->indexOfProperty( m_properties.key(propertyName) );
    if (index >= meta->propertyCount())
        return QVariant();
    return meta->property( index ).read(this);
}

void PolaroidBorderDrawer::setPropertyValue(const QString & propertyName, const QVariant & value)
{
    if (!m_properties.key(propertyName))
        return;
    const QMetaObject * meta = this->metaObject();
    int index = meta->indexOfProperty( m_properties.key(propertyName) );
    if (index >= meta->propertyCount())
        return;
    meta->property( index ).write(this, value);
}

QDomElement PolaroidBorderDrawer::toSvg(QDomDocument & document) const
{
    QDomElement result = document.createElement("g");
    QDomElement path = document.createElement("path");
    result.appendChild(path);
    path.setAttribute("d", pathToSvg(m_path));
    path.setAttribute("fill", "#ffffff");
    path.setAttribute("fill-rule", "evenodd");

    QPainterPath p;
    p.addText(0, 0, m_font, m_text);
    p.translate(m_text_rect.center() - p.boundingRect().center());

    QDomElement text = document.createElement("path");
    result.appendChild(text);
    text.setAttribute("d", pathToSvg(p));
    text.setAttribute("fill", m_color.name());

    return result;
}

QString PolaroidBorderDrawer::name() const
{
    return i18n("Polaroid border");
}

QString PolaroidBorderDrawer::toString() const
{
    return name().append(" [") + m_text + QString("]");
}

PolaroidBorderDrawer::operator QString() const
{
    return this->toString();
}

QVariant PolaroidBorderDrawer::minimumValue(const QMetaProperty & property)
{
    const char * name = property.name();
    if (!QString("width").compare(name))
        return 0;
    return QVariant();
}

QVariant PolaroidBorderDrawer::maximumValue(const QMetaProperty & property)
{
    const char * name = property.name();
    if (!QString("width").compare(name))
        return 100;
    return QVariant();
}

QVariant PolaroidBorderDrawer::stepValue(const QMetaProperty & property)
{
    const char * name = property.name();
    if (!QString("width").compare(name))
        return 1;
    return QVariant();
}

QString PolaroidBorderDrawer::pathToSvg(const QPainterPath & path) const
{
    int count = path.elementCount();
    QString str_path_d;
    for (int i = 0; i < count; ++i)
    {
        QPainterPath::Element e = path.elementAt(i);
        switch (e.type)
        {
        case QPainterPath::LineToElement:
            str_path_d.append("L " + QString::number(e.x) + ' ' + QString::number(e.y) + ' ');
            break;
        case QPainterPath::MoveToElement:
            str_path_d.append("M " + QString::number(e.x) + ' ' + QString::number(e.y) + ' ');
            break;
        case QPainterPath::CurveToElement:
            str_path_d.append("C " + QString::number(e.x) + ' ' + QString::number(e.y) + ' ');
            break;
        case QPainterPath::CurveToDataElement:
            str_path_d.append(QString::number(e.x) + ' ' + QString::number(e.y) + ' ');
            break;
        }
    }
    str_path_d.append("z");
    return str_path_d;
}
