#include "UndoMoveRowsCommand.h"
#include "LayersModel.h"
#include "LayersModelItem.h"

using namespace KIPIPhotoLayoutsEditor;

UndoMoveRowsCommand::UndoMoveRowsCommand(int startingRow, int rowsCount, const QModelIndex & sourceParent, int destinationRow, const QModelIndex & destinationParent, LayersModel * model, QUndoCommand * parent) :
    QUndoCommand(parent),
    m_model(model)
{
    if (m_model)
    {
        if (sourceParent == destinationParent)
        {
            if (startingRow > destinationRow)
                this->setText("Move up");
            else
                this->setText("Move down");
        }
        else
            this->setText("Change parent");
        m_src_parent_row = model->getItem(sourceParent);
        m_dest_parent_row = model->getItem(destinationParent);
        m_starting_row = startingRow;
        m_rows_count = rowsCount;
        m_destination_row = destinationRow;
    }
#ifdef QT_DEBUG
    else
    {
        qDebug() << "Can't create UndoMoveRowsCommand [NO MODEL!]:";
        qDebug() << "\tStarting Row =" << startingRow;
        qDebug() << "\tRows count =" << rowsCount;
        qDebug() << "\tDestination Row =" << destinationRow;
        qDebug() << "\tSource Parent =" << sourceParent;
        qDebug() << "\tDestination Parent =" << destinationParent;
    }
#endif
}

void UndoMoveRowsCommand::redo()
{
    if (m_model &&
        m_model->moveRows( m_starting_row, m_rows_count, m_model->findIndex(m_src_parent_row), m_destination_row, m_model->findIndex(m_dest_parent_row) ))
    {
        reverse();
    }
#ifdef QT_DEBUG
    else
    {
        qDebug() << "Can't redo from UndoMoveRowsCommand:";
        qDebug() << "\tStarting Row =" << m_starting_row;
        qDebug() << "\tRows count =" << m_rows_count;
        qDebug() << "\tDestination Row =" << m_destination_row;
        qDebug() << "\tSource Parent =" << m_model->findIndex(m_src_parent_row);
        qDebug() << "\tDestination Parent =" << m_model->findIndex(m_dest_parent_row);
    }
#endif
}

void UndoMoveRowsCommand::undo()
{
    if (m_model &&
        m_model->moveRows( m_starting_row, m_rows_count, m_model->findIndex(m_src_parent_row), m_destination_row, m_model->findIndex(m_dest_parent_row) ))
    {
        reverse();
    }
#ifdef QT_DEBUG
    else
    {
        qDebug() << "Can't undo from UndoMoveRowsCommand:";
        qDebug() << "\tStarting Row =" << m_starting_row;
        qDebug() << "\tRows count =" << m_rows_count;
        qDebug() << "\tDestination Row =" << m_destination_row;
        qDebug() << "\tSource Parent =" << m_model->findIndex(m_src_parent_row);
        qDebug() << "\tDestination Parent =" << m_model->findIndex(m_dest_parent_row);
    }
#endif
}

void UndoMoveRowsCommand::reverse()
{
    int temp = m_destination_row;
    m_destination_row = m_starting_row;
    m_starting_row = temp;
    if (m_dest_parent_row == m_src_parent_row)
    {
        if (m_destination_row > m_starting_row)
            m_destination_row += m_rows_count;
        else
            m_starting_row -= m_rows_count;
    }
    else
    {
        LayersModelItem * temp2 = m_dest_parent_row;
        m_dest_parent_row = m_src_parent_row;
        m_src_parent_row = temp2;
    }
}
