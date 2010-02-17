/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-15
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2010 by Jens Mueller <tschenser at gmx dot de>
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

#include "picasawebreplacedialog.h"

#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QPainter>
#include <QTimer>

#include <kpushbutton.h>
#include <klocale.h>
#include <kseparator.h>
#include <ksqueezedtextlabel.h>

namespace KIPIPicasawebExportPlugin
{

class PicasawebReplaceDialog::PicasawebReplaceDialogPrivate
{
public:
    PicasawebReplaceDialogPrivate(){
        bCancel = 0;
        bAdd = bAddAll = bReplace = bReplaceAll = 0;
        iface = 0;
        lbSrc = lbDest = 0;
        progressPix = SmallIcon("process-working", 22);
        progressCount = 0;
        progressTimer = 0;
    }
    KPushButton *bCancel;
    KPushButton *bAdd;
    KPushButton *bAddAll;
    KPushButton *bReplace;
    KPushButton *bReplaceAll;
    KUrl src;
    KUrl dest;
    KIPI::Interface *iface;
    QLabel *lbSrc;
    QLabel *lbDest;
    QByteArray buffer;
    QPixmap mimePix;
    QPixmap progressPix;
    int progressCount;
    QTimer* progressTimer;
};

PicasawebReplaceDialog::PicasawebReplaceDialog(QWidget *parent, const QString & _caption,
                                               KIPI::Interface *_iface, const KUrl &_src, const KUrl &_dest)
 : QDialog(parent), d(new PicasawebReplaceDialogPrivate)
{
    setObjectName("PicasawebReplaceDialog");

    d->src = _src;
    d->dest = _dest;
    d->iface = _iface;

    setWindowTitle(_caption);

    d->bCancel = new KPushButton(KStandardGuiItem::cancel(), this);
    connect(d->bCancel, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    d->bAdd = new KPushButton(i18n("&Add As New"), this);
    d->bAdd->setToolTip(i18n("Item will be added alongside the linked version."));
    connect(d->bAdd, SIGNAL(clicked()), this, SLOT(addPressed()));

    d->bAddAll = new KPushButton(i18n("&Add All"), this);
    d->bAddAll->setToolTip(i18n("Items will be added alongside the linked version. You will not be prompted again."));
    connect(d->bAddAll, SIGNAL(clicked()), this, SLOT(addAllPressed()));

    d->bReplace = new KPushButton(i18n("&Replace"), this);
    d->bReplace->setToolTip(i18n("Item will be replacing the linked version."));
    connect(d->bReplace, SIGNAL(clicked()), this, SLOT(replacePressed()));

    d->bReplaceAll = new KPushButton(i18n("&Replace All"), this);
    d->bReplaceAll->setToolTip(i18n("Items will be replacing the linked version. You will be prompted again."));
    connect(d->bReplaceAll, SIGNAL(clicked()), this, SLOT(replaceAllPressed()));

    QVBoxLayout* pLayout = new QVBoxLayout(this);
    pLayout->addStrut(360);	// makes dlg at least that wide
    
    QGridLayout * gridLayout = new QGridLayout();
    pLayout->addLayout(gridLayout);

    QString sentence1 = i18n("A linked item already exists.");

    QLabel* lb1 = new KSqueezedTextLabel(sentence1, this);
    lb1->setAlignment(Qt::AlignHCenter);
    gridLayout->addWidget(lb1, 0, 0, 1, 3);

    d->mimePix = KIO::pixmapForUrl(d->dest);

    d->lbDest = new QLabel(this);
    d->lbDest->setPixmap(d->mimePix);
    d->lbDest->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(d->lbDest, 1, 0, 1, 1);

    d->lbSrc = new QLabel(this);
    d->lbSrc->setPixmap(KIO::pixmapForUrl(d->src));
    d->lbSrc->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(d->lbSrc, 1, 2, 1, 1);

    lb1 = new KSqueezedTextLabel(i18n("Destination"), this);
    lb1->setAlignment(Qt::AlignHCenter);
    gridLayout->addWidget(lb1, 2, 0, 1, 1);

    lb1 = new KSqueezedTextLabel(i18n("Source"), this);
    lb1->setAlignment(Qt::AlignHCenter);
    gridLayout->addWidget(lb1, 2, 2, 1, 1);

    QHBoxLayout* layout2 = new QHBoxLayout();
    pLayout->addLayout(layout2);

    KSeparator* separator = new KSeparator(this);
    pLayout->addWidget(separator);

    QHBoxLayout* layout = new QHBoxLayout();
    pLayout->addLayout(layout);

    layout->addStretch(1);

    layout->addWidget(d->bAdd);
    setTabOrder(d->bAdd, d->bCancel);

    layout->addWidget(d->bAddAll);
    setTabOrder(d->bAddAll, d->bCancel);

    layout->addWidget(d->bReplace);
    setTabOrder(d->bReplace, d->bCancel);

    layout->addWidget(d->bReplaceAll);
    setTabOrder(d->bReplaceAll, d->bCancel);

    d->bCancel->setDefault(true);
    layout->addWidget(d->bCancel);

    d->progressTimer = new QTimer(this);

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));

    d->progressTimer->start(300);

    // get source thumbnail
    if (d->iface && d->src.isValid())
    {
        connect(d->iface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
                this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));

        d->iface->thumbnail(d->src, KIconLoader::SizeLarge);
    }

    // get dest thumbnail
    d->buffer.resize(0);

    if (d->dest.isValid())
    {
        KIO::TransferJob* job = KIO::get(d->dest, KIO::NoReload, KIO::HideProgressInfo);
        job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );

        connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
                this, SLOT(slotData(KIO::Job*, const QByteArray&)));

        connect(job, SIGNAL(result(KJob *)),
                this, SLOT(slotResult(KJob *)));
    }

    resize(sizeHint());
}

void PicasawebReplaceDialog::slotResult(KJob *job)
{
    d->progressTimer->stop();
    if (job->error() || static_cast<KIO::TransferJob*>(job)->isErrorPage())
    {
        return;
    }
    if (!d->buffer.isEmpty())
    {
        QPixmap pxm;
        pxm.loadFromData(d->buffer);
        d->lbDest->setPixmap(pxm.scaled(200, 200, Qt::KeepAspectRatio, Qt::FastTransformation));
    }
}

void PicasawebReplaceDialog::slotData(KIO::Job */*job*/, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = d->buffer.size();
    d->buffer.resize(d->buffer.size() + data.size());
    memcpy(d->buffer.data()+oldSize, data.data(), data.size());
}

void PicasawebReplaceDialog::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    if (url == d->src)
    {
        d->lbSrc->setPixmap(pix.scaled(200, 200, Qt::KeepAspectRatio, Qt::FastTransformation));
    }
}

PicasawebReplaceDialog::~PicasawebReplaceDialog()
{
    delete d;
}

void PicasawebReplaceDialog::cancelPressed()
{
    done(PWR_CANCEL);
}

void PicasawebReplaceDialog::addPressed()
{
    done(PWR_ADD);
}

void PicasawebReplaceDialog::addAllPressed()
{
    done(PWR_ADD_ALL);
}

void PicasawebReplaceDialog::replacePressed()
{
    done(PWR_REPLACE);
}

void PicasawebReplaceDialog::replaceAllPressed()
{
    done(PWR_REPLACE_ALL);
}

QPixmap PicasawebReplaceDialog::setProgressAnimation(const QPixmap& thumb, const QPixmap& pix)
{
    QPixmap overlay = thumb;
    QPixmap mask(overlay.size());
    mask.fill(QColor(128, 128, 128, 192));
    QPainter p(&overlay);
    p.drawPixmap(0, 0, mask);
    p.drawPixmap((overlay.width()/2) - (pix.width()/2), (overlay.height()/2) - (pix.height()/2), pix);
    return overlay;
}

void PicasawebReplaceDialog::slotProgressTimerDone()
{
    QPixmap pix(d->progressPix.copy(0, d->progressCount*22, 22, 22));
    d->lbDest->setPixmap(setProgressAnimation(d->mimePix, pix));

    d->progressCount++;
    if (d->progressCount == 8)
        d->progressCount = 0;

    d->progressTimer->start(300);
}


} // namespace KIPIPicasawebExportPlugin
