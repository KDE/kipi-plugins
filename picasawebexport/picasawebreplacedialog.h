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

#ifndef PICASAWEBREPLACEDIALOG_H
#define PICASAWEBREPLACEDIALOG_H

#include <QDialog>
#include <QString>

#include <kurl.h>
#include <kio/global.h>
#include <kio/job.h>

#include <libkipi/interface.h>

using namespace KIPI;

namespace KIPIPicasawebExportPlugin
{

enum PicasawebReplaceDialog_Result { PWR_REPLACE = 3,
                                     PWR_REPLACE_ALL = 4,
                                     PWR_ADD = 1,
                                     PWR_ADD_ALL = 2,
                                     PWR_CANCEL = 0 };

class PicasawebReplaceDialog : public QDialog
{
    Q_OBJECT

public:

    PicasawebReplaceDialog(QWidget* parent, const QString& caption,
                           Interface* iface, const KUrl& src, const KUrl& dest);
    ~PicasawebReplaceDialog();

public Q_SLOTS:

    void cancelPressed();
    void addPressed();
    void addAllPressed();
    void replacePressed();
    void replaceAllPressed();

private Q_SLOTS:

    void slotResult(KJob *job);
    void slotData(KIO::Job *job, const QByteArray& data);
    void slotThumbnail(const KUrl& url, const QPixmap& pix);
    void slotProgressTimerDone();

private:

    QPixmap setProgressAnimation(const QPixmap& thumb, const QPixmap& pix);

private:

    class PicasawebReplaceDialogPrivate;

    PicasawebReplaceDialogPrivate* const d;
};

} // namespace KIPIPicasawebExportPlugin

#endif // PICASAWEBREPLACEDIALOG_H
