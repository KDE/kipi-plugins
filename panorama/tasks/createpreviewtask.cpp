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

CreatePreviewTask::CreatePreviewTask(QObject* parent, const KUrl& workDir, const KUrl& input,
                                     KUrl& previewPtoUrl, const ItemUrlsMap& preProcessedUrlsMap)
    : Task(parent, CREATEMKPREVIEW, workDir), previewPtoUrl(&previewPtoUrl),
      ptoUrl(input), preProcessedUrlsMap(preProcessedUrlsMap)
{}

CreatePreviewTask::CreatePreviewTask(const KUrl& workDir, const KUrl& input,
                                     KUrl& previewPtoUrl, const ItemUrlsMap& preProcessedUrlsMap)
    : Task(0, CREATEMKPREVIEW, workDir), previewPtoUrl(&previewPtoUrl),
      ptoUrl(input), preProcessedUrlsMap(preProcessedUrlsMap)
{}

CreatePreviewTask::~CreatePreviewTask()
{}

void CreatePreviewTask::run()
{
    kDebug() << "Preview Generation (" << ptoUrl.toLocalFile() << ")";
    QFile input(ptoUrl.toLocalFile());
    QStringList pto;
    if (!input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        errString = i18n("Cannot read project file.");
        kDebug() << "Can't read PTO File!";
        successFlag = false;
        return;
    }
    else
    {
        QTextStream in(&input);
        while (!in.atEnd())
        {
            pto.append(in.readLine());
        }
        input.close();
    }

    if (pto.count() == 0)
    {
        errString = i18n("Empty project file.");
        kDebug() << "Pto file empty!!";
        successFlag = false;
        return;
    }

    (*previewPtoUrl) = tmpDir;
    previewPtoUrl->setFileName("preview.pto");
    QFile output(previewPtoUrl->toLocalFile());
    if (!output.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        errString = i18n("Preview project file cannot be created.");
        kDebug() << "Can't create a new PTO File!";
        successFlag = false;
        return;
    }

    QTextStream previewPtoStream(&output);

    KPMetadata metaIn(preProcessedUrlsMap.begin().value().preprocessedUrl.toLocalFile());
    KPMetadata metaOut(preProcessedUrlsMap.begin().value().previewUrl.toLocalFile());
    double scalingFactor = ((double) metaOut.getPixelSize().width()) / ((double) metaIn.getPixelSize().width());

    // TODO: change that to use boost::spirit (parser) and boost::karma (generator)
    foreach(const QString& line, pto)
    {
        if (line.isEmpty())
        {
            continue;
        }

        if (isAbortedFlag)
        {
            successFlag = false;
            return;
        }

        QString tmp;
        QStringList parameters = line.split(' ', QString::SkipEmptyParts);

        if (line[0] == 'p')
        {
            tmp.clear();
            foreach(const QString& p, parameters)
            {
                if (p[0] == 'w' || p[0] == 'h')
                {
                    int size = ((double) (p.right(p.size() - 1)).toInt()) * scalingFactor;
                    tmp.append(p[0]);
                    tmp.append(QString::number(size));
                }
                else if (p[0] == 'n')
                {
                    tmp.append("n\"JPEG q90\"");
                    break;          // n should be the last parameter (and the space before qXX introduce another parameter)
                }
                else
                {
                    tmp.append(p);
                }
                tmp.append(" ");
            }
        }
        else if (line[0] == 'm')
        {
            tmp = line;
        }
        else if (line[0] == 'i')
        {
            tmp.clear();
            QStringList realParameters;
            bool nRead = false;
            foreach(const QString& p, parameters)
            {
                if (p[0] != 'n')
                {
                    if (nRead)
                    {
                        realParameters[realParameters.size() - 1] += ' ' + p;
                    }
                    else
                    {
                        realParameters << p;
                    }
                }
                else
                {
                    nRead = true;
                    realParameters << p;
                }
            }
            foreach(const QString& p, realParameters)
            {
                if (p[0] == 'w')
                {
                    tmp.append("w");
                    tmp.append(QString::number(metaOut.getPixelSize().width()));
                }
                else if (p[0] == 'h')
                {
                    tmp.append("h");
                    tmp.append(QString::number(metaOut.getPixelSize().height()));
                }
                else if (p[0] == 'n')
                {
                    QString imgFileName = p.mid(2, p.size() - 3);
                    KUrl imgUrl(KUrl(tmpDir), imgFileName);
                    ItemUrlsMap::iterator it;
                    const ItemUrlsMap *ppum = &preProcessedUrlsMap;
                    for (it = (ItemUrlsMap::iterator) ppum->begin(); it != ppum->end() && it.value().preprocessedUrl != imgUrl; ++it);
                    if (it == ppum->end())
                    {
                        input.close();
                        errString = i18n("Unknown input file in the project file: %1", imgFileName);
                        kDebug() << "Unknown input File in the PTO: " << imgFileName;
                        kDebug() << "IMG: " << imgUrl.toLocalFile();
                        successFlag = false;
                        return;
                    }
                    tmp.append("n\"");
                    tmp.append(it.value().previewUrl.fileName());
                    tmp.append("\"");
                    break;
                }
                else
                {
                    tmp.append(p);
                }
                tmp.append(" ");
            }
        }
        else
        {
            continue;
        }
        previewPtoStream << tmp << endl;
    }

    // Add two commented line for a JPEG output
    previewPtoStream << "#hugin_outputImageType jpg" << endl;
    previewPtoStream << "#hugin_outputJPEGQuality 90" << endl;

    output.close();

    kDebug() << "Preview PTO File created: " << ptoUrl.fileName();

    successFlag = true;
    return;
}

}  // namespace KIPIPanoramaPlugin
