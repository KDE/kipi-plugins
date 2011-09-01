#ifndef GLOBAL_H
#define GLOBAL_H

#define Q_DELETE(ptr)   if(ptr) { delete ptr; ptr = 0; }

#include <QUndoCommand>
#include <QPrinter>
#include <QSizeF>
#include <QMap>
#include <QDomDocument>
#include <QPainterPath>

namespace KIPIPhotoLayoutsEditor
{
    extern QString name();
    extern QString uri();
    extern void PLE_PostUndoCommand(QUndoCommand * command);
    extern QDomElement pathToSvg(const QPainterPath & path, QDomDocument & document);
    extern QPainterPath pathFromSvg(const QDomElement & element);
}

#endif // GLOBAL_H
