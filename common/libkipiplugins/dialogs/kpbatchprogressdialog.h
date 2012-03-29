/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-04
 * Description : Batch progress dialog
 *
 * Copyright (C) 2004-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

/** @file batchprogressdialog.h */

#ifndef KPBATCHPROGRESSDIALOG_H
#define KPBATCHPROGRESSDIALOG_H

// KDE includes

#include <kdialog.h>
#include <kvbox.h>

// Local includes

#include "kipiplugins_export.h"
#include "kpprogresswidget.h"

namespace KIPIPlugins
{

enum ActionMessageType
{
    StartingMessage = 0,
    SuccessMessage,
    WarningMessage,
    ErrorMessage,
    ProgressMessage
};

// --------------------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT KPBatchProgressWidget : public KVBox
{
    Q_OBJECT

public:

    explicit KPBatchProgressWidget(QWidget* const parent=0);
    ~KPBatchProgressWidget();

    KPProgressWidget* progressBar() const;

    void addedAction(const QString& text, int type);

    void setProgress(int current, int total);
    int  progress() const;
    int  total() const;
    void reset();

public Q_SLOTS:

    void setProgress(int);
    void setTotal(int total);

private Q_SLOTS:

    void slotContextMenu();
    void slotCopy2ClipBoard();

private:

    class KPBatchProgressWidgetPriv;
    KPBatchProgressWidgetPriv* const d;
};

// --------------------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT KPBatchProgressDialog : public KDialog
{
    Q_OBJECT

public:

    explicit KPBatchProgressDialog(QWidget* const parent=0, const QString& caption=QString());
    ~KPBatchProgressDialog();

    KPBatchProgressWidget* progressWidget() const;

private:

    class KPBatchProgressDialogPriv;
    KPBatchProgressDialogPriv* const d;
};

}  // namespace KIPIPlugins

#endif  // BATCHPROGRESSDIALOG_H
