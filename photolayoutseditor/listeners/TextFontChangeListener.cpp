#include "TextFontChangeListener.h"
#include "TextItem.h"

#include <QtFontPropertyManager>

using namespace KIPIPhotoLayoutsEditor;

TextFontChangeListener::TextFontChangeListener(TextItem * item, QObject * parent) :
    QObject(parent),
    m_item(item)
{}

void TextFontChangeListener::propertyChanged(QtProperty * property)
{
    QtFontPropertyManager * manager = dynamic_cast<QtFontPropertyManager*>(property->propertyManager());
    if (manager && m_item->font() != manager->value(property))
        m_item->setFont(manager->value(property));
}
