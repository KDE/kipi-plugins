#ifndef ABSTRACTPHOTOEFFECTSGROUP_H
#define ABSTRACTPHOTOEFFECTSGROUP_H

#include <QPixmap>
#include <QDomDocument>

#include "AbstractMovableModel.h"

namespace KIPIPhotoLayoutsEditor
{
    class AbstractPhoto;
    class PhotoEffectsLoader;
    class AbstractPhotoEffectInterface;

    class PhotoEffectsGroup : public AbstractMovableModel
    {
            Q_OBJECT

            AbstractPhoto * m_photo;
            QList<AbstractPhotoEffectInterface*> m_effects_list;

            class MoveItemsUndoCommand;
            class RemoveItemsUndoCommand;
            class InsertItemUndoCommand;

        public:

            explicit PhotoEffectsGroup(AbstractPhoto * photo, QObject * parent = 0);
            QDomElement toSvg(QDomDocument & document) const;
            static PhotoEffectsGroup * fromSvg(const QDomElement & element, AbstractPhoto * graphicsItem);
            AbstractPhoto * photo() const;
            virtual QObject * item(const QModelIndex & index) const;
            virtual void setItem(QObject * graphicsItem, const QModelIndex & index);
            AbstractPhotoEffectInterface * graphicsItem(const QModelIndex & index = QModelIndex()) const;
            bool moveRows(int sourcePosition, int sourceCount, int destPosition);
            bool insertRow(int row, AbstractPhotoEffectInterface * effect);
            bool insertRow(int row, const QModelIndex & index = QModelIndex());

            // Reimplemented QAbstractItemModel methods
            virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
            virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
            virtual Qt::ItemFlags flags(const QModelIndex & index) const;
            virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
            virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
            virtual QModelIndex parent(const QModelIndex & index) const;
            virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
            virtual bool removeRows(int row, int count, const QModelIndex & parent);

        signals:

            void effectsChanged();

        public slots:

            void push_back(AbstractPhotoEffectInterface * effect);
            void push_front(AbstractPhotoEffectInterface * effect);
            void emitEffectsChanged(AbstractPhotoEffectInterface * effect = 0);
            QPixmap apply(const QPixmap & pixmap);

        private:

            void moveRowsInModel(int sourcePosition, int sourceCount, int destPosition);
            QList<AbstractPhotoEffectInterface*> removeRowsInModel(int startingPosition, int count);
            void insertRemovedRowsInModel(const QList<AbstractPhotoEffectInterface*> & itemList, int startingPosition);
            void setEffectPointer(int row, AbstractPhotoEffectInterface * effect);

        friend class AbstractPhoto;
        friend class MoveItemsUndoCommand;
        friend class RemoveItemsUndoCommand;
        friend class InsertItemUndoCommand;
    };
}

#endif // ABSTRACTPHOTOEFFECTSGROUP_H
