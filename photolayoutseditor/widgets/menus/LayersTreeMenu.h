#ifndef LAYERSTREEMENU_H
#define LAYERSTREEMENU_H

#include <kmenu.h>

namespace KIPIPhotoLayoutsEditor
{
    class LayersTree;

    class LayersTreeMenu : public KMenu
    {
        public:

            LayersTreeMenu(LayersTree * parent);
            void setMoveUpEnabled(bool enabled)
            {
                moveUpItems->setEnabled(enabled);
            }
            void setMoveDownEnabled(bool enabled)
            {
                moveDownItems->setEnabled(enabled);
            }
            void setDeleteEnabled(bool enabled)
            {
                deleteItems->setEnabled(enabled);
            }

        private:

            QAction * moveUpItems;
            QAction * moveDownItems;
            QAction * deleteItems;
    };
}


#endif // LAYERSTREEMENU_H
