#include "BorderChangeListener.h"
#include "BordersGroup.h"
#include "global.h"

#include <QtProperty>
#include <KEditFactory.h>
#include <QtVariantPropertyManager>

#include <QUndoCommand>
#include <QMetaProperty>

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::BorderChangeCommand : public QUndoCommand
{
        BorderDrawerInterface * drawer;
        QString propertyName;
        QVariant value;
    public:
        BorderChangeCommand(BorderDrawerInterface * drawer, QUndoCommand * parent = 0) :
            QUndoCommand(parent),
            drawer(drawer)
        {
        }
        virtual void redo()
        {
            QVariant temp = drawer->propertyValue(propertyName);
            drawer->setPropertyValue(propertyName, value);
            value = temp;
            if (drawer->group())
                drawer->group()->refresh();
        }
        virtual void undo()
        {
            QVariant temp = drawer->propertyValue(propertyName);
            drawer->setPropertyValue(propertyName, value);
            value = temp;
            if (drawer->group())
                drawer->group()->refresh();
        }
        void setPropertyValue(const QString & propertyName, const QVariant & value)
        {
            this->propertyName = propertyName;
            this->value = value;
        }
};

BorderChangeListener::BorderChangeListener(BorderDrawerInterface * drawer, QObject * parent, bool createCommands) :
    QObject(parent),
    drawer(drawer),
    command(0),
    createCommands(createCommands)
{
}

void BorderChangeListener::propertyChanged(QtProperty * property)
{
    if (!drawer)
        return;

    if (!command)
        command = new BorderChangeCommand(drawer);

    QtIntPropertyManager * integerManager = qobject_cast<QtIntPropertyManager*>(property->propertyManager());
    if (integerManager)
    {
        command->setPropertyValue(property->propertyName(), integerManager->value(property));
        return;
    }
    QtDoublePropertyManager * doubleManager = qobject_cast<QtDoublePropertyManager*>(property->propertyManager());
    if (doubleManager)
    {
        command->setPropertyValue(property->propertyName(), doubleManager->value(property));
        return;
    }
    QtVariantPropertyManager * variantManager = qobject_cast<QtVariantPropertyManager*>(property->propertyManager());
    if (variantManager)
    {
        command->setPropertyValue(property->propertyName(), variantManager->value(property));
        return;
    }
}

void BorderChangeListener::editingFinished()
{
    if (command)
    {
        if (createCommands)
            PLE_PostUndoCommand(command);
        else
        {
            command->redo();
            delete command;
        }
    }
    command = 0;
}
