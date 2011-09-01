#ifndef CANVASCREATIONDIALOG_H
#define CANVASCREATIONDIALOG_H

#include "CanvasSize.h"

// KDE
#include <kdialog.h>

// Qt
#include <QSize>

namespace KIPIPhotoLayoutsEditor
{
    class CanvasSizeDialogPrivate;

    class CanvasSizeDialog : public KDialog
    {
            Q_OBJECT

        public:

            explicit CanvasSizeDialog(QWidget * parent = 0);
            explicit CanvasSizeDialog(const CanvasSize & canvasSize, QWidget * parent = 0);
            ~CanvasSizeDialog();

            Q_PROPERTY(CanvasSize canvas_size READ canvasSize)
            CanvasSize canvasSize() const;

        protected Q_SLOTS:

            void recalculatePaperSize(const QString & paperSize);
            void sizeUnitsChanged(const QString & unit);
            void resolutionUnitsChanged(const QString & unit);
            void setHorizontal(bool);
            void setVertical(bool);
            void widthChanged(double width);
            void heightChanged(double height);
            void xResolutionChanged(double xResolution);
            void yResolutionChanged(double yResolution);

        private:

            void setupDialog(const QSizeF & size, const QString & sizeUnits, const QSizeF & resolution, const QString & resolutionUnits);
            void prepareSignalsConnections();

            CanvasSizeDialogPrivate * d;
    };
}

#endif // CANVASCREATIONDIALOG_H
