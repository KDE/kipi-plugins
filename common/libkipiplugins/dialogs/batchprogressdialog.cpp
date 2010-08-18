/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-05-04
 * Description : Batch progress dialog
 *
 * Copyright (C) 2004-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QListWidget>
#include <QProgressBar>
#include <QLayout>

// KDE includes

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kvbox.h>

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

class BatchProgressDialog::BatchProgressDialogPriv
{
public:

    BatchProgressDialogPriv()
    {
        progress    = 0;
        actionsList = 0;
    }

    QProgressBar* progress;

    QListWidget*  actionsList;
};

BatchProgressDialog::BatchProgressDialog(QWidget* parent, const QString& caption)
                   : KDialog(parent), d(new BatchProgressDialogPriv)
{
    setCaption(caption);
    setButtons(Cancel);
    setDefaultButton(Cancel);
    setModal(true);

    KVBox* box = new KVBox(this);
    box->layout()->setSpacing(KDialog::spacingHint());
    setMainWidget(box);

    //---------------------------------------------

    d->actionsList = new QListWidget(box);
    d->actionsList->setSortingEnabled(false);
    d->actionsList->setWhatsThis(i18n("<p>This is the current processing status.</p>"));

    //---------------------------------------------

    d->progress = new QProgressBar(box);
    d->progress->setRange(0, 100);
    d->progress->setValue(0);
    d->progress->setWhatsThis(i18n("<p>This is the batch job progress as a percentage.</p>"));
    resize(600, 400);
}

BatchProgressDialog::~BatchProgressDialog()
{
    delete d;
}

void BatchProgressDialog::addedAction(const QString& text, int type)
{
    BatchProgressItem* item = new BatchProgressItem(d->actionsList, text, type);
    d->actionsList->setCurrentItem(item);
}

void BatchProgressDialog::reset()
{
    d->actionsList->clear();
    d->progress->setValue(0);
}

void BatchProgressDialog::setProgress(int current, int total)
{
    d->progress->setMaximum(total);
    d->progress->setValue(current);
}

int BatchProgressDialog::progress() const
{
    return d->progress->value();
}

int BatchProgressDialog::total() const
{
    return d->progress->maximum();
}

void BatchProgressDialog::setTotal(int total)
{
    d->progress->setMaximum(total);
}

void BatchProgressDialog::setProgress(int current)
{
    d->progress->setValue(current);
}

}  // namespace KIPIPlugins
