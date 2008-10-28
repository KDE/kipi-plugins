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

#include "qtwainmainwindow.h"
#include "qtwainmainwindow.moc"

// Qt includes.

#include <QPainter>
#include <QTimer>

// KDE includes.

#include <kmessagebox.h>
#include <klocale.h>

// Local includes.

#include "qtwain.h"
#include "dib.h"

QTwainMainWindow::QTwainMainWindow(QWidget* parent, Qt::WindowFlags f)
                : QMainWindow(parent, f)
{
    m_pTwain         = new QTwain(0);
    m_pWidget        = new QWidget(this);
    m_pVBox          = new QGridLayout(m_pWidget);
    m_pLabel         = new QLabel();

    m_pVBox->addWidget(m_pLabel, 1, 0, 2, 2, 0);
    m_pWidget->setMinimumSize(500, 500);
    m_pPixmap = 0;

    connect(m_pTwain, SIGNAL(dibAcquired(CDIB*)),
            this, SLOT(onDibAcquired(CDIB*)));

    setMinimumSize(800, 600);
    setGeometry(300, 300, 800, 600) ;

    QTimer::singleShot(0, this, SLOT(slotInit()));
}

QTwainMainWindow::~QTwainMainWindow()
{
    delete m_pTwain;
}

void QTwainMainWindow::slotInit()
{
    m_pTwain->setParent(this);
    m_pTwain->selectSource();

    if (!m_pTwain->acquire())
        KMessageBox::error(this, i18n("Cannot acquire image..."));
}

bool QTwainMainWindow::winEvent(MSG* pMsg, long* /*result*/)
{
    //return (m_pTwain->processMessage(*pMsg) == true);
    m_pTwain->processMessage(*pMsg);
    return false;
}

void QTwainMainWindow::onDibAcquired(CDIB* pDib)
{
    delete m_pPixmap;
    m_pPixmap = QTwainInterface::convertToPixmap(pDib);
    m_pLabel->setPixmap(*m_pPixmap);
    delete pDib;
}

void QTwainMainWindow::paintEvent(QPaintEvent*)
{
    if (m_pPixmap)
    {
        //QPainter p(m_pWidget);
        //m_pVBox->addWidget(p,1,0,0);
        /*p.drawPixmap(0, 71, 
                     *m_pPixmap,
                     0, 0,
                     width(), height() - 71);
                     */
    }
}
