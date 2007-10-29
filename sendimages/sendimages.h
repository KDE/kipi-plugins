/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-02-25
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef SENDIMAGES_H
#define SENDIMAGES_H

// Qt includes.

#include <qobject.h>
#include <qstring.h>
#include <qthread.h>
#include <qstringlist.h>

// KDE includes.

#include <kurl.h>

// LibKipi includes.

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

    ///Constructor: saves system handoff parameters in member variables
    SendImages(KIPI::Interface* interface, const QString &tmpFolder, 
                const KIPI::ImageCollection& imagesCollection, QObject *parent=0);
    ///Destructor
    ~SendImages();

    ///Prepares the image list. This includes resizing, copying, maintaining an image's exif, 
    ///and dropping evil characters out of filenames ;-)
    virtual void run();

    ///Invokes the User Dialog Window
    void showDialog(void);

    ///Gets input from the user dialog and store it into member variables
    void prepare(void);

    ///Shows up an error dialog and the problematic images
    bool showErrors(void);

    /// Creates a text file with the images comments.
    void makeCommentsFile(void);

    /** Invokes mail agent. Depending on which mail agent to be used, we have different
    proceedings. Easy for every agent except of mozilla derivates */
    bool invokeMailAgent(void);

    ///Cleans up the temp directory
    void removeTmpFiles(void);

    /** Returns a list of Filenames, whose sum filesize is smaller than the quota
    The returned images are deleted from the m_filesSendList*/
    KURL::List divideEmails(void);
	   
private slots:   

    ///If Mozilla wasn't started before, now it is and so we can begin with the transmission
    void slotMozillaTimeout(void);

    ///If mozilla, or thunderbird or any derivate isn't already running, 
    ///start it now, wait 5 seconds and start SlotMozillaTimeout()
    void slotMozillaExited(KProcess* proc);

    ///Handles mozillas errors
    void slotMozillaReadStderr(KProcess* proc, char *buffer, int buflen);

private:

    ///Returns the file-extension of the corresponding fileformat
    QString extension(const QString& imageFileFormat);

    ///in sendimagesplugin dialog the user can select a compression of images
    ///this function returns the pixel-size of the selected entry
    int getSize( int choice );

    ///Checks if directory is empty and invokes its deletion
    bool DeleteDir(QString dirname);

    ///Deletes a directory and all its contents - Please call it using "DeleteDir"
    bool deldir(QString dirname);

    ///Resizes the Images before Sending...
    bool resizeImageProcess(const QString &SourcePath, const QString &DestPath,
                            const QString &ImageFormat, const QString &ImageName,
                            int SizeFactor, int ImageCompression, QSize &newsize);

    ///This function should copy the images to tempfolder in order to avoid suspicious filenames
    ///It is used, when no resizing should take place
    bool copyImageProcess(const QString &oldFilePath, const QString &DestPath,
                                  const QString &ImageName);
    
    ///Tests if an entry already exists in the filename list
    bool entry_already_exists(KURL::List filenamelist,QString entry);

    ///Makes a deep copy of a KURL-list: Real and slow copying instead of only pointer arithmetics
    bool kurllistdeepcopy(KURL::List &Destination, KURL::List Source);

private:

    bool m_invokedBefore;
    /** Change image properties options in setup dialog.*/

    bool                   m_changeProp;                
    
    /** Image size factor in the setup dialog.*/
    int                    m_sizeFactor;                

    /** Image compression factor in the setup dialog.*/
    int                    m_imageCompression;          

    /** Biggest permitted email.*/
    unsigned long          m_attachmentlimit;
    
    QObject               *m_parent;
    
    QTimer                *m_mozillaTimer;
    
    QString                m_mozillaStdErr;
    QString                m_tmp;
    
    /** Image format option in the setup dialog.*/
    QString                m_imageFormat;               
    QString                m_thunderbirdUrl;
    
    KProcess              *m_mailAgentProc;
    KProcess              *m_mailAgentProc2;
    KProcess              *m_mailAgentProc3;

    /** List of images in the setup dialog.*/
    KURL::List             m_images;       
             
    /** URL of resized images.*/
    KURL::List             m_filesSendList;
    KURL::List             m_filesSendList_copy;

    /** URL of original images that cannot be resized.*/
    KURL::List             m_imagesResizedWithError;    

    /** URL of original images than have been resized 
        and KURL of resized images (used for to create 
        the comments file).*/
    KURL::List             m_imagesPackage;             

    KIPI::ImageCollection  m_collection;
    
    KIPI::Interface       *m_interface;
    
    SendImagesDialog      *m_sendImagesDialog;
};

}  // NameSpace KIPISendimagesPlugin

#endif  // SENDIMAGES_H
