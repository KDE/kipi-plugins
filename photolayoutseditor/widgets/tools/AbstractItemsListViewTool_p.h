#ifndef ABSTRACTLISTTOOL_P_H
#define ABSTRACTLISTTOOL_P_H

#include <QWidget>
#include <QListView>
#include <kpushbutton.h>

namespace KIPIPhotoLayoutsEditor
{
    class AbstractItemsListViewTool;

    class AbstractListToolViewDelegate : public QWidget
    {
            KPushButton * m_acceptButton;
            Q_OBJECT
        public:
            AbstractListToolViewDelegate(QWidget * parent = 0);
        signals:
            void editorClosed();
            void editorAccepted();
            void itemSelected(const QString & selectedItem);
        protected slots:
            void emitEditorClosed()
            {
                emit editorClosed();
            }
            void emitEditorAccepted()
            {
                emit editorAccepted();
            }
            void emitItemSelected(const QString & selectedItem)
            {
                m_acceptButton->setEnabled(!selectedItem.isEmpty());
                emit itemSelected(selectedItem);
            }
    };

    class AbstractListToolView : public QListView
    {
            Q_OBJECT
        public:
            AbstractListToolView(QWidget * parent = 0) :
                QListView(parent)
            {
                this->setSelectionMode(QAbstractItemView::SingleSelection);
            }
            QModelIndex selectedIndex() const
            {
                QModelIndexList indexes = selectedIndexes();
                if (indexes.count() == 1)
                    return indexes.at(0);
                return QModelIndex();
            }
        signals:
            void selectedIndex(const QModelIndex & index);
        protected:
            virtual void selectionChanged(const QItemSelection & selected, const QItemSelection & /*deselected*/)
            {
                QModelIndexList indexes = selected.indexes();
                if (indexes.count())
                {
                    QModelIndex index = indexes.at(0);
                    if (index.isValid())
                    {
                        emit selectedIndex(index);
                        return;
                    }
                }
                emit selectedIndex(QModelIndex());
            }

        friend class AbstractItemsListViewTool;
    };
}

#endif // ABSTRACTLISTTOOL_P_H
