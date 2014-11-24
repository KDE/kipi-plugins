/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
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

#ifndef TASK_H
#define TASK_H

// KDE includes

#include <kurl.h>
#include <threadweaver/Job.h>

// LibKDcraw includes

#include <libkdcraw/rawdecodingsettings.h>

// Local includes

#include "kpsavesettingswidget.h"
#include "actions.h"

using namespace ThreadWeaver;
using namespace KIPIPlugins;
using namespace KDcrawIface;

namespace KIPIRawConverterPlugin
{

class Task : public Job
{
    Q_OBJECT

public:

    Task(QObject* const parent, const KUrl& url, const Action& action);
    ~Task();

    void setSettings(const RawDecodingSettings& rawDecodingSettings,
                     KPSaveSettingsWidget::OutputFormat outputFormat);

Q_SIGNALS:

    void signalStarting(const KIPIRawConverterPlugin::ActionData& ad);
    void signalFinished(const KIPIRawConverterPlugin::ActionData& ad);

public Q_SLOTS:

    void slotCancel();

protected:

    void run();

private:

    class Private;
    Private* const d;
};

}  // namespace KIPIRawConverterPlugin

#endif /* TASK_H */
