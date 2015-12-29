/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-01
 * Description : image files selector dialog.
 *
 * Copyright (C) 2004-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPIMAGEDIALOG_H
#define KPIMAGEDIALOG_H

// Qt includes

#include <QImage>
#include <QUrl>
#include <QList>
#include <QScrollArea>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPImageDialogPreview : public QScrollArea
{
    Q_OBJECT

public:

    explicit KPImageDialogPreview(QWidget* const parent=0);
    ~KPImageDialogPreview();

    QSize sizeHint() const;

private Q_SLOTS:

    void showPreview();
    void showPreview(const QUrl&);
    void slotThumbnail(const QUrl&, const QPixmap&);
    void clearPreview();

private:

    void resizeEvent(QResizeEvent* e);

private:

    class Private;
    Private* const d;
};

// ------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT KPImageDialog
{

public:

    KPImageDialog(QWidget* const parent, bool singleSelect=false, bool onlyRaw=false);
    ~KPImageDialog();

    QUrl        url() const;
    QList<QUrl> urls() const;

    bool        onlyRaw() const;
    bool        singleSelect() const;
    QString     fileFormats() const;

    static QUrl        getImageUrl(QWidget* const parent, bool onlyRaw=false);
    static QList<QUrl> getImageUrls(QWidget* const parent, bool onlyRaw=false);

    static QStringList supportedMimeTypes();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIPlugins

#endif /* KPIMAGEDIALOG_H */
