/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-11-29
 * Description : a preview widget to display correction results
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

// Qt includes

#include <QGraphicsView>
#include <QLabel>
#include <QPixmap>

class QWidget;


namespace KIPIRemoveRedEyesPlugin
{

struct PreviewWidgetPriv;

class PreviewWidget : public QGraphicsView
{
    Q_OBJECT

public:

    enum DisplayMode
    {
        BusyMode = 0,
        LockedMode,
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
    ~PreviewWidget();

    QString& currentImage() const;
    void setCurrentImage(const QString& image);

    void setPreviewImage(ImageType type, const QString& filename);

public Q_SLOTS:

    void reset();

Q_SIGNALS:

    void settingsChanged();

protected:

    void enterEvent (QEvent* e);
    void resizeEvent(QResizeEvent* e);

private Q_SLOTS:

    void updateSettings();

    void correctedClicked();
    void maskClicked();
    void originalClicked();
    void zoomInClicked();
    void zoomOutClicked();

private:

    bool previewsComplete();
    void resetPreviews();

    void setMode(DisplayMode mode);

    QPixmap openFile(const QString& filename);

private:

    PreviewWidgetPriv* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* PREVIEWWIDGET_H */
