/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "bracketstack.h"

// Qt includes

#include <QTreeWidget>

namespace KIPIExpoBlendingPlugin
{

BracketStackItem::BracketStackItem(QTreeWidget* parent)
                : QTreeWidgetItem(parent)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    setCheckState(0, Qt::Unchecked);
}

BracketStackItem::~BracketStackItem()
{
}

void BracketStackItem::setUrl(const KUrl& url)
{
    m_url = url;
}

KUrl BracketStackItem::url() const
{
    return m_url;
}

void BracketStackItem::setAlignedUrl(const KUrl& alignedUrl)
{
    m_alignedUrl = alignedUrl;
}

KUrl BracketStackItem::alignedUrl() const
{
    return m_alignedUrl;
}

void BracketStackItem::setThumbnail(const QPixmap& pix)
{
}

void BracketStackItem::setExposure(const QString& exp)
{
}

bool BracketStackItem::isOn() const
{
    return (checkState(0) == Qt::Checked ? true : false);
}

void BracketStackItem::setOn(bool b)
{
    setCheckState(0, b ? Qt::Checked : Qt::Unchecked);
}

}  // namespace KIPIExpoBlendingPlugin
