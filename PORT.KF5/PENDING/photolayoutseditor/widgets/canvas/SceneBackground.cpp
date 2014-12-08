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

#include "SceneBackground.moc"
#include "global.h"

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

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::SceneBackground::BackgroundImageChangedCommand : public QUndoCommand
{
    QImage m_image;
    Qt::Alignment m_alignment;
    Qt::AspectRatioMode m_aspect_ratio;
    QSize m_size;
    bool m_repeat;
    SceneBackground * m_backgropund_item;
public:

    BackgroundImageChangedCommand(const QImage & image, Qt::Alignment alignment, const QSize & size, bool repeat, SceneBackground * backgroundItem, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Background Change"), parent),
        m_image(image),
        m_alignment(alignment),
        m_aspect_ratio(Qt::IgnoreAspectRatio),
        m_size(size),
        m_repeat(repeat),
        m_backgropund_item(backgroundItem)
    {
    }

    BackgroundImageChangedCommand(const QImage & image, Qt::Alignment alignment, Qt::AspectRatioMode aspectRatio, bool repeat, SceneBackground * backgroundItem, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Background Change"), parent),
        m_image(image),
        m_alignment(alignment),
        m_aspect_ratio(aspectRatio),
        m_size(image.size()),
        m_repeat(repeat),
        m_backgropund_item(backgroundItem)
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
        m_backgropund_item->m_first_brush.setTextureImage(m_image);
        m_image = temp;

        Qt::Alignment temp2 = m_backgropund_item->m_image_align;
        m_backgropund_item->m_image_align = m_alignment;
        m_alignment = temp2;

        Qt::AspectRatioMode temp3 = m_backgropund_item->m_image_aspect_ratio;
        m_backgropund_item->m_image_aspect_ratio = m_aspect_ratio;
        m_aspect_ratio = temp3;

        bool temp4 = m_backgropund_item->m_image_repeat;
        m_backgropund_item->m_image_repeat = m_repeat;
        m_repeat = temp4;

        QSize temp5 = m_backgropund_item->m_image_size;
        m_backgropund_item->m_image_size = m_size;
        m_size = temp5;

        m_backgropund_item->render();
        m_backgropund_item->update();
    }
};
class KIPIPhotoLayoutsEditor::SceneBackground::BackgroundFirstBrushChangeCommand : public QUndoCommand
{
    QBrush m_brush;
    SceneBackground * m_background;
public:
    BackgroundFirstBrushChangeCommand(const QBrush & brush, SceneBackground * background, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Background Change"), parent),
        m_brush(brush),
        m_background(background)
    {}
    virtual void redo()
    {
        this->run();
    }
    virtual void undo()
    {
        this->run();
    }
    void run()
    {
        QBrush temp = m_background->m_first_brush;
        m_background->m_first_brush = m_brush;
        m_brush = temp;

        m_background->render();
        m_background->update();
    }
};
class KIPIPhotoLayoutsEditor::SceneBackground::BackgroundSecondBrushChangeCommand : public QUndoCommand
{
    QBrush m_brush;
    SceneBackground * m_background;
public:
    BackgroundSecondBrushChangeCommand(const QBrush & brush, SceneBackground * background, QUndoCommand * parent = 0) :
        QUndoCommand(i18n("Background Change"), parent),
        m_brush(brush),
        m_background(background)
    {}
    virtual void redo()
    {
        this->run();
    }
    virtual void undo()
    {
        this->run();
    }
    void run()
    {
        QBrush temp = m_background->m_second_brush;
        m_background->m_second_brush = m_brush;
        m_brush = temp;

        m_background->render();
        m_background->update();
    }
};

SceneBackground::SceneBackground(QGraphicsScene * scene) :
    QGraphicsItem(0, scene),
    m_first_brush(Qt::transparent),
    m_second_brush(Qt::transparent)
{
    this->setZValue(-std::numeric_limits<double>::infinity());
    this->setFlags(0);
    this->sceneChanged();
}

QRectF SceneBackground::boundingRect() const
{
    return m_rect;
}

void SceneBackground::setSecondColor(const QColor & color)
{
    bool colorChanged = (m_second_brush.color() != color);
    bool patternChanged = (m_second_brush.style() != Qt::SolidPattern);
    if (colorChanged || patternChanged)
    {
        QUndoCommand * command = new BackgroundSecondBrushChangeCommand(QBrush(color), this);
        PLE_PostUndoCommand(command);
    }
}

void SceneBackground::setSolidColor(const QColor & color)
{
    bool colorChanged = (m_first_brush.color() != color);
    bool patternChaged = (m_first_brush.style() != Qt::SolidPattern);
    bool secondColorChanged = (m_second_brush.color() != Qt::transparent);

    QUndoCommand * parent = 0;
    QUndoCommand * command = 0;

    if ((colorChanged && secondColorChanged) ||
        (patternChaged && secondColorChanged))
        parent = new QUndoCommand(i18n("Background Change"));

    if (colorChanged || patternChaged)
        command = new BackgroundFirstBrushChangeCommand(QBrush(color), this, parent);
    if (secondColorChanged)
        command = new BackgroundSecondBrushChangeCommand(QBrush(Qt::transparent), this, parent);

    if (parent)
        PLE_PostUndoCommand(parent);
    else if (command)
        PLE_PostUndoCommand(command);
}

void SceneBackground::setPattern(const QColor & firstColor, const QColor & secondColor, Qt::BrushStyle patternStyle)
{
    bool color1Changed = (firstColor != m_first_brush.color() || patternStyle != m_first_brush.style());
    bool color2Changed = (secondColor != m_second_brush.color() || m_second_brush.style() != Qt::SolidPattern);

    QUndoCommand * parent = 0;
    if (color1Changed && color2Changed)
        parent = new QUndoCommand("Background Change");
    QUndoCommand * command = 0;

    if (color1Changed)
        command = new BackgroundFirstBrushChangeCommand(QBrush(firstColor, patternStyle), this, parent);
    if (color2Changed)
        command = new BackgroundSecondBrushChangeCommand(QBrush(secondColor, Qt::SolidPattern), this, parent);

    if (parent)
        PLE_PostUndoCommand(parent);
    else
        PLE_PostUndoCommand(command);
}

void SceneBackground::setImage(const QImage & image, const QColor & backgroundColor, Qt::Alignment align, Qt::AspectRatioMode aspectRatio, bool repeat)
{
    bool imageChanged = (m_first_brush.textureImage() != image ||
                         m_first_brush.style() != Qt::TexturePattern ||
                         m_image_align != align ||
                         m_image_aspect_ratio != aspectRatio ||
                         m_image_repeat != repeat);

    bool colorChanged = (m_second_brush.color() != backgroundColor || m_second_brush.style() != Qt::SolidPattern);

    QUndoCommand * parent = 0;
    if (imageChanged && colorChanged)
        parent = new QUndoCommand(i18n("Background Change"));

    QUndoCommand * command = 0;
    if (imageChanged)
        command = new BackgroundImageChangedCommand(image, align, aspectRatio, repeat, this, parent);
    if (colorChanged)
        command = new BackgroundSecondBrushChangeCommand(QBrush(backgroundColor, Qt::SolidPattern), this, parent);

    if (parent)
        PLE_PostUndoCommand(parent);
    else if (command)
        PLE_PostUndoCommand(command);
}

void SceneBackground::setImage(const QImage & image, const QColor & backgroundColor, Qt::Alignment align, const QSize & fixedSize, bool repeat)
{
    bool imageChanged = (m_first_brush.textureImage() != image ||
                         m_first_brush.style() != Qt::TexturePattern ||
                         m_image_align != align ||
                         m_image_size != fixedSize ||
                         m_image_repeat != repeat);

    bool colorChanged = (m_second_brush.color() != backgroundColor || m_second_brush.style() != Qt::SolidPattern);

    QUndoCommand * parent = 0;
    if (imageChanged && colorChanged)
        parent = new QUndoCommand(i18n("Background Change"));

    QUndoCommand * command = 0;
    if (imageChanged)
        command = new BackgroundImageChangedCommand(image, align, fixedSize, repeat, this, parent);
    if (colorChanged)
        command = new BackgroundSecondBrushChangeCommand(QBrush(backgroundColor, Qt::SolidPattern), this, parent);

    if (parent)
        PLE_PostUndoCommand(parent);
    else if (command)
        PLE_PostUndoCommand(command);
}

bool SceneBackground::isColor() const
{
    return m_first_brush.style() == Qt::SolidPattern;
}

bool SceneBackground::isGradient() const
{
    return m_first_brush.style() == Qt::LinearGradientPattern ||
           m_first_brush.style() == Qt::RadialGradientPattern ||
           m_first_brush.style() == Qt::ConicalGradientPattern;
}

bool SceneBackground::isImage() const
{
    return m_first_brush.style() == Qt::TexturePattern;
}

bool SceneBackground::isPattern() const
{
    return !(isColor() || isGradient() || isImage());
}

QDomElement SceneBackground::toSvg(QDomDocument & document) const
{
    QDomElement result = document.createElement("g");
    result.setAttribute("id", "background");
    QDomElement defs = document.createElement("defs");
    result.appendChild(defs);
    QDomElement type = document.createElement("type");
    defs.appendChild(type);
    if (this->isColor())
    {
        type.appendChild( document.createTextNode("color") );
        QDomElement rect = document.createElement("rect");
        rect.setAttribute("width",  m_rect.width());
        rect.setAttribute("height", m_rect.height());
        rect.setAttribute("x", 0);
        rect.setAttribute("y", 0);
        rect.setAttribute("fill", m_first_brush.color().name());
        rect.setAttribute("opacity", QString::number(m_first_brush.color().alphaF()));
        result.appendChild(rect);
    }
    else if (this->isPattern())
    {
        QDomElement pattern = document.createElement("image");
        pattern.setAttribute("width",  m_image.rect().width());
        pattern.setAttribute("height", m_image.rect().height());
        pattern.setAttribute("x", 0);
        pattern.setAttribute("y", 0);
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        m_temp_image.save(&buffer, "PNG");
        pattern.setAttribute("xlink:href", QString("data:image/png;base64,") + byteArray.toBase64());
        result.appendChild(pattern);

        type.appendChild( document.createTextNode("pattern") );
        QDomElement bs = document.createElement("brush_style");
        bs.appendChild( document.createTextNode(QString::number(m_first_brush.style())) );
        defs.appendChild(bs);
        QDomElement c1 = document.createElement("color1");
        c1.appendChild( document.createTextNode(m_first_brush.color().name()) );
        c1.setAttribute("opacity", QString::number(m_first_brush.color().alphaF()) );
        defs.appendChild(c1);
        QDomElement c2 = document.createElement("color2");
        c2.appendChild( document.createTextNode(m_second_brush.color().name()) );
        c2.setAttribute("opacity", QString::number(m_second_brush.color().alphaF()) );
        defs.appendChild(c2);
    }
    else if (this->isImage())
    {
        type.appendChild( document.createTextNode("image") );

        QSize s = m_first_brush.textureImage().size();
        QDomElement pattern = document.createElement("pattern");
        pattern.setAttribute("x", QString::number(m_first_brush.transform().m31()));
        pattern.setAttribute("y", QString::number(m_first_brush.transform().m32()));
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

        QDomElement align = document.createElement("align");
        align.appendChild( document.createTextNode(QString::number(m_image_align)) );
        defs.appendChild(align);

        QDomElement aspect_ratio = document.createElement("aspect_ratio");
        aspect_ratio.appendChild( document.createTextNode(QString::number(m_image_aspect_ratio)) );
        defs.appendChild(aspect_ratio);

        QDomElement repeat = document.createElement("repeat");
        repeat.appendChild( document.createTextNode(QString::number(m_image_repeat)) );
        defs.appendChild(repeat);

        QDomElement bColor = document.createElement("background_color");
        bColor.appendChild( document.createTextNode(m_second_brush.color().name()) );
        bColor.setAttribute("opacity", QString::number(m_second_brush.color().alphaF()));
        defs.appendChild(bColor);

        QDomElement bckColor = document.createElement("rect");
        bckColor.setAttribute("x", 0);
        bckColor.setAttribute("y", 0);
        bckColor.setAttribute("width", m_image.width());
        bckColor.setAttribute("height",m_image.height());
        bckColor.setAttribute("fill", m_second_brush.color().name());
        bckColor.setAttribute("opacity", QString::number(m_second_brush.color().alphaF()));
        result.appendChild(bckColor);

        QDomElement bckg = document.createElement("rect");
        if (m_image_repeat)
        {
            bckg.setAttribute("x", 0);
            bckg.setAttribute("y", 0);
            bckg.setAttribute("width", m_image.width());
            bckg.setAttribute("height",m_image.height());
        }
        else
        {
            bckg.setAttribute("x", pattern.attribute("x"));
            bckg.setAttribute("y", pattern.attribute("y"));
            bckg.setAttribute("width", pattern.attribute("width"));
            bckg.setAttribute("height",pattern.attribute("height"));
        }
        bckg.setAttribute("fill", "url(#"+pattern.attribute("id")+')');
        result.appendChild(bckg);
    }
    else if (this->isGradient())
    {
        type.appendChild( document.createTextNode("gradient") );
    }

    return result;
}

bool SceneBackground::fromSvg(QDomElement & element)
{
    QDomNodeList list = element.childNodes();
    QDomElement background;
    for (int i = list.count()-1; i >= 0; --i)
    {
        if (!list.at(i).isElement())
            continue;
        background = list.at(i).toElement();
        if (background.attribute("id") != "background")
        {
            background = QDomElement();
            continue;
        }
    }
    if (background.isNull())
        return false;

    QDomElement defs = background.firstChildElement("defs");
    if (defs.isNull())
        return false;
    QString type = defs.firstChildElement("type").text();
    if (type == "color")
    {
        QDomElement rect = background.firstChildElement("rect");
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

        QDomElement bColor = defs.firstChildElement("background_color");
        QColor backgroundColor(bColor.text());
        backgroundColor.setAlphaF(bColor.attribute("opacity", "1.0").toDouble());
        m_second_brush.setColor(backgroundColor);
    }
    else if (type == "gradient")
    {}
    else
        return false;

    render();

    return true;
}

QColor SceneBackground::firstColor() const
{
    return m_first_brush.color();
}

QColor SceneBackground::secondColor() const
{
    return m_second_brush.color();
}

Qt::BrushStyle SceneBackground::pattern() const
{
    return m_first_brush.style();
}

QImage SceneBackground::image() const
{
    return m_image;
}

Qt::Alignment SceneBackground::imageAlignment() const
{
    return m_image_align;
}

Qt::AspectRatioMode SceneBackground::imageAspectRatio() const
{
    return m_image_aspect_ratio;
}

QSize SceneBackground::imageSize() const
{
    return m_image_size;
}

bool SceneBackground::imageRepeated() const
{
    return m_image_repeat;
}

QVariant SceneBackground::itemChange(GraphicsItemChange change, const QVariant &value)
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

void SceneBackground::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * /*widget*/)
{
    if (!m_rect.isValid())
        return;
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawImage(QPoint(0,0), m_temp_image, option->exposedRect);
}

void SceneBackground::render(QPainter * painter, const QRect & rect)
{
    if (!rect.isValid())
        return;
    QRect r = rect;
    painter->fillRect(r, m_second_brush);
    if (this->isImage())
    {
        QSize scaleSize = (m_image_aspect_ratio == Qt::IgnoreAspectRatio ? m_image_size : rect.size());
        m_first_brush.setTextureImage( m_image.scaled(scaleSize, m_image_aspect_ratio, Qt::SmoothTransformation) );
        m_image_size = m_first_brush.textureImage().size();
        QSize bgSize = rect.size();
        QSize imSize = m_first_brush.textureImage().size();
        QTransform tr;
        qreal x = 0;
        if (m_image_align & Qt::AlignHCenter)
            x = (bgSize.width() - imSize.width()) / 2.0;
        else if (m_image_align & Qt::AlignRight)
            x = bgSize.width() - imSize.width();
        qreal y = 0;
        if (m_image_align & Qt::AlignVCenter)
            y = (bgSize.height() - imSize.height()) / 2.0;
        else if (m_image_align & Qt::AlignBottom)
            y = bgSize.height() - imSize.height();
        tr.translate(x,y);
        m_first_brush.setTransform(tr);
        if (!this->m_image_repeat)
            r = m_first_brush.transform().mapRect(QRect(0, 0, m_image_size.width(), m_image_size.height()));
    }
    painter->fillRect(r, m_first_brush);
}

void SceneBackground::render()
{
    this->m_temp_image.fill(Qt::transparent);
    QPainter p(&this->m_temp_image);
    this->render(&p, this->m_temp_image.rect());
    p.end();
    emit changed();
}

void SceneBackground::sceneChanged()
{
    if (scene())
    {
        sceneRectChanged(scene()->sceneRect());
        this->connect(scene(), SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChanged(QRectF)));
    }
    else
        sceneRectChanged(QRectF());
}

void SceneBackground::sceneRectChanged(const QRectF & sceneRect)
{
    if (sceneRect.isValid())
    {
        m_rect = sceneRect;
        m_temp_image = QImage(m_rect.size().toSize(), QImage::Format_ARGB32);
        m_temp_image.fill(Qt::transparent);
        QPainter p(&m_temp_image);
        render(&p, m_rect.toRect());
    }
    else
        m_rect = QRectF();
}
