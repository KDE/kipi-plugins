/* ============================================================
 * File  : plugin_slideshow.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-01-31
 * Description :
 *
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <digikam/albummanager.h>
#include <digikam/albuminfo.h>

#include "plugin_slideshow.h"
#include "slideshow.h"
#include "slideshowgl.h"
#include "slideshowconfig.h"

K_EXPORT_COMPONENT_FACTORY( kipiplugin_slideshow,
                            KGenericFactory<Plugin_SlideShow>("kipiplugin_slideshow"));

Plugin_SlideShow::Plugin_SlideShow(QObject *parent,
                                   const char*,
                                   const QStringList&)
    : KIPI::Plugin(parent, "SlideShow")
{
    KGlobal::locale()->insertCatalogue("kipiplugin_slideshow");

    kdDebug() << "Plugin_SlideShow plugin loaded"
              << endl;

    action = new KAction (i18n("SlideShow..."),
                          "slideshow",
                          0,
                          this,
                          SLOT(slotActivate()),
                          actionCollection(),
                          "slideshow");
    action->setEnabled(false);

    connect(Digikam::AlbumManager::instance(),
            SIGNAL(signalAlbumCurrentChanged(Digikam::AlbumInfo*)),
            SLOT(slotAlbumChanged(Digikam::AlbumInfo*)));
}


Plugin_SlideShow::~Plugin_SlideShow()
{
}


void Plugin_SlideShow::slotActivate()
{
    SlideShowConfig *slideShowConfig = new SlideShowConfig;
    connect(slideShowConfig, SIGNAL(okClicked()),
            this, SLOT(slotSlideShow()));
    slideShowConfig->show();
}



void Plugin_SlideShow::slotSlideShow()
{
    Digikam::AlbumInfo *album =
        Digikam::AlbumManager::instance()->currentAlbum();

    if (!album) return;

    KConfig config("digikamrc");

    bool    opengl;
    int     delay;
    bool    printFileName;
    bool    loop;
    bool    showSelectedFilesOnly;
    QString effectName;

    config.setGroup("SlideShow Settings");
    opengl                = config.readBoolEntry("OpenGL");
    delay                 = config.readNumEntry("Delay");
    printFileName         = config.readBoolEntry("Print Filename");
    loop                  = config.readBoolEntry("Loop");
    showSelectedFilesOnly = config.readBoolEntry("Show Selected Files Only");
    if (!opengl)
        effectName        = config.readEntry("Effect Name");
    else
        effectName        = config.readEntry("Effect Name (OpenGL)");

    QStringList fileList;

    if (showSelectedFilesOnly)
        fileList = album->getSelectedItemsPath();
    else
        fileList = album->getAllItemsPath();

    if (fileList.empty())
        return;

    if (!opengl) {
        SlideShow *slideShow =
            new SlideShow(fileList, delay, printFileName,
                          loop, effectName);
        slideShow->show();
    }
    else {
        if (!QGLFormat::hasOpenGL())
            KMessageBox::error(0, i18n("Sorry. OpenGL support not available on your system"));
        else {
            SlideShowGL *slideShow =
                new SlideShowGL(fileList, delay, loop, effectName);
            slideShow->show();
        }
    }
}

void Plugin_SlideShow::slotAlbumChanged(Digikam::AlbumInfo* album)
{
    if (!album)
        action->setEnabled(false);
    else
        action->setEnabled(true);
}

KIPI::Category Plugin_SlideShow::category() const
{
    return KIPI::TOOLSPLUGIN;
}
