#include "UndoCommandEventFilter.h"
#include "UndoCommandEvent.h"
#include "global.h"
#include "photolayoutseditor.h"
#include <QtGlobal>
#include <QDebug>

using namespace KIPIPhotoLayoutsEditor;

bool UndoCommandEventFilter::eventFilter(QObject * watched, QEvent * event)
{
    if (event->type() == UndoCommandEvent::registeredEventType())
    {
        PhotoLayoutsEditor * editor = qobject_cast<PhotoLayoutsEditor*>(watched);
        if (editor)
        {
            //editor->undoCommandEvent(dynamic_cast<UndoCommandEvent*>(event));
            return true;
        }
        else
            return false;
    }
    return QObject::eventFilter(watched, event);
}
