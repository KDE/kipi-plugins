/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-23
 * Description : a widget to manage preview.
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PREVIEW_MANAGER_H
#define PREVIEW_MANAGER_H

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

class KIPIPLUGINS_EXPORT PreviewManager : public QStackedWidget
{
    Q_OBJECT

public:

    enum DisplayMode
    {
        MessageMode = 0,
        PreviewMode
    };

public:

    PreviewManager(QWidget* parent);
    ~PreviewManager();

    void load(const QString& file, bool fit = true);
    void setImage(const QImage& img, bool fit = true);
    void setText(const QString& text, const QColor& color=Qt::white);
    void setBusy(bool b, const QString& text=QString());
    void setThumbnail(const QPixmap& preview=QPixmap());
    void setButtonText(const QString& text);
    void setButtonVisible(bool b);

Q_SIGNALS:

    void signalButtonClicked();

public Q_SLOTS:

    void slotLoad(const KUrl& url);

private Q_SLOTS:

    void slotProgressTimerDone();

private:

    class PreviewManagerPriv;
    PreviewManagerPriv* const d;
};

} // namespace KIPIPlugins

#endif /* PREVIEW_MANAGER_H */
