#ifndef TEMPLATESVIEW_H
#define TEMPLATESVIEW_H

#include <QTableView>

namespace KIPIPhotoLayoutsEditor
{
    class TemplatesView : public QAbstractItemView
    {
            Q_OBJECT

            int columns;
            mutable int idealWidth;
            mutable int idealHeight;
            mutable bool hashIsDirty;

        public:

            explicit TemplatesView(QWidget * parent = 0);

            void mousePressEvent(QMouseEvent * event);
            void updateGeometries();
            void resizeEvent(QResizeEvent*);
            void paintOutline(QPainter * painter, const QRectF &rectangle);
            void paintEvent(QPaintEvent*);
            QRegion visualRegionForSelection(const QItemSelection &selection) const;
            void setSelection(const QRect &rect, QFlags<QItemSelectionModel::SelectionFlag> flags);
            void scrollContentsBy(int dx, int dy);
            int horizontalOffset() const;
            int verticalOffset() const;
            QModelIndex moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers);
            void rowsInserted(const QModelIndex & parent, int start, int end);
            void rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);
            void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
            QModelIndex indexAt(const QPoint &point_) const;
            void scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint);
            bool isIndexHidden(const QModelIndex&) const;
            QRectF viewportRectForRow(int row) const;
            QRect visualRect(const QModelIndex &index) const;
            void calculateRectsIfNecessary() const;
            void setModel(QAbstractItemModel * model);

            QString selectedPath() const;

        signals:

        public slots:

    };
}
#endif // TEMPLATESVIEW_H
