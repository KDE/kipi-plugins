/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-04
 * Description : Batch progress dialog
 *
 * Copyright (C) 2004-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>
#include <kguiitem.h>
#include <kstandardguiitem.h>

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
   : RVBox(parent),
     d(new Private)
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

    connect(this, &KPBatchProgressWidget::customContextMenuRequested,
            this, &KPBatchProgressWidget::slotContextMenu);

    connect(d->progress, &KPProgressWidget::signalProgressCanceled,
            this, &KPBatchProgressWidget::signalProgressCanceled);
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
    QAction* const action = new QAction(QIcon::fromTheme("edit-copy"), i18n("Copy to Clipboard"), this);

    connect(action, &QAction::triggered,
            this, &KPBatchProgressWidget::slotCopy2ClipBoard);

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

class KPBatchProgressDialog::Private
{
public:

    Private()
        : progressWidget(0),
          buttonBox(0)
    {
    }

    KPBatchProgressWidget* progressWidget;
    QDialogButtonBox*      buttonBox;
};

KPBatchProgressDialog::KPBatchProgressDialog(QWidget* const /*parent*/, const QString& caption)
   : QDialog(0),
     d(new Private)
{
    setModal(false);
    setWindowTitle(caption);

    d->buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    d->buttonBox->button(QDialogButtonBox::Cancel)->setDefault(true);

    d->progressWidget = new KPBatchProgressWidget(this);
    d->progressWidget->progressScheduled(caption, QIcon::fromTheme("kipi").pixmap(22, 22));

    QVBoxLayout* const mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(d->progressWidget);
    mainLayout->addWidget(d->buttonBox);

    connect(d->buttonBox, &QDialogButtonBox::rejected,
            this, &KPBatchProgressDialog::cancelClicked);

    connect(d->progressWidget, &KPBatchProgressWidget::signalProgressCanceled,
            this, &KPBatchProgressDialog::cancelClicked);

    connect(this, &KPBatchProgressDialog::cancelClicked,
            this, &KPBatchProgressDialog::slotCancel);

    resize(600, 400);
}

KPBatchProgressDialog::~KPBatchProgressDialog()
{
}

KPBatchProgressWidget* KPBatchProgressDialog::progressWidget() const
{
    return d->progressWidget;
}

void KPBatchProgressDialog::setButtonClose()
{
    KGuiItem::assign(d->buttonBox->button(QDialogButtonBox::Cancel), KStandardGuiItem::close());

    // Clicking "Close" now does two things:
    //  1. Emits signal cancelClicked(),
    //  2. Accepts the dialog.
    connect(d->buttonBox, &QDialogButtonBox::rejected,
            this, &KPBatchProgressDialog::accept);
}

void KPBatchProgressDialog::slotCancel()
{
    progressWidget()->progressCompleted();
}

}  // namespace KIPIPlugins
