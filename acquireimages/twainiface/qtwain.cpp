/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-27-10
 * Description : Twain interface
 *
 * Copyright (C) 2002-2003 Stephan Stapel <stephan dot stapel at web dot de>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "qtwain.h"
#include "qtwain.moc"

// Windows includes.

#include <windows.h>

// Qt includes.

#include <QPixmap>
#include <QDataStream>
#include <QByteArray>

QTwain::QTwain(QWidget* parent)
      : QObject(parent), TwainIface()
{
    setParent(parent);
}

QTwain::~QTwain()
{
    ReleaseTwain();
}

bool QTwain::selectSource()
{
    return (SelectSource() == true);
}

bool QTwain::acquire(unsigned int maxNumImages)
{
    int nMaxNum = 1;

    if (maxNumImages == UINT_MAX)
        nMaxNum = TWCPP_ANYCOUNT;


    return (Acquire(nMaxNum) == true);
}

bool QTwain::isValidDriver() const
{
    return (IsValidDriver() == true);
}

void QTwain::setParent(QWidget* parent)
{
    m_parent = parent;
    if (m_parent)
    {
        if (!onSetParent())
        {
            // TODO
        }
    }
}

bool QTwain::onSetParent()
{
    WId id = m_parent->winId();
    InitTwain(id);

    return IsValidDriver();
}

bool QTwain::processMessage(MSG& msg)
{
    if (msg.message == 528) // TODO: don't really know why...
        return false;

    if (m_hMessageWnd == 0)
        return false;

    return (ProcessMessage(msg) == true);
}

void QTwain::CopyImage(TW_MEMREF pdata, TW_IMAGEINFO& info)
{
    if (pdata && (info.ImageWidth != -1) && (info.ImageLength != - 1))
    {
        // Under Windows, Tawin interface return a DIB data structure.
        // See http://en.wikipedia.org/wiki/Device-independent_bitmap#DIBs_in_memory for details.
        HGLOBAL hDIB     = (HGLOBAL)(long)pdata;
        int size         = (int)GlobalSize(hDIB);
        const char* bits = (const char*)GlobalLock(hDIB);

        // DIB is BMP without header. we will add it to load data in QImage using std loader from Qt.
        QByteArray ba, tmp;
        ba.append("BM");
        qint32 filesize = size + 14;
        tmp             = QByteArray(&filesize, 4);
        ba.append(tmp);
        ba.append("\x00\x00\x00\x00");
        qint32 filesize = 14 + 40 + 0;
        tmp             = QByteArray(&filesize, 4);
        ba.append(tmp);

        QDataStream ds(da);
        QImage img;
        ds >> img;
        emit signalImageAcquired(img);
    }
}
