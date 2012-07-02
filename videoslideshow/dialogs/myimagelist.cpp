/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-06-24
 * Description : file list view and items.
 *
 * Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "myimagelist.h"

// Qt includes

#include <QFileInfo>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>

namespace KIPIVideoSlideShowPlugin
{

MyImageList::MyImageList(QWidget* const parent)
    : KPImagesList(parent)
{
    setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    listView()->setColumnLabel(KPImagesListView::Filename, i18n("Image"));
    listView()->setColumn(static_cast<KPImagesListView::ColumnType>(MyImageList::SPECIALEFFECT),  i18n("Special Effect"),         true);
    listView()->setColumn(static_cast<KPImagesListView::ColumnType>(MyImageList::TIME),           i18n("Frames per Image"),       true);
    listView()->setColumn(static_cast<KPImagesListView::ColumnType>(MyImageList::TRANSITION),     i18n("Transition"),             true);
    listView()->setColumn(static_cast<KPImagesListView::ColumnType>(MyImageList::TRANSSPEED),     i18n("Transition Speed"),       true);
}

MyImageList::~MyImageList()
{
}

/** Replaces the ImagesList::slotAddImages method, so that
  * MyImageListViewItems can be added instead of ImagesListViewItems
  */
void MyImageList::slotAddImages(const KUrl::List& list)
{
    // Figure out which of the supplied URL's should actually be added and which
    // of them already exist.
    bool found = false;

    for (KUrl::List::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it)
    {
        KUrl imageUrl = *it;
        found         = false;

        for (int i = 0; i < listView()->topLevelItemCount(); ++i)
        {
            MyImageListViewItem* currItem = dynamic_cast<MyImageListViewItem*>(listView()->topLevelItem(i));
            if (currItem && currItem->url() == imageUrl)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            new MyImageListViewItem(listView(), imageUrl);
        }
    }

    // Duplicate the signalImageListChanged of the ImageWindow, to enable the
    // upload button again.
    emit signalImageListChanged();
}

void MyImageList::slotRemoveItems()
{
    bool find = false;
    do
    {
        find = false;
        QTreeWidgetItemIterator it(listView());
        while (*it)
        {
            MyImageListViewItem* item = dynamic_cast<MyImageListViewItem*>(*it);
            if (item->isSelected())
            {
                delete item;
                find = true;
                break;
            }
        ++it;
        }
    }
    while(find);
}

// ------------------------------------------------------------------------------------------------

MyImageListViewItem::MyImageListViewItem(KPImagesListView* const view, const KUrl& url)
    : KPImagesListViewItem(view, url)
{
    setEffectName("None");
    setTime(25);
    setTransition("Random");
    setTransitionSpeed("Medium");
}

MyImageListViewItem::~MyImageListViewItem()
{
}

void MyImageListViewItem::setEffectName(const QString& str)
{
    m_effect = str;
    setText(MyImageList::SPECIALEFFECT, m_effect);
}

QString MyImageListViewItem::EffectName() const
{
    return m_effect;
}

void MyImageListViewItem::setTime(const int time)
{
    m_time = time;
    setText(MyImageList::TIME, QString::number(time));
}

void MyImageListViewItem::setTransition(const QString& str)
{
    m_transition = str;
    setText(MyImageList::TRANSITION, m_transition);
}

QString MyImageListViewItem::getTransition() const
{
    return m_transition;
}

int MyImageListViewItem::getTime() const
{
    return m_time;
}

QString MyImageListViewItem::getTransitionSpeed() const
{
    return m_transSpeed;
}

void MyImageListViewItem::setTransitionSpeed(const QString& str)
{
    m_transSpeed = str;
    setText(MyImageList::TRANSSPEED, m_transSpeed);
}

int MyImageList::getTotalFrames()
{
    return 100;
}

} // namespace KIPIVideoSlideShowPlugin
