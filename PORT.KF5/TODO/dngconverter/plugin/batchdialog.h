/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : DNG converter batch dialog
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef BATCHDIALOG_H
#define BATCHDIALOG_H

// KDE includes

#include <kurl.h>

// Local includes

#include "kp4tooldialog.h"

class QCloseEvent;

using namespace KIPIPlugins;

namespace KIPIDNGConverterPlugin
{

class ActionData;
class DNGConverterAboutData;

class BatchDialog : public KP4ToolDialog
{

Q_OBJECT

public:

    BatchDialog(DNGConverterAboutData* const about);
    ~BatchDialog();

   void addItems(const QUrl::List& itemList);

protected:

    void closeEvent(QCloseEvent* e);

private:

    void readSettings();
    void saveSettings();

    void busy(bool busy);

    void processAll();
    void processed(const QUrl& url, const QString& tmpFile);
    void processingFailed(const QUrl& url, int result);

private Q_SLOTS:

    void slotDefault();
    void slotClose();
    void slotStartStop();
    void slotAborted();
    void slotThreadFinished();

    /** Set Identity and Target file.
     */
    void slotIdentify();

    void slotAction(const KIPIDNGConverterPlugin::ActionData&);

private:

    class Private;
    Private* const d;
};

} // namespace KIPIDNGConverterPlugin

#endif /* BATCHDIALOG_H */
