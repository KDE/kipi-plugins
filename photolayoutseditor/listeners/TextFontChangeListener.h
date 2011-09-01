#ifndef TEXTFONTCHANGELISTENER_H
#define TEXTFONTCHANGELISTENER_H

#include <QObject>
#include <QtColorPropertyManager>

namespace KIPIPhotoLayoutsEditor
{
    class TextItem;
    class TextFontChangeListener : public QObject
    {
            Q_OBJECT
            TextItem * m_item;
        public:
            TextFontChangeListener(TextItem * item, QObject * parent = 0);
        public slots:
            void propertyChanged(QtProperty * property);
    };
}

#endif // TEXTFONTCHANGELISTENER_H
