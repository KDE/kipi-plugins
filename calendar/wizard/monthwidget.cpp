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

#include "monthwidget.h"

// Qt includes

#include <QDragEnterEvent>
#include <QFileInfo>
#include <QImageReader>
#include <QMatrix>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QUrl>

// KDE includes

#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "calsettings.h"
#include "kpimagedialog.h"
#include "kipiplugins_debug.h"

using namespace KIPIPlugins;

namespace KIPICalendarPlugin
{

MonthWidget::MonthWidget(Interface* const interface, QWidget* const parent, int month)
    : QPushButton(parent),
      thumbSize(64, 64),
      interface_(interface)
{
    setAcceptDrops(true);
    setFixedSize(QSize(74, 94));
    month_     = month;
    imagePath_ = QUrl();
    setThumb(QPixmap(QIcon::fromTheme(QStringLiteral("image-x-generic")).pixmap(32, QIcon::Disabled)));

    connect(interface_, SIGNAL(gotThumbnail(QUrl,QPixmap)),
            this, SLOT(gotThumbnail(QUrl,QPixmap)));

    connect(this, SIGNAL(pressed()), 
            this, SLOT(monthSelected()));
}

MonthWidget::~MonthWidget()
{
}

QUrl MonthWidget::imagePath() const
{
    return imagePath_;
}

void MonthWidget::paintEvent(QPaintEvent* event)
{
    QRect cr;

    QPushButton::paintEvent(event);
    QPainter painter(this);
    QString name = KLocale::global()->calendar()->monthName(
                   month_, CalSettings::instance()->year(), KCalendarSystem::ShortName);

    cr = contentsRect();
    cr.setBottom(70);
    painter.drawPixmap(cr.width() / 2 - thumb_.width() / 2,
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

void MonthWidget::setImage(const QUrl& url)
{
    if (!url.isValid())
    {
        return;
    }

    // check if the file is an image

    // Check if RAW file.
    bool isRAW = false;

    if (interface_)
    {
        QPointer<RawProcessor> rawdec = interface_->createRawProcessor();
        isRAW                         = (rawdec && rawdec->isRawFile(url));
    }
    
    if (isRAW)
    {
        // Check if image can be loaded by native Qt loader.
        if (QImageReader::imageFormat(url.path()).isEmpty())
        {
            qCDebug(KIPIPLUGINS_LOG) << "Unknown image format for: " << url.toDisplayString();
            return;
        }
    }

    imagePath_ = url;
    CalSettings::instance()->setImage(month_, imagePath_);

    if (interface_)
    {
        interface_->thumbnail(url, thumbSize.width());
    }
}

void MonthWidget::dropEvent(QDropEvent* event)
{
    QList<QUrl> srcURLs = event->mimeData()->urls();

    if (srcURLs.isEmpty())
    {
        return;
    }

    QUrl url = srcURLs.first();
    setImage(url);
}

void MonthWidget::monthSelected()
{
  emit monthSelected(month_);
}

void MonthWidget::gotThumbnail(const QUrl& url, const QPixmap& pix)
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
        imagePath_ = QUrl();
        CalSettings::instance()->setImage(month_, imagePath_);
        setThumb(QPixmap(QIcon::fromTheme(QStringLiteral("image-x-generic")).pixmap(32, QIcon::Disabled)));
    }
}

}  // NameSpace KIPICalendarPlugin
