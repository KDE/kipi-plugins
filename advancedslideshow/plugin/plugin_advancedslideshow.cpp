/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-01-31
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2006-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
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

#include "plugin_advancedslideshow.moc"

// C ANSI includes

extern "C"
{
#include <sys/time.h>
}

// C++ includes

#include <cstdlib>

// Qt includes

#include <QList>
#include <QPair>
#include <QStringList>

// KDE includes

#include <klocale.h>
#include <kaction.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <kactioncollection.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "slideshowconfig.h"
#include "slideshow.h"
#include "slideshowgl.h"
#include "slideshowkb.h"
#include "commoncontainer.h"
#include "kpimageinfo.h"

namespace KIPIAdvancedSlideshowPlugin
{

K_PLUGIN_FACTORY( AdvancedSlideshowFactory, registerPlugin<Plugin_AdvancedSlideshow>(); )
K_EXPORT_PLUGIN ( AdvancedSlideshowFactory("kipiplugin_advancedslideshow") )

Plugin_AdvancedSlideshow::Plugin_AdvancedSlideshow(QObject* const parent, const QVariantList &/*args*/)
    : KIPI::Plugin(AdvancedSlideshowFactory::componentData(),
                   parent, "AdvancedSlideshow")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_AdvancedSlideshow plugin loaded" ;
    m_actionSlideShow = 0;
    m_sharedData      = 0;
    m_interface       = 0;

    setUiBaseName("kipiplugin_advancedslideshowui.rc");
    setupXML();
}

Plugin_AdvancedSlideshow::~Plugin_AdvancedSlideshow()
{
}

void Plugin_AdvancedSlideshow::setup(QWidget* const widget)
{
    KIPI::Plugin::setup(widget);
    setupActions();

    m_interface = interface();

    if (!m_interface)
    {
        kError() << "KIPI interface is null!";
        return;
    }

    m_urlList = KUrl::List();

    connect(m_interface, SIGNAL(currentAlbumChanged(bool)),
            this, SLOT(slotAlbumChanged(bool)));

    slotAlbumChanged(m_interface->currentAlbum().isValid());
}

void Plugin_AdvancedSlideshow::setupActions()
{
    setDefaultCategory(ToolsPlugin);

    m_actionSlideShow = new KAction(this);
    m_actionSlideShow->setText(i18n("Advanced Slideshow..."));
    m_actionSlideShow->setIcon(KIcon("kipi-slideshow"));
    m_actionSlideShow->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_F9));
    m_actionSlideShow->setEnabled(false);

    connect(m_actionSlideShow, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("advancedslideshow", m_actionSlideShow);
}

void Plugin_AdvancedSlideshow::slotActivate()
{
    if (!interface())
    {
        kError() << "Kipi m_interface is null!";
        return;
    }

    m_sharedData = new SharedContainer();

    m_sharedData->setIface(m_interface);
    m_sharedData->showSelectedFilesOnly = true;
    m_sharedData->ImagesHasComments     = m_interface->hasFeature(KIPI::ImagesHasComments);
    m_sharedData->urlList               = m_urlList;
    KIPI::ImageCollection currSel       = m_interface->currentSelection();

    if (!currSel.isValid() || currSel.images().count() <= 1)
    {
        m_sharedData->showSelectedFilesOnly = false;
    }

    SlideShowConfig* const slideShowConfig = new SlideShowConfig(kapp->activeWindow(), m_sharedData);

    connect(slideShowConfig, SIGNAL(buttonStartClicked()),
            this, SLOT(slotSlideShow()));

    slideShowConfig->show();
}

void Plugin_AdvancedSlideshow::slotAlbumChanged(bool anyAlbum)
{
    if (!anyAlbum)
    {
        m_actionSlideShow->setEnabled(false);
        return;
    }

    KIPI::Interface* const m_interface = dynamic_cast<KIPI::Interface*> (parent());

    if (!m_interface)
    {
        kError() << "Kipi m_interface is null!";
        m_actionSlideShow->setEnabled(false);
        return;
    }

    KIPI::ImageCollection currAlbum = m_interface->currentAlbum();

    if (!currAlbum.isValid())
    {
        kError() << "Current image collection is not valid.";
        m_actionSlideShow->setEnabled(false);
        return;
    }

    m_actionSlideShow->setEnabled(true);
}

void Plugin_AdvancedSlideshow::slotSlideShow()
{
    if (!m_interface)
    {
        kError() << "Kipi m_interface is null!";
        return;
    }

    KConfig config("kipirc");
    KConfigGroup grp = config.group("Advanced Slideshow Settings");
    bool opengl      = grp.readEntry("OpenGL",  false);
    bool shuffle     = grp.readEntry("Shuffle", false);
    bool wantKB      = grp.readEntry("Effect Name (OpenGL)") == QString("Ken Burns");
    m_urlList        = m_sharedData->urlList;

    if (m_urlList.isEmpty())
    {
        KMessageBox::sorry(kapp->activeWindow(), i18n("There are no images to show."));
        return;
    }

    typedef QPair<QString, int>  FileAnglePair;
    typedef QList<FileAnglePair> FileList;
    FileList                     fileList;
    QStringList                  commentsList;

    for (KUrl::List::ConstIterator urlIt = m_urlList.constBegin(); urlIt != m_urlList.constEnd(); ++urlIt)
    {
        fileList.append(FileAnglePair((*urlIt).toLocalFile(), 0));
        commentsList.append(QString());
/*
        KIPIPlugins::KPImageInfo info(m_interface, *urlIt);
        fileList.append(FileAnglePair((*urlIt).toLocalFile(), info.orientation()));
        commentsList.append(info.description());
*/
    }

    m_urlList.clear();

    if (shuffle)
    {
        struct timeval tv;
        gettimeofday(&tv, 0);
        srand(tv.tv_sec);

        FileList::iterator it       = fileList.begin();
        FileList::iterator it1;

        QStringList::iterator itcom = commentsList.begin();
        QStringList::iterator itcom1;

        for (uint i = 0; i < (uint) fileList.size(); ++i)
        {
            int inc = (int) (float(fileList.count()) * qrand() / (RAND_MAX + 1.0));

            it1  = fileList.begin();
            it1 += inc;

            itcom1  = commentsList.begin();
            itcom1 += inc;

            qSwap(*(it++), *(it1));
            qSwap(*(itcom++), *(itcom1));
        }
    }

    if (!opengl)
    {
        SlideShow* const slideShow = new SlideShow(fileList, commentsList, m_sharedData);
        slideShow->show();
    }
    else
    {
        if (!QGLFormat::hasOpenGL())
        {
            KMessageBox::error(kapp->activeWindow(),
                               i18n("OpenGL support is not available on your system."));
        }
        else
        {
            if (wantKB)
            {
                SlideShowKB* const slideShow = new SlideShowKB(fileList, commentsList, m_sharedData);
                slideShow->show();
            }
            else
            {
                SlideShowGL* const slideShow = new SlideShowGL(fileList, commentsList, m_sharedData);
                slideShow->show();
            }
        }
    }
}

}  // namespace KIPIAdvancedSlideshowPlugin
