#ifndef MOTELITEM_H
#define MOTELITEM_H

#include <QObject>
#include <QDebug>
#include <QGraphicsItem>

namespace KIPIPhotoLayoutsEditor
{
    class AbstractPhoto;
    class LayersModel;

    class LayersModelItem : public QObject
    {
            Q_OBJECT

            static const int COLUMN_COUNT = 4;

        public:

            enum
            {
                EyeIcon = 1, PadLockIcon, NameString = COLUMN_COUNT-1, Thumbnail
            };

            LayersModelItem(AbstractPhoto * item, LayersModelItem * parent, LayersModel * model);
            virtual ~LayersModelItem();
            void removeChild(LayersModelItem * child);
            LayersModelItem * parent() const;
            void setParent(LayersModelItem * parent);
            int row() const;
            int columnCount() const;
            int childCount() const;
            LayersModelItem * child(int row) const;
            QVariant data(int column) const;
            QList<QVariant> data() const;
            bool insertChildren(int position, LayersModelItem * item);
            bool removeChildren(int position, int count);
            bool moveChildren(int sourcePosition, int count, LayersModelItem * destParent, int destPosition);
            bool setData(const QVariant & data, int type);
            void setPhoto(AbstractPhoto * photo);
            AbstractPhoto * photo() const;

        protected:

            void setData(const QList<QVariant> & data);

        protected slots:

            void updateData();

        private:

            void refreshZValues();

            LayersModelItem * parentItem;
            QList<LayersModelItem*> childItems;
            AbstractPhoto * itemPhoto;
            LayersModel * itemModel;
    };
}

#endif // MOTELITEM_H
