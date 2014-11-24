/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "Canvas.moc"
#include "Canvas_p.h"

#include <QPrinter>
#include <QPaintDevice>
#include <QPaintEngine>
#include <QVBoxLayout>

#include <kapplication.h>
#include <kmessagebox.h>
#include <klocalizedstring.h>

#include "Scene.h"
#include "LayersModel.h"
#include "LayersModelItem.h"
#include "LayersSelectionModel.h"
#include "UndoMoveRowsCommand.h"
#include "UndoBorderChangeCommand.h"
#include "PLEConfigSkeleton.h"
#include "ImageLoadingThread.h"
#include "global.h"
#include "ProgressEvent.h"
#include "photolayoutseditor.h"
#include "CanvasLoadingThread.h"
#include "CanvasSavingThread.h"
#include "PLEStatusBar.h"

#define MAX_SCALE_LIMIT 4
#define MIN_SCALE_LIMIT 0.5

using namespace KIPIPhotoLayoutsEditor;

Canvas::Canvas(const CanvasSize & size, QWidget * parent) :
    QGraphicsView(parent),
    d(new CanvasPrivate)
{
    d->m_size = size;
    m_scene = new Scene(QRectF(QPointF(0,0), d->m_size.size(CanvasSize::Pixels)), this);
    this->init();
}

Canvas::Canvas(Scene * scene, QWidget * parent) :
    QGraphicsView(parent),
    d(new CanvasPrivate)
{
    Q_ASSERT(scene != 0);
    m_scene = scene;
    m_scene->setParent(this);
    this->setScene(m_scene);

    this->init();
}

Canvas::~Canvas()
{
    delete d;
}

/** ###########################################################################################################################
* Initialize Canvas object
#############################################################################################################################*/
void Canvas::init()
{
    m_is_saved = true;
    m_saved_on_index = 0;
    m_undo_stack = new QUndoStack(this);
    m_scale_factor = 1;

    this->setupGUI();
    this->enableViewingMode();
    this->prepareSignalsConnection();
}

/** ###########################################################################################################################
* Setup GUI of canvas widget
#############################################################################################################################*/
void Canvas::setupGUI()
{
    this->setAcceptDrops(true);
    this->setAutoFillBackground(true);
    this->viewport()->setAutoFillBackground(true);
    this->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    this->setCacheMode(QGraphicsView::CacheNone);
    this->setAntialiasing( PLEConfigSkeleton::antialiasing() );
    connect(PLEConfigSkeleton::self(), SIGNAL(antialiasingChanged(bool)), this, SLOT(setAntialiasing(bool)));

    // Transparent scene background
//    QPixmap pixmap(20,20);
//    QPainter p(&pixmap);
//    p.fillRect(0,0,20,20,Qt::lightGray);
//    p.fillRect(0,10,10,10,Qt::darkGray);
//    p.fillRect(10,0,10,10,Qt::darkGray);
//    QPalette viewportPalette = this->viewport()->palette();
//    viewportPalette.setBrush(QPalette::Background, QBrush(pixmap));
//    this->viewport()->setPalette(viewportPalette);
//    this->viewport()->setBackgroundRole(QPalette::Background);

    QVBoxLayout * layout = new QVBoxLayout();
    this->setLayout(layout);
    layout->addWidget(this->viewport());

    this->setScene(m_scene);
}

/** ###########################################################################################################################
* Connect signals to slots
#############################################################################################################################*/
void Canvas::prepareSignalsConnection()
{
    connect(m_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(m_scene, SIGNAL(itemAboutToBeRemoved(AbstractPhoto*)), this, SLOT(removeItem(AbstractPhoto*)));
    connect(m_scene, SIGNAL(itemsAboutToBeRemoved(QList<AbstractPhoto*>)), this, SLOT(removeItems(QList<AbstractPhoto*>)));
    connect(m_scene->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
    connect(m_undo_stack, SIGNAL(indexChanged(int)), this, SLOT(isSavedChanged(int)));
    connect(m_undo_stack, SIGNAL(cleanChanged(bool)), this, SLOT(isSavedChanged(bool)));
}

/** ###########################################################################################################################
 * Change selection mode
 #############################################################################################################################*/
void Canvas::setSelectionMode(SelectionMode mode)
{
    if (mode & Viewing)
    {
        this->setInteractive(true);
        this->setDragMode(QGraphicsView::ScrollHandDrag);
        m_scene->setSelectionMode(Scene::NoSelection);
        goto save;
    }
    else if (mode & Zooming)
    {
        this->setInteractive(true);
        this->setDragMode(QGraphicsView::NoDrag);
        m_scene->setSelectionMode(Scene::NoSelection);
        goto save;
    }
    else if (mode & MultiSelecting)
    {
        this->setInteractive(true);
        this->setDragMode(QGraphicsView::RubberBandDrag);
        m_scene->setSelectionMode(Scene::MultiSelection);
        goto save;
    }
    else if (mode & SingleSelcting)
    {
        this->setInteractive(true);
        this->setDragMode(QGraphicsView::NoDrag);
        m_scene->setSelectionMode(Scene::SingleSelection);
        goto save;
    }
    return;
    save:
        m_selection_mode = mode;
}

/** ###########################################################################################################################
 * Data model connected with this canvas (in fact this is a Scene's model)
 #############################################################################################################################*/
LayersModel * Canvas::model() const
{
    return m_scene->model();
}

/** ###########################################################################################################################
 * Data selection model connected with this canvas (in fact this is a Scene's selection model)
 #############################################################################################################################*/
LayersSelectionModel * Canvas::selectionModel() const
{
    return m_scene->selectionModel();
}

/** ###########################################################################################################################
 * gets canvas size
 #############################################################################################################################*/
CanvasSize Canvas::canvasSize() const
{
    return d->m_size;
}

/** ###########################################################################################################################
 * Set canvas size
 #############################################################################################################################*/
void Canvas::setCanvasSize(const CanvasSize & size)
{
    if (!size.isValid())
        return;
    d->m_size = size;
    m_scene->setSceneRect(QRectF(QPointF(0,0), size.size(CanvasSize::Pixels)));
}

/** ###########################################################################################################################
 * Sets QPrinter's paper size, etc.
 #############################################################################################################################*/
void Canvas::preparePrinter(QPrinter * printer)
{
    printer->setPageMargins(0, 0, 0, 0, QPrinter::Millimeter);
    CanvasSize::SizeUnits su = d->m_size.sizeUnit();
    QSizeF cs = d->m_size.size();
    bool setResolution = true;
    switch (su)
    {
    case CanvasSize::Centimeters:
        cs *= 10;
    case CanvasSize::Milimeters:
        printer->setPaperSize(cs, QPrinter::Millimeter);
        break;
    case CanvasSize::Inches:
        printer->setPaperSize(cs, QPrinter::Inch);
        break;
    case CanvasSize::Points:
        printer->setPaperSize(cs, QPrinter::Point);
        break;
    case CanvasSize::Picas:
        printer->setPaperSize(cs, QPrinter::Pica);
        break;
    case CanvasSize::Pixels:
        printer->setPaperSize(cs, QPrinter::DevicePixel);
        setResolution = false;
        break;
    default:
        printer->setPaperSize(cs, QPrinter::DevicePixel);
        setResolution = false;
        qDebug() << "Unhandled size unit at:" << __FILE__ << ":" << __LINE__;
    }
    if (setResolution)
    {
        QSize printerResolution = d->m_size.resolution(CanvasSize::PixelsPerInch).toSize();
        printer->setResolution(qMin(printerResolution.width(), printerResolution.height()));
    }
}

/** ###########################################################################################################################
 * Add new image from QImage object
 #############################################################################################################################*/
void Canvas::addImage(const QImage & image)
{
    // Create & setup item
    PhotoItem * it = new PhotoItem(image);

    // Add item to scene & model
    m_scene->addItem(it);

    // Fits item to the scenes rect
    it->fitToRect(m_scene->sceneRect().toRect());
}

/** ###########################################################################################################################
 * Add new image from the specified url
 #############################################################################################################################*/
void Canvas::addImage(const KUrl & imageUrl)
{
    ImageLoadingThread * ilt = new ImageLoadingThread(this);
    ilt->setImageUrl(imageUrl);
    ilt->setMaximumProgress(0.9);
    connect(ilt, SIGNAL(imageLoaded(KUrl,QImage)), this, SLOT(imageLoaded(KUrl,QImage)));
    ilt->start();
}

/** ###########################################################################################################################
 * Add images from the specified url's list
 #############################################################################################################################*/
void Canvas::addImages(const KUrl::List & images)
{
    ImageLoadingThread * ilt = new ImageLoadingThread(this);
    ilt->setImagesUrls(images);
    ilt->setMaximumProgress(0.9);
    connect(ilt, SIGNAL(imageLoaded(KUrl,QImage)), this, SLOT(imageLoaded(KUrl,QImage)));
    ilt->start();
}

/** ###########################################################################################################################
 * Add new text item
 #############################################################################################################################*/
void Canvas::addText(const QString & text)
{
    // Create & setup item
    TextItem * it = new TextItem(text);

    // Add item to scene & model
    m_scene->addItem(it);
}

/** ###########################################################################################################################
 * Used when new item has been created and needs to be added to the scene and to the model
 #############################################################################################################################*/
void Canvas::addNewItem(AbstractPhoto * item)
{
    if (!item)
        return;

    m_scene->addItem(item);

    m_scene->clearSelection();
    m_scene->clearFocus();

    item->setSelected(true);
    item->setFocus( Qt::OtherFocusReason );
}

/** ###########################################################################################################################
 * Turns on/off antialiasing mode
 #############################################################################################################################*/
void Canvas::setAntialiasing(bool antialiasing)
{
    this->setRenderHint(QPainter::Antialiasing, antialiasing);                            /// It causes worst quality!
    this->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, antialiasing);    /// It causes worst quality!
    this->update();
}

/** ###########################################################################################################################
 * Makes use of loaded image
 #############################################################################################################################*/
void Canvas::imageLoaded(const KUrl & url, const QImage & image)
{
    if (!image.isNull())
    {
        // Create & setup item
        PhotoItem * it = new PhotoItem(image, url.fileName(), m_scene);
        // Add item to scene & model
        m_scene->addItem(it);
    }
}

/** ##########################################################################################################################
 * Creates item move down command and push it to the undo stack
 #############################################################################################################################*/
void Canvas::moveRowsCommand(const QModelIndex & startIndex, int count, const QModelIndex & parentIndex, int move, const QModelIndex & destinationParent)
{
    int destination = startIndex.row();
    if (move > 0)
        destination += count + move;
    else if (move < 0)
        destination += move;
    else
        return;
    UndoMoveRowsCommand * undo = new UndoMoveRowsCommand(startIndex.row(), count, parentIndex, destination, destinationParent, model());
    m_undo_stack->push(undo);
}

/** ##########################################################################################################################
 * Move selected items up on scene & model. (Called by layers tree)
 #############################################################################################################################*/
void Canvas::moveSelectedRowsUp()
{
    QModelIndexList selectedIndexes = selectionModel()->selectedIndexes();
    if (!selectedIndexes.count())
    {
        #ifdef  QT_DEBUG
        qDebug() << "No items selected to move!" << selectedIndexes;
        #endif
        return;
    }
    // Check continuity of selection
    QModelIndexList::iterator it = selectedIndexes.begin();
    QModelIndex startIndex = *it;
    if (startIndex.isValid())
    {
        int minRow = startIndex.row();
        int maxRow = startIndex.row();
        int sumRows = startIndex.row();
        for(++it; it != selectedIndexes.end(); ++it)
        {
            if (it->column() != LayersModelItem::NameString)
                continue;
            if (startIndex.parent() != it->parent())
            {
                #ifdef  QT_DEBUG
                qDebug() << "Different parents of items!\n" << selectedIndexes;
                #endif
                return;
            }
            else if (!it->isValid())
            {
                #ifdef  QT_DEBUG
                qDebug() << "Invalid items!\n" << selectedIndexes;
                #endif
                return;
            }
            if (it->row() < minRow)
            {
                startIndex = *it;
                minRow = it->row();
            }
            if (it->row() > maxRow)
                maxRow = it->row();
            sumRows += it->row();
        }
        if ((((minRow+maxRow)*(maxRow-minRow+1))/2.0) != sumRows)
        {
            #ifdef  QT_DEBUG
            qDebug() << "Unordered items!\n" << selectedIndexes;
            #endif
            return;
        }
        if (minRow) // It means "is there any space before starting index to move selection"
            moveRowsCommand(startIndex, selectedIndexes.count(), startIndex.parent(), -1, startIndex.parent());
    }
    this->selectionChanged();
}

/** ##########################################################################################################################
 * Move selected items down on scene & model. (Called by layers tree)
 #############################################################################################################################*/
void Canvas::moveSelectedRowsDown()
{
    QModelIndexList selectedIndexes = selectionModel()->selectedIndexes();
    if (!selectedIndexes.count())
    {
        #ifdef  QT_DEBUG
        qDebug() << "No items selected to move!" << selectedIndexes;
        #endif
        return;
    }
    // Check continuity of selection
    QModelIndexList::iterator it = selectedIndexes.begin();
    QModelIndex startIndex = *it;
    if (startIndex.isValid())
    {
        int minRow = startIndex.row();
        int maxRow = startIndex.row();
        int sumRows = startIndex.row();
        for(++it; it != selectedIndexes.end(); ++it)
        {
            if (it->column() != LayersModelItem::NameString)
                continue;
            if (startIndex.parent() != it->parent())
            {
                #ifdef  QT_DEBUG
                qDebug() << "Different parents of items!\n" << selectedIndexes;
                #endif
                return;
            }
            else if (!it->isValid())
            {
                #ifdef  QT_DEBUG
                qDebug() << "Invalid items!\n" << selectedIndexes;
                #endif
                return;
            }
            if (it->row() < minRow)
            {
                startIndex = *it;
                minRow = it->row();
            }
            if (it->row() > maxRow)
                maxRow = it->row();
            sumRows += it->row();
        }
        if ((((minRow+maxRow)*(maxRow-minRow+1))/2.0) != sumRows)
        {
            #ifdef  QT_DEBUG
            qDebug() << "Unordered items!\n" << selectedIndexes;
            #endif
            return;
        }
        if (maxRow+1 < model()->rowCount(startIndex.parent())) // It means "is there any space before starting index to move selection"
            moveRowsCommand(startIndex, selectedIndexes.count(), startIndex.parent(), 1, startIndex.parent());
    }
    this->selectionChanged();
}

/** ##########################################################################################################################
 * Remove from model ONE item removed from scene
 #############################################################################################################################*/
void Canvas::removeItem(AbstractPhoto * item)
{
    if (item)
        m_scene->removeItem(item);
}

/** ##########################################################################################################################
 * Remove from model items removed from scene
 #############################################################################################################################*/
void Canvas::removeItems(const QList<AbstractPhoto*> & items)
{
    m_scene->removeItems(items);
}

/** ##########################################################################################################################
 * Remove from scene items removed from layert tree.
 #############################################################################################################################*/
void Canvas::removeSelectedRows()
{
    QList<AbstractPhoto*> items;
    QModelIndexList selectedIndexes = selectionModel()->selectedRows();
    foreach(QModelIndex index, selectedIndexes)
        items << static_cast<LayersModelItem*>(index.internalPointer())->photo();
    m_scene->removeItems(items);
}

/** ###########################################################################################################################
 * Synchronize scene's selection with model's selection
 #############################################################################################################################*/
void Canvas::selectionChanged()
{
    QList<AbstractPhoto*> selectedItems = m_scene->selectedItems();
    QModelIndexList oldSelected = selectionModel()->selectedIndexes();
    QModelIndexList newSelected = model()->itemsToIndexes(selectedItems);
    foreach(QModelIndex index, oldSelected)
    {
        if (!newSelected.contains(index) && index.column() == LayersModelItem::NameString)
            selectionModel()->select(index, QItemSelectionModel::Rows | QItemSelectionModel::Deselect);
    }
    foreach(QModelIndex index, newSelected)
    {
        if (!selectionModel()->isSelected(index) && index.column() == LayersModelItem::NameString)
            selectionModel()->select(index, QItemSelectionModel::Rows | QItemSelectionModel::Select);
    }

    // Selection change signals
    selectedItems = m_scene->selectedItems();
    if (m_selection_mode & SingleSelcting)
    {
        if (selectedItems.count() == 1)
        {
            AbstractPhoto * item = selectedItems.at(0);
            emit hasSelectionChanged(true);
            emit selectedItem(item);
        }
        else
        {
            emit hasSelectionChanged(false);
            emit selectedItem(0);
        }
    }
    else if (m_selection_mode & MultiSelecting)
        emit hasSelectionChanged(selectedItems.count());
}

/** ###########################################################################################################################
 * Synchronize model's selection with scene's selection
 #############################################################################################################################*/
void Canvas::selectionChanged(const QItemSelection & newSelection, const QItemSelection & oldSelection)
{
    LayersModelItem * temp;
    const QModelIndexList & oldSel = oldSelection.indexes();
    const QModelIndexList & newSel = newSelection.indexes();
    QSet<QModelIndex> deselected = oldSel.toSet().subtract(newSel.toSet());
    foreach(QModelIndex index, deselected)
    {
        if (index.column() != LayersModelItem::NameString)
            continue;
        temp = static_cast<LayersModelItem*>(index.internalPointer());
        if (temp->photo() && temp->photo()->isSelected())
            temp->photo()->setSelected(false);
    }
    QSet<QModelIndex> selected = newSel.toSet().subtract(oldSel.toSet());
    foreach(QModelIndex index, selected)
    {
        if (index.column() != LayersModelItem::NameString)
            continue;
        temp = static_cast<LayersModelItem*>(index.internalPointer());
        if (temp->photo() && !temp->photo()->isSelected())
            temp->photo()->setSelected(true);
    }
}

/** ###########################################################################################################################
 * Sets selecting mode
 #############################################################################################################################*/
void Canvas::enableDefaultSelectionMode()
{
    this->unsetCursor();
    m_scene->setInteractionMode(Scene::Selecting | Scene::Moving);
    setSelectionMode(MultiSelecting);
    m_scene->clearSelectingFilters();
}

/** ###########################################################################################################################
 * Sets viewing mode
 #############################################################################################################################*/
void Canvas::enableViewingMode()
{
    this->unsetCursor();
    m_scene->setInteractionMode(Scene::View);
    setSelectionMode(Viewing);
    m_scene->clearSelectingFilters();
}

/** ###########################################################################################################################
 * Sets zooming mode
 #############################################################################################################################*/
void Canvas::enableZoomingMode()
{
    this->unsetCursor();
    setSelectionMode(Zooming);
    this->setCursor(QCursor(QPixmap(":/zoom_cursor.png").scaled(QSize(24,24))));
    m_scene->clearSelectingFilters();
}

/** ###########################################################################################################################
 * Sets canvas editing mode
 #############################################################################################################################*/
void Canvas::enableCanvasEditingMode()
{
    this->unsetCursor();
    m_scene->setInteractionMode(Scene::NoSelection);
    setSelectionMode(Viewing);
    this->setCursor(QCursor(Qt::ArrowCursor));
    m_scene->clearSelectingFilters();
}

/** ###########################################################################################################################
 * Sets effects editing mode
 #############################################################################################################################*/
void Canvas::enableEffectsEditingMode()
{
    this->unsetCursor();
    m_scene->setInteractionMode(Scene::Selecting);
    setSelectionMode(SingleSelcting);
    this->setCursor(QCursor(Qt::ArrowCursor));
    m_scene->clearSelectingFilters();
    m_scene->addSelectingFilter(PhotoItem::staticMetaObject);
}

/** ###########################################################################################################################
 * Sets text editing mode
 #############################################################################################################################*/
void Canvas::enableTextEditingMode()
{
    this->unsetCursor();
    m_scene->setInteractionMode(Scene::Selecting | Scene::MouseTracking | Scene::OneclickFocusItems);
    setSelectionMode(SingleSelcting);
    m_scene->clearSelectingFilters();
    m_scene->addSelectingFilter(TextItem::staticMetaObject);
}

/** ###########################################################################################################################
 * Sets rotating mode
 #############################################################################################################################*/
void Canvas::enableRotateEditingMode()
{
    this->unsetCursor();
    m_scene->setInteractionMode(Scene::Selecting | Scene::Rotating);
    setSelectionMode(SingleSelcting);
    this->setCursor(Qt::ArrowCursor);
    m_scene->clearSelectingFilters();
}

/** ###########################################################################################################################
 * Sets scaling mode
 #############################################################################################################################*/
void Canvas::enableScaleEditingMode()
{
    this->unsetCursor();
    m_scene->setInteractionMode(Scene::Selecting | Scene::Scaling);
    setSelectionMode(SingleSelcting);
    this->setCursor(Qt::ArrowCursor);
    m_scene->clearSelectingFilters();
}

/** ###########################################################################################################################
 * Sets cropping mode
 #############################################################################################################################*/
void Canvas::enableCropEditingMode()
{
    this->unsetCursor();
    m_scene->setInteractionMode(Scene::Selecting | Scene::Cropping);
    setSelectionMode(SingleSelcting);
    this->setCursor(Qt::ArrowCursor);
    m_scene->clearSelectingFilters();
}

/** ###########################################################################################################################
 * Sets borders editing mode
 #############################################################################################################################*/
void Canvas::enableBordersEditingMode()
{
    this->unsetCursor();
    m_scene->setInteractionMode(Scene::Selecting);
    setSelectionMode(SingleSelcting);
    this->setCursor(Qt::ArrowCursor);
    m_scene->clearSelectingFilters();
}

/** ###########################################################################################################################
 * Connect widgets to canvas signals
 #############################################################################################################################*/
void Canvas::refreshWidgetConnections(bool isVisible)
{
    if (isVisible)
    {
        connect(this,SIGNAL(hasSelectionChanged(bool)),sender(),SLOT(setEnabled(bool)));
        emit hasSelectionChanged(m_scene->selectedItems().count());
    }
    else
        disconnect(this,SIGNAL(hasSelectionChanged(bool)),sender(),0);
}

/** ###########################################################################################################################
 * Appends new undo command
 #############################################################################################################################*/
void Canvas::newUndoCommand(QUndoCommand * command)
{
    m_undo_stack->push(command);
}

/** ###########################################################################################################################
 * Progress event type
 #############################################################################################################################*/
void Canvas::progressEvent(ProgressEvent * event)
{
    QProgressBar * temp = d->progressMap[event->sender()];
    switch (event->type())
    {
        case ProgressEvent::Init:
            if (!temp)
                this->layout()->addWidget( temp = d->progressMap[event->sender()] = new QProgressBar(this) );
            temp->setMaximum(1000);
            temp->setValue(0);
            this->setEnabled(false);
            {
                PLEStatusBar * sb = dynamic_cast<PLEStatusBar*>(PhotoLayoutsEditor::instance()->statusBar());
                if (sb)
                    sb->runBusyIndicator();
            }
            break;
        case ProgressEvent::ProgressUpdate:
            if (temp)
                temp->setValue(event->data().toDouble() * 1000);
            break;
        case ProgressEvent::ActionUpdate:
            if (temp)
                temp->setFormat(event->data().toString() + " [%p%]");
            break;
        case ProgressEvent::Finish:
            if (temp)
            {
                temp->setValue( temp->maximum() );
                d->progressMap.take(event->sender())->deleteLater();
            }
            this->setEnabled(true);
            {
                PLEStatusBar * sb = dynamic_cast<PLEStatusBar*>(PhotoLayoutsEditor::instance()->statusBar());
                if (sb)
                    sb->stopBusyIndicator();
            }
            break;
        default:
            temp = 0;
            break;
    }
    event->setAccepted(temp);
}

/** ###########################################################################################################################
 * Wheel event - scales the canvas
 #############################################################################################################################*/
void Canvas::wheelEvent(QWheelEvent * event)
{
    int steps = event->delta() / 120;
    double scaleFactor;
    scaleFactor = (m_scale_factor + steps * 0.1) / m_scale_factor;
    scale(scaleFactor, event->pos());
}

/** ###########################################################################################################################
 * Converts canvas to SVG image
 #############################################################################################################################*/
QDomDocument Canvas::toSvg() const
{
    QDomDocument result(" svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\"");
    QDomElement svg;// = ;//m_scene->toSvg(result);
    result.appendChild(svg);
    svg.setAttribute("width", QString::number(d->m_size.size().width()));
    svg.setAttribute("height", QString::number(d->m_size.size().height()));
    switch (d->m_size.sizeUnit())
    {
        case CanvasSize::Centimeters:
            svg.setAttribute("width", svg.attribute("width") + "cm");
            svg.setAttribute("height", svg.attribute("height") + "cm");
            break;
        case CanvasSize::Milimeters:
            svg.setAttribute("width", svg.attribute("width") + "mm");
            svg.setAttribute("height", svg.attribute("height") + "mm");
            break;
        case CanvasSize::Inches:
            svg.setAttribute("width", svg.attribute("width") + "in");
            svg.setAttribute("height", svg.attribute("height") + "in");
            break;
        case CanvasSize::Picas:
            svg.setAttribute("width", svg.attribute("width") + "pc");
            svg.setAttribute("height", svg.attribute("height") + "pc");
            break;
        case CanvasSize::Points:
            svg.setAttribute("width", svg.attribute("width") + "pt");
            svg.setAttribute("height", svg.attribute("height") + "pt");
            break;
        case CanvasSize::Pixels:
            svg.setAttribute("width", svg.attribute("width") + "px");
            svg.setAttribute("height", svg.attribute("height") + "px");
            break;
        default:
            svg.setAttribute("width", svg.attribute("width") + "px");
            svg.setAttribute("height", svg.attribute("height") + "px");
            qDebug() << "Unhandled size unit at:" << __FILE__ << ":" << __LINE__;
    }
    QDomElement resolution = result.createElementNS(KIPIPhotoLayoutsEditor::uri(), "page");
    resolution.setAttribute("width", QString::number(d->m_size.resolution().width()));
    resolution.setAttribute("height", QString::number(d->m_size.resolution().height()));
    resolution.setAttribute("unit", CanvasSize::resolutionUnitName(d->m_size.resolutionUnit()));
    svg.appendChild(resolution);
    return result;
}

/** ###########################################################################################################################
 * Loads canvas state from SVG file
 #############################################################################################################################*/
Canvas * Canvas::fromSvg(QDomDocument & document)
{
    Canvas * result = 0;
    QDomNodeList children = document.childNodes();
    if (children.count())
    {
        QDomElement element = document.firstChildElement("svg");
        if (!element.isNull())
        {
            QString width = element.attribute("width");
            QString height = element.attribute("height");
            QDomElement pageElement = element.firstChildElement("page");
            QString xResolution = pageElement.attribute("width");
            QString yResolution = pageElement.attribute("height");
            QString resUnit = pageElement.attribute("unit");
            qDebug() << pageElement.namespaceURI() << KIPIPhotoLayoutsEditor::templateUri();

            // Canvas size validation
            QRegExp sizeRegExp("[0-9.]+((cm)|(mm)|(in)|(pc)|(pt)|(px))");
            QRegExp resRegExp("[0-9.]+");
            if (sizeRegExp.exactMatch(width) &&
                    sizeRegExp.exactMatch(height) &&
                    width.right(2) == height.right(2) &&
                    resRegExp.exactMatch(xResolution) &&
                    resRegExp.exactMatch(yResolution) &&
                    CanvasSize::resolutionUnit(resUnit) != CanvasSize::UnknownResolutionUnit)
            {
                CanvasSize size;
                size.setSizeUnit( CanvasSize::sizeUnit(width.right(2)) );
                width.remove(width.length()-2, 2);
                height.remove(height.length()-2, 2);
                QSizeF dimension(width.toDouble(),height.toDouble());
                size.setSize(dimension);
                size.setResolutionUnit( CanvasSize::resolutionUnit(resUnit) );
                QSizeF resolution(xResolution.toDouble(), yResolution.toDouble());
                size.setResolution(resolution);
                if (dimension.isValid())
                {
                    QDomElement sceneElement = element.firstChildElement("g");
                    while (!sceneElement.isNull() && sceneElement.attribute("id") != "Scene")
                        sceneElement = sceneElement.nextSiblingElement("g");
                    Scene * scene = Scene::fromSvg(sceneElement);
                    if (scene)
                    {
                        result = new Canvas(scene);
                        result->setEnabled(false);
                        result->d->m_size = size;
                        result->d->m_template = (pageElement.namespaceURI() == KIPIPhotoLayoutsEditor::templateUri());
                    }
                }
            }
            else
                KMessageBox::error(0, i18n("Invalid image size!"));
        }
    }
    return result;
}

/** ###########################################################################################################################
 * Scales canvas view by the selected factor
 #############################################################################################################################*/
void Canvas::scale(qreal factor, const QPoint & center)
{
    // Scaling limitation
    if (factor <= 0 || !scene() || ((m_scale_factor*factor <= 0.1 && factor < 1) || (m_scale_factor*factor > 7)))
        return;

    QGraphicsView::scale(factor, factor);

    if (center.isNull())
        centerOn( m_scene->sceneRect().center() );
    else
        centerOn( mapToScene(center) );

    m_scale_factor *= factor;
}

/** ###########################################################################################################################
 * Scales canvas view to fit in rect
 #############################################################################################################################*/
void Canvas::scale(const QRect & rect)
{   
    QRectF r(this->mapToScene(rect.topLeft()),
             this->mapToScene(rect.bottomRight()));
    QSizeF viewSize = r.size();
    viewSize.setWidth(  qAbs(viewSize.width())  );
    viewSize.setHeight( qAbs(viewSize.height()) );
    QSizeF sceneSize = m_scene->sceneRect().size();
    qreal xFactor = sceneSize.width() / viewSize.width();
    qreal yFactor = sceneSize.height() / viewSize.height();
    qreal newFactor;
    if (xFactor > 1 && yFactor > 1)
        newFactor = xFactor > yFactor ? xFactor : yFactor;
    else
        newFactor = xFactor < yFactor ? xFactor : yFactor;

    if (m_scale_factor*newFactor > 7)
        newFactor = 7 / m_scale_factor;
    this->scale(newFactor, rect.center());
}

/** ###########################################################################################################################
 * Returns file object connected with canvas
 #############################################################################################################################*/
KUrl Canvas::file() const
{
    return m_file;
}

/** ###########################################################################################################################
 * Sets the file connected with canvas
 #############################################################################################################################*/
void Canvas::setFile(const KUrl & file)
{
    if (file.isValid() && !file.isEmpty())
        m_file = file;
}

/** ###########################################################################################################################
 * Sets the file connected with canvas
 #############################################################################################################################*/
void Canvas::save(const KUrl & fileUrl, bool setAsDefault)
{
    KUrl tempFile = fileUrl;
    if (fileUrl.isEmpty() || !fileUrl.isValid())
    {
        if (m_file.isEmpty() || !m_file.isValid())
        {
            KMessageBox::detailedError(0,
                                       i18n("Can't save canvas!"),
                                       i18n("Invalid file path."));
            return;
        }
        tempFile = m_file;
    }

    if (setAsDefault)
       m_file = tempFile;

    CanvasSavingThread * thread = new CanvasSavingThread(this);
    connect(thread, SIGNAL(saved()), this, SLOT(savingFinished()));
    thread->save(this, m_file);
}

/** ###########################################################################################################################
 * Save canvas as a template
 #############################################################################################################################*/
void Canvas::saveTemplate(const KUrl & fileUrl)
{
    if (fileUrl.isEmpty() || !fileUrl.isValid())
    {
        KMessageBox::detailedError(0,
                                   i18n("Can't save canvas!"),
                                   i18n("Invalid file path."));
        return;
    }

    CanvasSavingThread * thread = new CanvasSavingThread(this);
    connect(thread, SIGNAL(saved()), this, SLOT(savingFinished()));
    thread->saveAsTemplate(this, fileUrl);
}

/** ###########################################################################################################################
 * Check if canvas is saved
 #############################################################################################################################*/
bool Canvas::isSaved()
{
    return m_is_saved;
}

/** ###########################################################################################################################
 * Controls changes on cavnas (based on QUndoStack state)
 #############################################################################################################################*/
void Canvas::isSavedChanged(int /*currentCommandIndex*/)
{
    m_is_saved = (m_saved_on_index == m_undo_stack->index());
    emit savedStateChanged();
}

/** ###########################################################################################################################
 * Controls changes on cavnas (based on QUndoStack state)
 #############################################################################################################################*/
void Canvas::isSavedChanged(bool /*isStackClean*/)
{
    if (m_undo_stack->isClean())
        m_is_saved = m_undo_stack->isClean();
    else
        m_is_saved = (m_saved_on_index == m_undo_stack->index());
    emit savedStateChanged();
}

/** ###########################################################################################################################
 * Controls changes on cavnas (based on QUndoStack state)
 #############################################################################################################################*/
bool Canvas::isTemplate() const
{
    return d->m_template;
}

/** ###########################################################################################################################
 * Saving finished slot
 #############################################################################################################################*/
void Canvas::savingFinished()
{
    m_is_saved = true;
    m_saved_on_index = m_undo_stack->index();
    emit savedStateChanged();
}

/** ###########################################################################################################################
 * Draws canvas content onto the QPaintDevice
 #############################################################################################################################*/
void Canvas::renderCanvas(QPaintDevice * device)
{
    if (scene())
    {
        scene()->setSelectionVisible(false);
        bool isGridVisible = scene()->isGridVisible();
        scene()->setGridVisible(false);

        scene()->setSelectionVisible(false);
        QPainter p(device);
        if (d->m_size.sizeUnit() != CanvasSize::Pixels &&
                d->m_size.sizeUnit() != CanvasSize::UnknownSizeUnit)
        {
            QSizeF resolution = d->m_size.resolution(CanvasSize::PixelsPerInch);
            p.setTransform( QTransform::fromScale( device->logicalDpiX() / resolution.width(),
                                                   device->logicalDpiY() / resolution.height()) );
        }
        scene()->render(&p, scene()->sceneRect(), scene()->sceneRect());
        p.end();

        scene()->setSelectionVisible(true);
        scene()->setGridVisible(isGridVisible);
    }
}

/** ###########################################################################################################################
 * Draws canvas content onto the printer
 #############################################################################################################################*/
void Canvas::renderCanvas(QPrinter * device)
{
    renderCanvas(static_cast<QPaintDevice*>(device));
}

/** ###########################################################################################################################
 * Groups operations into one undo operation
 #############################################################################################################################*/
void Canvas::beginRowsRemoving()
{
    m_undo_stack->beginMacro(i18n("Remove items"));
}

/** ###########################################################################################################################
 * Finish group of undo operations
 #############################################################################################################################*/
void Canvas::endRowsRemoving()
{
    m_undo_stack->endMacro();
}


#undef MAX_SCALE_LIMIT
#undef MIN_SCALE_LIMIT
