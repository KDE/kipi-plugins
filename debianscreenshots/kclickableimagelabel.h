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

#ifndef KCLICKABLEIMAGELABEL_H
#define KCLICKABLEIMAGELABEL_H

// Qt includes

#include <QLabel>
#include <QUrl>

namespace KIPIDebianScreenshotsPlugin
{

class KClickableImageLabel : public QLabel
{
    Q_OBJECT

public:

    explicit KClickableImageLabel(QWidget* const parent = 0, Qt::WindowFlags f = 0);
    explicit KClickableImageLabel(const QString& text, QWidget* const parent = 0, Qt::WindowFlags f = 0);
    void setUrl(const QUrl& url);
    QUrl url() const;

protected:

    void mousePressEvent(QMouseEvent* ev);
    void mouseMoveEvent(QMouseEvent* ev);

private:

    QUrl m_url;
};

} // KIPIDebianScreenshotsPlugin

#endif // KCLICKABLEIMAGELABEL_H
