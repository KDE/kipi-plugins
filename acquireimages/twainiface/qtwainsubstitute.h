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

#ifndef _QTWAINSUBSTITUTE_H_
#define _QTWAINSUBSTITUTE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "qtwaininterface.h"

class QTwainSubstitute : public QTwainInterface
{

public:

    /**
     * Standard constructor
     */
    QTwainSubstitute(QWidget* parent = 0);

    /**
     * Standard destructor
     */
    virtual ~QTwainSubstitute();

    /**
     * @name necessary function implementations
     */
    /*@{*/
    virtual bool selectSource();
    virtual bool acquire(unsigned int maxNumImages = UINT_MAX);
    virtual bool isValidDriver() const;
    virtual bool processMessage(MSG& msg);
    /*@}*/
};

#endif // !_QTWAINSUBSTITUTE_H_
