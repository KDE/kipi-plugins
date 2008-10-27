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

#include <windows.h>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>
#include "dib.h"
#include "qtwainsubstitute.h"





QTwainSubstitute::QTwainSubstitute(QWidget* parent)
	: QTwainInterface(parent)
{
} // !QTwainSubstitute()



QTwainSubstitute::~QTwainSubstitute()
{
} // !~QTwainSubstitute()



bool QTwainSubstitute::selectSource()
{
	QMessageBox::information(m_pParent, "Select Source", "Here, the source should be selected by the user", QMessageBox::Ok);
	return true;
} // !selectSource()



bool QTwainSubstitute::acquire(unsigned int maxNumImages)
{
	QString s = QFileDialog::getOpenFileName(m_pParent, tr("Open File"),
		                                     "/home",
											 "Images (*.png *.jpg)");

	if (s != QString::null)
	{
		QPixmap* pPixmap = new QPixmap();
		pPixmap->load(s);

		if (emitPixmaps())
		{			
			emit pixmapAcquired(pPixmap);			
		}
		else
		{
			CDIB* pDib = QTwainInterface::convertToDib(pPixmap);
			emit dibAcquired(pDib);
			delete pPixmap;
		}
	}

	return true;
} // !acquire()



bool QTwainSubstitute::isValidDriver() const
{
	return true;
} // !isValidDriver()



bool QTwainSubstitute::processMessage(MSG& msg)
{
	return false;
} // !processMessage()
