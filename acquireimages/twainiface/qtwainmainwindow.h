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

#ifndef QTWAINMAINWINDOW_H
#define QTWAINMAINWINDOW_H

// Qt includes.

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QImage>

// Local includes.

#include "qtwain.h"

class QTwainMainWindow : public QMainWindow
{
    Q_OBJECT

public:

    QTwainMainWindow(QWidget* parent=0, Qt::WindowFlags f=Qt::Window);
    virtual ~QTwainMainWindow();

protected:

    virtual bool winEvent(MSG* pMsg, long *result);

private slots:

    void slotInit();
    void slotImageAcquired(const QImage& img);

protected:

    QWidget*     m_pWidget;

    QLabel*      m_pLabel;

    QTwain*      m_pTwain;
};

#endif /* QTWAINMAINWINDOW_H */
