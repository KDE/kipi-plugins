#ifndef CANVASEDITTOOL_H
#define CANVASEDITTOOL_H

#include <QWidget>

#include <kcombobox.h>

#include "AbstractTool.h"

namespace KIPIPhotoLayoutsEditor
{
    class CanvasEditToolPrivate;
    class CanvasEditTool : public AbstractTool
    {
            Q_OBJECT

            CanvasEditToolPrivate * d;
            bool hold_update;

        public:

            explicit CanvasEditTool(Scene * scene, QWidget * parent = 0);
            virtual ~CanvasEditTool();

        signals:

        public slots:

            void backgroundTypeChanged(const QString & typeName);

        protected:

            virtual void sceneChange();
            virtual void sceneChanged();

        protected slots:

            // Type of background selection
            void colorBackgroundSelected();
            void gradientBackgroundSelected();
            void imageBackgroundSelected();
            void patternBackgroundSelected();

            // Solid background
            void solidColorChanged(const QColor & color);
            void imageBackgroundColorChanged(const QColor & color);

            // Pattern background
            void patternFirstColorChanged(const QColor & color);
            void patternSecondColorChanged(const QColor & color);
            void patternStyleChanged(Qt::BrushStyle patternStyle);

            // Image background
            void imageUrlRequest();
            void imageScallingChanged(const QString & scallingName);
            void imageTiledChanged(int state);
            void imageHorizontalAlignmentChanged(int index);
            void imageVerticalAlignmentChanged(int index);
            void imageWidthChanged();
            void imageHeightChanged();

            // Other (currently unused... :P)
            void readMousePosition(const QPointF & scenePos);

        private:

            void setImageBackground();
            void setPatternBackground();
            void setupGUI();
    };
}

#endif // CANVASEDITTOOL_H
