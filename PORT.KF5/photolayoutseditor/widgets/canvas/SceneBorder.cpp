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

#include "SceneBorder.moc"

// C++ std includes

#include <limits>

// Qt includes

#include <QPainter>
#include <QGraphicsScene>
#include <QUndoCommand>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QPixmap>
#include <QBuffer>

#include <klocalizedstring.h>

#include "global.h"

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::SceneBorder::BorderImageChangedCommand : public QUndoCommand
{
    QImage       m_image;
    SceneBorder* m_backgropund_item;

public:

    BorderImageChangedCommand(const QImage & image, SceneBorder * borderItem, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Border Change"), parent),
        m_image(image),
        m_backgropund_item(borderItem)
    {
    }

    virtual void redo()
    {
        run();
    }
    virtual void undo()
    {
        run();
    }
    void run()
    {
        QImage temp = m_backgropund_item->m_image;
        m_backgropund_item->m_image = m_image;
        m_image = temp;

        m_backgropund_item->render();
        m_backgropund_item->update();
    }
};

SceneBorder::SceneBorder(QGraphicsScene * scene) :
    QGraphicsItem(0, scene)
{
    this->setZValue(std::numeric_limits<double>::infinity());
    this->setFlags(0);
    this->sceneChanged();
}

QRectF SceneBorder::boundingRect() const
{
    return m_rect;
}

void SceneBorder::setImage(const QImage & image)
{
    bool imageChanged = m_image != image;

    QUndoCommand * parent = 0;
    if (imageChanged)
        parent = new QUndoCommand(i18n("Border Change"));

    QUndoCommand * command = new BorderImageChangedCommand(image, this, parent);
    PLE_PostUndoCommand(command);
}

QDomElement SceneBorder::toSvg(QDomDocument & document) const
{
    QDomElement result = document.createElement("g");
    result.setAttribute("id", "border");

    QDomElement defs = document.createElement("defs");
    result.appendChild(defs);

    QSize s = this->m_rect.size().toSize();

    QDomElement pattern = document.createElement("pattern");
    pattern.setAttribute("x", 0);
    pattern.setAttribute("y", 0);
    pattern.setAttribute("width", QString::number(s.width())+"px");
    pattern.setAttribute("height",QString::number(s.height())+"px");
    pattern.setAttribute("patternUnits", "userSpaceOnUse");
    defs.appendChild(pattern);

    QDomElement image = document.createElement("image");
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    m_image.save(&buffer, "PNG");
    image.setAttribute("width", QString::number(s.width())+"px");
    image.setAttribute("height",QString::number(s.height())+"px");
    image.setAttribute("xlink:href",QString("data:image/png;base64,")+byteArray.toBase64());
    pattern.setAttribute("id", QString::number(qChecksum(byteArray.constData(), byteArray.length())).append("bkg"));
    pattern.appendChild(image);

    QDomElement bckg = document.createElement("rect");
    bckg.setAttribute("x", 0);
    bckg.setAttribute("y", 0);
    bckg.setAttribute("width", image.attribute("width"));
    bckg.setAttribute("height",image.attribute("height"));
    bckg.setAttribute("fill", "url(#"+pattern.attribute("id")+')');
    result.appendChild(bckg);

    return result;
}

bool SceneBorder::fromSvg(QDomElement & /*element*/)
{
/*    QDomNodeList list = element.childNodes();
    QDomElement border;
    for (int i = list.count()-1; i >= 0; --i)
    {
        if (!list.at(i).isElement())
            continue;
        border = list.at(i).toElement();
        if (border.attribute("id") != "border")
        {
            border = QDomElement();
            continue;
        }
    }
    if (border.isNull())
        return false;

    QDomElement defs = border.firstChildElement("defs");
    if (defs.isNull())
        return false;
    QString type = defs.firstChildElement("type").text();
    if (type == "color")
    {
        QDomElement rect = border.firstChildElement("rect");
        if (rect.isNull())
            return false;
        QColor color(rect.attribute("fill"));
        color.setAlphaF(rect.attribute("opacity").toDouble());
        if (!color.isValid())
            return false;
        m_first_brush.setColor(color);
    }
    else if (type == "pattern")
    {
        bool ok = true;
        QDomElement bse = defs.firstChildElement("brush_style");
        if (bse.isNull()) return false;
        Qt::BrushStyle bs = (Qt::BrushStyle) bse.text().toInt(&ok);

        QDomElement c1e = defs.firstChildElement("color1");
        if (c1e.isNull()) return false;
        QColor color1(c1e.text());
        color1.setAlphaF(c1e.attribute("opacity").toInt());

        QDomElement c2e = defs.firstChildElement("color2");
        if (c2e.isNull()) return false;
        QColor color2(c2e.text());
        color2.setAlphaF(c2e.attribute("opacity").toInt());

        if (!color1.isValid() || !color2.isValid() || !ok || bs <= Qt::SolidPattern || bs >= Qt::LinearGradientPattern)
            return false;
        m_first_brush.setStyle(bs);
        m_first_brush.setColor(color1);
        m_second_brush.setStyle(Qt::SolidPattern);
        m_second_brush.setColor(color2);
    }
    else if (type == "image")
    {
        m_image_align = (Qt::Alignment) defs.firstChildElement("align").text().toInt();
        m_image_aspect_ratio = (Qt::AspectRatioMode) defs.firstChildElement("aspect_ratio").text().toInt();
        m_image_repeat = (bool) defs.firstChildElement("repeat").text().toInt();

        QDomElement pattern = defs.firstChildElement("pattern");
        if (pattern.isNull())
            return false;
        QDomElement image = pattern.firstChildElement("image");
        if (image.isNull())
            return false;
        m_image_size.setWidth(image.attribute("width").remove("px").toInt());
        m_image_size.setHeight(image.attribute("height").remove("px").toInt());
        m_image = QImage::fromData( QByteArray::fromBase64(image.attributeNS("http://www.w3.org/1999/xlink", "href").remove("data:image/png;base64,").toAscii()) );
        m_first_brush.setTextureImage(m_image.scaled(m_image_size, m_image_aspect_ratio));

        QDomElement bColor = defs.firstChildElement("border_color");
        QColor borderColor(bColor.text());
        borderColor.setAlphaF(bColor.attribute("opacity", "1.0").toDouble());
        m_second_brush.setColor(borderColor);
    }
    else if (type == "gradient")
    {}
    else
        return false;

    render();
*/
    return true;
}

QImage SceneBorder::image() const
{
    return m_image;
}

QSize SceneBorder::imageSize() const
{
    return m_image.size();
}

QVariant SceneBorder::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch(change)
    {
    case QGraphicsItem::ItemParentChange:
        return QVariant(0);
    case QGraphicsItem::ItemSceneChange:
        this->disconnect(scene(), 0, this, 0);
        break;
    case QGraphicsItem::ItemSceneHasChanged:
        sceneChanged();
        break;
    default: break;
    }
    return QGraphicsItem::itemChange(change, value);
}

void SceneBorder::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * /*widget*/)
{
    if (m_temp_image.isNull() || !m_rect.isValid())
        return;
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawImage(QPoint(0,0), m_temp_image, option->exposedRect);
}

void SceneBorder::render(QPainter * painter, const QRect & rect)
{
    if (rect.isValid())
    {
        painter->drawImage(rect, m_temp_image, m_rect);
    }
}

void SceneBorder::render()
{
    if (m_rect.isValid())
    {
        if (m_image.isNull())
            return;
        m_temp_image = m_image.scaled(m_rect.size().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
}

void SceneBorder::sceneChanged()
{
    if (scene())
    {
        sceneRectChanged(scene()->sceneRect());
        this->connect(scene(), SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChanged(QRectF)));
    }
    else
        sceneRectChanged(QRectF());
}

void SceneBorder::sceneRectChanged(const QRectF & sceneRect)
{
    if (sceneRect.isValid())
    {
        m_rect = sceneRect;
        if (m_image.isNull())
            return;
        render();
    }
    else
    {
        m_rect = QRectF();
    }
}
