#include "AbstractItemsTool.h"
#include "AbstractPhoto.h"
#include "ToolsDockWidget.h"

using namespace KIPIPhotoLayoutsEditor;

AbstractItemsTool::AbstractItemsTool(Scene * scene, Canvas::SelectionMode selectionMode, QWidget * parent) :
    AbstractTool(scene, selectionMode, parent)
{
}

AbstractPhoto * AbstractItemsTool::currentItem()
{
    return m_photo;
}

void AbstractItemsTool::setCurrentItem(AbstractPhoto * photo)
{
    if (m_photo == photo)
        return;
    currentItemAboutToBeChanged();
    m_photo = photo;
    setEnabled((bool)m_photo);
    currentItemChanged();
}

QPointF AbstractItemsTool::mousePosition()
{
    return m_point;
}

void AbstractItemsTool::setMousePosition(const QPointF & position)
{
    if (m_point == position)
        return;
    positionAboutToBeChanged();
    m_point = position;
    positionChanged();
}
