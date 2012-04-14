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

#include "createptotask.h"

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

CreatePtoTask::CreatePtoTask(QObject* parent, const KUrl& workDir, PanoramaFileType fileType, bool hdr,
                             KUrl& ptoUrl, const KUrl::List& inputFiles, const ItemUrlsMap& preProcessedMap)
    : Task(parent, CREATEPTO, workDir), ptoUrl(&ptoUrl), preProcessedMap(&preProcessedMap),
      fileType(fileType), hdr(hdr), inputFiles(&inputFiles)
{}

CreatePtoTask::CreatePtoTask(const KUrl& workDir, PanoramaFileType fileType, bool hdr,
                             KUrl& ptoUrl, const KUrl::List& inputFiles, const ItemUrlsMap& preProcessedMap)
    : Task(0, CREATEPTO, workDir), ptoUrl(&ptoUrl), preProcessedMap(&preProcessedMap),
      fileType(fileType), hdr(hdr), inputFiles(&inputFiles)
{}

CreatePtoTask::~CreatePtoTask()
{}

void CreatePtoTask::run()
{
    (*ptoUrl) = tmpDir;
    ptoUrl->setFileName(QString("pano_base.pto"));

    QFile pto(ptoUrl->toLocalFile());
    if (pto.exists())
    {
        errString = i18n("PTO file already created in the temporary directory.");
        successFlag = false;
        return;
    }
    if (!pto.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        errString = i18n("PTO file cannot be created in the temporary directory.");
        successFlag = false;
        return;
    }

    QTextStream pto_stream(&pto);

    // The pto is created following the file format described here:
    // http://hugin.sourceforge.net/docs/nona/nona.txt

    // 1. Project parameters
    pto_stream << "p";
    pto_stream << " f1";                        // Cylindrical projection
    pto_stream << " n\"TIFF_m c:LZW\"";
    pto_stream << " R" << (hdr ? '1' : '0');    // HDR output
    //pto_stream << " T\"FLOAT\"";              // 32bits color depth
    //pto_stream << " S," << X_left << "," << X_right << "," << X_top << "," << X_bottom;   // Crop values
    pto_stream << " k0";                        // Reference image
    pto_stream << endl;

    // 2. Images
    pto_stream << endl;
    int i = 0;
    for (i = 0; i < inputFiles->size(); ++i)
    {
        KUrl inputFile(inputFiles->at(i));
        KUrl preprocessedUrl(preProcessedMap->value(inputFile).preprocessedUrl);
        KPMetadata meta;
        meta.load(preprocessedUrl.toLocalFile());
        QSize size = meta.getPixelSize();

        pto_stream << "i";
        pto_stream << " f0";                    // Lens projection type (rectilinear)
        pto_stream << " w" << size.width();     // Image width
        pto_stream << " h" << size.height();    // Image height
        if (i > 0)
        {
            // We suppose that the pictures are all taken with the same camera and lens
            pto_stream << " a=0 b=0 c=0 d=0 e=0 v=0 g=0 t=0";           // Geometry
            pto_stream << " Va=0 Vb=0 Vc=0 Vd=0 Vx=0 Vy=0";             // Vignetting
        }
        pto_stream << " n\"" << preprocessedUrl.toLocalFile() << '"';
        pto_stream << endl;
    }

    // 3. Variables to optimize
    pto_stream << endl;
    // Geometry optimization
    pto_stream << "v a0" << endl;
    pto_stream << "v b0" << endl;
    pto_stream << "v c0" << endl;
    pto_stream << "v d0" << endl;
    pto_stream << "v e0" << endl;
    pto_stream << "v Va0" << endl;
    pto_stream << "v Vb0" << endl;
    pto_stream << "v Vc0" << endl;
    pto_stream << "v Vd0" << endl;
    pto_stream << "v Vx0" << endl;
    pto_stream << "v Vy0" << endl;
    for (int j = 0; j < i; ++j)
    {
        // Colors optimization
        pto_stream << "v Ra" << j << endl;
        pto_stream << "v Rb" << j << endl;
        pto_stream << "v Rc" << j << endl;
        pto_stream << "v Rd" << j << endl;
        pto_stream << "v Re" << j << endl;
        pto_stream << "v Eev" << j << endl;
        pto_stream << "v Erv" << j << endl;
        pto_stream << "v Ebv" << j << endl;
        // Position optimization
        pto_stream << "v y" << j << endl;
        pto_stream << "v p" << j << endl;
        pto_stream << "v r" << j << endl;
    }

    switch (fileType)
    {
        case TIFF:
            pto_stream << "#hugin_outputImageType tif" << endl;
            pto_stream << "#hugin_outputImageTypeCompression LZW" << endl;
            break;
        case JPEG:
            pto_stream << "#hugin_outputImageType jpg" << endl;
            pto_stream << "#hugin_outputJPEGQuality 95" << endl;
            break;
    }

    pto.close();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
