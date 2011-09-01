#ifndef UNDOBORDERCHANGECOMMAND_H
#define UNDOBORDERCHANGECOMMAND_H

#include <QUndoCommand>
#include <QColor>

namespace KIPIPhotoFramesEditor
{
    class AbstractPhoto;

    class UndoBorderChangeCommand : public QUndoCommand
    {
            AbstractPhoto * m_photo;
            qreal m_width;
            Qt::PenJoinStyle m_corner_style;
            QColor m_color;

        public:

            explicit UndoBorderChangeCommand(AbstractPhoto * photo, qreal newWidth, Qt::PenJoinStyle newCornerStyle, const QColor & newColor, QUndoCommand * parent = 0);
            virtual void redo();
            virtual void undo();
    };
}

#endif // UNDOBORDERCHANGECOMMAND_H
