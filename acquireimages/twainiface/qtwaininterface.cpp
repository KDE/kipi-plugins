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

#include <QWidget>
#include <QPixmap>
#include <QImage>

#include "dib.h"
#include "dibutil.h"
#include "dibapi.h"
#include "qtwaininterface.h"

QTwainInterface::QTwainInterface(QWidget* parent)
{
    m_bEmitPixmaps = false;
    setParent(parent);
}

QTwainInterface::~QTwainInterface()
{
    setParent(0);
}

void QTwainInterface::setParent(QWidget* parent)
{
    m_pParent = parent;
    if (parent)
    {
        if (!onSetParent())
        {
            /**
             * @todo print error message
             */
        }
    }
}



QPixmap* QTwainInterface::convertToPixmap(CDIB* pDib) 
//,const unsigned int nWidth,const unsigned int nHeight)
{
    if (!pDib)
        return 0;

    unsigned int nUseWidth;
    unsigned int nUseHeight;

//    if (nUseWidth == 0)
    nUseWidth = pDib->Width();
//    if (nUseHeight == 0)
    nUseHeight = pDib->Height();

    //QPixmap* retval = new QPixmap(nUseWidth, nUseHeight);
    HANDLE hdib=pDib->DIBHandle();
    HPALETTE pepe =CreateDIBPalette(hdib);
    HBITMAP ima =DIBToBitmap(hdib,pepe);
    QPixmap* retval2= new QPixmap(QPixmap::fromWinHBITMAP ( ima,QPixmap::NoAlpha));

    //return retval2;
    //HDC hehe;

    /*pDib->BitBlt(hehe, 
                 0, 0, 
                 nUseWidth, nUseHeight,
                 0, 0);
                 QPixmap buffer(400, 200);
...
bitBlt(&buffer, 0, 0, src, 0, 0, 400, 200);
...
QPainter p(this);
p.drawPixmap(0, 0, buffer);

    //HBITMAP ima =pDib->DIBToDDB(pdib);
    //QPixmap* retval2= new QPixmap(retval->fromWinHBITMAP ( ima,QPixmap::NoAlpha));
*/
    return retval2;
}

QImage* QTwainInterface::convertToImage(CDIB* pDib, unsigned int nWidth, unsigned int nHeight)
{
    if (!pDib)
        return 0;

    unsigned int nUseWidth  = nWidth;
    unsigned int nUseHeight = nHeight;

    if (nUseWidth == 0)
        nUseWidth = pDib->Width();
    if (nUseHeight == 0)
        nUseHeight = pDib->Height();    

    QPixmap pixmap(pDib->Width(), pDib->Height());
/*    pDib->BitBlt(pixmap.handle(), 
                 0, 0, 
                 pDib->Width(), pDib->Height(),
                 0, 0);
*/
    QImage* retval = new QImage( pixmap.toImage().scaled(nUseWidth, nUseHeight,Qt::KeepAspectRatio,Qt::SmoothTransformation) );
    return retval;
}

CDIB* QTwainInterface::convertToDib(QPixmap* pPixmap, unsigned int nWidth, unsigned int nHeight)
{
    CDIB* pRetval = new CDIB();

    /**
     * @todo implement this function
     */

    return pRetval;
}

bool QTwainInterface::onSetParent()
{
    return true;
}
