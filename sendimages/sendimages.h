//////////////////////////////////////////////////////////////////////////////
//
//    SENDIMAGES.H
//
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////


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

   bool showDialog();
   bool showErrors();
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

   KURL::List             m_filesSendList;
   KURL::List             m_imagesResizedWithError;

   KIPI::ImageCollection  m_collection;
   
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

