#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "global.h"
#include "CanvasSize.h"

// Qt
#include <QObject>
#include <QSizeF>
#include <QRectF>
#include <QGraphicsScene>
#include <QItemSelection>
#include <QUndoStack>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QDomDocument>
#include <QFile>
#include <QDebug>

// KDE
#include <klocalizedstring.h>
#include <kurl.h>

namespace KIPIPhotoLayoutsEditor
{
    class Scene;
    class LayersModel;
    class LayersSelectionModel;
    class AbstractPhoto;
    class CanvasPrivate;

    class Canvas : public QGraphicsView
    {
            Q_OBJECT

            CanvasPrivate * d;

        public:

            enum SelectionMode
            {
                Viewing = 1,
                MultiSelecting = 2,
                SingleSelcting = 4,
            };

            explicit Canvas(const CanvasSize & size, QWidget * parent = 0);

            virtual void wheelEvent(QWheelEvent *event);

            QDomDocument toSvg() const;
            static Canvas * fromSvg(QDomDocument & document);

            void scale(qreal factor, const QPoint & center = QPoint());
            void scale(const QRect & rect);

            /// Hold URL to the file connected with this canvas.
            Q_PROPERTY(KUrl m_file READ file WRITE setFile)
            KUrl file() const;
            void setFile(const KUrl & file);

            /// Saves canvas state to SVG format file
            QString save(const KUrl & file, bool setAsDefault = true);

            /// Check if canvas is saved
            bool isSaved();

            /// Set selection mode
            void setSelectionMode(SelectionMode mode);

            Scene * scene() const
            {
                return m_scene;
            }

            LayersModel * model() const;

            LayersSelectionModel * selectionModel() const;

            QUndoStack * undoStack() const
            {
                return m_undo_stack;
            }

            CanvasSize canvasSize() const;
            void setCanvasSize(const CanvasSize & size);

            void preparePrinter(QPrinter * printer);

            operator Scene*()
            {
                return m_scene;
            }

            operator LayersModel*()
            {
                return this->model();
            }

            operator LayersSelectionModel*()
            {
                return this->selectionModel();
            }

            operator QUndoStack*()
            {
                return m_undo_stack;
            }

        public slots:

            void addImage(const QImage & image);
            void addText(const QString & text);

            /// Creates move rows command and pushes it onto the stack
            void moveRowsCommand(const QModelIndex & startIndex, int count, const QModelIndex & parentIndex, int move, const QModelIndex & destinationParent);

            /// Move selected items up on scene & model. (Called by layers tree)
            void moveSelectedRowsUp();

            /// Move selected items down on scene & model. (Called by layers tree)
            void moveSelectedRowsDown();

            /// Remove item selected on scene (remove from scene & model => calls removeComand())
            void removeItem(AbstractPhoto * item);

            /// Remove items selected on scene (remove from scene & model => calls removeComand())
            void removeItems(const QList<AbstractPhoto*> & items);

            /// Remove items selected on model (remove from model & scene => calls removeComand())
            void removeSelectedRows();

            /// Select items on model (synchronize model with scene)
            void selectionChanged();

            /// Select items on scene (synchronize scene with model)
            void selectionChanged(const QItemSelection & newSelection, const QItemSelection & oldSelection);

            /// Conrtols saved-state of the canvas
            void isSavedChanged(int currentCommandIndex);
            void isSavedChanged(bool isStackClean);

            /// Draws whole canvas onto the QPaintDevice
            void renderCanvas(QPaintDevice * device);

            /// Draws whole canvas content onto the printer
            void renderCanvas(QPrinter * device);

            /// Groups operations into one undo operation
            void beginRowsRemoving()
            {
                m_undo_stack->beginMacro(i18n("Remove items"));
            }

            /// Finish group of undo operations
            void endRowsRemoving()
            {
                m_undo_stack->endMacro();
            }

            /// Sets selecting mode
            void enableDefaultSelectionMode();

            /// Sets viewing mode
            void enableViewingMode();

            /// Sets canvas editing mode
            void enableCanvasEditingMode();

            /// Sets effects editing mode
            void enableEffectsEditingMode();

            /// Sets text editing mode
            void enableTextEditingMode();

            /// Sets rotating mode
            void enableRotateEditingMode();

            /// Sets scaling mode
            void enableScaleEditingMode();

            /// Sets cropping mode
            void enableCropEditingMode();

            /// Sets borders editing mode
            void enableBordersEditingMode();

            /// Refresh widgets connections to canvas signals
            void refreshWidgetConnections(bool isVisible);

            /// Appends new undo command
            void newUndoCommand(QUndoCommand * command);

        signals:

            void hasSelectionChanged(bool hasSelection);
            void selectedItem(AbstractPhoto * photo);
            void setInitialValues(qreal width, Qt::PenJoinStyle cornersStyle, const QColor & color);
            void savedStateChanged();

        protected slots:

            /// Used when new item has been created and needs to be added to the scene and to the model
            void addNewItem(AbstractPhoto * item);
            void setAntialiasing(bool antialiasing);

        private:

            explicit Canvas(Scene * scene, QWidget * parent = 0);

            void init();
            void setupGUI();
            void prepareSignalsConnection();

            KUrl m_file;
            bool m_is_saved;
            int m_saved_on_index;

            Scene * m_scene;
            QUndoStack * m_undo_stack;
            double m_scale_factor;

            SelectionMode m_selection_mode;

        friend class CanvasPrivate;
    };
}

#endif // DOCUMENT_H
