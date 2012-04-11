/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SIMPLEVIEWER_H
#define SIMPLEVIEWER_H

// Qt includes

#include <QObject>
#include <QList>
#include <QImage>
#include <QDomDocument>
#include <QDomElement>

// KDE includes

#include <kurl.h>
#include <kzip.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "simpleviewersettingscontainer.h"
#include "kpbatchprogressdialog.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIFlashExportPlugin
{

class SimpleViewer : public QObject
{
    Q_OBJECT

public:

    explicit SimpleViewer(Interface* const interface, QObject* const parent=0);
    ~SimpleViewer();

    void initProgressWdg() const;

    /**
     * Installs the SimpleViewer files for the later export
     * on the users machine
     */
    bool unzip(const QString& url) const;

    KPBatchProgressWidget* progressWidget() const;

    void setSettings(SimpleViewerSettingsContainer* const settings);

    void startExport();

    void appendPluginFiles(int pluginType);

private:

    /**
     * Creates the standard simpleviewer directories
     *
     * @return true=ok
     */
    bool createExportDirectories() const;

    /**
     * Creates the images and thumbnails
     *
     * @return true=ok
     */
    bool exportImages();

    /**
     * Create images and thumbnail from every list on images
     * @param images - list with image's url
     * @param xmlDoc - xml configuration file for simpleviewer plugin
     * @param galleryElem - xml tag that contains info about all uploaded images( SimpleViewer, Autoviewer, PostcardViewer)
     * @param photosElem - xml tag that contains info about all uploaded images (TiltViewer)
     */
    void processKUrlList(KUrl::List& images, QDomDocument& xmlDoc,
                         QDomElement& galleryElem, QDomElement& photosElem);

    /**
     * Creates a simpleviewer thumbnail from images
     *
     * @param image the original images
     * @param thumbnail the created thumbnail
     *
     * @return true=ok
     */
    bool createThumbnail(const QImage& image, QImage& thumbnail) const;

    /**
     * Resizes the image for the gallery
     *
     * @param image the original images
     * @param thumbnail the created thumbnail
     *
     * @return true=ok
     */
    bool resizeImage(const QImage& image, int maxSize, QImage& resized) const;

    /**
     * Adds an image to the simpleviewer config file
     *
     * @param xmlDoc main XML document
     * @param galleryElem gallery XML element
     * @param url path to original image
     * @param newName new image file name used by gallery
     */
    void cfgAddImage(QDomDocument& xmlDoc, QDomElement& galleryElem,
                     const KUrl& url, const QString& newName) const;

    /**
     * Creates the index.html file
     */
    bool createIndex() const;

    /**
     * Copies simpleviewers files into the export directory
     */
    bool copySimpleViewer() const;

    bool upload() const;

    bool openArchive(KZip& zip) const;

    bool extractArchive(KZip& zip) const;

    bool extractFile(const KArchiveEntry* entry) const;

    static bool cmpUrl(const KUrl& url1, const KUrl& url2);

Q_SIGNALS:

    void signalProcessingDone();

public Q_SLOTS:

    void slotProcess();
    void slotCancel();

private:

    class SimpleViewerPriv;
    SimpleViewerPriv* const d;
};

} // namespace KIPIFlashExportPlugin

#endif /* SIMPLEVIEWER_H */
