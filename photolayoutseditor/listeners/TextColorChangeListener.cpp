#include "TextColorChangeListener.h"

#include "TextItem.h"

using namespace KIPIPhotoLayoutsEditor;

TextColorChangeListener::TextColorChangeListener(TextItem * item, QObject * parent) :
    QObject(parent),
    m_item(item)
{}

void TextColorChangeListener::propertyChanged(QtProperty * property)
{
    QtColorPropertyManager * manager = dynamic_cast<QtColorPropertyManager*>(property->propertyManager());
    if (manager && m_item->color() != manager->value(property))
        m_item->setColor(manager->value(property));
}
