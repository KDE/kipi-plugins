/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-05-01
 * Description : image files selector dialog.
 *
 * Copyright (C) 2004-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

// Qt includes

#include <QThread>
#include <QImage>

// KDE includes

#include <kurl.h>
#include <kpreviewwidgetbase.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT ImageDialogPreview : public KPreviewWidgetBase
{
    Q_OBJECT

public:

    explicit ImageDialogPreview(KIPI::Interface* iface, QWidget* parent=0);
    ~ImageDialogPreview();

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

    class ImageDialogPreviewPrivate;
    ImageDialogPreviewPrivate* const d;
};

// ------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT ImageDialog
{

public:

    ImageDialog(QWidget* parent, KIPI::Interface* iface, bool singleSelect=false, bool onlyRaw=false);
    ~ImageDialog();

    KUrl       url() const;
    KUrl::List urls() const;

    bool       onlyRaw() const;
    bool       singleSelect() const;
    QString    fileFormats() const;

    static KUrl getImageUrl(QWidget* parent, KIPI::Interface* iface, bool onlyRaw=false);
    static KUrl::List getImageUrls(QWidget* parent, KIPI::Interface* iface, bool onlyRaw=false);

private:

    class ImageDialogPrivate;
    ImageDialogPrivate* const d;
};

// ------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT LoadRawThumbThread : public QThread
{
    Q_OBJECT

public:

    explicit LoadRawThumbThread(QObject* parent, int size=256);
    ~LoadRawThumbThread();

    void getRawThumb(const KUrl& url);
    void cancel();

Q_SIGNALS:

    void signalRawThumb(const KUrl&, const QImage& img);

private:

    void run();

private:

    class LoadRawThumbThreadPriv;
    LoadRawThumbThreadPriv* const d;
};

} // namespace KIPIPlugins

#endif /* IMAGEDIALOG_H */
