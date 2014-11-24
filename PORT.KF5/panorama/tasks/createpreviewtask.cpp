/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-03-15
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

#include "createpreviewtask.h"

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

CreatePreviewTask::CreatePreviewTask(QObject* parent, const KUrl& workDir, const PTOType& inputPTO, 
                                     KUrl& previewPtoUrl, const ItemUrlsMap& preProcessedUrlsMap)
    : Task(parent, CREATEMKPREVIEW, workDir), previewPtoUrl(&previewPtoUrl),
      ptoData(inputPTO), preProcessedUrlsMap(preProcessedUrlsMap)
{}

CreatePreviewTask::CreatePreviewTask(const KUrl& workDir, const PTOType& inputPTO, 
                                     KUrl& previewPtoUrl, const ItemUrlsMap& preProcessedUrlsMap)
    : Task(0, CREATEMKPREVIEW, workDir), previewPtoUrl(&previewPtoUrl),
      ptoData(inputPTO), preProcessedUrlsMap(preProcessedUrlsMap)
{}

CreatePreviewTask::~CreatePreviewTask()
{}

void CreatePreviewTask::run()
{

    PTOType data(ptoData);
    if (data.images.size() != preProcessedUrlsMap.size())
    {
        errString = i18n("Project file parsing failed.");
        kDebug() << "Missing parsing data!";
        successFlag = false;
        return;
    }

    KPMetadata metaIn(preProcessedUrlsMap.begin().value().preprocessedUrl.toLocalFile());
    KPMetadata metaOut(preProcessedUrlsMap.begin().value().previewUrl.toLocalFile());
    double scalingFactor = ((double) metaOut.getPixelSize().width()) / ((double) metaIn.getPixelSize().width());

    data.project.fileFormat.fileType = PTOType::Project::FileFormat::JPEG;
    data.project.fileFormat.quality = 90;
    data.project.size.setHeight(data.project.size.height() * scalingFactor);
    data.project.size.setWidth(data.project.size.width() * scalingFactor);
    data.project.crop = QRect();
    for (int imageId = 0; imageId < data.images.size(); imageId++)
    {
        PTOType::Image& image = data.images[imageId];
        KUrl imgUrl(KUrl(tmpDir), image.fileName);
        ItemUrlsMap::const_iterator it;
        const ItemUrlsMap *ppum = &preProcessedUrlsMap;
        for (it = ppum->constBegin(); it != ppum->constEnd() && it.value().preprocessedUrl != imgUrl; ++it);
        if (it == ppum->constEnd())
        {
            errString = i18n("Unknown input file in the project file: %1", image.fileName);
            kDebug() << "Unknown input File in the PTO: " << image.fileName;
            kDebug() << "IMG: " << imgUrl.toLocalFile();
            successFlag = false;
            return;
        }
        image.fileName = it.value().previewUrl.fileName();
        KUrl preview(KUrl(tmpDir), image.fileName);
        KPMetadata metaImage(preview.toLocalFile());
        image.size = metaImage.getPixelSize();
        image.optimisationParameters.clear();
    }

    // Remove unncessary stuff
    data.controlPoints.clear();

    // Add two commented line for a JPEG output
    data.lastComments.clear();
    data.lastComments << "#hugin_outputImageType jpg";
    data.lastComments << "#hugin_outputJPEGQuality 90";

    (*previewPtoUrl) = tmpDir;
    previewPtoUrl->setFileName("preview.pto");
    data.createFile(previewPtoUrl->toLocalFile());

    kDebug() << "Preview PTO File created: " << previewPtoUrl->fileName();

    successFlag = true;
}

}  // namespace KIPIPanoramaPlugin
