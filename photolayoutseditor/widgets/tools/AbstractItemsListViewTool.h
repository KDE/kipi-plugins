#ifndef ABSTRACTLISTTOOL_H
#define ABSTRACTLISTTOOL_H

#include "AbstractItemsTool.h"
#include "AbstractMovableModel.h"

namespace KIPIPhotoLayoutsEditor
{
    class AbstractItemsListViewToolPrivate;

    class AbstractListToolView;
    class AbstractListToolViewDelegate;

    class AbstractItemsListViewTool : public AbstractItemsTool
    {
            Q_OBJECT

        public:
            explicit AbstractItemsListViewTool(const QString & toolsName, Scene * scene, Canvas::SelectionMode selectionMode, QWidget * parent = 0);
            ~AbstractItemsListViewTool();
            virtual void currentItemAboutToBeChanged();
            virtual void currentItemChanged();
            virtual void positionAboutToBeChanged(){} // Unused
            virtual void positionChanged(){} // Unused
            virtual QStringList options() const = 0;
            virtual QObject * createItem(const QString & name) = 0;

        protected slots:

            virtual AbstractMovableModel * model() = 0;
            void viewCurrentEditor(const QModelIndex & index);
            virtual QWidget * createEditor(QObject * item, bool createCommands = true) = 0;
            void createChooser();
            void itemSelected(const QString & selectedItem);
            virtual void addItemCommand(QObject * item, int row);
            void chooserAccepted();
            void chooserCancelled();
            void removeSelected();
            void moveSelectedDown();
            void moveSelectedUp();

        private:
            void closeEditor();
            AbstractItemsListViewToolPrivate * d;
    };
}

#endif // ABSTRACTLISTTOOL_H
