#ifndef BORDERSGROUP_H
#define BORDERSGROUP_H

#include "AbstractMovableModel.h"

#include <QPainter>
#include <QPainterPath>
#include <QDomDocument>
#include <QStyleOptionGraphicsItem>
#include "BorderDrawerInterface.h"

namespace KIPIPhotoLayoutsEditor
{
    class BordersGroupPrivate;

    class AbstractPhoto;

    class BordersGroup : public AbstractMovableModel
    {
            Q_OBJECT

            BordersGroupPrivate * d;

        public:

            BordersGroup(AbstractPhoto * graphicsItem);
            QPainterPath shape();
            AbstractPhoto * graphicsItem() const;
            void paint(QPainter * painter, const QStyleOptionGraphicsItem * option);

            // Method used for model manipulation
            bool prependDrawer(BorderDrawerInterface * drawer);
            bool insertDrawer(BorderDrawerInterface * drawer, int position);
            bool appendDrawer(BorderDrawerInterface * drawer);
            BorderDrawerInterface * removeDrawer(int position);
            bool moveDrawer(int sourcePosition, int destinationPosition);

            QDomElement toSvg(QDomDocument & document);
            static BordersGroup * fromSvg(QDomElement & element, AbstractPhoto * graphicsItem);

        protected:

            virtual QObject * item(const QModelIndex & index) const;
            virtual void setItem(QObject * graphicsItem, const QModelIndex & index);
            virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
            virtual QVariant data(const QModelIndex & index, int role) const;
            virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
            virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
            virtual QModelIndex parent(const QModelIndex & child) const;
            virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
            virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
            virtual bool moveRows(int sourcePosition, int sourceCount, int destPosition);

        public slots:

            void refresh();

        private:

            void calculateShape();
            Q_DISABLE_COPY(BordersGroup)
    };
}

#endif // BORDERSGROUP_H
