// Qt
#include <qpainter.h>
#include <qrect.h>
#include <qpoint.h>
#include <qsize.h>
#include <qevent.h>
#include <qstring.h>
#include <qstyle.h>
#include <qpixmap.h>
#include <qptrlist.h>
#include <qcursor.h>
#include <qtimer.h>
#include <qdragobject.h>
#include <qstrlist.h>
#include <qapplication.h>
// KDE
#include <kiconloader.h>
// Standard
#include <stdlib.h>
#include <iostream>

// To get INT_MAX
extern "C" {
#include <limits.h>
}
// Local
#include "thumbitem.h"
#include "thumbview.h"


#define RECT_EXTENSION 300

class ThumbViewPrivate {

public:
    ThumbItem *firstItem;
    ThumbItem *lastItem;
    int spacing;
    int count;
    bool clearing;
    bool pressedMoved;
    QRect *rubber;
    QPoint dragStartPos;
    QPtrList<ThumbItem> selectedItems;
    QTimer* updateTimer;
    struct ItemContainer {
	ItemContainer(ItemContainer *p, ItemContainer *n, const QRect &r) : prev(p), next(n), rect(r) {
		items.setAutoDelete(false);
		if (prev) {
			prev->next = this;
		}
		if (next) {
			next->prev = this;
		}
	}
	ItemContainer *prev, *next;
	QRect rect;
	QPtrList<ThumbItem> items;
    } *firstContainer, *lastContainer;
    ThumbItem *startDragItem;
    struct SortableItem {
        ThumbItem *item;
    };   
};


static int cmpItems( const void *n1, const void *n2 ) {
    if (!n1 || !n2) {
        return 0;
    }
    ThumbViewPrivate::SortableItem *i1 = (ThumbViewPrivate::SortableItem *)n1;
    ThumbViewPrivate::SortableItem *i2 = (ThumbViewPrivate::SortableItem *)n2;
    return i1->item->compare( i2->item );
}


ThumbView::ThumbView(QWidget* parent, const char* name, WFlags fl) : QScrollView(parent, name, Qt::WStaticContents | fl) {
    setBackgroundMode(Qt::NoBackground);
    viewport()->setBackgroundMode(Qt::NoBackground);
    viewport()->setFocusProxy(this);
    viewport()->setFocusPolicy(QWidget::TabFocus);
    renamingItem = 0;
    d = new ThumbViewPrivate;
    d->firstItem = 0;
    d->lastItem  = 0;
    d->spacing = 5;
    d->count = 0;
    d->clearing = false;
    d->pressedMoved = false;
    d->rubber = 0;
    d->firstContainer = 0;
    d->lastContainer  = 0;
    d->selectedItems.setAutoDelete(false);
    d->updateTimer = new QTimer(this);
    d->startDragItem = 0;
    connect(d->updateTimer, SIGNAL(timeout()), this, SLOT(slotUpdate()));
}

ThumbView::~ThumbView() {
    clear(false);
    if (d->rubber) {
        delete d->rubber;
    }
    delete d->updateTimer;
    delete d;
}

void ThumbView::clear(bool update) {
    d->clearing = true;
    renamingItem = 0;
    deleteContainers();
    d->selectedItems.clear();
    emit signalSelectionChanged();
    ThumbItem *item = d->firstItem;
    while (item) {
        ThumbItem *tmp = item->next;
        delete item;
        item = tmp;
    }
    d->firstItem = 0;
    d->lastItem = 0;
    viewport()->setUpdatesEnabled(false);
    resizeContents(0, 0);
    viewport()->setUpdatesEnabled(true);
    if (update) {
        updateContents();
    }
    d->clearing = false;
}

int ThumbView::count() {
    return d->count;    
}

int ThumbView::index(ThumbItem* item) {
    if (!item) {
	return -1;
    }
    if (item == d->firstItem) {
	return 0;
    } else if ( item == d->lastItem ) {
	return d->count - 1;
    } else {
	ThumbItem *i = d->firstItem;
	int j = 0;
	while ( i && i != item ) {
	    i = i->next;
	    ++j;
	}
	return i ? j : -1;
    }
}

ThumbItem* ThumbView::firstItem() {
    return d->firstItem;
}

ThumbItem* ThumbView::lastItem() {
    return d->lastItem;
}

void ThumbView::insertItem(ThumbItem *item) {
    if (!item) {
	return;
    }
    if (!d->firstItem) {
        d->firstItem = item;
        d->lastItem = item;
        item->prev = 0;
        item->next = 0;
    } else {
        d->lastItem->next = item;
        item->prev = d->lastItem;
        item->next = 0;
        d->lastItem = item;
    }
    d->count++;
    // this way one can insert items in a loop without too many paintevents
    d->updateTimer->start(0, true);
}

void ThumbView::takeItem(ThumbItem *item) {
    if (!item) {
	return;
    }
    d->count--;
    // First remove item from any containers holding it
    ThumbViewPrivate::ItemContainer *tmp = d->firstContainer;
    while (tmp) {
        tmp->items.remove(item);
        tmp = tmp->next;
    }
    // Remove from selected item list
    d->selectedItems.remove(item);
    if (item == d->firstItem) {
	d->firstItem = d->firstItem->next;
	if (d->firstItem) {
	    d->firstItem->prev = 0;
	} else {
            d->firstItem = d->lastItem = 0;
	}
    } else if (item == d->lastItem) {
	d->lastItem = d->lastItem->prev;
	if ( d->lastItem ) {
	    d->lastItem->next = 0;
	} else {
            d->firstItem = d->lastItem = 0;
	}
    } else {
	ThumbItem *i = item;
	if (i) {
	    if (i->prev) {
		i->prev->next = i->next;
	    }
	    if (i->next) {
		i->next->prev = i->prev;
	    }
	}
    }
    if (!d->clearing) {
        QRect r(contentsRectToViewport(item->rect())); 
        viewport()->repaint(r);
    }
}

void ThumbView::slotUpdate() {
    d->updateTimer->stop();
    sort();
    rearrangeItems();
}

void ThumbView::sort() {
    ThumbViewPrivate::SortableItem *items = new ThumbViewPrivate::SortableItem[ count() ];
    ThumbItem *item = d->firstItem;
    int i = 0;
    for ( ; item; item = item->next) {
        items[ i++ ].item = item;
    }
    qsort( items, count(), sizeof( ThumbViewPrivate::SortableItem ), cmpItems );
    ThumbItem *prev = 0;
    item = 0;
    for (i = 0; i < (int)count(); ++i) {
        item = items[ i ].item;
        if ( item ) {
            item->prev = prev;
            if (item->prev) {
                item->prev->next = item;
	    }
            item->next = 0;
        }
        if (i == 0) {
            d->firstItem = item;
	}
        if (i == (int)count() - 1) {
            d->lastItem = item;
	}
        prev = item;
    }
    delete [] items;
}

void ThumbView::viewportPaintEvent(QPaintEvent *pe) {
    QRect r(pe->rect());
    QRegion paintRegion(pe->region());
    QPainter painter(viewport());
    painter.setClipRegion(paintRegion);
    ThumbViewPrivate::ItemContainer *c = d->firstContainer;
    for ( ; c; c = c->next) {
        QRect cr(contentsRectToViewport(c->rect));
        if (r.intersects(cr)) {
            ThumbItem *item = c->items.first();
            for ( ; item; item = c->items.next()) {
                QRect ir(contentsRectToViewport(item->rect()));
                if (r.intersects(ir)) {
                    item->paintItem(&painter, colorGroup());
                    paintRegion -= QRegion(ir);
                }
            }
        }
    }
    painter.setClipRegion(paintRegion);
    painter.fillRect(r, colorGroup().base());
    painter.end();
}

void ThumbView::resizeEvent(QResizeEvent* e) {
    QScrollView::resizeEvent(e);
    //d->updateTimer->start(0, true);
    rearrangeItems();
}

void ThumbView::rearrangeItems(bool update) {
    if (!d->firstItem || !d->lastItem) {
        return;
    }
    int w = 0, h = 0, y = d->spacing;
    ThumbItem *item = d->firstItem;
    bool changedLayout = false;
    while (item) {
        bool changed;
        ThumbItem *next = makeRow(item, y, changed);
        changedLayout = changed || changedLayout;
        item = next;
        w = QMAX(w, item->x() + item->width());
        h = QMAX(h, item->y() + item->height());
        h = QMAX(h, y);
        if (!item || !item->next) {
            break;
	}
        item = item->next;
    }
    w = QMAX(w, d->lastItem->x() + d->lastItem->width());
    h = QMAX(h, d->lastItem->y() + d->lastItem->height());
    int vw = visibleWidth();
    viewport()->setUpdatesEnabled(false);
    resizeContents( w, h );
    bool doAgain = visibleWidth() != vw;
    if (doAgain) {
        rearrangeItems(false);
    }
    viewport()->setUpdatesEnabled(true);
    rebuildContainers();
    if (changedLayout && update) {
        viewport()->update();
    } 
}

ThumbItem* ThumbView::makeRow(ThumbItem *begin, int &y, bool &changed) {
    ThumbItem *end = 0;
    changed = false;
    // first calculate the row height
    int h = 0;
    int x = 0;
    ThumbItem *item = begin;
    for (;;) {
        x += d->spacing + item->width();
        //int maxW = visibleWidth();
        int maxW = frameRect().width() - 20;
        if (x  > maxW && item != begin) {
            item = item->prev;
            break;
        }
        h = QMAX(h, item->height());
        ThumbItem *old = item;
        item = item->next;
        if (!item) {
            item = old;
            break;
        }
    }
    end = item;
    // now move the items
    item = begin;
    for (;;) {
        int x;
        if (item == begin) {
            x = d->spacing;
        } else {
            x = item->prev->x() + item->prev->width() + d->spacing;
        }
        changed = item->move(x, y) || changed;
        if (item == end) {
            break;
	}
        item = item->next;
    }
    y += h + d->spacing;
    return end;
}

void ThumbView::drawRubber(QPainter *p) {
    if (!p || !d->rubber) {
        return;
    }
    QRect r(d->rubber->normalize());
    r = contentsRectToViewport(r);
    QPoint pnt(r.x(), r.y());
    style().drawPrimitive(QStyle::PE_FocusRect, p, QRect( pnt.x(), pnt.y(), r.width(), r.height()), colorGroup(), QStyle::Style_Default, QStyleOption(colorGroup().base()));
}

void ThumbView::contentsMousePressEvent(QMouseEvent *e) {
    // If renaming any item, cancel it --------------------------
    if (renamingItem) {
        renamingItem->cancelRenameItem();
    }
    // Delete any existing rubber -------------------------------
    if ( d->rubber ) {
	QPainter p;
	p.begin(viewport());
	p.setRasterOp(NotROP);
	p.setPen(QPen(color0, 1));
	p.setBrush(NoBrush);
	drawRubber(&p);
	p.end();
	delete d->rubber;
	d->rubber = 0;
    }
    d->dragStartPos = e->pos();
    ThumbItem *item = findItem(e->pos());
    if (item) {
        if (e->state() & Qt::ControlButton) {
            item->setSelected(!item->isSelected(), false);
        } else if (e->state() & Qt::ShiftButton) {
            // different selection mode than the Trolls
            ThumbItem *lastSelectedItem = 0;
            bool bwdSelection = false;
            // first go backwards
            for (ThumbItem *it = item->prev; it; it = it->prev) {
                if (it->isSelected()) {
                    lastSelectedItem = it;
                    bwdSelection = true;
                    break;
                }
            }
            bool fwdSelection = false;
            if (!lastSelectedItem) {
                // Now go forward
                for (ThumbItem *it = item->next; it; it = it->next) {
                    if (it->isSelected()) {
                        lastSelectedItem = it;
                        fwdSelection = true;
                        break;
                    }
                }
            }
            blockSignals(true);
            if (bwdSelection) {
                for (ThumbItem *it = lastSelectedItem;
                     it && it != item->next; it = it->next) {
                    if (!it->isSelected()) {
                        it->setSelected(true, false);
                    }
                }
            } else if (fwdSelection) {
                for (ThumbItem *it = item;
                     it && it != lastSelectedItem->next; it = it->next) {
                    if (!it->isSelected()) {
                        it->setSelected(true, false);
                    }
                }
            } else {
                item->setSelected(!item->isSelected(), false);
            }
            blockSignals(false);
            emit signalSelectionChanged();
        } else {
            if (!item->isSelected()) {
                item->setSelected(true, true);
            }
        }
        d->startDragItem = item;
        return;
    }
    // Press outside any item. unselect all
    clearSelection();
    // If not item then initiate rubber
    if ( d->rubber ) {
        delete d->rubber;
        d->rubber = 0;
    }
    d->rubber = new QRect( e->x(), e->y(), 0, 0 );
    QPainter p;
    p.begin( viewport() );
    p.setRasterOp( NotROP );
    p.setPen( QPen( color0, 1 ) );
    p.setBrush( NoBrush );
    drawRubber( &p );
    p.end();
    d->pressedMoved = false;
}

void ThumbView::contentsMouseMoveEvent(QMouseEvent *e) {
    if (!e) {
	return;
    }
    if (e->state() == NoButton) {
        return;
    }
    // Dragging ?
    if (d->startDragItem) {
        if ((d->dragStartPos - e->pos() ).manhattanLength() > QApplication::startDragDistance()) {
            startDrag();
        }
        return;
    }
    if (!d->rubber) {
	return;
    }
    QRect oldRubber = QRect(*d->rubber);
    d->rubber->setRight( e->pos().x() );
    d->rubber->setBottom( e->pos().y() );
    QRegion paintRegion;
    viewport()->setUpdatesEnabled(false);
    QRect nr(d->rubber->normalize());
    QRect rubberUnion = nr.unite(oldRubber.normalize());
    bool changed = false;
    ThumbViewPrivate::ItemContainer *c = d->lastContainer;
    for (; c; c = c->prev) {
	if ( rubberUnion.intersects(c->rect) ) {
            ThumbItem *item = c->items.last();
	    for ( ; item; item = c->items.prev() ) {
                if (nr.intersects(item->rect())) {
                    if (!item->isSelected()) {
                        item->setSelected(true, false);
                        changed = true;
                        paintRegion += QRect(item->rect());
                    }
                } else {
                    if (item->isSelected()) {
                        item->setSelected(false, false);
                        changed = true;
                        paintRegion += QRect(item->rect());
                    }
                }
            }
        }
    }
    viewport()->setUpdatesEnabled(true);
    QRect r = *d->rubber;
    *d->rubber = oldRubber;
    QPainter p;
    p.begin( viewport() );
    p.setRasterOp( NotROP );
    p.setPen( QPen( color0, 1 ) );
    p.setBrush( NoBrush );
    drawRubber( &p );
    p.end();
    if (changed) {
        emit signalSelectionChanged();
        paintRegion.translate(-contentsX(), -contentsY());
        viewport()->repaint(paintRegion);
    }
    ensureVisible(e->pos().x(), e->pos().y());
    *d->rubber = r;
    p.begin(viewport());
    p.setRasterOp(NotROP);
    p.setPen(QPen(color0, 1));
    p.setBrush(NoBrush);
    drawRubber(&p);
    p.end();
    d->pressedMoved = true;
}

void ThumbView::contentsMouseReleaseEvent(QMouseEvent *e) {
    if (!e) {
	return;
    }
    d->startDragItem = 0;
    if (d->rubber) {
        QPainter p;
	p.begin( viewport() );
	p.setRasterOp( NotROP );
	p.setPen( QPen( color0, 1 ) );
	p.setBrush( NoBrush );
	drawRubber( &p );
	p.end();
        delete d->rubber;
        d->rubber = 0;
    }
    if (e->button() == Qt::RightButton) {
        ThumbItem *item = findItem(e->pos());
        if (item) {
            emit signalRightButtonClicked(item, e->globalPos());
	} else {
            emit signalRightButtonClicked(e->globalPos());
	}
    } else if ((e->button() == Qt::LeftButton) && !(e->state() & Qt::ShiftButton) && !(e->state() & Qt::ControlButton)) {
        if (d->pressedMoved) {
            d->pressedMoved = false;
            return;
        }
        ThumbItem *item = findItem(e->pos());
        if (item) {
            item->setSelected(true, true);
	}
    }
}

void ThumbView::contentsMouseDoubleClickEvent(QMouseEvent *e) {
    ThumbItem *item = findItem(e->pos());
    if (item) {
        blockSignals(true);
        clearSelection();
        if (renamingItem) {
            renamingItem->cancelRenameItem();
	}
        blockSignals(false);
        item->setSelected(true);
        emit signalDoubleClicked(item);
    }
}

void ThumbView::rebuildContainers() {
    deleteContainers();
    ThumbItem *item = d->firstItem;
    appendContainer();
    ThumbViewPrivate::ItemContainer* c = d->lastContainer;
    while (item) {
	if (c->rect.contains(item->rect())) {
	    c->items.append(item);
	    item = item->next;
	} else if (c->rect.intersects(item->rect())) {
	    c->items.append( item );
	    c = c->next;
	    if (!c) {
		appendContainer();
		c = d->lastContainer;
	    }
	    c->items.append(item);
	    item = item->next;
	    c = c->prev;
	} else {
            if (item->y() < c->rect.y() && c->prev) {
                c = c->prev;
                continue;
            }
	    c = c->next;
	    if (!c) {
		appendContainer();
		c = d->lastContainer;
	    }
	}
    }
}

void ThumbView::appendContainer() {
    QSize s;
    s = QSize(INT_MAX - 1, RECT_EXTENSION );
    if (!d->firstContainer) {
	d->firstContainer = new ThumbViewPrivate::ItemContainer(0, 0, QRect(QPoint(0, 0), s));
	d->lastContainer = d->firstContainer;
    } else {
        d->lastContainer = new ThumbViewPrivate::ItemContainer(d->lastContainer, 0, QRect(d->lastContainer->rect.bottomLeft(), s));
    }  
}

void ThumbView::deleteContainers() {
    ThumbViewPrivate::ItemContainer *c = d->firstContainer, *tmpc;
    while (c) {
	tmpc = c->next;
	delete c;
	c = tmpc;
    }
    d->firstContainer = d->lastContainer = 0;
}

void ThumbView::updateItemContainer(ThumbItem *item) {
    if (!item)	return;
    // First remove item from any containers holding it
    ThumbViewPrivate::ItemContainer *tmp = d->firstContainer;
    while (tmp) {
        tmp->items.remove(item);
        tmp = tmp->next;
    }
    ThumbViewPrivate::ItemContainer *c = d->firstContainer;
    if (!c) {
	appendContainer();
	c = d->firstContainer;
    }
    const QRect ir = item->rect();
    bool contains = false;
    for (;;) {
	if (c->rect.intersects(ir)) {
	    contains = c->rect.contains(ir);
	    break;
	}
	c = c->next;
	if (!c) {
	    appendContainer();
	    c = d->lastContainer;
	}
    }
    if ( !c ) {
	return;
    }
    c->items.append(item);
    if (!contains) {
	c = c->next;
	if (!c) {
	    appendContainer();
	    c = d->lastContainer;
	}
	c->items.append( item );
    }
    if (contentsWidth() < ir.right() || contentsHeight() < ir.bottom()) {
	resizeContents(QMAX(contentsWidth(), ir.right()), QMAX(contentsHeight(), ir.bottom()));
    }
}

ThumbItem* ThumbView::findItem(const QPoint& pos) {
    if (!d->firstItem) {
	return 0;
    }
    ThumbViewPrivate::ItemContainer *c = d->lastContainer;
    for (; c; c = c->prev) {
	if ( c->rect.contains(pos) ) {
	    ThumbItem *item = c->items.last();
	    for ( ; item; item = c->items.prev()) {
		if (item->rect().contains( pos )) {
		    return item;
		}
	    }
	}
    }
    return 0;
}

ThumbItem* ThumbView::findItem(const QString& text) {
    if (!d->firstItem) {
        return 0;
    }
    bool found = false;
    ThumbItem *item = 0;
    for (item = d->firstItem; item; item = item->next) {
        if (item->text() == text) {
            found = true;
            break;
        }
    }
    if (found) {
        return item;
    } else {
        return 0;
    }
}

QRect ThumbView::contentsRectToViewport(const QRect& r) {
    QRect vr = QRect(contentsToViewport(QPoint(r.x(), r.y())), r.size());
    return vr;
}

void ThumbView::clearSelection() {
    blockSignals(true);
    for (ThumbItem* item = d->firstItem; item; item = item->next) {
        if (item->isSelected()) {
            item->setSelected(false, false);
            d->selectedItems.remove(item);
        }
    }
    blockSignals(false);
    emit signalSelectionChanged();
}

void ThumbView::selectAll() {
    blockSignals(true);
    for (ThumbItem* item = d->firstItem; item; item = item->next) {
        if (!item->isSelected()) {
            item->setSelected(true, false);
            d->selectedItems.append(item);
        }
    }
    blockSignals(false);
    emit signalSelectionChanged();
}

void ThumbView::invertSelection() {
    blockSignals(true);
    for (ThumbItem* item = d->firstItem; item; item = item->next) {
        if (!item->isSelected()) {
            item->setSelected(true, false);
            d->selectedItems.append(item);
        } else {
            item->setSelected(false, false);
            d->selectedItems.remove(item);
        }
    }
    blockSignals(false);
    emit signalSelectionChanged();
}

void ThumbView::selectItem(ThumbItem* item, bool select) {
    if (!item) {
	return;
    }
    if (select) {
        d->selectedItems.append(item);
    } else {
        d->selectedItems.remove(item);
    }
    emit signalSelectionChanged();
}

void ThumbView::emitRenamed(ThumbItem *item) {
    if (!item) {
	return;
    }
    emit signalItemRenamed(item);
}

void ThumbView::startDrag() {
    if (!d->startDragItem) {
	return;
    }
    QStrList uris;
    for (ThumbItem *it = firstItem(); it; it=it->nextItem()) {
        if (it->isSelected()) {
	    // PENDING(Aur�lien) Check if .ascii() is ok here
            uris.append(it->text().ascii());
        }
    }
    QUriDrag* drag = new QUriDrag(uris, this);
    if (!drag) {
	return;
    }
    drag->setPixmap(*d->startDragItem->pixmap());
    d->startDragItem = 0;
    drag->dragCopy();
}

void ThumbView::contentsDropEvent(QDropEvent *e) {
    if (!e) {
	return;
    }
    if (e->source() == this) {
        e->accept();
        return;
    }
}

void ThumbView::keyPressEvent(QKeyEvent *e) {
    bool handled = false;
    if (!d->firstItem) {
	return;
    }
    ThumbItem *currItem = d->selectedItems.first();
    if (!currItem) {
        d->firstItem->setSelected(true, true);
        return;
    }
    switch ( e->key() ) {
	case Key_Home: {
	    d->firstItem->setSelected(true, true);
	    ensureItemVisible(d->firstItem);
	    handled = true;
	    break;
	}
	case Key_End: {
	    d->lastItem->setSelected(true, true);
	    ensureItemVisible(d->lastItem);
	    handled = true;
	    break;
	}
	case Key_Enter:
	case Key_Return: {
	    emit signalReturnPressed(currItem);
	    break;
	}
	case Key_Right: {
	    ThumbItem *item = currItem->next;
	    if (item) {
		item->setSelected(true,true);
		ensureItemVisible(item);
		handled = true;
	    }
	    break;
	}        
	case Key_Left: {
	    ThumbItem *item = currItem->prev;
	    if (item) {
		item->setSelected(true,true);
		ensureItemVisible(item);
		handled = true;
	    }
	    break;
	}
	case Key_Up: {
	    int x = currItem->x() + currItem->width()/2;
	    int y = currItem->y() - d->spacing*2;
	    ThumbItem *item = 0;
	    while (!item && y > 0) {
		item = findItem(QPoint(x,y));
		y -= d->spacing * 2;
	    }
	    if (item) {
		item->setSelected(true,true);
		ensureItemVisible(item);
		handled = true;
	    }
	    break;
	}
	case Key_Down: {
	    int x = currItem->x() + currItem->width()/2;
	    int y = currItem->y() + currItem->height() + d->spacing * 2;
	    ThumbItem *item = 0;
	    while (!item && y < contentsHeight()) {
		item = findItem(QPoint(x,y));
		y += d->spacing * 2;
	    }
	    if (item) {
		item->setSelected(true,true);
		ensureItemVisible(item);
		handled = true;
	    }
	    break;
	}
	case Key_Next: {
	    QRect r( 0, currItem->y() + visibleHeight(),
		     contentsWidth(), visibleHeight() );
	    ThumbItem *ni = findFirstVisibleItem(r);
	    if (!ni) {
		r = QRect( 0, currItem->y() + currItem->height(), contentsWidth(), contentsHeight() );
		ni = findLastVisibleItem( r );
	    }
	    if (ni) {
		ni->setSelected(true, true);
		ensureItemVisible(ni);
		handled = true;
	    }
	    break;
	}
	case Key_Prior: {
	    QRect r(0, currItem->y() - visibleHeight(), contentsWidth(), visibleHeight() );
	    ThumbItem *ni = findFirstVisibleItem(r);
	    if ( !ni ) {
		r = QRect( 0, 0, contentsWidth(), currItem->y() );
		ni = findFirstVisibleItem( r );
	    }
	    if ( ni ) {
		ni->setSelected(true, true);
		ensureItemVisible(ni);
		handled = true;
	    }
	    break;
	}
	default:
	    e->ignore();
	    return;
    }
    if (handled) {
        viewport()->repaint();
        emit signalSelectionChanged();
    }
}

void ThumbView::ensureItemVisible(ThumbItem *item) {
    if (!item) {
	return;
    }
    int w = item->width();
    int h = item->height();
    ensureVisible( item->x() + w / 2, item->y() + h / 2, w / 2 + 1, h / 2 + 1 );
}

ThumbItem* ThumbView::findFirstVisibleItem(const QRect &r ) const {
    ThumbViewPrivate::ItemContainer *c = d->firstContainer;
    ThumbItem *i = 0;
    bool alreadyIntersected = false;
    for ( ; c; c = c->next ) {
	if ( c->rect.intersects( r ) ) {
	    alreadyIntersected = true;
	    ThumbItem *item = c->items.first();
	    for ( ; item; item = c->items.next() ) {
		if ( r.intersects( item->rect() ) ) {
		    if ( !i ) {
			i = item;
		    } else {
			QRect r2 = item->rect();
			QRect r3 = i->rect();
			if (r2.y() < r3.y()) {
			    i = item;
			} else if (r2.y() == r3.y() && r2.x() < r3.x()) {
			    i = item;
			}
		    }
		}
	    }
	} else {
	    if (alreadyIntersected) {
		break;
	    }
	}
    }
    return i;
}

ThumbItem* ThumbView::findLastVisibleItem(const QRect &r ) const {
    ThumbViewPrivate::ItemContainer *c = d->firstContainer;
    ThumbItem *i = 0;
    bool alreadyIntersected = false;
    for ( ; c; c = c->next ) {
	if ( c->rect.intersects( r ) ) {
	    alreadyIntersected = true;
	    ThumbItem *item = c->items.first();
	    for ( ; item; item = c->items.next() ) {
		if ( r.intersects( item->rect() ) ) {
		    if ( !i ) {
			i = item;
		    } else {
			QRect r2 = item->rect();
			QRect r3 = i->rect();
			if (r2.y() > r3.y()) {
			    i = item;
			} else if (r2.y() == r3.y() && r2.x() > r3.x()) {
			    i = item;
			}
		    }
		}
	    }
	} else {
	    if (alreadyIntersected) {
		break;
	    }
	}
    }
    return i;
}

