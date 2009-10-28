/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Copyright (C) 2009 by Aurélien Gâteau <agateau@kde.org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "pixmapview.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// C++ includes

#include <cstdio>

// Qt includes

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>
#include <QScrollBar>
#include <QWheelEvent>
#include <QDateTime>

// KDE includes

#include <kapplication.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <kio/global.h>
#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kurl.h>

namespace KIPIBatchProcessImagesPlugin
{

PixmapView::PixmapView(QWidget *parent)
          : QAbstractScrollArea(parent),
            m_zoomFactor(0)
{
    m_pix          = 0;
    m_validPreview = false;
    setMinimumSize(QSize(300, 300));
    horizontalScrollBar()->setSingleStep(1);
    horizontalScrollBar()->setPageStep(1);
    verticalScrollBar()->setSingleStep(1);
    verticalScrollBar()->setPageStep(1);
    viewport()->setCursor(Qt::OpenHandCursor);
}

PixmapView::~PixmapView()
{
    delete m_pix;
}

void PixmapView::setImage(const QString& ImagePath, const QString& tmpPath, bool cropAction)
{
    m_cropAction      = cropAction;
    m_previewFileName = tmpPath + QString("/") + QString::number(getpid()) + QString("-") +
                        QString::number(QDateTime::currentDateTime().toTime_t()) +
                        QString("PreviewImage.PNG");

    if (m_cropAction == true)
    {
        PreviewCal(ImagePath, tmpPath);
    }
    else
    {
        if (m_img.load(ImagePath) == false)
        {
            PreviewCal(ImagePath, tmpPath);
        }
        else 
        {
            if (!m_pix) m_pix = new QPixmap(m_img.width(), m_img.height());
            m_w               = m_img.width();
            m_h               = m_img.height();
            m_validPreview    = true;
            updateView();
            horizontalScrollBar()->setSingleStep(1);
            verticalScrollBar()->setSingleStep(1);
        }
    }
}

void PixmapView::PreviewCal(const QString& ImagePath, const QString& /*tmpPath*/)
{
    m_pix = new QPixmap(300, 300);
    QPainter p;
    p.begin(m_pix);
    p.fillRect(0, 0, m_pix->width(), m_pix->height(), Qt::white);
    p.setPen(Qt::green);
    p.drawText(0, 0, m_pix->width(), m_pix->height(), Qt::AlignCenter,
               i18n("Preview\nimage\nprocessing\nin\nprogress..."));
    p.end();

    m_previewOutput = "convert";
    m_PreviewProc   = new KProcess(this);
    m_PreviewProc->setOutputChannelMode(KProcess::MergedChannels);
    *m_PreviewProc << "convert";
    *m_PreviewProc << "-verbose";

    if (m_cropAction == true)
    {
        *m_PreviewProc << "-crop" << "300x300+0+0";
        m_previewOutput.append(" -crop 300x300+0+0 ");
    }

    *m_PreviewProc << ImagePath;
    *m_PreviewProc << m_previewFileName;
    m_previewOutput.append(" -verbose " + ImagePath + " " + m_previewFileName + "\n\n");

    connect(m_PreviewProc, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(slotPreviewProcessFinished()));

    connect(m_PreviewProc, SIGNAL(readyRead()),
            this, SLOT(slotPreviewReadyRead()));

    m_PreviewProc->start();
    if (!m_PreviewProc->waitForStarted())
    {
        delete m_PreviewProc;
        KMessageBox::error(this, i18n("Cannot start 'convert' program from 'ImageMagick' package;\n"
                                      "please check your installation."));
        return;
    }
}

void PixmapView::slotPreviewReadyRead()
{
    QByteArray output = m_PreviewProc->readAll();
    m_previewOutput.append(QString::fromLocal8Bit(output.data(), output.size()));
}

void PixmapView::slotPreviewProcessFinished()
{
    int ValRet = m_PreviewProc->exitCode();
    kDebug() << "Convert exit (" << ValRet << ")" ;

    if (ValRet == 0)
    {
        if (m_img.load(m_previewFileName) == true)
        {
            if (!m_pix) m_pix = new QPixmap(300, 300);
            m_w            = m_img.width();
            m_h            = m_img.height();
            m_validPreview = true;
            updateView();
            horizontalScrollBar()->setSingleStep(1);
            verticalScrollBar()->setSingleStep(1);
            KUrl deletePreviewImage(m_previewFileName);

            KIO::NetAccess::del(deletePreviewImage, kapp->activeWindow());
        }
        else
        {
            m_pix = new QPixmap(viewport()->size());
            QPainter p;
            p.begin(m_pix);
            p.fillRect(0, 0, m_pix->width(), m_pix->height(), Qt::white);
            p.setPen(Qt::red);
            p.drawText(0, 0, m_pix->width(), m_pix->height(), Qt::AlignCenter,
                       i18n("Cannot\nprocess\npreview\nimage."));
            p.end();
            viewport()->update();
            m_validPreview = false;
        }
    }
}

void PixmapView::setZoom(int zoomFactor)
{
    if (m_zoomFactor == zoomFactor)
    {
        return;
    }

    m_zoomFactor = zoomFactor;

    if (!m_validPreview)
    {
        return;
    }

    updateView();
}

void PixmapView::updateView()
{
    int w = m_w - (int)((float)m_w * (100 - (float)m_zoomFactor) / 100);
    int h = m_h - (int)((float)m_h * (100 - (float)m_zoomFactor) / 100);

    QImage imgTmp = m_img.scaled(w, h);

    if (m_pix) delete m_pix;

    m_pix = new QPixmap(QPixmap::fromImage(imgTmp));

    updateScrollBars();
    viewport()->update();
}

void PixmapView::paintEvent(QPaintEvent*)
{
    if (!m_pix) return;
    QPainter painter(viewport());
    const int x = horizontalScrollBar()->value();
    const int y = verticalScrollBar()->value();
    painter.drawPixmap(0, 0, *m_pix, x, y, viewport()->width(), viewport()->height());
}

void PixmapView::resizeEvent(QResizeEvent*)
{
    updateScrollBars();
}

void PixmapView::contentsWheelEvent(QWheelEvent* e)
{
    emit wheelChanged(e->delta());
}

void PixmapView::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        viewport()->setCursor(Qt::ClosedHandCursor);
        m_dragPos = e->pos();
    }
}

void PixmapView::mouseReleaseEvent(QMouseEvent*)
{
    viewport()->setCursor(Qt::OpenHandCursor);
}

void PixmapView::mouseMoveEvent(QMouseEvent* e)
{
    if (e->modifiers() == Qt::LeftButton)
    {
        QPoint delta = e->pos() - m_dragPos;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_dragPos = e->pos();
    }
}

void PixmapView::updateScrollBars()
{
    horizontalScrollBar()->setMaximum(m_pix->width() - viewport()->width());
    verticalScrollBar()->setMaximum(m_pix->height() - viewport()->height());
}

} // namespace KIPIBatchProcessImagesPlugin
