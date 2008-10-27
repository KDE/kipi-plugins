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

#include <QWidget>
#include <QPixmap>
#include <QImage>

#include "dib.h"
#include "qtwain.h"






QTwain::QTwain(QWidget* parent)
	: QTwainInterface(parent), CTwain()
{	
} // !QTwain()




QTwain::~QTwain()
{	
	ReleaseTwain();
} // !~QTwain()



bool QTwain::selectSource()
{
	return (SelectSource() == true);
} // !selectSource()



bool QTwain::acquire(unsigned int maxNumImages)
{
	unsigned int nMaxNum = 1;

	if (maxNumImages == UINT_MAX)
		nMaxNum = TWCPP_ANYCOUNT;
		

	return (Acquire(nMaxNum) == true);
} // !acquire()



bool QTwain::isValidDriver() const
{
	return (IsValidDriver() == true);
} // !isValidDriver()



// internal, implementation of CTwain function
void QTwain::CopyImage(HANDLE hBitmap, TW_IMAGEINFO& info)
{
	CDIB* dib = new CDIB();
	CDIB temp_dib;
	temp_dib.CreateFromHandle(hBitmap, info.BitsPerPixel);
	if (info.BitsPerPixel == 24)
		*dib = temp_dib;
	else
		Create24Bit(temp_dib, *dib);

	if (emitPixmaps())
	{
		QPixmap* retval = convertToPixmap(dib);
		emit pixmapAcquired(retval);
		delete dib;
	}
	else
	{
		emit dibAcquired(dib);
	}
} // !CopyImage()



bool QTwain::processMessage(MSG& msg)
{
	if (msg.message == 528) // don't really know why...
		return false;

	if (m_hMessageWnd == 0)
		return false;
	return (ProcessMessage(msg) == true);
} // !processMessage()



static unsigned char masktable[] = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };

void QTwain::Create24Bit(CDIB& source,CDIB& dest)
{
	int pal;
	dest.Create(source.Width(),source.Height(),24);
	pal = source.GetPaletteSize();
	BYTE palet[768];

	for(int i=0; i < pal; i++)
	{
		COLORREF col = source.PaletteColor(i);
		palet[i*3+2] = GetRValue(col);
		palet[i*3+1] = GetGValue(col);
		palet[i*3+0] = GetBValue(col);
	}

	int j;
	int i;
	BYTE *src,*dst,*ptr;

	for(i=0; i < source.Height(); i++)
	{
		src = source.GetLinePtr(i);
		dst = dest.GetLinePtr(i);
		ptr = dst;
		int index;
		for(j=0; j < source.Width(); j++,ptr+=3)
		{
			switch(pal)
			{
			case 2:
				if(src[j>>3] & masktable[j&7])
				{
					index = 1;
				}
				else
				{
					index = 0;
				}
				break;
			case 16:
				if(j & 1)
				{
					index = src[j>>1] & 0x0f;
				}
				else
				{
					index = (src[j>>1] >> 4) & 0x0f;
				}
				break;
			case 256:
				index = src[j];
				break;
			}
			Q_ASSERT(index < pal);
			memcpy(ptr,palet+index*3,3);
		}
		index = (ptr - dst)/3;
		Q_ASSERT(index <= source.Width());
	}		
} // !Create24Bit()



bool QTwain::onSetParent()
{
	WId id = m_pParent->winId();
	InitTwain(id);

	return IsValidDriver();
} // !onSetParent()
