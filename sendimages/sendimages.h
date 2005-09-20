/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 *         from digiKam project.
 * Date  : 2004-02-25
 * Description : a kipi plugin for e-mailing images
 * 
 * Copyright 2004-2005 by Gilles Caulier
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

#ifndef SENDIMAGES_H
#define SENDIMAGES_H

// Include files for Qt.

#include <qobject.h>
#include <qstring.h>
#include <qthread.h>
#include <qstringlist.h>

// Include files for KDE.

#include <kurl.h>

// Includes file for libKIPI.

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

class QFile;
class QTimer;
class QCustomEvent;

class KProcess;

namespace KIPISendimagesPlugin
{

class SendImagesDialog;

class SendImages : public QObject, public QThread
{
Q_OBJECT

public:

   SendImages(KIPI::Interface* interface, const QString &tmpFolder, 
              const KIPI::ImageCollection& imagesCollection, QObject *parent=0);
               
   ~SendImages();

   virtual void run();

   void showDialog(void);
   void prepare(void);
   bool showErrors(void);
   void makeCommentsFile(void);
   void invokeMailAgent(void);
   void removeTmpFiles(void);

private slots:   

   void slotMozillaTimeout(void);
   void slotMozillaExited(KProcess* proc);
   void slotMozillaReadStderr(KProcess* proc, char *buffer, int buflen);
   
protected:

   KIPI::Interface       *m_interface;

   SendImagesDialog      *m_sendImagesDialog;
   
   QObject               *m_parent;
   
   KProcess              *m_mailAgentProc;
   KProcess              *m_mailAgentProc2;
   KProcess              *m_mailAgentProc3;
   
   QTimer                *m_mozillaTimer;

   QString                m_mozillaStdErr;
   QString                m_tmp;

   bool                   m_changeProp;                // Change image properties options in setup dialog.
   
   QString                m_imageFormat;               // Image format option in the setup dialog.
   QString                m_thunderbirdUrl;
   
   int                    m_sizeFactor;                // Image size factor in the setup dialog.
   int                    m_imageCompression;          // Image compression factor in the setup dialog.
   
   KURL::List             m_images;                    // List of images in the setup dialog.
   KURL::List             m_filesSendList;             // KURL of resized images.
   KURL::List             m_imagesResizedWithError;    // KURL of orignal images that cannot be resized.
   KURL::List             m_imagesPackage;             // KURL of orignal images than have been resized 
                                                       // and KURL of resized images (used for to create 
                                                       // the comments file).

   KIPI::ImageCollection  m_collection;
   
protected:
   
   QString extension(const QString& imageFileFormat);
   int getSize( int choice );
   
   bool DeleteDir(QString dirname);
   bool deldir(QString dirname);
   
   bool resizeImageProcess( const QString &SourcePath, const QString &DestPath,
                            const QString &ImageFormat, const QString &ImageName,
                            int SizeFactor, int ImageCompression);
};

}  // NameSpace KIPISendimagesPlugin

#endif  // SENDIMAGES_H

