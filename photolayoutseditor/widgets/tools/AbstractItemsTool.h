#ifndef ABSTRACTPROPERTIESMODEL_H
#define ABSTRACTPROPERTIESMODEL_H

#include <QWidget>

#include "AbstractTool.h"

namespace KIPIPhotoLayoutsEditor
{
    class AbstractPhoto;
    class EffectsListView;
    class ToolsDockWidget;

    class AbstractItemsTool : public AbstractTool
    {
            Q_OBJECT
            Q_PROPERTY(AbstractPhoto * m_photo READ currentItem WRITE setCurrentItem)

            AbstractPhoto * m_photo;
            QPointF m_point;

        public:

            AbstractItemsTool(Scene * scene, Canvas::SelectionMode selectionMode, QWidget * parent = 0);

          /** Current photo property
            * This property holds an information which item is currently editing.
            */
            Q_PROPERTY(AbstractPhoto * m_photo READ currentItem WRITE setCurrentItem)

            AbstractPhoto * currentItem();
            void setCurrentItem(AbstractPhoto * photo);

            QPointF mousePosition();
            void setMousePosition(const QPointF & position);

        signals:

            void itemCreated(AbstractPhoto * item);

        public slots:

          /** This slot is called before current item change
            * It gives a chanse to save changes of currently edited item.
            */
            virtual void currentItemAboutToBeChanged() = 0;

          /** This slot is called after current item changed.
            * This is a notification to open editors/tools and configure it for new item.
            */
            virtual void currentItemChanged() = 0;

          /** This slot is called before current mouse position change.
            * This is a notification for the editor/tool to clear it's drawing on the current
            * position.
            */
            virtual void positionAboutToBeChanged() = 0;

          /** This slot is called after current mouse position changed.
            * This is a notification for the editor/tool to draw it's data on the new position.
            */
            virtual void positionChanged() = 0;

    };
}

#endif // ABSTRACTPROPERTIESMODEL_H
