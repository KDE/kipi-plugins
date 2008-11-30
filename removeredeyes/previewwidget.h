/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-11-29
 * Description : a preview widget to display correction results
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

// Qt includes.

#include <QStackedWidget>
#include <QPixmap>
#include <QLabel>

class QWidget;

class KUrl;

namespace KIPIRemoveRedEyesPlugin
{

class PreviewWidgetPriv;

class PreviewWidget : public QStackedWidget
{
    Q_OBJECT

public:

    enum PreviewMode
    {
        BusyMode = 0,
        NoSelectionMode,
        OriginalMode,
        CorrectedMode,
        MaskMode
    };

    enum ImageType
    {
        OriginalImage = 0,
        CorrectedImage,
        MaskImage
    };

public:

    PreviewWidget(QWidget* parent = 0);
    virtual ~PreviewWidget();

    void setPreviewImage(const QString& filename, ImageType type);
    void setCurrentImage(const KUrl& url);

    void setBusy(bool busy);
    void reset();

protected:

    void enterEvent(QEvent* e);
    void leaveEvent(QEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

private:

    void setMode(PreviewMode mode);
    QPixmap openFile(const QString& filename);

private:

    PreviewWidgetPriv* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* PREVIEWWIDGET_H */
