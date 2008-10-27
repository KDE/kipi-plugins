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

#ifndef _QTWAINMAINWINDOW_H_
#define _QTWAINMAINWINDOW_H_


#include <QMainWindow>
#include <QPushButton>
#include <QPixmap>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include "qtwaininterface.h"


class QTwainMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	/** 
	 * Standard constructor
	 */
	QTwainMainWindow(QWidget* parent = 0,  Qt::WindowFlags f =  Qt::Window);

	/**
	 * Standard destructor
	 */
	virtual ~QTwainMainWindow();

	/**
	 * @name overwritten to hook-in twain interface
	 */
	/*@{*/
	void showEvent(QShowEvent* thisEvent);
	
	/*@}*/

protected:	
	void paintEvent(QPaintEvent* thisEvent);
	void resizeEvent(QResizeEvent* thisEvent);
	virtual bool winEvent(MSG* pMsg,long * result);

private slots:
	void onSelectButton();
	void onAcquireButton();

	void onDibAcquired(CDIB* pDib);


protected:
	QTwainInterface* m_pTwain;
	QPushButton* m_pSelectButton;
	QPushButton* m_pAcquireButton;
	QPixmap* m_pPixmap;
	QGridLayout* m_pVBox;
	QWidget *m_pWidget;
	QLabel *m_pLabel;
};


#endif // !_QTWAINMAINWINDOW_H_
