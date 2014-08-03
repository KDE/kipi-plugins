/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-01
 * Description : image files selector dialog.
 *
 * Copyright (C) 2004-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <kurl.h>
#include <kpreviewwidgetbase.h>

// Local includes

#include "kipiplugins_export.h"

class KFileItem;

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPImageDialogPreview : public KPreviewWidgetBase
{
    Q_OBJECT

public:

    explicit KPImageDialogPreview(QWidget* const parent=0);
    ~KPImageDialogPreview();

    QSize sizeHint() const;

private Q_SLOTS:

    void showPreview();
    void showPreview(const KUrl&);
    void slotThumbnail(const KUrl&, const QPixmap&);
    void slotKDEPreview(const KFileItem&, const QPixmap&);
    void slotKDEPreviewFailed(const KFileItem&);
    void slotRawThumb(const KUrl&, const QImage&);
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

    KUrl       url() const;
    KUrl::List urls() const;

    bool       onlyRaw() const;
    bool       singleSelect() const;
    QString    fileFormats() const;

    static KUrl getImageUrl(QWidget* const parent, bool onlyRaw=false);
    static KUrl::List getImageUrls(QWidget* const parent, bool onlyRaw=false);

private:

    class Private;
    Private* const d;
};

} // namespace KIPIPlugins

#endif /* KPIMAGEDIALOG_H */
