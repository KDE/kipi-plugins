/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#include "AbstractItemsListViewTool.h"
#include "AbstractItemsListViewTool_p.h"
#include "AbstractPhoto.h"
#include "ToolsDockWidget.h"
#include "BorderDrawersLoader.h"
#include "global.h"

#include <QGridLayout>
#include <QLabel>
#include <QListView>
#include <QtAbstractPropertyBrowser>

#include <klocalizedstring.h>
#include <kpushbutton.h>
#include <kcombobox.h>

using namespace KIPIPhotoLayoutsEditor;

class ItemCreatedCommand : public QUndoCommand
{
    QObject * item;
    int row;
    AbstractMovableModel * model;
    bool done;
public:
    ItemCreatedCommand(QObject * item, int row, AbstractMovableModel * model, QUndoCommand * parent = 0) :
        QUndoCommand(parent),
        item(item),
        row(row),
        model(model),
        done(false)
    {}
    ~ItemCreatedCommand()
    {
        if (!done)
            delete item;
    }
    virtual void redo()
    {
        done = true;
        if (model->item(model->index(row, 0)) == item)
            return;
        model->insertRow(row);
        model->setItem(item, model->index(row, 0));
    }
    virtual void undo()
    {
        done = false;
        if (model->item(model->index(row, 0)) != item)
            return;
        model->removeRow(row);
    }
};
class ItemRemovedCommand : public QUndoCommand
{
    QObject * item;
    int row;
    AbstractMovableModel * model;
    bool done;
public:
    ItemRemovedCommand(QObject * item, int row, AbstractMovableModel * model, QUndoCommand * parent = 0) :
        QUndoCommand(parent),
        item(item),
        row(row),
        model(model),
        done(true)
    {}
    ~ItemRemovedCommand()
    {
        if (done)
            delete item;
    }
    virtual void redo()
    {
        done = true;
        if (model->item(model->index(row, 0)) != item)
            return;
        model->removeRow(row);
    }
    virtual void undo()
    {
        done = false;
        if (model->item(model->index(row, 0)) == item)
            return;
        model->insertRow(row);
        model->setItem(item, model->index(row, 0));
    }
};
class ItemMoveRowsCommand : public QUndoCommand
{
    int sourceStart;
    int count;
    int destinationRow;
    AbstractMovableModel * model;
public:
    ItemMoveRowsCommand(int sourceStart, int count, int destinationRow, AbstractMovableModel * model, QUndoCommand * parent = 0) :
        QUndoCommand(parent),
        sourceStart(sourceStart),
        count(count),
        destinationRow(destinationRow),
        model(model)
    {}
    virtual void redo()
    {
        model->moveRows(sourceStart, count, destinationRow);
        this->swap();
    }
    virtual void undo()
    {
        model->moveRows(sourceStart, count, destinationRow);
        this->swap();
    }
    void swap()
    {
        int temp = sourceStart;
        sourceStart = destinationRow;
        destinationRow = temp;
        if (destinationRow > sourceStart)
            destinationRow += count;
        else
            sourceStart -= count;
    }
};

class KIPIPhotoLayoutsEditor::AbstractItemsListViewToolPrivate
{
    AbstractItemsListViewToolPrivate() :
        m_list_widget(0),
        m_add_button(0),
        m_remove_button(0),
        m_down_button(0),
        m_up_button(0),
        m_opened_editor(0,QModelIndex()),
        m_editors_object(0)
    {}
    AbstractListToolView * m_list_widget;
    KPushButton * m_add_button;
    KPushButton * m_remove_button;
    KPushButton * m_down_button;
    KPushButton * m_up_button;
    QPair<AbstractListToolViewDelegate*,QPersistentModelIndex> m_opened_editor;
    QObject * m_editors_object;

    void closeChooser()
    {
        if (m_opened_editor.first)
            m_opened_editor.first->deleteLater();
        m_opened_editor.first = 0;
        m_opened_editor.second = QPersistentModelIndex();
        m_editors_object = 0;
    }

    void removeChoosed()
    {
        if (m_opened_editor.second.isValid() && !m_opened_editor.second.internalPointer())
            const_cast<QAbstractItemModel*>(m_opened_editor.second.model())->removeRow(m_opened_editor.second.row());
        if (m_editors_object)
            m_editors_object->deleteLater();
        m_editors_object = 0;
    }

    void setButtonsEnabled(bool isEnabled)
    {
        m_add_button->setEnabled(isEnabled);
        QModelIndex index = m_list_widget->selectedIndex();
        m_remove_button->setEnabled(isEnabled && index.isValid());
        m_down_button->setEnabled(isEnabled && index.isValid() && index.row() < index.model()->rowCount()-1);
        m_up_button->setEnabled(isEnabled && index.isValid() && index.row() > 0);
    }

    friend class AbstractItemsListViewTool;
};

AbstractItemsListViewTool::AbstractItemsListViewTool(const QString & toolName, Scene * scene, Canvas::SelectionMode selectionMode, QWidget * parent) :
    AbstractItemsTool(scene, selectionMode, parent),
    d(new AbstractItemsListViewToolPrivate)
{
    QGridLayout * layout = new QGridLayout(this);

    // Title
    QLabel * title = new QLabel(toolName, this);
    QFont titleFont = title->font();
    titleFont.setBold(true);
    title->setFont(titleFont);
    layout->addWidget(title,0,0);

    // Move add/remove buttons
    QHBoxLayout * addLayout = new QHBoxLayout();
    d->m_add_button = new KPushButton(KGuiItem("",":action_add.png"));
    d->m_add_button->setIconSize(QSize(16,16));
    d->m_add_button->setFixedSize(24,24);
    d->m_remove_button = new KPushButton(KGuiItem("",":action_remove.png"));
    d->m_remove_button->setIconSize(QSize(16,16));
    d->m_remove_button->setFixedSize(24,24);
    addLayout->addWidget(d->m_add_button);
    addLayout->addWidget(d->m_remove_button);
    addLayout->setSpacing(0);
    layout->addLayout(addLayout,0,1);
    connect(d->m_add_button,SIGNAL(clicked()),this,SLOT(createChooser()));
    connect(d->m_remove_button,SIGNAL(clicked()),this,SLOT(removeSelected()));

    // Move up/down buttons
    QHBoxLayout * moveLayout = new QHBoxLayout();
    d->m_down_button = new KPushButton(KGuiItem("",":arrow_down.png"));
    d->m_down_button->setIconSize(QSize(16,16));
    d->m_down_button->setFixedSize(24,24);
    d->m_up_button = new KPushButton(KGuiItem("",":arrow_top.png"));

    d->m_up_button->setIconSize(QSize(16,16));
    d->m_up_button->setFixedSize(24,24);
    moveLayout->addWidget(d->m_down_button);
    moveLayout->addWidget(d->m_up_button);
    moveLayout->setSpacing(0);
    layout->addLayout(moveLayout,0,2);
    connect(d->m_down_button,SIGNAL(clicked()),this,SLOT(moveSelectedDown()));
    connect(d->m_up_button,SIGNAL(clicked()),this,SLOT(moveSelectedUp()));

    // Effects list
    d->m_list_widget = new AbstractListToolView(this);
    layout->addWidget(d->m_list_widget,1,0,1,-1);
    connect(d->m_list_widget,SIGNAL(selectedIndex(QModelIndex)),this,SLOT(viewCurrentEditor(QModelIndex)));

    this->setLayout(layout);
    this->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::MinimumExpanding);
    layout->setRowStretch(2,1);
    d->setButtonsEnabled(false);
}

AbstractItemsListViewTool::~AbstractItemsListViewTool()
{
    delete d;
}

void AbstractItemsListViewTool::currentItemAboutToBeChanged()
{
    this->chooserCancelled();
}

void AbstractItemsListViewTool::currentItemChanged()
{
    d->m_list_widget->setModel(model());
    d->setButtonsEnabled(true);
}

void AbstractItemsListViewTool::viewCurrentEditor(const QModelIndex & index)
{
    closeEditor();
    d->setButtonsEnabled(true);
    QWidget * editor = createEditor(static_cast<QObject*>(index.internalPointer()), (static_cast<QObject*>(index.internalPointer()) != d->m_editors_object));
    if (editor)
    {
        static_cast<QGridLayout*>(layout())->addWidget(editor,2,0,1,-1);
        editor->show();
    }
}

void AbstractItemsListViewTool::createChooser()
{
    AbstractMovableModel * model = this->model();
    if (model)
    {
        // Calculate chooser position
        int row = 0;
        QModelIndex selectedIndex = d->m_list_widget->selectedIndex();
        if (selectedIndex.isValid())
            row = selectedIndex.row();
        model->insertRow(row);

        // Create chooser
        AbstractListToolViewDelegate * w = new AbstractListToolViewDelegate(this);
        d->m_opened_editor.first = w;
        d->m_opened_editor.second = QPersistentModelIndex(model->index(row,0));
        d->m_list_widget->setIndexWidget(model->index(row,0),w);


        d->m_list_widget->setSelectionMode(QAbstractItemView::NoSelection);
        connect(w,SIGNAL(editorAccepted()),this,SLOT(chooserAccepted()));
        connect(w,SIGNAL(editorClosed()),this,SLOT(chooserCancelled()));
        connect(w,SIGNAL(itemSelected(QString)),this,SLOT(itemSelected(QString)));
        d->setButtonsEnabled(false);
        d->m_list_widget->setSelection(QRect(),QItemSelectionModel::Clear);
    }
}

void AbstractItemsListViewTool::itemSelected(const QString & name)
{
    AbstractListToolViewDelegate * w = d->m_opened_editor.first;
    AbstractMovableModel * model = this->model();
    if (model && w)
    {
        if ((d->m_editors_object = createItem(name)))
        {
            model->setItem(d->m_editors_object, d->m_opened_editor.second);
            QWidget * editor = createEditor(d->m_editors_object, false);
            if (editor)
            {
                static_cast<QGridLayout*>(layout())->addWidget(editor,2,0,1,-1);
                editor->show();
            }
        }
    }
}

void AbstractItemsListViewTool::chooserAccepted()
{
    addItemCommand(d->m_editors_object, d->m_opened_editor.second.row());
    closeEditor();
    d->closeChooser();
    d->m_list_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    d->setButtonsEnabled(true);
}

void AbstractItemsListViewTool::chooserCancelled()
{
    closeEditor();
    d->removeChoosed();
    d->closeChooser();
    d->m_list_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    d->setButtonsEnabled(true);
}

void AbstractItemsListViewTool::removeSelected()
{
    if (!d->m_list_widget)
        return;
    QModelIndex index = d->m_list_widget->selectedIndex();
    AbstractMovableModel * model = this->model();
    if (model && index.isValid())
    {
        if (index.internalPointer())
        {
            ItemRemovedCommand * command = new ItemRemovedCommand(static_cast<QObject*>(index.internalPointer()), index.row(), model);
            PLE_PostUndoCommand(command);
        }
        else
            model->removeRow(index.row());
    }
}

void AbstractItemsListViewTool::moveSelectedDown()
{
    if (!d->m_list_widget)
        return;
    QModelIndex index = d->m_list_widget->selectedIndex();
    AbstractMovableModel * model = this->model();
    if (model && index.row() < model->rowCount()-1)
    {
        if (index.internalPointer())
        {
            QUndoCommand * command = new ItemMoveRowsCommand(index.row(),1,index.row()+2,model);
            PLE_PostUndoCommand(command);
        }
        else
            model->moveRows(index.row(),1,index.row()+2);
    }
    d->setButtonsEnabled(true);
}

void AbstractItemsListViewTool::moveSelectedUp()
{
    if (!d->m_list_widget)
        return;
    QModelIndex index = d->m_list_widget->selectedIndex();
    AbstractMovableModel * model = this->model();
    if (model && index.row() > 0)
    {
        if (index.internalPointer())
        {
            QUndoCommand * command = new ItemMoveRowsCommand(index.row(),1,index.row()-1,model);
            PLE_PostUndoCommand(command);
        }
        else
            model->moveRows(index.row(),1,index.row()-1);
    }
    d->setButtonsEnabled(true);
}

void AbstractItemsListViewTool::closeEditor()
{
    QLayoutItem * itemBrowser = static_cast<QGridLayout*>(layout())->itemAtPosition(2,0);
    if (!itemBrowser)
        return;
    QWidget * browser = itemBrowser->widget();
    if (!browser)
        return;
    static_cast<QGridLayout*>(layout())->removeWidget(browser);
    browser->deleteLater();
}

void AbstractItemsListViewTool::addItemCommand(QObject * item, int row)
{
    AbstractMovableModel * model = this->model();
    if (!item || !model)
        return;
    ItemCreatedCommand * command = new ItemCreatedCommand(item, row, model);
    PLE_PostUndoCommand(command);
}

AbstractListToolViewDelegate::AbstractListToolViewDelegate(AbstractItemsListViewTool * parent) :
    QWidget(parent)
{
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    this->setLayout(layout);
    QStringList registeredDrawers = parent->options();
    KComboBox * comboBox = new KComboBox(this);
    comboBox->addItems(registeredDrawers);
    comboBox->setCurrentIndex(-1);
    connect(comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(emitItemSelected(QString)));
    layout->addWidget(comboBox,1);
    m_acceptButton = new KPushButton(KIcon(":action_check.png"), "", this);
    m_acceptButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    m_acceptButton->setEnabled(false);
    connect(m_acceptButton,SIGNAL(clicked()),this,SLOT(emitEditorAccepted()));
    layout->addWidget(m_acceptButton);
    KPushButton * cancelButton = new KPushButton(KIcon(":action_delete.png"), "", this);
    cancelButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(emitEditorClosed()));
    layout->addWidget(cancelButton);
}
