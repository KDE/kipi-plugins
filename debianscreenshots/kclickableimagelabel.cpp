/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-29
 * Description : a kipi plugin to export images to Debian Screenshots
 *
 * Copyright (C) 2010 by Pau Garcia i Quiles <pgquiles at elpauer dot org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "kclickableimagelabel.moc"

// Qt includes

#include <QDesktopServices>
#include <QRect>
#include <QCursor>
#include <QDebug>

namespace KIPIDebianScreenshotsPlugin
{

KClickableImageLabel::KClickableImageLabel(QWidget* const parent, Qt::WindowFlags f)
    : QLabel(parent, f), m_url(QUrl())
{
    setMouseTracking(true);
}

KClickableImageLabel::KClickableImageLabel(const QString& text, QWidget* const parent, Qt::WindowFlags f)
    : QLabel(text, parent, f), m_url(QUrl())
{
}

void KClickableImageLabel::setUrl(const QUrl& url)
{
    m_url = url;
}

QUrl KClickableImageLabel::url() const
{
    return m_url;
}

void KClickableImageLabel::mousePressEvent(QMouseEvent* /* ev */)
{
    if(m_url.isEmpty())
    {
        return;
    }

    QDesktopServices::openUrl(m_url);
}

void KClickableImageLabel::mouseMoveEvent(QMouseEvent* /* ev */)
{
    if(pixmap()->isNull())
    {
        return;
    }

    if( rect().contains(mapFromGlobal(QCursor::pos())) )
    {
        setCursor(Qt::PointingHandCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

} // KIPIDebianScreenshotsPlugin
