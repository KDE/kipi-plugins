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

#ifndef _QTWAINMAINWINDOW_H_
#define _QTWAINMAINWINDOW_H_

// Qt includes.

#include <QMainWindow>
#include <QPushButton>
#include <QPixmap>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

// Local includes.

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
    QPushButton*     m_pSelectButton;
    QPushButton*     m_pAcquireButton;
    QPixmap*         m_pPixmap;
    QGridLayout*     m_pVBox;
    QWidget*         m_pWidget;
    QLabel*          m_pLabel;
};

#endif // !_QTWAINMAINWINDOW_H_
