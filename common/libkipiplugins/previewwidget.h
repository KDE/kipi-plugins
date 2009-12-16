/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-13
 * Description : a widget to preview image effect.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PREVIEW_WIDGET_H
#define PREVIEW_WIDGET_H

// Qt includes

#include <QWidget>
#include <QString>
#include <QColor>

// Local includes

#include "kipiplugins_export.h"

class QPaintEvent;
class QResizeEvent;

namespace KIPIPlugins
{
class PreviewWidgetPriv;

class KIPIPLUGINS_EXPORT PreviewWidget : public QWidget
{
    Q_OBJECT

public:

    PreviewWidget(QWidget *parent);
    ~PreviewWidget();

    void load(const QString& file);
    void setText(const QString& text, const QColor& color=Qt::white);
    void setBusy(bool b, const QString& text=QString());
    void setThumbnail(const QPixmap& preview=QPixmap());

protected:

    void paintEvent(QPaintEvent* e);
    void resizeEvent(QResizeEvent* e);

private Q_SLOTS:

    void slotProgressTimerDone();

private:

    PreviewWidgetPriv* const d;
};

} // namespace KIPIPlugins

#endif /* PREVIEW_WIDGET_H */
