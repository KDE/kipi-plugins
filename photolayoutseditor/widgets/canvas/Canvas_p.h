#ifndef CANVAS_P_H
#define CANVAS_P_H

#include "CanvasSize.h"

#include <QMap>
#include <QProgressBar>

namespace KIPIPhotoLayoutsEditor
{
    class Canvas;
    class CanvasSavingThread;
    class CanvasPrivate
    {
        CanvasSize m_size;

        QMap<QObject*,QProgressBar*> progressMap;

        friend class Canvas;
        friend class CanvasSavingThread;
    };
};

#endif // CANVAS_P_H
