#ifndef LAYERSTREE_H
#define LAYERSTREE_H

// Qt
#include <QMenu>
#include <QWidget>
#include <QAction>
#include <QTreeView>
#include <QAbstractItemModel>

// Local
#include "LayersModel.h"

class QGraphicsItem;

namespace KIPIPhotoLayoutsEditor
{
    class LayersTree;
    class LayersTreeMenu;
    class SwitchableIcon;

    class LayersTree : public QTreeView
    {
            Q_OBJECT

        public:

            explicit LayersTree(QWidget * parent = 0);
            virtual void setModel(QAbstractItemModel * model);

        public Q_SLOTS:

            void setSingleSelection();
            void setMultiSelection();

        signals:

            void selectedRowsAboutToBeRemoved();
            void selectedRowsAboutToBeMovedUp();
            void selectedRowsAboutToBeMovedDown();

        protected:

            void contextMenuEvent(QContextMenuEvent * event);

        protected Q_SLOTS:

            void removeSelectedRows();
            void moveSelectedRowsUp();
            void moveSelectedRowsDown();

        private:

            LayersTreeMenu * m_menu;
    };
}

#endif // LAYERSTREE_H
