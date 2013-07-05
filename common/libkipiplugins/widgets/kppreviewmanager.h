/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-23
 * Description : a widget to manage preview.
 *
 * Copyright (C) 2009-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef KPPREVIEW_MANAGER_H
#define KPPREVIEW_MANAGER_H

// Qt includes

#include <QStackedWidget>
#include <QString>
#include <QColor>
#include <QImage>

// KDE includes

#include <kurl.h>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPPreviewManager : public QStackedWidget
{
    Q_OBJECT

public:

    enum DisplayMode
    {
        MessageMode = 0,
        PreviewMode
    };

public:

    KPPreviewManager(QWidget* const parent);
    ~KPPreviewManager();

    bool load(const QString& file, bool fit = true);
    void setImage(const QImage& img, bool fit = true);
    void setText(const QString& text, const QColor& color=Qt::white);
    void setBusy(bool b, const QString& text=QString());
    void setThumbnail(const QPixmap& preview=QPixmap());
    void setButtonText(const QString& text);
    void setButtonVisible(bool b);
    void setSelectionAreaPossible(bool b);
    QRectF getSelectionArea();
    /**
     * Sets a selection area and show it
     * 
     * @param rectangle This rectangle should have height and width of 1.0
     */
    void setSelectionArea(QRectF rectangle);

Q_SIGNALS:

    void signalButtonClicked();

public Q_SLOTS:

    void slotLoad(const KUrl& url);

private Q_SLOTS:

    void slotProgressTimerDone();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIPlugins

#endif /* KPPREVIEW_MANAGER_H */
