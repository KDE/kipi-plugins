/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description : month image selection widget.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Tom Albers <tomalbers@kde.nl>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
 * Copyright (C) 2012      by Angelo Naselli <anaselli at linux dot it>
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

#include "monthwidget.moc"

// Qt includes

#include <QDragEnterEvent>
#include <QFileInfo>
#include <QImageReader>
#include <QMatrix>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>

// KDE includes

#include <kcalendarsystem.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kurl.h>

// LibKIPI includes

#include <libkipi/interface.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "calsettings.h"
#include "kpimagedialog.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPICalendarPlugin
{

MonthWidget::MonthWidget(Interface* const interface, QWidget* const parent, int month)
    : QPushButton(parent), thumbSize(64, 64), interface_(interface)
{
    setAcceptDrops(true);
    setFixedSize(QSize(74, 94));
    month_     = month;
    imagePath_ = QString("");
    setThumb(QPixmap(SmallIcon("image-x-generic",
                               KIconLoader::SizeMedium,
                               KIconLoader::DisabledState)));

    connect(interface_, SIGNAL(gotThumbnail(KUrl,QPixmap)),
            this, SLOT(gotThumbnail(KUrl,QPixmap)));

    connect(this, SIGNAL(pressed()), 
            this, SLOT(monthSelected()));
}

MonthWidget::~MonthWidget()
{
}

KUrl MonthWidget::imagePath() const
{
    return imagePath_;
}

void MonthWidget::paintEvent(QPaintEvent* event)
{
    QRect cr;

    QPushButton::paintEvent(event);
    QPainter painter(this);
    QString name = KGlobal::locale()->calendar()->monthName(
                   month_, CalSettings::instance()->year(), KCalendarSystem::ShortName);

    cr = contentsRect();
    cr.setBottom(70);
    painter.drawPixmap(cr.width()  / 2 - thumb_.width() / 2,
                       cr.height() / 2 - thumb_.height() / 2,
                       thumb_);

    cr = contentsRect();
    cr.setTop(70);
    painter.drawText(cr, Qt::AlignHCenter, name);
}

void MonthWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasImage())
    {
        event->acceptProposedAction();
    }
}

QPixmap MonthWidget::thumb() const
{
    return thumb_;
}

int MonthWidget::month()
{
  return month_;
}

void MonthWidget::setThumb(const QPixmap& pic)
{
    thumb_ = pic.scaled(thumbSize, Qt::KeepAspectRatio);
    update();
}

void MonthWidget::setImage(const KUrl& url)
{
    if (!url.isValid())
    {
        return;
    }

    // check if the file is an image

    // Check if RAW file.
    if (KPMetadata::isRawFile(url))
    {
        // Check if image can be loaded by native Qt loader.
        if (QImageReader::imageFormat(url.path()).isEmpty())
        {
            kWarning() << "Unknown image format for: " << url.prettyUrl();
            return;
        }
    }

    imagePath_ = url;
    CalSettings::instance()->setImage(month_, imagePath_);

    interface_->thumbnail(url, thumbSize.width());
}

void MonthWidget::dropEvent(QDropEvent* event)
{
    KUrl::List srcURLs = KUrl::List::fromMimeData(event->mimeData());

    if (srcURLs.isEmpty())
    {
        return;
    }

    KUrl url = srcURLs.first();
    setImage(url);
}

void MonthWidget::monthSelected()
{
  emit monthSelected(month_);
}

void MonthWidget::gotThumbnail(const KUrl& url, const QPixmap& pix)
{
    if (url != imagePath_)
    {
        return;
    }

    setThumb(pix);
}

void MonthWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (!contentsRect().contains(event->pos()))
    {
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        KPImageDialog dlg(this, true);
        setImage(dlg.url());
    }
    else if (event->button() == Qt::RightButton)
    {
        imagePath_ = QString("");
        CalSettings::instance()->setImage(month_, imagePath_);
        setThumb(QPixmap(SmallIcon("image-x-generic",
                                   KIconLoader::SizeMedium,
                                   KIconLoader::DisabledState)));
    }
}

}  // NameSpace KIPICalendarPlugin
