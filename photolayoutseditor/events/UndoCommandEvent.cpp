#include "UndoCommandEvent.h"

using namespace KIPIPhotoLayoutsEditor;

UndoCommandEvent::UndoCommandEvent() :
    QEvent(registeredEventType()),
    m_command(0)
{
}
