/* ============================================================
 * File  : monthwidget.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-03
 * Description :
 *
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <qdatetime.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qdragobject.h>
#include <qstrlist.h>

#include <kurl.h>
#include <kurldrag.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <klocale.h>
#include <kglobal.h>
#include <kio/previewjob.h>

#include "monthwidget.h"
#include "calsettings.h"
#include <libkipi/imagecollectiondialog.h>

namespace DKCalendar
{

MonthWidget::MonthWidget( KIPI::Interface* interface, QWidget *parent, int month)
    : QFrame(parent), interface_( interface )
{
    setAcceptDrops(true);
    month_     = month;
    imagePath_ = QString("");
    pixmap_    = new QPixmap(SmallIcon("file_broken",
                                       KIcon::SizeMedium,
                                       KIcon::DisabledState));
    setFixedSize(QSize(70,90));
    setFrameStyle(QFrame::Panel|QFrame::Raised);
}

MonthWidget::~MonthWidget()
{
    if (pixmap_) delete pixmap_;
}

KURL MonthWidget::imagePath()
{
    return imagePath_;
}

void MonthWidget::drawContents(QPainter *p)
{
    QString name = KGlobal::locale()->monthName(month_, true);
    QRect cr;

    cr = contentsRect();
    cr.setBottom(70);
    p->drawPixmap(cr.width()/2 - pixmap_->width()/2,
                  cr.height()/2 - pixmap_->height()/2,
                  *pixmap_);

    cr = contentsRect();
    cr.setTop(70);
    p->drawText(cr,Qt::AlignHCenter,name);
}

void MonthWidget::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept(QUriDrag::canDecode(event));
}

void MonthWidget::dropEvent(QDropEvent* event)
{
    KURL::List srcURLs;
    KURLDrag::decode(event, srcURLs);

    KIO::PreviewJob* thumbJob_ =
        KIO::filePreview( srcURLs,64);
    connect(thumbJob_, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            SLOT(slotGotThumbnaiL(const KFileItem*, const QPixmap&)));
}

void MonthWidget::slotGotThumbnaiL(const KFileItem* item, const QPixmap& pix)
{
    imagePath_ = item->url();

    CalSettings::instance()->setImage(month_,imagePath_);

    delete pixmap_;
    QPixmap image = pix;
    int angle = interface_->info( imagePath_ ).angle();
    if ( angle != 0 ) {
        QWMatrix matrix;
        matrix.rotate( angle );
        image = image.xForm( matrix );
    }

    pixmap_ = new QPixmap(image);
    update();
}

void MonthWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (!contentsRect().contains(e->pos())) return;

    if (e->button() == Qt::LeftButton) {
        KURL url = KIPI::ImageCollectionDialog::getImageURL(this, interface_);
        if (url.isValid()) {
            KURL::List urls;
            urls << url;

            KIO::PreviewJob* thumbJob_ =
                KIO::filePreview( urls,64);
            connect(thumbJob_, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
                    SLOT(slotGotThumbnaiL(const KFileItem*, const QPixmap&)));
        }
    }
    else if (e->button() == Qt::RightButton) {
        imagePath_ = QString("");
        CalSettings::instance()->setImage(month_,imagePath_);
        delete pixmap_;
        pixmap_    = new QPixmap(SmallIcon("file_broken",
                                           KIcon::SizeMedium,
                                           KIcon::DisabledState));
        update();
    }
}

}


#include "monthwidget.moc"
