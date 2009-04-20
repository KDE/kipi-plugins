/* ============================================================
 * Authors: Marcel Wiesweg <marcel.wiesweg@gmx.de>
 * Date   : 2007-02-13
 * Description : Layouting photos on a page
 * 
 * Copyright 2007 by Marcel Wiesweg
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

#include <math.h>

#include <QList>
#include <QtDebug>

#include "layouttree.h"
#include "atkinspagelayout.h"


class AtkinsPageLayoutPriv
{
public:
    AtkinsPageLayoutPriv()
    {
        tree = 0;
    }

    QMap<int, int> indexMap;
    LayoutTree *tree;
    QRectF pageRect;
};

AtkinsPageLayout::AtkinsPageLayout(const QRectF &pageRect)
{
    d = new AtkinsPageLayoutPriv;
    d->pageRect = pageRect;
    d->tree = new LayoutTree(aspectRatio(d->pageRect.size()), absoluteArea(d->pageRect.size()));
}

AtkinsPageLayout::~AtkinsPageLayout()
{
    delete d->tree;
    delete d;
}

double AtkinsPageLayout::aspectRatio(const QSizeF &size)
{
    return size.height() / size.width();
}

double AtkinsPageLayout::absoluteArea(const QSizeF &size)
{
    return size.height() * size.width();
}

void AtkinsPageLayout::addLayoutItem(int key, const QSizeF &size)
{
    double relativeArea = absoluteArea(size) / absoluteArea(d->pageRect.size());
    addLayoutItem(key, aspectRatio(size), relativeArea);
}

void AtkinsPageLayout::addLayoutItem(int key, double aspectRatio, double relativeArea)
{
    int index = d->tree->addImage(aspectRatio, relativeArea);
    d->indexMap[key] = index;
}

QRectF AtkinsPageLayout::itemRect(int key)
{
    QMap<int,int>::iterator it = d->indexMap.find(key);
    if (it != d->indexMap.end())
    {
        // get rect relative to 0,0
        QRectF rect = d->tree->drawingArea(*it, d->pageRect);
        // translate to page rect origin
        rect.translate(d->pageRect.topLeft());
        return rect;
    }
    return QRectF();
}



