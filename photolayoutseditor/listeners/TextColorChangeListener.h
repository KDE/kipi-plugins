#ifndef TEXTCOLORCHANGELISTENER_H
#define TEXTCOLORCHANGELISTENER_H

#include <QObject>

class QtProperty;

namespace KIPIPhotoLayoutsEditor
{
    class TextItem;
    class TextColorChangeListener : public QObject
    {
            Q_OBJECT
            TextItem * m_item;
        public:
            TextColorChangeListener(TextItem * item, QObject * parent = 0);
        public slots:
            void propertyChanged(QtProperty * property);
    };
}

#endif // TEXTCOLORCHANGELISTENER_H
