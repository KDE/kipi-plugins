/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-04
 * Description : Batch progress dialog
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

#include "kpbatchprogressdialog.moc"

// Qt includes

#include <QBrush>
#include <QWidget>
#include <QLayout>
#include <QListWidget>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>

// KDE includes

#include <kmenu.h>
#include <kaction.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>

// Local includes

#include <kpprogresswidget.h>

namespace KIPIPlugins
{

class KPBatchProgressItem : public QListWidgetItem
{
public:

    KPBatchProgressItem(QListWidget* const parent, const QString& message, int messageType)
        : QListWidgetItem(message, parent)
    {
        // Set the icon.

        switch(messageType)
        {
            case StartingMessage:
                setIcon(SmallIcon("system-run"));
                break;
            case SuccessMessage:
                setIcon(SmallIcon("dialog-ok"));
                break;
            case WarningMessage:
                setIcon(SmallIcon("dialog-warning"));
                setForeground(QBrush(Qt::darkYellow));
                break;
            case ErrorMessage:
                setIcon(SmallIcon("dialog-error"));
                setForeground(QBrush(Qt::red));
                break;
            case ProgressMessage:
                setIcon(SmallIcon("dialog-information"));
                break;
            default:
                setIcon(SmallIcon("dialog-information"));
                break;
        }

        // Set the message text.

        setText(message);
    }
};

// ----------------------------------------------------------------------

class KPBatchProgressWidget::Private
{
public:

    Private()
    {
        progress    = 0;
        actionsList = 0;
    }

    QListWidget*      actionsList;
    KPProgressWidget* progress;
};

KPBatchProgressWidget::KPBatchProgressWidget(QWidget* const parent)
   : KVBox(parent), d(new Private)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    layout()->setSpacing(KDialog::spacingHint());

    d->actionsList = new QListWidget(this);
    d->actionsList->setSortingEnabled(false);
    d->actionsList->setWhatsThis(i18n("<p>This is the current processing status.</p>"));

    //---------------------------------------------

    d->progress = new KPProgressWidget(this);
    d->progress->setRange(0, 100);
    d->progress->setValue(0);
    d->progress->setWhatsThis(i18n("<p>This is the batch job progress as a percentage.</p>"));

    //---------------------------------------------

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotContextMenu()));

    connect(d->progress, SIGNAL(signalProgressCanceled()),
            this, SIGNAL(signalProgressCanceled()));
}

KPBatchProgressWidget::~KPBatchProgressWidget()
{
    delete d;
}

void KPBatchProgressWidget::progressScheduled(const QString& title, const QPixmap& thumb)
{
    d->progress->progressScheduled(title, true, true);
    d->progress->progressThumbnailChanged(thumb);
}

void KPBatchProgressWidget::progressCompleted()
{
    d->progress->progressCompleted();
}

void KPBatchProgressWidget::addedAction(const QString& text, int type)
{
    KPBatchProgressItem* const item = new KPBatchProgressItem(d->actionsList, text, type);
    d->actionsList->setCurrentItem(item);
    d->progress->progressStatusChanged(text);
}

void KPBatchProgressWidget::reset()
{
    d->actionsList->clear();
    d->progress->setValue(0);
}

void KPBatchProgressWidget::setProgress(int current, int total)
{
    d->progress->setMaximum(total);
    d->progress->setValue(current);
}

int KPBatchProgressWidget::progress() const
{
    return d->progress->value();
}

int KPBatchProgressWidget::total() const
{
    return d->progress->maximum();
}

void KPBatchProgressWidget::setTotal(int total)
{
    d->progress->setMaximum(total);
}

void KPBatchProgressWidget::setProgress(int current)
{
    d->progress->setValue(current);
}

void KPBatchProgressWidget::slotContextMenu()
{
    KMenu popmenu(this);
    KAction* const action = new KAction(KIcon("edit-copy"), i18n("Copy to Clipboard"), this);

    connect(action, SIGNAL(triggered(bool)),
            this, SLOT(slotCopy2ClipBoard()));

    popmenu.addAction(action);
    popmenu.exec(QCursor::pos());
}

void KPBatchProgressWidget::slotCopy2ClipBoard()
{
    QString textInfo;

    for (int i=0 ; i < d->actionsList->count() ; ++i)
    {
        textInfo.append(d->actionsList->item(i)->text());
        textInfo.append("\n");
    }

    QMimeData* const mimeData = new QMimeData();
    mimeData->setText(textInfo);
    QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
}

// ---------------------------------------------------------------------------------

KPBatchProgressDialog::KPBatchProgressDialog(QWidget* const /*parent*/, const QString& caption)
   : KDialog(0)
{
    setCaption(caption);
    setButtons(Cancel);
    setDefaultButton(Cancel);
    setModal(false);

    KPBatchProgressWidget* const w = new KPBatchProgressWidget(this);
    w->progressScheduled(caption, KIcon("kipi").pixmap(22, 22));
    setMainWidget(w);
    resize(600, 400);

    connect(w, SIGNAL(signalProgressCanceled()),
            this, SIGNAL(cancelClicked()));

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));
}

KPBatchProgressDialog::~KPBatchProgressDialog()
{
}

KPBatchProgressWidget* KPBatchProgressDialog::progressWidget()
{
    return (qobject_cast<KPBatchProgressWidget*>(mainWidget()));
}

void KPBatchProgressDialog::slotCancel()
{
    progressWidget()->progressCompleted();
}

}  // namespace KIPIPlugins
