/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
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

#include "actionthread.moc"

// C++ includes

#include <iostream>

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QtDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QPointer>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ktempdir.h>
#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/JobCollection.h>
#include <threadweaver/DependencyPolicy.h>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

// Local includes

#include "kpmetadata.h"
#include "kpwriteimage.h"
#include "kpversion.h"
#include "ptoparser.h"

using namespace KIPIPlugins;
using namespace ThreadWeaver;

namespace KIPIPanoramaPlugin
{

class ActionThread::Task : public ThreadWeaver::Job
{
public:

    struct ActionPreprocessParameters
    {
        KUrl                                fileUrl;
        ItemPreprocessedUrls                *preProcessedUrls;
        KDcrawIface::RawDecodingSettings    settings;
    };

    struct ActionCreatePtoParameters
    {
        PanoramaFileType                    fileType;
        bool                                hdr;
        const KUrl::List                    *inputFiles;
        const ItemUrlsMap                   *itemUrlsMap;
        KUrl                                *ptoUrl;
    };

    struct ActionCPFindParameters
    {
        bool                                celeste;
        KUrl                                *ptoUrl;
        KUrl                                *cpFindPtoUrl;
        QString                             cpFindPath;
    };

    struct ActionCPCleanParameters
    {
        KUrl                                *cpFindPtoUrl;
        KUrl                                *cpCleanPtoUrl;
        QString                             cpCleanPath;
        PTOType                             *ptoUrlData;
    };

    struct ActionOptimizeParameters
    {
        KUrl                                *ptoUrl;
        KUrl                                *autoOptimiserPtoUrl;
        bool                                levelHorizon;
        bool                                optimizeProjectionAndSize;
        QString                             autooptimiserPath;
    };

    struct ActionCreatePreviewPtoParameters
    {
        KUrl                                ptoUrl;
        KUrl                                *previewPtoUrl;
        ItemUrlsMap                         preProcessedUrlsMap;
    };

    struct ActionCreateMKParameters
    {
        KUrl                                *ptoUrl;
        KUrl                                *mkUrl;
        KUrl                                *panoUrl;
        PanoramaFileType                    fileType;
        QString                             pto2mkPath;
        QString                             nonaPath;
        QString                             enblendPath;
    };

    struct ActionMakeFile
    {
        KUrl                                *mkUrl;
        KUrl                                *panoUrl;
        QString                             makePath;
    };

    struct ActionCopy
    {
        KUrl                                panoUrl;
        KUrl                                finalPanoUrl;
        KUrl                                ptoUrl;
        const ItemUrlsMap                   *urlList;
        bool                                savePTO;
    };

public:

    QString                     errString;

    KUrl                        tmpDir;

    Action                      action;
    int                         id;

    union
    {
        ActionPreprocessParameters          *preProcessingParams;
        ActionCreatePtoParameters           *createPtoParams;
        ActionCPFindParameters              *cpFindParams;
        ActionCPCleanParameters             *cpCleanParams;
        ActionOptimizeParameters            *optimizeParams;
        ActionCreatePreviewPtoParameters    *createPreviewPtoParams;
        ActionCreateMKParameters            *createMKParams;
        ActionMakeFile                      *nonaFileParams;
        ActionMakeFile                      *stitchParams;
        ActionCopy                          *copyParams;
    };

private:

    bool                        successFlag;
    bool                        isAbortedFlag;

    QPointer<KDcraw>            rawProcess;
    KProcess                    *process;

public:

    Task(QObject* parent = 0)
        : Job(parent), id(0), successFlag(false), isAbortedFlag(false), rawProcess(0), process(0)
    {
    }

    ~Task()
    {
        switch (action)
        {
            case PREPROCESS_INPUT:
            {
                delete preProcessingParams;
                break;
            }
            case CREATEPTO:
            {
                delete createPtoParams->itemUrlsMap;
                delete createPtoParams;
                break;
            }
            case CPFIND:
            {
                delete cpFindParams->ptoUrl;
                delete cpFindParams;
                break;
            }
            case CPCLEAN:
            {
                delete cpCleanParams->cpFindPtoUrl;
                delete cpCleanParams->cpCleanPtoUrl;
                delete cpCleanParams;
                break;
            }
            case OPTIMIZE:
            {
                delete optimizeParams;
                break;
            }
            case CREATEPREVIEWPTO:
            {
                delete createPreviewPtoParams;
                break;
            }
            case CREATEMK:
            case CREATEMKPREVIEW:
            {
                delete createMKParams->ptoUrl;
                delete createMKParams;
                break;
            }
            case NONAFILE:
            case NONAFILEPREVIEW:
            {
                delete nonaFileParams;
                break;
            }
            case STITCH:
            case STITCHPREVIEW:
            {
                delete stitchParams->mkUrl;
                delete stitchParams->panoUrl;
                delete stitchParams;
                break;
            }
            case COPY:
            {
                delete copyParams;
                break;
            }
            case NONE:
            {
                break;
            }
        }
    }

    void requestAbort ()
    {
        isAbortedFlag = true;

        if (!rawProcess.isNull())
        {
            rawProcess->cancel();
        }

        if (process != 0)
        {
            process->kill();
        }
    }

    bool success() const
    {
        return successFlag;
    }

protected:

    void run()
    {
        switch (action)
        {
            case PREPROCESS_INPUT:
            {
                kDebug() << "Starting Preview Generation" << id;
                successFlag = preprocessFile();
                kDebug() << "Preview Generated" << id;
                break;
            }
            case CREATEPTO:
            {
                kDebug() << "Starting Initial PTO Generation";
                successFlag = createPTO();
                kDebug() << "Initial PTO Generated";
                break;
            }
            case CPFIND:
            {
                kDebug() << "Starting CPFind";
                successFlag = startCPFind();
                kDebug() << "CPFind finished";
                break;
            }
            case CPCLEAN:
            {
                kDebug() << "Starting CPClean";
                successFlag = startCPClean();
                kDebug() << "CPClean finished";
                break;
            }
            case OPTIMIZE:
            {
                kDebug() << "Starting Optimization";
                successFlag = startOptimization();
                kDebug() << "Optimization finished";
                break;
            }
            case CREATEPREVIEWPTO:
            {
                kDebug() << "Starting Preview PTO Generation";
                successFlag = createPreviewPto();
                kDebug() << "Preview PTO Generated";
                break;
            }
            case CREATEMK:
            case CREATEMKPREVIEW:
            {
                kDebug() << "Starting Makefile Generation";
                successFlag = createMK();
                kDebug() << "Makefile Generated";
                break;
            }
            case NONAFILE:
            case NONAFILEPREVIEW:
            {
                kDebug() << "Starting Step" << id << "Of Panorama Compilation";
                successFlag = compileMKStep();
                kDebug() << "Panorama Compilation Step" << id << "Finished";
                break;
            }
            case STITCH:
            case STITCHPREVIEW:
            {
                kDebug() << "Starting Panorama Stitching";
                successFlag = compileMK();
                kDebug() << "Panorama Stitched";
                break;
            }
            case COPY:
            {
                kDebug() << "Starting Panorama Copy";
                successFlag = copyFiles();
                kDebug() << "Panorama Copied";
                break;
            }
            case NONE:
            {
                kError() << "Unknown action specified";
            }
        }
    }

private:

    bool preprocessFile()
    {
        if (isRawFile(preProcessingParams->fileUrl.toLocalFile()))
        {
            preProcessingParams->preProcessedUrls->preprocessedUrl  = tmpDir;

            if (!convertRaw())
            {
                return false;
            }
        }
        else
        {
            // NOTE: in this case, preprocessed Url is the original file Url.
            preProcessingParams->preProcessedUrls->preprocessedUrl  = preProcessingParams->fileUrl;
        }

        preProcessingParams->preProcessedUrls->previewUrl       = tmpDir;

        if (!computePreview(preProcessingParams->preProcessedUrls->preprocessedUrl))
        {
            return false;
        }

        return true;
    }

    bool computePreview(const KUrl& inUrl)
    {
        KUrl& outUrl = preProcessingParams->preProcessedUrls->previewUrl;

        QFileInfo fi(inUrl.toLocalFile());
        outUrl.setFileName(fi.completeBaseName().replace('.', '_') + QString("-preview.jpg"));

        QImage img;
        if (img.load(inUrl.toLocalFile()))
        {
            QImage preview = img.scaled(1280, 1024, Qt::KeepAspectRatio);
            bool saved     = preview.save(outUrl.toLocalFile(), "JPEG");
            // save exif information also to preview image for auto rotation
            if (saved)
            {
                KPMetadata metaIn(inUrl.toLocalFile());
                KPMetadata metaOut(outUrl.toLocalFile());
                metaOut.setImageOrientation(metaIn.getImageOrientation());
                metaOut.setImageDimensions(QSize(preview.width(), preview.height()));
                metaOut.applyChanges();
            }
            kDebug() << "Preview Image url: " << outUrl << ", saved: " << saved;
            return saved;
        }
        else
        {
            errString = i18n("Input image cannot be loaded for preview generation");
        }
        return false;
    }

    static bool isRawFile(const KUrl& url)
    {
        QString rawFilesExt(KDcraw::rawFiles());

        QFileInfo fileInfo(url.toLocalFile());
        if (rawFilesExt.toUpper().contains(fileInfo.suffix().toUpper()))
            return true;

        return false;
    }

    bool convertRaw()
    {
        KUrl &inUrl = preProcessingParams->fileUrl;
        KUrl &outUrl = preProcessingParams->preProcessedUrls->preprocessedUrl;

        int        width, height, rgbmax;
        QByteArray imageData;

        rawProcess = new KDcraw;
        bool decoded = rawProcess->decodeRAWImage(inUrl.toLocalFile(), preProcessingParams->settings, imageData, width, height, rgbmax);


        if (decoded)
        {
            uchar* sptr  = (uchar*)imageData.data();
            float factor = 65535.0 / rgbmax;
            unsigned short tmp16[3];

            // Set RGB color components.
            for (int i = 0 ; !isAbortedFlag && (i < width * height) ; ++i)
            {
                // Swap Red and Blue and re-ajust color component values
                tmp16[0] = (unsigned short)((sptr[5]*256 + sptr[4]) * factor);      // Blue
                tmp16[1] = (unsigned short)((sptr[3]*256 + sptr[2]) * factor);      // Green
                tmp16[2] = (unsigned short)((sptr[1]*256 + sptr[0]) * factor);      // Red
                memcpy(&sptr[0], &tmp16[0], 6);
                sptr += 6;
            }

            if (isAbortedFlag)
            {
                errString = i18n("Operation canceled.");
                return false;
            }

            KPMetadata metaIn, metaOut;
            metaIn.load(inUrl.toLocalFile());
            KPMetadata::MetaDataMap m = metaIn.getExifTagsDataList(QStringList("Photo"), true);
            KPMetadata::MetaDataMap::iterator it;
            for (it = m.begin(); it != m.end(); ++it)
            {
                metaIn.removeExifTag(it.key().toAscii().data(), false);
            }
            metaOut.setData(metaIn.data());
            metaOut.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
            metaOut.setImageDimensions(QSize(width, height));
            metaOut.setExifTagString("Exif.Image.DocumentName", inUrl.fileName());
            metaOut.setXmpTagString("Xmp.tiff.Make",  metaOut.getExifTagString("Exif.Image.Make"));
            metaOut.setXmpTagString("Xmp.tiff.Model", metaOut.getExifTagString("Exif.Image.Model"));
            metaOut.setImageOrientation(KPMetadata::ORIENTATION_NORMAL);

            QByteArray prof = KPWriteImage::getICCProfilFromFile(preProcessingParams->settings.outputColorSpace);

            KPWriteImage wImageIface;
            wImageIface.setCancel(&isAbortedFlag);
            wImageIface.setImageData(imageData, width, height, true, false, prof, metaOut);
            QFileInfo fi(inUrl.toLocalFile());
            outUrl.setFileName(fi.completeBaseName().replace('.', '_') + QString(".tif"));

            if (!wImageIface.write2TIFF(outUrl.toLocalFile()))
            {
                errString = i18n("Tiff image creation failed.");
                return false;
            }
            else
            {
                metaOut.save(outUrl.toLocalFile());
            }
        }
        else
        {
            errString = i18n("Raw file conversion failed.");
            return false;
        }

        kDebug() << "Convert RAW output url: " << outUrl;

        return true;
    }

    bool createPTO()
    {
        (*createPtoParams->ptoUrl) = tmpDir;
        createPtoParams->ptoUrl->setFileName(QString("pano_base.pto"));

        QFile pto(createPtoParams->ptoUrl->toLocalFile());
        if (pto.exists())
        {
            errString = i18n("PTO file already created in the temporary directory.");
            return false;
        }
        if (!pto.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            errString = i18n("PTO file cannot be created in the temporary directory.");
            return false;
        }

        QTextStream pto_stream(&pto);

        // The pto is created following the file format described here:
        // http://hugin.sourceforge.net/docs/nona/nona.txt

        // 1. Project parameters
        pto_stream << "p";
        pto_stream << " f1";                    // Cylindrical projection
        pto_stream << " n\"TIFF_m c:LZW\"";
        pto_stream << " R" << (createPtoParams->hdr ? '1' : '0');// HDR output
        //pto_stream << " T\"FLOAT\"";            // 32bits color depth
        //pto_stream << " S," << X_left << "," << X_right << "," << X_top << "," << X_bottom;   // Crop values
        pto_stream << " k0";                    // Reference image
        pto_stream << endl;

        // 2. Images
        pto_stream << endl;
        int i = 0;
        for (i = 0; i < createPtoParams->inputFiles->size(); ++i)
        {
            KUrl inputFile(createPtoParams->inputFiles->at(i));
            KUrl preprocessedUrl(createPtoParams->itemUrlsMap->value(inputFile).preprocessedUrl);
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

        switch (createPtoParams->fileType)
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

        return true;
    }

    bool startCPFind()
    {
        // Run CPFind to get control points and order the images

        (*cpFindParams->cpFindPtoUrl) = tmpDir;
        cpFindParams->cpFindPtoUrl->setFileName(QString("cp_pano.pto"));

        process = new KProcess();
        process->clearProgram();
        process->setWorkingDirectory(tmpDir.toLocalFile());
        process->setOutputChannelMode(KProcess::MergedChannels);
        process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

        QStringList args;
        args << cpFindParams->cpFindPath;
        if (cpFindParams->celeste)
            args << "--celeste";
        args << "-o";
        args << cpFindParams->cpFindPtoUrl->toLocalFile();
        args << cpFindParams->ptoUrl->toLocalFile();

        process->setProgram(args);

        kDebug() << "CPFind command line: " << process->program();

        process->start();

        if (!process->waitForFinished(-1) || process->exitCode() != 0)
        {
            errString = getProcessError(process);
            delete process;
            return false;
        }

        delete process;

        return true;
    }

    bool startCPClean()
    {
        (*cpCleanParams->cpCleanPtoUrl) = tmpDir;
        cpCleanParams->cpCleanPtoUrl->setFileName(QString("cp_pano_clean.pto"));

        process = new KProcess();
        process->clearProgram();
        process->setWorkingDirectory(tmpDir.toLocalFile());
        process->setOutputChannelMode(KProcess::MergedChannels);
        process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

        QStringList args;
        args << cpCleanParams->cpCleanPath;
        args << "-o";
        args << cpCleanParams->cpCleanPtoUrl->toLocalFile();
        args << cpCleanParams->cpFindPtoUrl->toLocalFile();

        process->setProgram(args);

        kDebug() << "CPClean command line: " << process->program();

        process->start();

        if (!process->waitForFinished(-1) || process->exitCode() != 0)
        {
            errString = getProcessError(process);
            return false;
        }

        cpCleanParams->ptoUrlData = new PTOType();
        if (!PTOParser::parseFile(cpCleanParams->cpCleanPtoUrl->toLocalFile(), *cpCleanParams->ptoUrlData))
        {
            kDebug() << "Parse Failed!!";
        }

        return true;
    }

    static QString getProcessError(KProcess* proc)
    {
        if (!proc)
            return QString();

        QString std = proc->readAll();
        return (i18n("Cannot run %1:\n\n %2", proc->program()[0], std));
    }

    bool startOptimization()
    {
        (*optimizeParams->autoOptimiserPtoUrl) = tmpDir;
        optimizeParams->autoOptimiserPtoUrl->setFileName(QString("auto_op_pano.pto"));

        process = new KProcess();
        process->clearProgram();
        process->setWorkingDirectory(tmpDir.toLocalFile());
        process->setOutputChannelMode(KProcess::MergedChannels);
        process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

        QStringList argsAO;
        argsAO << optimizeParams->autooptimiserPath;
        argsAO << "-am";
        if (optimizeParams->levelHorizon)
        {
            argsAO << "-l";
        }
        if (optimizeParams->optimizeProjectionAndSize)
        {
            argsAO << "-s";
        }
        argsAO << "-o";
        argsAO << optimizeParams->autoOptimiserPtoUrl->toLocalFile();
        argsAO << optimizeParams->ptoUrl->toLocalFile();

        process->setProgram(argsAO);

        kDebug() << "autooptimiser command line: " << process->program();

        process->start();

        if (!process->waitForFinished(-1) || process->exitCode() != 0)
        {
            errString = getProcessError(process);
            return false;
        }

        return true;
    }

    bool createPreviewPto()
    {
        kDebug() << "Preview Generation (" << createPreviewPtoParams->ptoUrl.toLocalFile() << ")";
        QFile input(createPreviewPtoParams->ptoUrl.toLocalFile());
        QStringList pto;
        if (!input.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            errString = i18n("Cannot read project file.");
            kDebug() << "Can't read PTO File!";
            return false;
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
            return false;
        }

        (*createPreviewPtoParams->previewPtoUrl) = tmpDir;
        createPreviewPtoParams->previewPtoUrl->setFileName("preview.pto");
        QFile output(createPreviewPtoParams->previewPtoUrl->toLocalFile());
        if (!output.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            errString = i18n("Preview project file cannot be created.");
            kDebug() << "Can't create a new PTO File!";
            return false;
        }

        QTextStream previewPtoStream(&output);

        KPMetadata metaIn(createPreviewPtoParams->preProcessedUrlsMap.begin().value().preprocessedUrl.toLocalFile());
        KPMetadata metaOut(createPreviewPtoParams->preProcessedUrlsMap.begin().value().previewUrl.toLocalFile());
        double scalingFactor = ((double) metaOut.getPixelSize().width()) / ((double) metaIn.getPixelSize().width());

        // TODO: change that to use boost::spirit (parser) and boost::karma (generator)
        foreach(const QString& line, pto)
        {
            if (line.isEmpty())
            {
                continue;
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
                        ItemUrlsMap *ppum = &createPreviewPtoParams->preProcessedUrlsMap;
                        for (it = (ItemUrlsMap::iterator) ppum->begin(); it != ppum->end() && it.value().preprocessedUrl != imgUrl; ++it);
                        if (it == ppum->end())
                        {
                            input.close();
                            errString = i18n("Unknown input file in the project file: %1", imgFileName);
                            kDebug() << "Unknown input File in the PTO: " << imgFileName;
                            kDebug() << "IMG: " << imgUrl.toLocalFile();
                            return false;
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

        kDebug() << "Preview PTO File created: " << createPreviewPtoParams->ptoUrl.fileName();

        return true;
    }

    bool createMK()
    {
        QFileInfo fi(createMKParams->ptoUrl->toLocalFile());
        (*createMKParams->mkUrl) = tmpDir;
        createMKParams->mkUrl->setFileName(fi.completeBaseName() + QString(".mk"));

        (*createMKParams->panoUrl) = tmpDir;
        switch (createMKParams->fileType)
        {
            case JPEG:
                createMKParams->panoUrl->setFileName(fi.completeBaseName() + QString(".jpg"));
                break;
            case TIFF:
                createMKParams->panoUrl->setFileName(fi.completeBaseName() + QString(".tif"));
                break;
        }

        process = new KProcess();
        process->clearProgram();
        process->setWorkingDirectory(tmpDir.toLocalFile());
        process->setOutputChannelMode(KProcess::MergedChannels);
        process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

        QStringList args;
        args << createMKParams->pto2mkPath;
        args << "-o";
        args << createMKParams->mkUrl->toLocalFile();
        args << "-p";
        args << fi.completeBaseName();
        args << createMKParams->ptoUrl->toLocalFile();

        process->setProgram(args);

        kDebug() << "pto2mk command line: " << process->program();

        process->start();

        if (!process->waitForFinished(-1) || process->exitCode() != 0)
        {
            errString = getProcessError(process);
            return false;
        }

        /* Just replacing strings in the generated makefile to reflect the
         * location of the binaries of nona and enblend. This ensures that
         * the make process will be able to execute those binaries without
         * worring that any binary is not in the system path.
         */
        QFile mkUrlFile(createMKParams->mkUrl->toLocalFile());
        mkUrlFile.open(QIODevice::ReadWrite);

        QString fileData = mkUrlFile.readAll();
        fileData.replace("NONA=\"nona\"", QString("NONA=\"%1\"").arg(createMKParams->nonaPath));
        fileData.replace("ENBLEND=\"enblend\"", QString("ENBLEND=\"%1\"").arg(createMKParams->enblendPath));

        mkUrlFile.seek(0L);
        mkUrlFile.write(fileData.toAscii());
        mkUrlFile.close();

        return true;
    }

    bool compileMKStep()
    {
        QFileInfo fi(nonaFileParams->mkUrl->toLocalFile());

        process = new KProcess();
        process->clearProgram();
        process->setWorkingDirectory(tmpDir.toLocalFile());
        process->setOutputChannelMode(KProcess::MergedChannels);
        process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

        QString mkFile = fi.completeBaseName() + (id >= 10 ? (id >= 100 ? "0" : "00") : "000") + QString::number(id) + ".tif";
        QStringList args;
        args << nonaFileParams->makePath;
        args << "-f";
        args << nonaFileParams->mkUrl->toLocalFile();
        args << mkFile;

        process->setProgram(args);
        kDebug() << "make command line: " << process->program();

        process->start();

        if (!process->waitForFinished(-1) || process->exitCode() != 0)
        {
            errString = getProcessError(process);
            return false;
        }

        return true;
    }

    bool compileMK()
    {
        process = new KProcess();
        process->clearProgram();
        process->setWorkingDirectory(tmpDir.toLocalFile());
        process->setOutputChannelMode(KProcess::MergedChannels);
        process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

        QStringList args;
        args << stitchParams->makePath;
        args << "-f";
        args << stitchParams->mkUrl->toLocalFile();

        process->setProgram(args);

        kDebug() << "make command line: " << process->program();

        process->start();

        if (!process->waitForFinished(-1) || process->exitCode() != 0)
        {
            errString = getProcessError(process);
            return false;
        }

        return true;
    }

    bool copyFiles()
    {
        QFile panoFile(copyParams->panoUrl.toLocalFile());
        QFile finalPanoFile(copyParams->finalPanoUrl.toLocalFile());

        QFileInfo fi(copyParams->finalPanoUrl.toLocalFile());
        KUrl finalPTOUrl(copyParams->finalPanoUrl);
        finalPTOUrl.setFileName(fi.completeBaseName() + ".pto");
        QFile ptoFile(copyParams->ptoUrl.toLocalFile());
        QFile finalPTOFile(finalPTOUrl.toLocalFile());

        if (!panoFile.exists())
        {
            errString = i18n("Temporary panorama file does not exists.");
            kDebug() << "Temporary panorama file does not exists: " + copyParams->panoUrl.toLocalFile();
            return false;
        }
        if (finalPanoFile.exists())
        {
            errString = i18n("A file named %1 already exists.", copyParams->finalPanoUrl.fileName());
            kDebug() << "Final panorama file already exists: " + copyParams->finalPanoUrl.toLocalFile();
            return false;
        }
        if (copyParams->savePTO && !ptoFile.exists())
        {
            errString = i18n("Temporary project file does not exist.");
            kDebug() << "Temporary project file does not exists: " + copyParams->ptoUrl.toLocalFile();
            return false;
        }
        if (copyParams->savePTO && finalPTOFile.exists())
        {
            errString = i18n("A file named %1 already exists.", finalPTOUrl.fileName());
            kDebug() << "Final project file already exists: " + finalPTOUrl.toLocalFile();
            return false;
        }

        kDebug() << "Copying panorama file...";
        if (!panoFile.copy(copyParams->finalPanoUrl.toLocalFile()) || !panoFile.remove())
        {
            errString = i18n("Cannot move panorama from %1 to %2.",
                             copyParams->panoUrl.toLocalFile(),
                             copyParams->finalPanoUrl.toLocalFile());
            kDebug() << "Cannot move panorama: QFile error = " + panoFile.error();
            return false;
        }

        if (copyParams->savePTO)
        {
            kDebug() << "Copying project file...";
            if (!ptoFile.copy(finalPTOUrl.toLocalFile()))
            {
                errString = i18n("Cannot move project file from %1 to %2.",
                                 copyParams->panoUrl.toLocalFile(),
                                 copyParams->finalPanoUrl.toLocalFile());
                return false;
            }

            kDebug() << "Copying converted RAW files...";
            for (ItemUrlsMap::iterator i = (ItemUrlsMap::iterator) copyParams->urlList->begin(); i != copyParams->urlList->end(); ++i)
            {
                if (isRawFile(i.key()))
                {
                    KUrl finalImgUrl(copyParams->finalPanoUrl);
                    finalImgUrl.setFileName(i->preprocessedUrl.fileName());
                    QFile imgFile(i->preprocessedUrl.toLocalFile());
                    if (!imgFile.copy(finalImgUrl.toLocalFile()))
                    {
                        errString = i18n("Cannot copy converted image file from %1 to %2.",
                        i->preprocessedUrl.toLocalFile(),
                        finalImgUrl.toLocalFile());
                        return false;
                    }
                }
            }
        }

        return true;
    }
};

struct ActionThread::ActionThreadPriv
{
    ActionThreadPriv()
        : celeste(false),
          hdr(false),
          fileType(JPEG),
          preprocessingTmpDir(0)
    {
    }

    bool                            celeste;
    bool                            hdr;
    PanoramaFileType                fileType;
    RawDecodingSettings             rawDecodingSettings;

    KTempDir*                       preprocessingTmpDir;

    void cleanPreprocessingTmpDir()
    {
        if (preprocessingTmpDir)
        {
            preprocessingTmpDir->unlink();
            delete preprocessingTmpDir;
            preprocessingTmpDir = 0;
        }
    }
};

ActionThread::ActionThread(QObject* const parent)
    : KPActionThreadBase(parent), d(new ActionThreadPriv)
{
    qRegisterMetaType<ActionData>();
}

ActionThread::~ActionThread()
{
    d->cleanPreprocessingTmpDir();

    delete d;
}

void ActionThread::setPreProcessingSettings(bool celeste, bool hdr, PanoramaFileType fileType,
                                            const RawDecodingSettings& settings)
{
    d->celeste              = celeste;
    d->hdr                  = hdr;
    d->fileType             = fileType;
    d->rawDecodingSettings  = settings;
}

void ActionThread::preProcessFiles(const KUrl::List& urlList, const QString& cpCleanPath,
                                   const QString& cpFindPath)
{
    d->cleanPreprocessingTmpDir();

    QString prefix = KStandardDirs::locateLocal("tmp", QString("kipi-panorama-tmp-") +
                                                       QString::number(QDateTime::currentDateTime().toTime_t()));

    d->preprocessingTmpDir = new KTempDir(prefix);

    JobCollection       *jobs           = new JobCollection();

    ItemUrlsMap         *items          = new ItemUrlsMap();
    KUrl                *ptoUrl         = new KUrl();
    KUrl                *cpFindPtoUrl   = new KUrl();
    Task                *pto            = new Task();

    // TODO: try to append these jobs as a JobCollection inside a JobSequence
    int id = 0;
    foreach (const KUrl& file, urlList)
    {
        items->insert(file, ItemPreprocessedUrls());

        Task *t                                     = new Task();
        t->action                                   = PREPROCESS_INPUT;
        t->tmpDir                                   = d->preprocessingTmpDir->name();
        t->id                                       = id++;
        t->preProcessingParams                      = new Task::ActionPreprocessParameters();
        t->preProcessingParams->fileUrl             = file;
        t->preProcessingParams->settings            = d->rawDecodingSettings;
        t->preProcessingParams->preProcessedUrls    = &(*items)[file];

        connect(t, SIGNAL(started(ThreadWeaver::Job*)),
                this, SLOT(slotStarting(ThreadWeaver::Job*)));
        connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotStepDone(ThreadWeaver::Job*)));

        jobs->addJob(t);
        DependencyPolicy::instance().addDependency(pto, t);
    }

    pto->action                             = CREATEPTO;
    pto->tmpDir                             = d->preprocessingTmpDir->name();
    pto->createPtoParams                    = new Task::ActionCreatePtoParameters();
    pto->createPtoParams->fileType          = d->fileType;
    pto->createPtoParams->hdr               = d->hdr;
    pto->createPtoParams->inputFiles        = &urlList;
    pto->createPtoParams->itemUrlsMap       = items;
    pto->createPtoParams->ptoUrl            = ptoUrl;

    connect(pto, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(pto, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotExtractItemUrlMaps(ThreadWeaver::Job*)));
    connect(pto, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));
    connect(pto, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotExtractPtoBase(ThreadWeaver::Job*)));

    jobs->addJob(pto);

    Task *cpFind                            = new Task();
    cpFind->action                          = CPFIND;
    cpFind->tmpDir                          = d->preprocessingTmpDir->name();
    cpFind->cpFindParams                    = new Task::ActionCPFindParameters();
    cpFind->cpFindParams->celeste           = d->celeste;
    cpFind->cpFindParams->cpFindPath        = cpFindPath;
    cpFind->cpFindParams->ptoUrl            = ptoUrl;
    cpFind->cpFindParams->cpFindPtoUrl      = cpFindPtoUrl;

    connect(cpFind, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(cpFind, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));
    connect(cpFind, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotExtractCpFindPto(ThreadWeaver::Job*)));

    jobs->addJob(cpFind);
    DependencyPolicy::instance().addDependency(cpFind, pto);

    Task *cpClean                           = new Task();
    cpClean->action                         = CPCLEAN;
    cpClean->tmpDir                         = d->preprocessingTmpDir->name();
    cpClean->cpCleanParams                  = new Task::ActionCPCleanParameters();
    cpClean->cpCleanParams->cpCleanPath     = cpCleanPath;
    cpClean->cpCleanParams->cpFindPtoUrl    = cpFindPtoUrl;
    cpClean->cpCleanParams->cpCleanPtoUrl   = new KUrl();

    connect(cpClean, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(cpClean, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotDone(ThreadWeaver::Job*)));
    connect(cpClean, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotExtractCpCleanPto(ThreadWeaver::Job*)));

    jobs->addJob(cpClean);
    DependencyPolicy::instance().addDependency(cpClean, cpFind);

    appendJob(jobs);
}

void ActionThread::optimizeProject(KUrl& ptoUrl, KUrl& optimizePtoUrl, bool levelHorizon,
                                   bool optimizeProjectionAndSize, const QString& autooptimiserPath)
{
    JobCollection       *jobs                       = new JobCollection();
    Task                *t                          = new Task();
    t->action                                       = OPTIMIZE;
    t->tmpDir                                       = d->preprocessingTmpDir->name();
    t->optimizeParams                               = new Task::ActionOptimizeParameters();
    t->optimizeParams->autooptimiserPath            = autooptimiserPath;
    t->optimizeParams->ptoUrl                       = &ptoUrl;
    t->optimizeParams->autoOptimiserPtoUrl          = &optimizePtoUrl;
    t->optimizeParams->levelHorizon                 = levelHorizon;
    t->optimizeParams->optimizeProjectionAndSize    = optimizeProjectionAndSize;

    connect(t, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(t, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotDone(ThreadWeaver::Job*)));
    connect(t, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotExtractOptimizePto(ThreadWeaver::Job*)));

    jobs->addJob(t);

    appendJob(jobs);
}

void ActionThread::generatePanoramaPreview(const KUrl& ptoUrl, const ItemUrlsMap& preProcessedUrlsMap,
                                           const QString& makePath, const QString& pto2mkPath,
                                           const QString& enblendPath, const QString& nonaPath)
{
    JobCollection   *jobs                                   = new JobCollection();

    Task            *ptoTask                                = new Task();
    ptoTask->action                                         = CREATEPREVIEWPTO;
    ptoTask->tmpDir                                         = d->preprocessingTmpDir->name();
    ptoTask->createPreviewPtoParams                         = new Task::ActionCreatePreviewPtoParameters();
    ptoTask->createPreviewPtoParams->preProcessedUrlsMap    = preProcessedUrlsMap;
    ptoTask->createPreviewPtoParams->ptoUrl                 = ptoUrl;
    ptoTask->createPreviewPtoParams->previewPtoUrl          = new KUrl();

    connect(ptoTask, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(ptoTask, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));

    jobs->addJob(ptoTask);

    appendStitchingJobs(ptoTask,
                        jobs,
                        ptoTask->createPreviewPtoParams->previewPtoUrl,
                        preProcessedUrlsMap,
                        JPEG,
                        makePath,
                        pto2mkPath,
                        enblendPath,
                        nonaPath,
                        true);

    appendJob(jobs);
}

void ActionThread::compileProject(const KUrl& ptoUrl, const ItemUrlsMap& preProcessedUrlsMap,
                                  PanoramaFileType fileType, const QString& makePath, const QString& pto2mkPath,
                                  const QString& enblendPath, const QString& nonaPath)
{
    JobCollection *jobs = new JobCollection();
    appendStitchingJobs(0,
                        jobs,
                        new KUrl(ptoUrl),
                        preProcessedUrlsMap,
                        fileType,
                        makePath,
                        pto2mkPath,
                        enblendPath,
                        nonaPath,
                        false);

    appendJob(jobs);
}

void ActionThread::copyFiles(const KUrl& ptoUrl, const KUrl& panoUrl, const KUrl& finalPanoUrl,
                             const ItemUrlsMap& preProcessedUrlsMap, bool savePTO)
{
    JobCollection   *jobs           = new JobCollection();

    Task            *t              = new Task();
    t->action                       = COPY;
    t->copyParams                   = new Task::ActionCopy();
    t->copyParams->ptoUrl           = ptoUrl;
    t->copyParams->panoUrl          = panoUrl;
    t->copyParams->finalPanoUrl     = finalPanoUrl;
    t->copyParams->urlList          = &preProcessedUrlsMap;
    t->copyParams->savePTO          = savePTO;

    connect(t, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(t, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotDone(ThreadWeaver::Job*)));

    jobs->addJob(t);

    appendJob(jobs);
}

void ActionThread::slotExtractItemUrlMaps(Job* j)
{
    Task *t = static_cast<Task*>(j);

    if (t->action != CREATEPTO)
        kError() << "Wrong task for item urls extraction!" << endl;
    else
        emit itemUrlsMapReady(*t->createPtoParams->itemUrlsMap);
}

void ActionThread::slotExtractPtoBase(Job* j)
{
    Task *t = static_cast<Task*>(j);

    if (t->action != CREATEPTO)
    {
        kError() << "Wrong task for base pto url extraction!" << endl;
    }
    if (t->success())
    {
        emit ptoBaseReady(*t->createPtoParams->ptoUrl);
    }
}

void ActionThread::slotExtractCpFindPto(Job* j)
{
    Task *t = static_cast<Task*>(j);

    if (t->action != CPFIND)
    {
        kError() << "Wrong task for base pto url extraction!" << endl;
    }
    if (t->success())
    {
        emit cpFindPtoReady(*t->cpFindParams->cpFindPtoUrl);
    }
}

void ActionThread::slotExtractCpCleanPto(Job* j)
{
    Task *t = static_cast<Task*>(j);

    if (t->action != CPCLEAN)
    {
        kError() << "Wrong task for base pto url extraction!" << endl;
    }
    if (t->success())
    {
        emit cpCleanPtoReady(*t->cpCleanParams->cpCleanPtoUrl);
    }
}

void ActionThread::slotExtractOptimizePto(Job* j)
{
    Task *t = static_cast<Task*>(j);

    if (t->action != OPTIMIZE)
    {
        kError() << "Wrong task for base pto url extraction!" << endl;
    }
    if (t->success())
    {
        emit optimizePtoReady(*t->optimizeParams->autoOptimiserPtoUrl);
    }
}

void ActionThread::slotExtractPreviewUrl(Job* j)
{
    Task *t = static_cast<Task*>(j);

    if (t->action != STITCHPREVIEW)
    {
        kError() << "Wrong task for base pto url extraction!" << endl;
    }
    if (t->success())
    {
        emit previewFileReady(*t->stitchParams->panoUrl);
    }
}

void ActionThread::slotExtractPanoUrl(Job* j)
{
    Task *t = static_cast<Task*>(j);

    if (t->action != STITCH)
    {
        kError() << "Wrong task for base pto url extraction!" << endl;
    }
    if (t->success())
    {
        emit panoFileReady(*t->stitchParams->panoUrl);
    }
}

void ActionThread::slotStarting(Job* j)
{
    Task *t = static_cast<Task*>(j);

    ActionData ad;
    ad.starting     = true;
    ad.id           = t->id;
    ad.action       = t->action;

    emit starting(ad);
}

void ActionThread::slotStepDone(Job* j)
{
    Task *t = static_cast<Task*>(j);

    ActionData ad;
    ad.starting     = false;
    ad.id           = t->id;
    ad.action       = t->action;
    ad.success      = t->success();
    ad.message      = t->errString;

    emit stepFinished(ad);

    ((QObject*) t)->deleteLater();
}

void ActionThread::slotDone(Job* j)
{
    Task *t = static_cast<Task*>(j);

    ActionData ad;
    ad.starting     = false;
    ad.id           = t->id;
    ad.action       = t->action;
    ad.success      = t->success();
    ad.message      = t->errString;

    emit finished(ad);

    ((QObject*) t)->deleteLater();
}

void ActionThread::appendStitchingJobs(Job* prevJob, JobCollection* jc, KUrl* ptoUrl, const ItemUrlsMap& preProcessedUrlsMap,
                                       PanoramaFileType fileType, const QString& makePath, const QString& pto2mkPath,
                                       const QString& enblendPath, const QString& nonaPath, bool preview)
{
    Task            *createMKTask                           = new Task();
    if (preview)
        createMKTask->action                                = CREATEMKPREVIEW;
    else
        createMKTask->action                                = CREATEMK;
    createMKTask->tmpDir                                    = d->preprocessingTmpDir->name();
    createMKTask->createMKParams                            = new Task::ActionCreateMKParameters();
    createMKTask->createMKParams->ptoUrl                    = ptoUrl;
    createMKTask->createMKParams->mkUrl                     = new KUrl();
    createMKTask->createMKParams->panoUrl                   = new KUrl();
    createMKTask->createMKParams->fileType                  = fileType;
    createMKTask->createMKParams->pto2mkPath                = pto2mkPath;
    createMKTask->createMKParams->nonaPath                  = nonaPath;
    createMKTask->createMKParams->enblendPath               = enblendPath;

    connect(createMKTask, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(createMKTask, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotStepDone(ThreadWeaver::Job*)));

    if (prevJob != 0)
    {
        DependencyPolicy::instance().addDependency(createMKTask, prevJob);
    }
    jc->addJob(createMKTask);

    Task            *compileMKTask                          = new Task();
    for (int i = 0; i < preProcessedUrlsMap.size(); i++)
    {
        Task        *t                                      = new Task();
        if (preview)
            t->action                                       = NONAFILEPREVIEW;
        else
            t->action                                       = NONAFILE;
        t->tmpDir                                           = d->preprocessingTmpDir->name();
        t->id                                               = i;
        t->nonaFileParams                                   = new Task::ActionMakeFile();
        t->nonaFileParams->mkUrl                            = createMKTask->createMKParams->mkUrl;
        t->nonaFileParams->makePath                         = makePath;

        connect(t, SIGNAL(started(ThreadWeaver::Job*)),
                this, SLOT(slotStarting(ThreadWeaver::Job*)));
        connect(t, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotStepDone(ThreadWeaver::Job*)));

        DependencyPolicy::instance().addDependency(t, createMKTask);
        DependencyPolicy::instance().addDependency(compileMKTask, t);
        jc->addJob(t);
    }

    if (preview)
        compileMKTask->action                               = STITCHPREVIEW;
    else
        compileMKTask->action                               = STITCH;
    compileMKTask->tmpDir                                   = d->preprocessingTmpDir->name();
    compileMKTask->stitchParams                             = new Task::ActionMakeFile();
    compileMKTask->stitchParams->mkUrl                      = createMKTask->createMKParams->mkUrl;
    compileMKTask->stitchParams->panoUrl                    = createMKTask->createMKParams->panoUrl;
    compileMKTask->stitchParams->makePath                   = makePath;

    connect(compileMKTask, SIGNAL(started(ThreadWeaver::Job*)),
            this, SLOT(slotStarting(ThreadWeaver::Job*)));
    connect(compileMKTask, SIGNAL(done(ThreadWeaver::Job*)),
            this, SLOT(slotDone(ThreadWeaver::Job*)));
    if (preview)
    {
        connect(compileMKTask, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotExtractPreviewUrl(ThreadWeaver::Job*)));
    }
    else
    {
        connect(compileMKTask, SIGNAL(done(ThreadWeaver::Job*)),
                this, SLOT(slotExtractPanoUrl(ThreadWeaver::Job*)));
    }

    jc->addJob(compileMKTask);
}

}  // namespace KIPIPanoramaPlugin
