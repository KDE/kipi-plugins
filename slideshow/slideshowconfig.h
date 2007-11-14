/* ============================================================
 * File  : slideshowconfig.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-17
 * Description : Digikam slideshow plugin.
 * 
 * Copyright 2003-2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright 2007 by Valerio Fuoglio <valerio.fuoglio@gmail.com>
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

#ifndef SLIDESHOWCONFIG_H
#define SLIDESHOWCONFIG_H

// Qt includes.

#include <qstring.h>
#include <qlistbox.h>

// KDE includes

#include <kconfig.h>
#include <kio/previewjob.h>
#include <kurl.h>

// libkipi includes

#include <libkipi/imagedialog.h>
#include <libkipi/interface.h>

// SlideShow includes

#include "slideshowconfigbase.h"


namespace KIPISlideShowPlugin
{
    
class SlideShowConfig : public SlideShowConfigBase
{
    Q_OBJECT

public:

    SlideShowConfig(bool allowSelectedOnly, KIPI::Interface* interface,
                    QWidget *parent, const char* name, bool ImagesHasComments,
                    KURL::List* urlList);
    ~SlideShowConfig();

private:

    void loadEffectNames();
    void loadEffectNamesGL();
    void readSettings();
    void saveSettings();
    
    void ShowNumberImages( int Number );
    void addItems(const KURL::List& fileList);

    int m_delayMsMaxValue;
    int m_delayMsMinValue;
    int m_delayMsLineStep;
    
    uint m_cacheSize;
    
    KConfig*    m_config;
    
    QString     m_effectName;
    QString     m_effectNameGL;
    
    KIO::PreviewJob*      m_thumbJob;
    KURL::List*           m_urlList;
    
    KIPI::Interface*      m_interface;
    
    
private slots:

    void slotStartClicked();
    void slotHelp();
    void slotOpenGLToggled();
    void slotEffectChanged();
    void slotDelayChanged();
    void slotUseMillisecondsToggled();
    void slotPrintCommentsToggled();
    void slotCommentsFontColorChanged();
    void slotCommentsBgColorChanged();
    
    void slotSelection();
    void slotCacheToggled();
   
    void SlotPortfolioDurationChanged ( int );
    void slotImagesFilesSelected( QListBoxItem *item );
    void slotAddDropItems(KURL::List filesUrl);
    void slotImagesFilesButtonAdd( void );
    void slotImagesFilesButtonDelete( void );
    void slotImagesFilesButtonUp( void );
    void slotImagesFilesButtonDown( void );
    void slotGotPreview(const KFileItem* , const QPixmap &pixmap);
    void slotFailedPreview(const KFileItem*);

signals:
    void buttonStartClicked(); // Signal needed by plugin_slideshow class

};

}  // NameSpace KIPISlideShowPlugin

#endif
