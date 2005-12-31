/* ============================================================
 * File  : simpleviewerexport.h
 * Author: Joern Ahrens <joern.ahrens@kdemail.net>
 * Date  : 2005-12-19
 * Description :
 *
 * Copyright 2005 by Joern Ahrens
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef SIMPLEVIEWEREXPORT_H
#define SIMPLEVIEWEREXPORT_H

// QT includes

#include <qobject.h>

// KIPI includes

#include <libkipi/interface.h>

namespace KIPI
{
    class BatchProgressDialog;
}

class QTimer;

namespace KIPISimpleViewerExportPlugin
{
class SVEDialog;

class SimpleViewerExport : public QObject
{
    Q_OBJECT

public:

    static void run( KIPI::Interface* interface, QObject *parent=0 );
 
private:

    SimpleViewerExport( KIPI::Interface* interface, QObject *parent=0 );
    ~SimpleViewerExport();

    void showDialog();
    void startExport();

    /**
     * Creates the standard simpleviewer directories
     *
     * @return true=ok
     */
    bool createExportDirectories();

    /**
     * Creates the images and thumbnails
     *
     * @return true=ok
     */
    bool exportImages();

    /**
     * Creates a simpleviewer thumbnail from images
     *
     * @param image the original images
     * @param thumbnail the created thumbnail
     *
     * @return true=ok
     */
    bool createThumbnail(const QImage &image, QImage &thumbnail);

    /**
     * Resizes the image for the gallery
     *
     * @param image the original images
     * @param thumbnail the created thumbnail
     *
     * @return true=ok
     */
    bool resizeImage(const QImage &image, int maxSize, QImage &resized);

    /**
     * Creates the header of the simpleviewer config file
     */
    void cfgCreateHeader(QTextStream &ts);

    /**
     * Adds an image to the simpleviewer config file
     *
     * @param kurl path to the images
     */
    void cfgAddImage(QTextStream &ts, const KURL &kurl);

    /**
     * Finishes the simpleviewer config file
     */
    void cfgCreateFooter(QTextStream &ts);
    
public slots:
    
    void slotProcess();
    void slotCancel();
    
private:
    
    SVEDialog                           *m_configDlg;
    KIPI::Interface                     *m_interface;
    KIPI::BatchProgressDialog           *m_progressDlg;
    QValueList<KIPI::ImageCollection>    m_albumsList;
    QTimer                              *m_timer;
    int                                  m_totalActions;
    bool                                 m_canceled;
};

}

#endif /* SIMPLEVIEWEREXPORT_H */

