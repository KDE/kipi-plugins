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

#ifndef _QTWAIN_H_
#define _QTWAIN_H_

#include "qtwaininterface.h"
#include "twaincpp.h"

const unsigned int AnyCount = UINT_MAX;

class QTwain : public QTwainInterface, TwainIface
{
public:

    /**
     * Standard constructor
     */
    QTwain(QWidget* parent = 0);

    /**
     * Standard destructor
     */
    virtual ~QTwain();

    /**
     * @name necessary function implementations
     */
    /*@{*/
    virtual bool selectSource();
    virtual bool acquire(unsigned int maxNumImages = AnyCount);
    virtual bool isValidDriver() const;
    virtual bool processMessage(MSG& msg);
    /*@}*/

protected:

    /**
     * Implemented hook-in function
     */
    virtual bool onSetParent();

    virtual void CopyImage(HANDLE hBitmap, TW_IMAGEINFO& info);
    void Create24Bit(CDIB& source,CDIB& dest);
};

#endif // !_QTWAIN_H_
