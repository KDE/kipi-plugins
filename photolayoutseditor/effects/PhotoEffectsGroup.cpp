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

#include "PhotoEffectsGroup.h"
#include "PhotoEffectsLoader.h"
#include "AbstractPhoto.h"
#include "AbstractPhotoEffectFactory.h"
#include "AbstractPhotoEffectInterface.h"
#include "UndoCommandEvent.h"
#include "global.h"

#include <QPainter>

#include <klocalizedstring.h>
#include <kapplication.h>

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::PhotoEffectsGroup::MoveItemsUndoCommand : public QUndoCommand
{
        PhotoEffectsGroup * m_model;
        int m_starting_row;
        int m_rows_count;
        int m_destination_row;

    public:
        MoveItemsUndoCommand(int sourcePosition, int sourceCount, int destPosition, PhotoEffectsGroup * model, QUndoCommand * parent = 0) :
            QUndoCommand(i18n("Change effect layer"), parent),
            m_model(model),
            m_starting_row(sourcePosition),
            m_rows_count(sourceCount),
            m_destination_row(destPosition)
        {}
        virtual void redo()
        {
            if (m_model)
            {
                m_model->moveRowsInModel(m_starting_row, m_rows_count, m_destination_row);
                reverse();
            }
        }
        virtual void undo()
        {
            if (m_model)
            {
                m_model->moveRowsInModel(m_starting_row, m_rows_count, m_destination_row);
                reverse();
            }
        }
    private:
        void reverse()
        {
            int temp = m_destination_row;
            m_destination_row = m_starting_row;
            m_starting_row = temp;
            if (m_destination_row > m_starting_row)
                m_destination_row += m_rows_count;
            else
                m_starting_row -= m_rows_count;
        }
};
class KIPIPhotoLayoutsEditor::PhotoEffectsGroup::RemoveItemsUndoCommand : public QUndoCommand
{
        int m_starting_pos;
        int m_count;
        PhotoEffectsGroup * m_model;
        QList<AbstractPhotoEffectInterface*> tempItemsList;
    public:
        RemoveItemsUndoCommand(int startingPos, int count, PhotoEffectsGroup * model, QUndoCommand * parent = 0) :
            QUndoCommand(i18n("Remove effect"), parent),
            m_starting_pos(startingPos),
            m_count(count),
            m_model(model)
        {}
        ~RemoveItemsUndoCommand()
        {
            qDeleteAll(tempItemsList);
        }
        virtual void redo()
        {
            tempItemsList = m_model->removeRowsInModel(m_starting_pos, m_count);
        }
        virtual void undo()
        {
            m_model->insertRemovedRowsInModel(tempItemsList, m_starting_pos);
            tempItemsList.clear();
        }
};
class KIPIPhotoLayoutsEditor::PhotoEffectsGroup::InsertItemUndoCommand : public QUndoCommand
{
        int m_row;
        AbstractPhotoEffectInterface * m_effect;
        PhotoEffectsGroup * m_model;
        bool commandCorupped;
        bool done;
    public:
        InsertItemUndoCommand(int row, AbstractPhotoEffectInterface * effect, PhotoEffectsGroup * model, QUndoCommand * parent = 0) :
            QUndoCommand(i18n("Add effect"), parent),
            m_row(row),
            m_effect(effect),
            m_model(model),
            commandCorupped(false),
            done(false)
        {}
        ~InsertItemUndoCommand()
        {
            if (!done)
                m_effect->deleteLater();
        }
        virtual void redo()
        {
            if (commandCorupped || !m_model)
                return;
            if (m_model->insertRow(m_row))
                m_model->setEffectPointer(m_row,m_effect);
            else
                commandCorupped = true;
            done = true;
        }
        virtual void undo()
        {
            if (commandCorupped || !m_model)
                return;
            QList<AbstractPhotoEffectInterface*> removedList = m_model->removeRowsInModel(m_row,1);
            if (removedList.count() != 1 || removedList.at(0) != m_effect)
                commandCorupped = true;
            done = false;
        }
};

PhotoEffectsGroup::PhotoEffectsGroup(AbstractPhoto * photo, QObject * parent) :
    AbstractMovableModel(parent),
    m_photo(photo)
{
}

QDomElement PhotoEffectsGroup::toSvg(QDomDocument & document) const
{
    QDomElement effectsGroup = document.createElement("effects");
    foreach (AbstractPhotoEffectInterface * effect, m_effects_list)
    {
        AbstractPhotoEffectFactory * factory = PhotoEffectsLoader::getFactoryByName(effect->factory()->effectName());
        if (factory)
        {
            QDomElement effectSvg = factory->toSvg(effect, document);
            if (!effectSvg.isNull())
                effectsGroup.appendChild(effectSvg);
        }
        /// TODO: what if there is no given effect plugin?
    }
    return effectsGroup;
}

PhotoEffectsGroup * PhotoEffectsGroup::fromSvg(const QDomElement & element, AbstractPhoto * graphicsItem)
{
    QDomElement temp = element;
    if (temp.tagName() != "effects")
        temp = temp.firstChildElement("effects");
    if (temp.isNull())
        return 0;
    PhotoEffectsGroup * group = new PhotoEffectsGroup(0);
    QDomNodeList effectsList = temp.childNodes();
    for (int i = 0; i < effectsList.count(); ++i)
    {
        QDomElement effect = effectsList.at(i).toElement();
        if (effect.isNull())
            continue;
        AbstractPhotoEffectFactory * factory = PhotoEffectsLoader::getFactoryByName( effect.attribute("name") );
        if (!factory)
            continue;
        AbstractPhotoEffectInterface * interface = factory->fromSvg(effect);
        if (interface)
            group->push_back(interface);
    }
    group->m_photo = graphicsItem;
    return group;
}

void PhotoEffectsGroup::push_back(AbstractPhotoEffectInterface * effect)
{
    m_effects_list.push_back(effect);
    connect(effect, SIGNAL(changed()), this, SLOT(emitEffectsChanged()));
    effect->setParent(this);
    effect->setGroup(this);
    emit layoutChanged();
}

void PhotoEffectsGroup::push_front(AbstractPhotoEffectInterface * effect)
{
    m_effects_list.push_back(effect);
    connect(effect, SIGNAL(changed()), this, SLOT(emitEffectsChanged()));
    effect->setParent(this);
    effect->setGroup(this);
    emit layoutChanged();
}

QImage PhotoEffectsGroup::apply(const QImage & image)
{
    QImage temp = image;
    foreach (AbstractPhotoEffectInterface * effect, m_effects_list)
        if (effect)
            temp = effect->apply(temp);
    return temp;
}

AbstractPhoto * PhotoEffectsGroup::photo() const
{
    return m_photo;
}

QObject * PhotoEffectsGroup::item(const QModelIndex & index) const
{
    if (index.isValid() && index.row() < rowCount())
        return m_effects_list.at(index.row());
    return 0;
}

void PhotoEffectsGroup::setItem(QObject * item, const QModelIndex & index)
{
    AbstractPhotoEffectInterface * effect = dynamic_cast<AbstractPhotoEffectInterface*>(item);
    if (!effect || !index.isValid())
        return;
    int row = index.row();
    if (row < 0 || row >= rowCount())
        return;
    m_effects_list.removeAt(row);
    m_effects_list.insert(row, effect);
    effect->setParent(this);
    effect->setGroup(this);
    emitEffectsChanged(effect);
}

AbstractPhotoEffectInterface * PhotoEffectsGroup::graphicsItem(const QModelIndex & index) const
{
    return static_cast<AbstractPhotoEffectInterface*>(index.internalPointer());
}

bool PhotoEffectsGroup::moveRows(int sourcePosition, int sourceCount, int destPosition)
{
    if (    sourceCount                                          &&
            sourcePosition < rowCount()                          &&
            sourcePosition+sourceCount <= rowCount()             &&
            destPosition <= rowCount()                           &&
            sourcePosition != destPosition                       &&
            sourcePosition != destPosition-1                     &&
            sourcePosition >= 0                                  &&
            destPosition >= 0)
    {
        QUndoCommand * command = new MoveItemsUndoCommand(sourcePosition, sourceCount, destPosition, this);
        PLE_PostUndoCommand(command);
        return true;
    }
    return false;
}

bool PhotoEffectsGroup::insertRow(int row, AbstractPhotoEffectInterface * effect)
{
    if (row < 0 || row > rowCount() || !effect)
        return false;
    QUndoCommand * command = new InsertItemUndoCommand(row,effect,this);
    PLE_PostUndoCommand(command);
    return true;
}

bool PhotoEffectsGroup::insertRow(int row, const QModelIndex & index)
{
    return QAbstractItemModel::insertRow(row,index);
}

int PhotoEffectsGroup::columnCount(const QModelIndex & /*parent*/) const
{
    return 1;
}

QVariant PhotoEffectsGroup::data(const QModelIndex & index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (!index.isValid())
        return i18n("Effect name");
    else
    {
        AbstractPhotoEffectInterface * effect = graphicsItem(index);
        if (effect)
            return effect->toString();
        else
            return QVariant();
    }
}

Qt::ItemFlags PhotoEffectsGroup::flags(const QModelIndex & index) const
{
    Qt::ItemFlags result = QAbstractItemModel::flags(index);
    if (index.isValid() && !index.internalPointer())
        result |= Qt::ItemIsEditable;
    return result;
}

QModelIndex PhotoEffectsGroup::index(int row, int column, const QModelIndex & parent) const
{
    if (column != 0)
        return QModelIndex();
    if (row >= m_effects_list.count())
        return QModelIndex();
    if (parent.isValid())
        return QModelIndex();
    return createIndex(row,column,m_effects_list.at(rowCount()-row-1));
}

bool PhotoEffectsGroup::insertRows(int row, int count, const QModelIndex & parent)
{
    if (row < 0 || row > rowCount() || count < 1 || parent.isValid())
        return false;
    beginInsertRows(parent, row, row+count-1);
    row = rowCount()-row;
    while(count--)
        m_effects_list.insert(row,0);
    endInsertRows();
    emit layoutChanged();
    return true;
}

QModelIndex PhotoEffectsGroup::parent(const QModelIndex & /*index*/) const
{
    return QModelIndex();
}

int PhotoEffectsGroup::rowCount(const QModelIndex & parent) const
{
    if (!parent.isValid())
        return this->m_effects_list.count();
    else
        return 0;
}

bool PhotoEffectsGroup::removeRows(int row, int count, const QModelIndex & parent)
{
    if (!count || parent.isValid() || row < 0 || row >= rowCount() || row+count-1 >= rowCount())
        return false;
    beginRemoveRows(QModelIndex(), row, row+count-1);
    while (count--)
        m_effects_list.removeAt(row);
    endRemoveRows();
    this->emitEffectsChanged();
    emit layoutChanged();
    return true;
}

void PhotoEffectsGroup::emitEffectsChanged(AbstractPhotoEffectInterface * effect)
{
    if (!m_photo)
        return;
    m_photo->refresh();
    if (effect)
    {
        int row = m_effects_list.count()-m_effects_list.indexOf(effect)-1;
        QModelIndex indexChanged = index(row,0);
        emit dataChanged(indexChanged,indexChanged);
    }
    else if (rowCount())
        emit dataChanged(index(0,0),index(rowCount()-1,0));
    emit effectsChanged();
}

void PhotoEffectsGroup::moveRowsInModel(int sourcePosition, int sourceCount, int destPosition)
{
    beginMoveRows(QModelIndex(), sourcePosition, sourcePosition+sourceCount-1, QModelIndex(), destPosition);

    // Inverse directions becouse effects stack is presented in reverse order
    destPosition = rowCount()-destPosition;
    sourcePosition = rowCount()-sourcePosition-1;

    QList<AbstractPhotoEffectInterface*> movingItems;
    if (destPosition > sourcePosition)
        destPosition -= sourceCount;
    while(sourceCount--)
        movingItems.push_back(m_effects_list.takeAt(sourcePosition));
    for ( ; movingItems.count() ; movingItems.pop_back())
        m_effects_list.insert(destPosition, movingItems.last());

    endMoveRows();
    emitEffectsChanged();
    emit layoutChanged();
}

QList<AbstractPhotoEffectInterface*> PhotoEffectsGroup::removeRowsInModel(int startingPosition, int count)
{
    beginRemoveRows(QModelIndex(), startingPosition, startingPosition+count-1);

    // Inverse directions becouse effects stack is presented in reverse order
    startingPosition = rowCount()-startingPosition-1;

    QList<AbstractPhotoEffectInterface*> removedItems;
    int removedCount = 0;
    while (count--)
    {
        AbstractPhotoEffectInterface * temp = m_effects_list.takeAt(startingPosition);
        if (temp)
        {
            ++removedCount;
            removedItems.push_back(temp);
            temp->setParent(0);
            temp->setGroup(0);
        }
    }

    endRemoveRows();

    // Prevent emmiting changes then empty row is removed
    if (removedCount)
        emitEffectsChanged();

    emit layoutChanged();

    return removedItems;
}

void PhotoEffectsGroup::insertRemovedRowsInModel(const QList<AbstractPhotoEffectInterface*> & itemList, int startingPosition)
{
    beginInsertRows(QModelIndex(), startingPosition, startingPosition+itemList.count()-1);

    // Inverse directions becouse effects stack is presented in reverse order
    startingPosition = rowCount()-startingPosition;

    foreach (AbstractPhotoEffectInterface * effect, itemList)
    {
        m_effects_list.insert(startingPosition, effect);
        ++startingPosition;
        effect->setParent(this);
        effect->setGroup(this);
    }

    endInsertRows();
    emitEffectsChanged();
    emit layoutChanged();
}

void PhotoEffectsGroup::setEffectPointer(int row, AbstractPhotoEffectInterface * effect)
{
    if (row < 0 || row >= rowCount())
        return;
    int effectiveRow = rowCount()-row-1;
    AbstractPhotoEffectInterface * temp = m_effects_list[effectiveRow];
    if (temp)
        delete temp;
    m_effects_list[effectiveRow] = effect;
    effect->setParent(this);
    effect->setGroup(this);
    emitEffectsChanged(effect);
    emit dataChanged(index(row,0),index(row,0));
}
