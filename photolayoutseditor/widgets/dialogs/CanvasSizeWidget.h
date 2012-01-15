#ifndef CANVASSIZEWIDGET_H
#define CANVASSIZEWIDGET_H

#include <QWidget>

namespace KIPIPhotoLayoutsEditor
{
    class CanvasSize;
    class CanvasSizeWidget : public QWidget
    {
            Q_OBJECT

        public:

            enum Orientation
            {
                Horizontal,
                Vertical
            };

            explicit CanvasSizeWidget(QWidget *parent = 0);
            Orientation orientation() const;
            CanvasSize canvasSize() const;

        signals:

            void orientationChanged();

        public slots:

            void sizeUnitsChanged(const QString & unitName);
            void resolutionUnitsChanged(const QString & unitName);
            void setHorizontal(bool isHorizontal);
            void setVertical(bool isVertical);
            void widthChanged(double width);
            void heightChanged(double height);
            void xResolutionChanged(double xResolution);
            void yResolutionChanged(double yResolution);

        private:

            void setupUI(const QSizeF & size, const QString & sizeUnits, const QSizeF & resolution, const QString & resolutionUnits);

            void prepareSignalsConnections();

            class Private;
            Private * d;
            friend class Private;
    };
}

#endif // CANVASSIZEWIDGET_H
