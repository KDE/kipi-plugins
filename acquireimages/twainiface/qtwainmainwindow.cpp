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
#include <QPixmap>
#include <QGridLayout>

// KDE includes.

#include <kmessagebox.h>
#include <klocale.h>

QTwainMainWindow::QTwainMainWindow(QWidget* parent, Qt::WindowFlags f)
                : QMainWindow(parent, f)
{
    m_pTwain          = new QTwain(this);
    m_pWidget         = new QWidget(this);
    QGridLayout *grid = new QGridLayout(m_pWidget);
    m_pLabel          = new QLabel();

    grid->addWidget(m_pLabel, 1, 0, 2, 2, 0);
    m_pWidget->setMinimumSize(500, 500);

    connect(m_pTwain, SIGNAL(signalImageAcquired(const QImage&)),
            this, SLOT(slotImageAcquired(const QImage&)));

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

void QTwainMainWindow::slotImageAcquired(const QImage& img)
{
    QPixmap pix = QPixmap::fromImage(img);
    m_pLabel->setPixmap(pix);
}
