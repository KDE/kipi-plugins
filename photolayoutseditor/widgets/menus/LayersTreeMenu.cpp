#include "LayersTreeMenu.h"
#include "LayersTree.h"

using namespace KIPIPhotoLayoutsEditor;

LayersTreeMenu::LayersTreeMenu(LayersTree * parent) :
    KMenu(parent)
{
    moveUpItems = this->addAction("Move up");
    connect(moveUpItems, SIGNAL(triggered()), parent, SLOT(moveSelectedRowsUp()));
    moveDownItems = this->addAction("Move down");
    connect(moveDownItems, SIGNAL(triggered()), parent, SLOT(moveSelectedRowsDown()));
    this->addSeparator();
    deleteItems = this->addAction("Delete selected");
    connect(deleteItems, SIGNAL(triggered()), parent, SLOT(removeSelectedRows()));
}
