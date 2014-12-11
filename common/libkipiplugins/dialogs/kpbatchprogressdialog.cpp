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

#include "kpbatchprogressdialog.h"

// Qt includes

#include <QBrush>
#include <QWidget>
#include <QLayout>
#include <QListWidget>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QStyle>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "kpprogresswidget.h"

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
                setIcon(QIcon::fromTheme("system-run").pixmap(16, 16));
                break;
            case SuccessMessage:
                setIcon(QIcon::fromTheme("dialog-ok").pixmap(16, 16));
                break;
            case WarningMessage:
                setIcon(QIcon::fromTheme("dialog-warning").pixmap(16, 16));
                setForeground(QBrush(Qt::darkYellow));
                break;
            case ErrorMessage:
                setIcon(QIcon::fromTheme("dialog-error").pixmap(16, 16));
                setForeground(QBrush(Qt::red));
                break;
            case ProgressMessage:
            default:
                setIcon(QIcon::fromTheme("dialog-information").pixmap(16, 16));
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
   : RVBox(parent), d(new Private)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    layout()->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    d->actionsList = new QListWidget(this);
    d->actionsList->setSortingEnabled(false);
    d->actionsList->setWhatsThis(i18n("<p>This is the current processing status.</p>"));

    //---------------------------------------------

    d->progress = new KPProgressWidget(this);
    d->progress->setRange(0, 100);
    d->progress->setValue(0);
    d->progress->setWhatsThis(i18n("<p>This is the batch job progress as a percentage.</p>"));

    //---------------------------------------------

    connect(this, &KPBatchProgressWidget::customContextMenuRequested, this, &KPBatchProgressWidget::slotContextMenu);

    connect(d->progress, &KPProgressWidget::signalProgressCanceled, this, &KPBatchProgressWidget::signalProgressCanceled);
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
    QMenu popmenu(this);
    QAction * const action = new QAction(QIcon::fromTheme("edit-copy"), i18n("Copy to Clipboard"), this);

    connect(action, &QAction::triggered, this, &KPBatchProgressWidget::slotCopy2ClipBoard);

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

KPBatchProgressDialog::KPBatchProgressDialog(QWidget* const, const QString& caption)
   : QDialog(0)
{
    setModal(false);
    setWindowTitle(caption);

    m_progress                      = new KPBatchProgressWidget(this);
    m_progress->progressScheduled(caption, QIcon::fromTheme("kipi").pixmap(22, 22));
    QDialogButtonBox* const buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);

    QVBoxLayout* const vbx          = new QVBoxLayout(this);
    vbx->addWidget(m_progress);
    vbx->addWidget(buttons);
    setLayout(vbx);

    resize(600, 400);

    connect(m_progress, &KPBatchProgressWidget::signalProgressCanceled,
            this, &KPBatchProgressDialog::slotCancel);

    connect(buttons->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &KPBatchProgressDialog::slotCancel);
}

KPBatchProgressDialog::~KPBatchProgressDialog()
{
}

KPBatchProgressWidget* KPBatchProgressDialog::progressWidget() const
{
    return m_progress;
}

void KPBatchProgressDialog::slotCancel()
{
    progressWidget()->progressCompleted();
}

}  // namespace KIPIPlugins
