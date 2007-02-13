/***************************************************************************
 *   Copyright (C) 2007 by Markus Leuthold   *
 *   kusi (+at) forum.titlis.org   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.        *
 ***************************************************************************/
#ifndef TIMER_H
#define TIMER_H
#include <qstring.h>
#include <qdatetime.h>
#include <iostream>

/**
 * @short convenience class for profiling
 * @author Markus Leuthold <kusi (+at) forum.titlis.org>
 * @version 0.1
 */
class Timer{
public:
    Timer();
    ~Timer();
	void start();
	void at(QString s);
protected:
	QTime timer;
	int meantime;
};

#endif
