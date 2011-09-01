#ifndef EYEDELEGATE_H
#define EYEDELEGATE_H

#include <QStyledItemDelegate>

namespace KIPIPhotoLayoutsEditor
{
    class LayersTreeDelegate : public QStyledItemDelegate
    {

            Q_OBJECT

            QPixmap m_eye;
            QPixmap m_eye_off;
            QPixmap m_padlock;
            QPixmap m_padlock_off;

        public:

            LayersTreeDelegate(QWidget * parent = 0);
            virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
            virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

        public Q_SLOTS:

            void itemClicked(const QModelIndex & index);

        Q_SIGNALS:

            void itemRepaintNeeded(const QModelIndex & index);
    };
}

#endif // EYEDELEGATE_H
