#ifndef UNDOCOMMANDEVENTFILTER_H
#define UNDOCOMMANDEVENTFILTER_H

#include <QObject>

namespace KIPIPhotoLayoutsEditor
{
    class UndoCommandEventFilter : public QObject
    {
            Q_OBJECT

        public:

            explicit UndoCommandEventFilter(QObject *parent = 0) :
                QObject(parent)
            {}

            virtual bool eventFilter(QObject * watched, QEvent * event);

        signals:

        public slots:

    };
}

#endif // UNDOCOMMANDEVENTFILTER_H
