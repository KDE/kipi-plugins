/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-03-15
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2012-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIPanoramaPlugin
{

CreatePreviewTask::CreatePreviewTask(const QString& workDirPath, QSharedPointer<const PTOType> inputPTO,
                                     QUrl& previewPtoUrl, const ItemUrlsMap& preProcessedUrlsMap)
    : Task(CREATEMKPREVIEW, workDirPath),
      previewPtoUrl(previewPtoUrl),
      ptoData(inputPTO),
      preProcessedUrlsMap(preProcessedUrlsMap),
      m_iface(0),
      m_meta(0)
{    
    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        m_iface = pl->interface();
        
        if (m_iface)
            m_meta = m_iface->createMetadataProcessor();
    }
}

CreatePreviewTask::~CreatePreviewTask()
{
}

void CreatePreviewTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    if (!m_meta)
    {
        successFlag = false;
        return;
    }
    
    PTOType data(*ptoData);

    if (data.images.size() != preProcessedUrlsMap.size())
    {
        errString = i18n("Project file parsing failed.");
        qCDebug(KIPIPLUGINS_LOG) << "Missing parsing data!";
        successFlag = false;
        return;
    }

    m_meta->load(preProcessedUrlsMap.begin().value().preprocessedUrl);
    double wIn  = (double)m_meta->getPixelSize().width();
    
    m_meta->load(preProcessedUrlsMap.begin().value().previewUrl);
    double wOut = (double)m_meta->getPixelSize().width();
    
    double scalingFactor = wOut / wIn;

    data.project.fileFormat.fileType = PTOType::Project::FileFormat::JPEG;
    data.project.fileFormat.quality  = 90;
    data.project.size.setHeight(data.project.size.height() * scalingFactor);
    data.project.size.setWidth(data.project.size.width() * scalingFactor);
    data.project.crop                = QRect();

    for (auto& image : data.images)
    {
        QUrl imgUrl = tmpDir.resolved(QUrl::fromLocalFile(image.fileName));

        ItemUrlsMap::const_iterator it;
        const ItemUrlsMap* const ppum = &preProcessedUrlsMap;

        for (it = ppum->constBegin(); it != ppum->constEnd() && it.value().preprocessedUrl.toLocalFile() != imgUrl.toLocalFile(); ++it);

        if (it == ppum->constEnd())
        {
            errString = i18n("Unknown input file in the project file: <filename>%1</filename>", image.fileName);
            qCDebug(KIPIPLUGINS_LOG) << "Unknown input File in the PTO: " << image.fileName;
            qCDebug(KIPIPLUGINS_LOG) << "IMG: " << imgUrl.toLocalFile();
            successFlag = false;
            return;
        }

        image.fileName = it.value().previewUrl.toLocalFile();
        m_meta->load(QUrl::fromLocalFile(image.fileName));
        image.size     = m_meta->getPixelSize();
        image.optimisationParameters.clear();
    }

    // Remove unncessary stuff
    data.controlPoints.clear();

    // Add two commented line for a JPEG output
    data.lastComments.clear();
    data.lastComments << QStringLiteral("#hugin_outputImageType jpg");
    data.lastComments << QStringLiteral("#hugin_outputJPEGQuality 90");

    previewPtoUrl = tmpDir.resolved(QUrl::fromLocalFile(QStringLiteral("preview.pto")));
    data.createFile(previewPtoUrl.toLocalFile());

    qCDebug(KIPIPLUGINS_LOG) << "Preview PTO File created: " << previewPtoUrl.fileName();

    successFlag = true;
}

}  // namespace KIPIPanoramaPlugin
