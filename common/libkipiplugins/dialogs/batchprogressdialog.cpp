/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-05-04
 * Description : Batch progress dialog
 *
 * Copyright (C) 2004-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "batchprogressdialog.moc"

// Qt includes

#include <QBrush>
#include <QWidget>
#include <QProgressBar>
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

namespace KIPIPlugins
{

class BatchProgressItem : public QListWidgetItem
{
public:

BatchProgressItem(QListWidget* parent, const QString& message, int messageType)
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
    }

    // Set the message text.

    setText(message);
}

};

// ----------------------------------------------------------------------

class BatchProgressWidget::BatchProgressWidgetPriv
{
public:

    BatchProgressWidgetPriv()
    {
        progress    = 0;
        actionsList = 0;
    }

    QProgressBar* progress;

    QListWidget*  actionsList;
};

BatchProgressWidget::BatchProgressWidget(QWidget* parent)
   : KVBox(parent), d(new BatchProgressWidgetPriv)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    layout()->setSpacing(KDialog::spacingHint());

    d->actionsList = new QListWidget(this);
    d->actionsList->setSortingEnabled(false);
    d->actionsList->setWhatsThis(i18n("<p>This is the current processing status.</p>"));

    //---------------------------------------------

    d->progress = new QProgressBar(this);
    d->progress->setRange(0, 100);
    d->progress->setValue(0);
    d->progress->setWhatsThis(i18n("<p>This is the batch job progress as a percentage.</p>"));

    //---------------------------------------------

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotContextMenu()));
}

BatchProgressWidget::~BatchProgressWidget()
{
    delete d;
}

QListWidget* BatchProgressWidget::listView() const
{
    return d->actionsList;
}

QProgressBar* BatchProgressWidget::progressBar() const
{
    return d->progress;
}

void BatchProgressWidget::addedAction(const QString& text, int type)
{
    BatchProgressItem* item = new BatchProgressItem(d->actionsList, text, type);
    d->actionsList->setCurrentItem(item);
}

void BatchProgressWidget::reset()
{
    d->actionsList->clear();
    d->progress->setValue(0);
}

void BatchProgressWidget::setProgress(int current, int total)
{
    d->progress->setMaximum(total);
    d->progress->setValue(current);
}

int BatchProgressWidget::progress() const
{
    return d->progress->value();
}

int BatchProgressWidget::total() const
{
    return d->progress->maximum();
}

void BatchProgressWidget::setTotal(int total)
{
    d->progress->setMaximum(total);
}

void BatchProgressWidget::setProgress(int current)
{
    d->progress->setValue(current);
}

void BatchProgressWidget::slotContextMenu()
{
    KMenu popmenu(this);
    KAction* action = new KAction(KIcon("edit-copy"), i18n("Copy to Clipboard"), this);
    connect(action, SIGNAL(triggered(bool)),
            this, SLOT(slotCopy2ClipBoard()));

    popmenu.addAction(action);
    popmenu.exec(QCursor::pos());
}

void BatchProgressWidget::slotCopy2ClipBoard()
{
    QString textInfo;

    for (int i=0 ; i < d->actionsList->count() ; ++i)
    {
        textInfo.append(d->actionsList->item(i)->text());
        textInfo.append("\n");
    }

    QMimeData* mimeData = new QMimeData();
    mimeData->setText(textInfo);
    QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
}

// ---------------------------------------------------------------------------------

class BatchProgressDialog::BatchProgressDialogPriv
{
public:

    BatchProgressDialogPriv()
    {
        box = 0;
    }

    BatchProgressWidget* box;
};

BatchProgressDialog::BatchProgressDialog(QWidget* parent, const QString& caption)
   : KDialog(parent), d(new BatchProgressDialogPriv)
{
    setCaption(caption);
    setButtons(Cancel);
    setDefaultButton(Cancel);
    setModal(true);

    d->box = new BatchProgressWidget(this);
    setMainWidget(d->box);
    resize(600, 400);
}

BatchProgressDialog::~BatchProgressDialog()
{
    delete d;
}

BatchProgressWidget* BatchProgressDialog::progressWidget() const
{
    return d->box;
}

}  // namespace KIPIPlugins
