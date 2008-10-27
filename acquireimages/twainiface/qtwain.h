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

#ifndef _QTWAIN_H_
#define _QTWAIN_H_

#include "qtwaininterface.h"
#include "twaincpp.h"


const unsigned int AnyCount = UINT_MAX;



class QTwain : public QTwainInterface, CTwain
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
