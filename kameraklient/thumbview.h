#ifndef THUMBVIEW_H
#define THUMBVIEW_H

#include <qscrollview.h>

class QPainter;
class QMouseEvent;
class QPaintEvent;
class QDropEvent;
class QPoint;

namespace KIPIKameraKlientPlugin
{

class ThumbItem;
class ThumbViewPrivate;

class ThumbView : public QScrollView {

    Q_OBJECT

    friend class ThumbItem;
    
public:

    ThumbView(QWidget* parent=0, const char* name=0,
              WFlags fl=0);
    ~ThumbView();

    ThumbItem* firstItem();
    ThumbItem* lastItem();
    ThumbItem* findItem(const QPoint& pos);
    ThumbItem* findItem(const QString& text);

    int count();
    int index(ThumbItem* item);
    
    virtual void clear(bool update=true);
    void rearrangeItems(bool update=true);

    void clearSelection();
    void selectAll();
    void invertSelection();

    void selectItem(ThumbItem* item, bool select);

    virtual void insertItem(ThumbItem *item);
    virtual void takeItem(ThumbItem *item);
    void updateItemContainer(ThumbItem *item);
    QRect contentsRectToViewport(const QRect& r);

    void ensureItemVisible(ThumbItem *item);
    ThumbItem *findFirstVisibleItem(const QRect &r ) const;
    ThumbItem *findLastVisibleItem(const  QRect &r ) const;

    void sort();
    
protected:

    virtual void contentsMousePressEvent(QMouseEvent *e);
    virtual void contentsMouseMoveEvent(QMouseEvent *e);
    virtual void contentsMouseReleaseEvent(QMouseEvent *e);
    virtual void contentsMouseDoubleClickEvent(QMouseEvent *e);
    
    virtual void viewportPaintEvent(QPaintEvent *pe);
    virtual void resizeEvent(QResizeEvent* e);

    virtual void keyPressEvent(QKeyEvent *e);
        
    virtual void startDrag();
    virtual void contentsDropEvent(QDropEvent *e);

private:

    void drawRubber(QPainter *p);

    void rebuildContainers();
    void appendContainer();
    void deleteContainers();

private:

    ThumbItem* makeRow(ThumbItem *begin, int &y, bool &changed);
    void emitRenamed(ThumbItem *item);

private:


    ThumbViewPrivate *d;
    ThumbItem *renamingItem;

signals:

    void signalSelectionChanged();
    void signalRightButtonClicked(const QPoint &pos);
    void signalRightButtonClicked(ThumbItem *item, const QPoint &pos);
    void signalDoubleClicked(ThumbItem *item);
    void signalReturnPressed(ThumbItem *item);
    void signalItemRenamed(ThumbItem *item);

public slots:

    void slotUpdate();
    
};

}  // NameSpace KIPIKameraKlientPlugin

#endif
