#include "UndoBorderChangeCommand.h"
#include "AbstractPhoto.h"

#include <QDebug>

#include <klocalizedstring.h>

using namespace KIPIPhotoFramesEditor;

UndoBorderChangeCommand::UndoBorderChangeCommand(AbstractPhoto * photo, qreal newWidth, Qt::PenJoinStyle newCornerStyle, const QColor & newColor, QUndoCommand * parent) :
    QUndoCommand(i18n("Border changed"), parent),
    m_photo(photo),
    m_width(newWidth),
    m_corner_style(newCornerStyle),
    m_color(newColor)
{
}

void UndoBorderChangeCommand::redo()
{
    qreal tempWidth = m_photo->borderWidth();
    QColor tempColor = m_photo->borderColor();
    Qt::PenJoinStyle tempCornerStyle = m_photo->borderCornersStyle();
    m_photo->setBorderWidth(m_width);
    m_photo->setBorderColor(m_color);
    m_photo->setBorderCornersStyle(m_corner_style);
    m_width = tempWidth;
    m_color = tempColor;
    m_corner_style = tempCornerStyle;
}

void UndoBorderChangeCommand::undo()
{
    qreal tempWidth = m_photo->borderWidth();
    QColor tempColor = m_photo->borderColor();
    Qt::PenJoinStyle tempCornerStyle = m_photo->borderCornersStyle();
    m_photo->setBorderWidth(m_width);
    m_photo->setBorderColor(m_color);
    m_photo->setBorderCornersStyle(m_corner_style);
    m_width = tempWidth;
    m_color = tempColor;
    m_corner_style = tempCornerStyle;
}
