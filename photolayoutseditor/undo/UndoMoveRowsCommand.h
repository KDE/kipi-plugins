#ifndef UNDOMOVEROWSCOMMAND_H
#define UNDOMOVEROWSCOMMAND_H

#include <QModelIndex>
#include <QUndoCommand>

namespace KIPIPhotoLayoutsEditor
{
    class LayersModel;
    class LayersModelItem;

    class UndoMoveRowsCommand : public QUndoCommand
    {
            LayersModel * m_model;
            LayersModelItem * m_src_parent_row;
            LayersModelItem * m_dest_parent_row;
            int m_starting_row;
            int m_rows_count;
            int m_destination_row;

        public:

            UndoMoveRowsCommand(int startingRow, int rowsCount, const QModelIndex & sourceParent, int destinationRow, const QModelIndex & destinationParent, LayersModel * model, QUndoCommand * parent = 0);
            virtual void redo();
            virtual void undo();

        private:

            void reverse();
    };
}

#endif // UNDOMOVEROWSCOMMAND_H
