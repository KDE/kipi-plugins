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

    enum DisplayMode
    {
        BusyMode = 0,
        LockedMode,
        OriginalMode,
        CorrectedMode,
        MaskMode
    };

    enum PreviewType
    {
        OriginalImage = 0,
        CorrectedImage,
        MaskImage
    };

public:

    PreviewWidget(QWidget* parent = 0);
    virtual ~PreviewWidget();

    QString& image() const;
    void setImage(const QString& image);

    void setPreview(PreviewType type, const QString& filename);

public slots:

    void reset();

signals:

    void settingsChanged();

protected:

    void enterEvent(QEvent* e);
    void leaveEvent(QEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

private slots:

    void updateSettings();

private:

    bool previewsLoaded();
    void resetPreviews();
    void setMode(DisplayMode mode);
    QPixmap openFile(const QString& filename);

private:

    PreviewWidgetPriv* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* PREVIEWWIDGET_H */
