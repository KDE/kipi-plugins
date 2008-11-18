/* ============================================================
 * File  : monthwidget.cpp
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Tom Albers <tomalbers@kde.nl>
 *          Orgad Shaneh <orgads@gmail.com>
 * Date  : 2008-11-13
 * Description: month image selection widget
 *
 * Copyright 2003 by Renchi Raju
 * Copyright 2006 by Tom Albers
 * Copyright 2008 by Orgad Shaneh
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

// Qt includes.

#include <QMatrix>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QImageReader>
#include <QDragEnterEvent>

// KDE includes.

#include <KUrl>
#include <KDebug>
#include <KLocale>
#include <KGlobal>
#include <KIconLoader>
#include <KCalendarSystem>

// LibKIPI includes.

#include <imagedialog.h>
#include <libkipi/interface.h>

// Local includes.

#include "monthwidget.h"
#include "calsettings.h"

namespace KIPICalendarPlugin
{

MonthWidget::MonthWidget( KIPI::Interface* interface, QWidget *parent, int month )
    : QPushButton(parent), thumbSize( 64, 64 ), interface_( interface )
{
    setAcceptDrops(true);
    month_     = month;
    setFixedSize(QSize(74, 94));
    imagePath_ = QString("");
    setThumb(QPixmap(SmallIcon("image-x-generic",
                     KIconLoader::SizeMedium,
                     KIconLoader::DisabledState)));

    connect(interface_, SIGNAL(gotThumbnail(const KUrl &, const QPixmap &)),
            SLOT(gotThumbnail(const KUrl &, const QPixmap &)));
}

MonthWidget::~MonthWidget()
{
}

KUrl MonthWidget::imagePath()
{
    return imagePath_;
}

void MonthWidget::paintEvent(QPaintEvent *event)
{
  QRect cr;

  QPushButton::paintEvent(event);
  QPainter painter(this);
  QString name = KGlobal::locale()->calendar()->monthName(
                   month_, CalSettings::instance()->year(), KCalendarSystem::ShortName);

  cr = contentsRect();
  cr.setBottom(70);
  painter.drawPixmap(cr.width()/2 - thumb_.width()/2,
                     cr.height()/2 - thumb_.height()/2,
                     thumb_);

  cr = contentsRect();
  cr.setTop(70);
  painter.drawText(cr,Qt::AlignHCenter, name);
}

void MonthWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasImage())
        event->acceptProposedAction();
}

QPixmap MonthWidget::thumb() const
{
  return thumb_;
}

void MonthWidget::setThumb(const QPixmap &pic)
{
  thumb_ = pic.scaled(thumbSize, Qt::KeepAspectRatio);
  update();
}

void MonthWidget::setImage( const KUrl &url )
{
    if (!url.isValid())
        return;

    // check if the file is an image
    if ( QImageReader::imageFormat( url.path() ).isEmpty() )
    {
        kWarning( 51001 ) << "Unknown image format for: "
                << url.prettyUrl() << endl;
        return;
    }

    imagePath_ = url;
    CalSettings::instance()->setImage(month_, imagePath_);

    interface_->thumbnail( url, thumbSize.width() );
}

void MonthWidget::dropEvent(QDropEvent* event)
{
    KUrl::List srcURLs = KUrl::List::fromMimeData( event->mimeData() );

    if ( srcURLs.isEmpty() )
        return;

    KUrl url = srcURLs.first();
    setImage( url );
}

void MonthWidget::gotThumbnail( const KUrl &url, const QPixmap &pix )
{
    if ( url != imagePath_ )
      return;

    QPixmap image = pix;
    int angle = interface_->info( url ).angle();
    if ( angle != 0 ) {
        QMatrix matrix;
        matrix.rotate( angle );
        image = image.transformed( matrix );
    }

    setThumb(image);
}

void MonthWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (!contentsRect().contains(event->pos())) return;

    if (event->button() == Qt::LeftButton)
    {
        KIPIPlugins::ImageDialog dlg( this, interface_, true );
        setImage(dlg.url());
    }
    else if (event->button() == Qt::RightButton) {
        imagePath_ = QString("");
        CalSettings::instance()->setImage(month_,imagePath_);
        setThumb(QPixmap(SmallIcon("image-x-generic",
                         KIconLoader::SizeMedium,
                         KIconLoader::DisabledState)));
    }
}

}  // NameSpace KIPICalendarPlugin
