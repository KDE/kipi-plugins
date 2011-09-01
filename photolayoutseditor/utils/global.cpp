#include "global.h"
#include "UndoCommandEvent.h"
#include "photolayoutseditor.h"

#include <QPrinter>
#include <QQueue>

#include <kapplication.h>

QString KIPIPhotoLayoutsEditor::name()
{
    return QString("pfe");
}

QString KIPIPhotoLayoutsEditor::uri()
{
    return QString("http://coder89.pl/pfe");
}

void KIPIPhotoLayoutsEditor::PLE_PostUndoCommand(QUndoCommand * command)
{
    PhotoLayoutsEditor::instance()->addUndoCommand(command);
}

QDomElement KIPIPhotoLayoutsEditor::pathToSvg(const QPainterPath & path, QDomDocument & document)
{
    // If path is empty
    if (path.isEmpty())
        return QDomElement();

    // Conversion loop
    QString str_path_d;
    int elementsCount = path.elementCount();
    for (int i = 0; i < elementsCount; ++i)
    {
        QPainterPath::Element e = path.elementAt(i);
        switch (e.type)
        {
            case QPainterPath::LineToElement:
                str_path_d.append("L " + QString::number(e.x) + " " + QString::number(e.y) + " ");
                break;
            case QPainterPath::MoveToElement:
                str_path_d.append("M " + QString::number(e.x) + " " + QString::number(e.y) + " ");
                break;
            case QPainterPath::CurveToElement:
                str_path_d.append("C " + QString::number(e.x) + " " + QString::number(e.y) + " ");
                break;
            case QPainterPath::CurveToDataElement:
                str_path_d.append(QString::number(e.x) + " " + QString::number(e.y) + " ");
                break;
            default:
                Q_ASSERT(e.type == QPainterPath::CurveToDataElement ||
                         e.type == QPainterPath::CurveToElement ||
                         e.type == QPainterPath::LineToElement ||
                         e.type == QPainterPath::MoveToElement);
        }
    }
    str_path_d.append("z");

    // If path length is empty
    if (str_path_d.isEmpty())
        return QDomElement();

    // Create QDomElement
    QDomElement element = document.createElement("path");
    element.setAttribute("d", str_path_d);
    return element;
}

QPainterPath KIPIPhotoLayoutsEditor::pathFromSvg(const QDomElement & element)
{
    QPainterPath result;
    if (element.tagName() != "path")
        return result;
    QString d = element.attribute("d");
    QStringList list = d.split(" ", QString::SkipEmptyParts);
    QStringList::const_iterator it = list.begin();
    QQueue<qreal> coordinates;
    QQueue<char> operations;
    while (it != list.end())
    {
        if (*it == "M")
            operations.enqueue('M');
        else if (*it == "L")
            operations.enqueue('L');
        else if (*it == "C")
            operations.enqueue('C');
        else if (*it == "z")
            operations.enqueue('z');
        else
        {
            QString str = *it;
            bool isOK;
            qreal value = str.toDouble(&isOK);
            if (isOK)
                coordinates.enqueue(value);
            else
                return QPainterPath();
        }
        ++it;
    }
    qreal t1, t2, t3, t4, t5, t6;
    while (operations.count())
    {
        char opc = operations.dequeue();
        switch (opc)
        {
            case 'M':
                if (coordinates.count() < 2)
                    return QPainterPath();
                t1 = coordinates.dequeue();
                t2 = coordinates.dequeue();
                result.moveTo(t1, t2);
                break;
            case 'L':
                if (coordinates.count() < 2)
                    return QPainterPath();
                t1 = coordinates.dequeue();
                t2 = coordinates.dequeue();
                result.lineTo(t1, t2);
                break;
            case 'C':
                if (coordinates.count() < 4)
                    return QPainterPath();
                t1 = coordinates.dequeue();
                t2 = coordinates.dequeue();
                t3 = coordinates.dequeue();
                t4 = coordinates.dequeue();
                t5 = coordinates.dequeue();
                t6 = coordinates.dequeue();
                result.cubicTo(t1, t2, t3, t4, t5, t6);
                break;
            case 'z':
                result.closeSubpath();
                break;
            default:
                return QPainterPath();
        }
    }
    return result;
}
