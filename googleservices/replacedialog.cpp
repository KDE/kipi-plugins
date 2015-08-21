/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#include "replacedialog.h"

// Qt includes

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QPainter>
#include <QTimer>
#include <QPushButton>
#include <QDialogButtonBox>

// KDE includes

#include <klocalizedstring.h>
#include <ksqueezedtextlabel.h>

// Libkdcraw includes

#include <KDCRAW/RWidgetUtils>

namespace KIPIGoogleServicesPlugin
{

class ReplaceDialog::Private
{
public:

    Private()
    {
        progressPix   = KDcrawIface::WorkingPixmap();
        bAdd          = 0;
        bAddAll       = 0;
        bReplace      = 0;
        bReplaceAll   = 0;
        iface         = 0;
        lbSrc         = 0;
        lbDest        = 0;
        progressCount = 0;
        progressTimer = 0;
        result        =-1;
    }
    
    QPushButton*                 bAdd;
    QPushButton*                 bAddAll;
    QPushButton*                 bReplace;
    QPushButton*                 bReplaceAll;
    QUrl                         src;
    QUrl                         dest;
    Interface*                   iface;
    QLabel*                      lbSrc;
    QLabel*                      lbDest;
    QByteArray                   buffer;
    QPixmap                      mimePix;
    KDcrawIface::WorkingPixmap   progressPix;
    int                          progressCount;
    QTimer*                      progressTimer;
    int                          result;
};

ReplaceDialog::ReplaceDialog(QWidget* const parent, const QString& _caption,
                                               Interface* const _iface, const QUrl& _src, const QUrl& _dest)
                      : QDialog(parent), d(new Private)
{
    setObjectName("ReplaceDialog");

    d->src   = _src;
    d->dest  = _dest;
    d->iface = _iface;

    setWindowTitle(_caption);
    
    QDialogButtonBox* const buttonBox   = new QDialogButtonBox();
    
    buttonBox->addButton(QDialogButtonBox::Cancel);
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(cancelPressed()));
    
    d->bAdd = new QPushButton(buttonBox);
    d->bAdd->setText(i18n("Add As New"));
    d->bAdd->setToolTip(i18n("Item will be added alongside the linked version."));
    connect(d->bAdd, SIGNAL(clicked()),
            this, SLOT(addPressed()));
    
    d->bAddAll = new QPushButton(buttonBox);
    d->bAddAll->setText(i18n("Add All"));
    d->bAddAll->setToolTip(i18n("Items will be added alongside the linked version. You will not be prompted again."));
    connect(d->bAddAll, SIGNAL(clicked()),
            this, SLOT(addAllPressed()));    
    
    d->bReplace = new QPushButton(buttonBox);
    d->bReplace->setText(i18n("Replace"));
    d->bReplace->setToolTip(i18n("Item will be replacing the linked version."));
    connect(d->bReplace, SIGNAL(clicked()),
            this, SLOT(replacePressed()));
    
    d->bReplaceAll = new QPushButton(buttonBox);
    d->bReplaceAll->setText(i18n("Replace All"));
    d->bReplaceAll->setToolTip(i18n("Items will be replacing the linked version. You will not be prompted again."));
    connect(d->bReplaceAll, SIGNAL(clicked()),
            this, SLOT(replaceAllPressed()));
    
    buttonBox->addButton(d->bAdd, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(d->bAddAll, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(d->bReplace, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(d->bReplaceAll, QDialogButtonBox::AcceptRole);
    
    connect(buttonBox, SIGNAL(accepted()),
            this, SLOT(accept()));

    connect(buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));

    QVBoxLayout* const pLayout = new QVBoxLayout(this);
    pLayout->addStrut(360);	// makes dlg at least that wide

    QGridLayout* const gridLayout = new QGridLayout();
    pLayout->addLayout(gridLayout);

    QString sentence1 = i18n("A linked item already exists.");

    QLabel* lb1 = new KSqueezedTextLabel(sentence1, this);
    lb1->setAlignment(Qt::AlignHCenter);
    gridLayout->addWidget(lb1, 0, 0, 1, 3);

    d->mimePix = KIO::pixmapForUrl(d->dest);
    d->lbDest  = new QLabel(this);
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

    QHBoxLayout* const layout2 = new QHBoxLayout();
    pLayout->addLayout(layout2);

    KDcrawIface::RLineWidget* const separator = new KDcrawIface::RLineWidget(Qt::Horizontal,this);
    pLayout->addWidget(separator);

    QHBoxLayout* const layout = new QHBoxLayout();
    pLayout->addLayout(layout);

    layout->addStretch(1);

    layout->addWidget(buttonBox);

    d->progressTimer = new QTimer(this);

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));

    d->progressTimer->start(300);

    // get source thumbnail
    if (d->iface && d->src.isValid())
    {
        connect(d->iface, SIGNAL(gotThumbnail(QUrl,QPixmap)),
                this, SLOT(slotThumbnail(QUrl,QPixmap)));

        d->iface->thumbnail(d->src, 48);
    }

    // get dest thumbnail
    d->buffer.resize(0);

    if (d->dest.isValid())
    {
        KIO::TransferJob* const job = KIO::get(d->dest, KIO::NoReload, KIO::HideProgressInfo);
        job->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded" );

        connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
                this, SLOT(slotData(KIO::Job*,QByteArray)));

        connect(job, SIGNAL(result(KJob*)),
                this, SLOT(slotResult(KJob*)));
    }

    resize(sizeHint());
}

void ReplaceDialog::slotResult(KJob *job)
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

void ReplaceDialog::slotData(KIO::Job* /*job*/, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = d->buffer.size();
    d->buffer.resize(d->buffer.size() + data.size());
    memcpy(d->buffer.data()+oldSize, data.data(), data.size());
}

void ReplaceDialog::slotThumbnail(const QUrl& url, const QPixmap& pix)
{
    if (url == d->src)
    {
        d->lbSrc->setPixmap(pix.scaled(200, 200, Qt::KeepAspectRatio, Qt::FastTransformation));
    }
}

ReplaceDialog::~ReplaceDialog()
{
    delete d;
}

void ReplaceDialog::cancelPressed()
{
    close();
    d->result = PWR_CANCEL;
}

void ReplaceDialog::addPressed()
{
    close();
    d->result = PWR_ADD;
}

void ReplaceDialog::addAllPressed()
{
    close();
    d->result = PWR_ADD_ALL;
}

void ReplaceDialog::replacePressed()
{
    close();
    d->result = PWR_REPLACE;
}

void ReplaceDialog::replaceAllPressed()
{
    close();
    d->result = PWR_REPLACE_ALL;
}

QPixmap ReplaceDialog::setProgressAnimation(const QPixmap& thumb, const QPixmap& pix)
{
    QPixmap overlay = thumb;
    QPixmap mask(overlay.size());
    mask.fill(QColor(128, 128, 128, 192));
    QPainter p(&overlay);
    p.drawPixmap(0, 0, mask);
    p.drawPixmap((overlay.width()/2) - (pix.width()/2), (overlay.height()/2) - (pix.height()/2), pix);
    return overlay;
}

void ReplaceDialog::slotProgressTimerDone()
{
    d->lbDest->setPixmap(setProgressAnimation(d->mimePix, d->progressPix.frameAt(d->progressCount)));
    d->progressCount++;

    if (d->progressCount == 8)
        d->progressCount = 0;

    d->progressTimer->start(300);
}

int ReplaceDialog::getResult()
{
    return d->result;
}

} // namespace KIPIGoogleServicesPlugin
