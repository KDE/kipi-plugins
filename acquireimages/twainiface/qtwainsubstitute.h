/*
QTwain interface class set
Copyright (C) 2002-2003         Stephan Stapel
                                stephan.stapel@web.de

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

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
