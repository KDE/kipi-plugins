#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsScene>
#include <QPainter>
#include <QColor>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QGraphicsItem>

// Local
#include "PhotoItem.h"
#include "TextItem.h"

namespace KIPIPhotoLayoutsEditor
{
    class ScenePrivate;

    // My classes declarations
    class Canvas;
    class SceneBackground;
    class MousePressListener;
    class LayersModel;
    class LayersSelectionModel;

    // Undo commands
    class AddItemsCommand;
    class MoveItemsCommand;
    class RemoveItemsCommand;
    class RotateItemCommand;
    class CropItemsCommand;

    class Scene : public QGraphicsScene
    {
            Q_OBJECT

            ScenePrivate * d;

            enum EditMode
            {
                View = 0,
                Selecting = 1,
                Moving = 2,
                MouseTracking = 4,
                OneclickFocusItems = 8,
                Rotating = 16,
                Scaling = 32,
                Cropping = 64,
            };

            enum SelectionMode
            {
                NoSelection = 0,
                SingleSelection = 1,
                MultiSelection = 2,
            };

        public:

            explicit Scene(const QRectF & dimension, QObject * parent = 0);
            ~Scene();

            SceneBackground * background();

            LayersModel * model() const;
            LayersSelectionModel * selectionModel() const;

            void addItem(AbstractPhoto * photo);
            void addItems(const QList<AbstractPhoto*> & item);
            void removeItem(AbstractPhoto * item);
            void removeItems(const QList<AbstractPhoto*> & items);
            QList<AbstractPhoto*> selectedItems() const;
            void setInteractionMode(int mode);
            void setSelectionMode(SelectionMode selectionMode);
            bool isSelectionVisible();
            QDomElement toSvg(QDomDocument & document);
            static Scene * fromSvg(QDomElement & svgImage);
            void addSelectingFilter(const QMetaObject & classMeta);
            void clearSelectingFilters();
            void setRotationWidgetVisible(bool isVisible);
            void setScalingWidgetVisible(bool isVisible);
            void setCropWidgetVisible(bool isVisible);
            qreal gridHorizontalDistance() const;
            qreal gridVerticalDistance() const;
            const QGraphicsScene * toGraphicsScene() const
            {
                return this;
            }
            QGraphicsScene * toGraphicsScene()
            {
                return this;
            }
            void render(QPainter * painter, const QRectF & target = QRectF(), const QRectF & source = QRectF(), Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio);

            void readSceneMousePress(MousePressListener * mouseListener);

        Q_SIGNALS:

            void finishEditing(const QPainterPath & path);
            void itemAboutToBeRemoved(AbstractPhoto * item);
            void itemsAboutToBeRemoved(const QList<AbstractPhoto*> & items);
            void mousePressedPoint(const QPointF & point);

        public Q_SLOTS:

            void removeSelectedItems();
            void setGrid(double x, double y);
            void setHorizontalGrid(double x);
            void setVerticalGrid(double y);
            void setGridVisible(bool visible);
            bool isGridVisible();
            void updateSelection();
            void setSelectionVisible(bool isVisible);
            void closeCropWidget();

        protected:

            virtual void contextMenuEvent(QGraphicsSceneMouseEvent * event);
            virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
            virtual void drawBackground(QPainter * painter, const QRectF & rect);
            virtual void drawForeground(QPainter * painter, const QRectF & rect);
            virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
            virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
            virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
            virtual void dropEvent(QGraphicsSceneDragDropEvent * event);
            virtual void keyPressEvent(QKeyEvent * event);
            virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
            virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
            virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
            virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);

        private Q_SLOTS:

            void calcSelectionBoundingRect();

        private:

            bool askAboutRemoving(int count);
            bool canDecode(const QMimeData * mimeData);

            int m_interaction_mode;
            static const int DEFAULT_EDITING_MODE = Moving & Selecting;

            SelectionMode selectionMode;
            static const SelectionMode DEFAULT_SELECTING_MODE = MultiSelection;

            QGraphicsPathItem * temp_widget;
            QPainterPath temp_path;

            // Grid properties
            qreal x_grid;
            qreal y_grid;
            bool  grid_visible;
            QGraphicsItemGroup * grid_item;
            bool grid_changed;

            static QColor OUTSIDE_SCENE_COLOR;

            friend class Canvas;
            friend class CanvasWidget;
            friend class ScenePrivate;
            friend class AbstractPhoto;
            friend class MoveItemsCommand;
    };

}

#endif // CANVAS_H
