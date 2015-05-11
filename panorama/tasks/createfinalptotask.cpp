/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-12-05
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "createfinalptotask.h"

// Qt includes

#include <QFile>

// KDE includes

#include <klocale.h>
#include <kdebug.h>

// Local includes

#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIPanoramaPlugin
{

CreateFinalPtoTask::CreateFinalPtoTask(QObject* parent, const KUrl& workDir, const PTOType& ptoData, KUrl& finalPtoUrl, const QRect& crop)
    : Task(parent, CREATEFINALPTO, workDir),
      ptoData(ptoData),
      finalPtoUrl(&finalPtoUrl),
      crop(crop)
{
}

CreateFinalPtoTask::CreateFinalPtoTask(const KUrl& workDir, const PTOType& ptoData, KUrl& finalPtoUrl, const QRect& crop)
    : Task(0, CREATEFINALPTO, workDir),
      ptoData(ptoData),
      finalPtoUrl(&finalPtoUrl),
      crop(crop)
{
}

CreateFinalPtoTask::~CreateFinalPtoTask()
{
}

void CreateFinalPtoTask::run()
{
    (*finalPtoUrl) = tmpDir;
    finalPtoUrl->setFileName(QString("final.pto"));

    QFile pto(finalPtoUrl->toLocalFile());

    if (pto.exists())
    {
        errString   = i18n("PTO file already created in the temporary directory.");
        successFlag = false;
        return;
    }

    if (!pto.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        errString   = i18n("PTO file cannot be created in the temporary directory.");
        successFlag = false;
        return;
    }

    ptoData.project.crop = crop;
    ptoData.createFile(finalPtoUrl->toLocalFile());

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
