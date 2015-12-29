/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-09
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2012-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "slideshowconfig.h"

// Qt includes

#include <QFileInfo>
#include <QFont>
#include <QIcon>
#include <QMenu>
#include <QTabWidget>

// KDE includes

#include <kconfig.h>
#include <kconfiggroup.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/ImageCollection>

// local includes

#include "kipiplugins_debug.h"
#include "kpaboutdata.h"
#include "commoncontainer.h"
#include "maindialog.h"
#include "captiondialog.h"
#include "soundtrackdialog.h"
#include "advanceddialog.h"

namespace KIPIAdvancedSlideshowPlugin
{

class SlideShowConfig::Private
{

public:

    Private()
    {
        tab        = 0;
        sharedData = 0;
        config     = 0;
    }

    QTabWidget*      tab;
    SharedContainer* sharedData;
    KConfig*         config;
};

SlideShowConfig::SlideShowConfig(QWidget* const parent, SharedContainer* const sharedData)
    : KPToolDialog(parent),
      d(new Private)
{
    setObjectName(QStringLiteral("Advanced Slideshow Settings"));
    setWindowTitle(i18n("Advanced Slideshow"));

    d->config     = new KConfig(QStringLiteral("kipirc"));
    d->sharedData = sharedData;

    startButton()->setDefault(true);
    startButton()->setIcon(QIcon::fromTheme(QStringLiteral("system-run")));
    startButton()->setText(i18n("Start Slideshow"));
    setModal(true);

    // --- Pages settings ---

    d->tab = new QTabWidget(this);
    
    d->sharedData->mainPage  = new MainDialog(this, d->sharedData);
    d->tab->addTab(d->sharedData->mainPage,
                   QIcon::fromTheme(QStringLiteral("view-presentation")),
                   i18n("Main Settings"));

    d->sharedData->captionPage  = new CaptionDialog(this, d->sharedData);
    d->tab->addTab(d->sharedData->captionPage,
                   QIcon::fromTheme(QStringLiteral("draw-freehand")),
                   i18nc("captions for the slideshow", "Caption"));

#ifdef HAVE_PHONON
    d->sharedData->soundtrackPage  = new SoundtrackDialog(this, d->sharedData);
    d->tab->addTab(d->sharedData->soundtrackPage,
                   QIcon::fromTheme(QStringLiteral("speaker")),
                   i18n("Soundtrack"));
#endif
    
    d->sharedData->advancedPage  = new AdvancedDialog(this, d->sharedData);
    d->tab->addTab(d->sharedData->advancedPage,
                   QIcon::fromTheme(QStringLiteral("configure")),
                   i18n("Advanced"));

    setMainWidget(d->tab);

    // --- About --

    KPAboutData* const about = new KPAboutData(ki18n("Advanced Slideshow"),
                                               0,
                                               KAboutLicense::GPL,
                                               ki18n("A Kipi plugin to slide images with special effects"),
                                               ki18n("(c) 2003-2004, Renchi Raju\n"
                                                     "(c) 2006-2009, Valerio Fuoglio"));

    about->addAuthor(ki18n("Renchi Raju").toString(),
                     ki18n("Developer").toString(),
                     QStringLiteral("renchi dot raju at gmail dot com"));

    about->addAuthor(ki18n("Valerio Fuoglio").toString(),
                     ki18n("Developer").toString(),
                     QStringLiteral("valerio dot fuoglio at gmail dot com"));

    about->setHandbookEntry(QStringLiteral("slideshow"));
    setAboutData(about);

    // Slot connections

    connect(startButton(), &QPushButton::clicked,
            this, &SlideShowConfig::slotStartClicked);

    readSettings();
}

SlideShowConfig::~SlideShowConfig ()
{
    delete d->config;
    delete d;
}

void SlideShowConfig::readSettings()
{
    KConfigGroup grp = d->config->group(objectName());

    d->sharedData->opengl            = grp.readEntry("OpenGL", false);
    d->sharedData->openGlFullScale   = grp.readEntry("OpenGLFullScale", false);
    d->sharedData->delay             = grp.readEntry("Delay", 1500);
    d->sharedData->printFileName     = grp.readEntry("Print Filename", true);
    d->sharedData->printProgress     = grp.readEntry("Print Progress Indicator", true);
    d->sharedData->printFileComments = grp.readEntry("Print Comments", false);
    d->sharedData->loop              = grp.readEntry("Loop", false);
    d->sharedData->shuffle           = grp.readEntry("Shuffle", false);
    d->sharedData->effectName        = grp.readEntry("Effect Name", "Random");
    d->sharedData->effectNameGL      = grp.readEntry("Effect Name (OpenGL)", "Random");

    d->sharedData->delayMsMaxValue   = 120000;
    d->sharedData->delayMsMinValue   = 100;
    d->sharedData->delayMsLineStep   = 100;

    // Comments tab settings
    QFont *savedFont = new QFont();
    savedFont->setFamily(    grp.readEntry("Comments Font Family"));
    savedFont->setPointSize( grp.readEntry("Comments Font Size", 10 ));
    savedFont->setBold(      grp.readEntry("Comments Font Bold", false));
    savedFont->setItalic(    grp.readEntry("Comments Font Italic", false));
    savedFont->setUnderline( grp.readEntry("Comments Font Underline", false));
    savedFont->setOverline(  grp.readEntry("Comments Font Overline", false));
    savedFont->setStrikeOut( grp.readEntry("Comments Font StrikeOut", false));
    savedFont->setFixedPitch(grp.readEntry("Comments Font FixedPitch", false));

    d->sharedData->captionFont = savedFont;

    d->sharedData->commentsFontColor   = grp.readEntry("Comments Font Color", 0xffffff);
    d->sharedData->commentsBgColor     = grp.readEntry("Comments Bg Color", 0x000000);
    d->sharedData->commentsDrawOutline = grp.readEntry("Comments Text Outline", true);
    d->sharedData->bgOpacity           = grp.readEntry("Background Opacity", 10);

    d->sharedData->commentsLinesLength = grp.readEntry("Comments Lines Length", 72);

#ifdef HAVE_PHONON
    // Soundtrack tab
    d->sharedData->soundtrackLoop             = grp.readEntry("Soundtrack Loop", false);
    d->sharedData->soundtrackPath             = QUrl(grp.readEntry("Soundtrack Path", "" ));
    d->sharedData->soundtrackRememberPlaylist = grp.readEntry("Soundtrack Remember Playlist", false);
#endif

    // Advanced tab
    d->sharedData->useMilliseconds     = grp.readEntry("Use Milliseconds", false);
    d->sharedData->enableMouseWheel    = grp.readEntry("Enable Mouse Wheel", true);

    d->sharedData->kbDisableFadeInOut  = grp.readEntry("KB Disable FadeInOut", false);
    d->sharedData->kbDisableCrossFade  = grp.readEntry("KB Disable Crossfade", false);

    d->sharedData->enableCache         = grp.readEntry("Enable Cache", false);
    d->sharedData->cacheSize           = grp.readEntry("Cache Size", 5);

    if (d->sharedData->soundtrackRememberPlaylist)
    {
        QString groupName(objectName() + QStringLiteral(" Soundtrack ") + d->sharedData->iface()->currentAlbum().url().toLocalFile());
        KConfigGroup soundGrp = d->config->group(groupName);

        // load and check playlist files, if valid, add to tracklist widget
        QList<QUrl> playlistFiles = soundGrp.readEntry("Tracks", QList<QUrl>());

        foreach(const QUrl& playlistFile, playlistFiles)
        {
            QUrl file(playlistFile);
            QFileInfo fi(file.toLocalFile());
            if (fi.isFile())
            {
                d->sharedData->soundtrackUrls << file;
            }
        }
    }

    d->sharedData->mainPage->readSettings();
    d->sharedData->captionPage->readSettings();
    d->sharedData->advancedPage->readSettings();

#ifdef HAVE_PHONON
    d->sharedData->soundtrackPage->readSettings();
#endif
}

void SlideShowConfig::saveSettings()
{
    if (!d->config) return;

    d->sharedData->mainPage->saveSettings();
    d->sharedData->captionPage->saveSettings();
    d->sharedData->advancedPage->saveSettings();

#ifdef HAVE_PHONON
    d->sharedData->soundtrackPage->saveSettings();
#endif

    KConfigGroup grp = d->config->group(objectName());
    grp.writeEntry("OpenGL",                   d->sharedData->opengl);
    grp.writeEntry("OpenGLFullScale",          d->sharedData->openGlFullScale);
    grp.writeEntry("Delay",                    d->sharedData->delay);
    grp.writeEntry("Print Filename",           d->sharedData->printFileName);
    grp.writeEntry("Print Progress Indicator", d->sharedData->printProgress);
    grp.writeEntry("Print Comments",           d->sharedData->printFileComments);
    grp.writeEntry("Loop",                     d->sharedData->loop);
    grp.writeEntry("Shuffle",                  d->sharedData->shuffle);
    grp.writeEntry("Use Milliseconds",         d->sharedData->useMilliseconds);
    grp.writeEntry("Enable Mouse Wheel",       d->sharedData->enableMouseWheel);

    // Comments tab settings
    QFont* commentsFont = d->sharedData->captionFont;
    grp.writeEntry("Comments Font Family",     commentsFont->family());
    grp.writeEntry("Comments Font Size",       commentsFont->pointSize());
    grp.writeEntry("Comments Font Bold",       commentsFont->bold());
    grp.writeEntry("Comments Font Italic",     commentsFont->italic());
    grp.writeEntry("Comments Font Underline",  commentsFont->underline());
    grp.writeEntry("Comments Font Overline",   commentsFont->overline());
    grp.writeEntry("Comments Font StrikeOut",  commentsFont->strikeOut());
    grp.writeEntry("Comments Font FixedPitch", commentsFont->fixedPitch());
    grp.writeEntry("Comments Font Color",      d->sharedData->commentsFontColor);
    grp.writeEntry("Comments Bg Color",        d->sharedData->commentsBgColor);
    grp.writeEntry("Comments Text Outline",    d->sharedData->commentsDrawOutline);
    grp.writeEntry("Background Opacity",       d->sharedData->bgOpacity);
    grp.writeEntry("Comments Lines Length",    d->sharedData->commentsLinesLength);
    grp.writeEntry("Effect Name (OpenGL)",     d->sharedData->effectNameGL);
    grp.writeEntry("Effect Name",              d->sharedData->effectName);

#ifdef HAVE_PHONON
    // Soundtrack tab
    grp.writeEntry("Soundtrack Loop",              d->sharedData->soundtrackLoop);
    grp.writeEntry("Soundtrack Path",              d->sharedData->soundtrackPath.toLocalFile());
    grp.writeEntry("Soundtrack Remember Playlist", d->sharedData->soundtrackRememberPlaylist);
#endif

    // Advanced settings
    grp.writeEntry("KB Disable FadeInOut", d->sharedData->kbDisableFadeInOut);
    grp.writeEntry("KB Disable Crossfade", d->sharedData->kbDisableCrossFade);
    grp.writeEntry("Enable Cache",         d->sharedData->enableCache);
    grp.writeEntry("Cache Size",           d->sharedData->cacheSize);

    // --------------------------------------------------------

    // only save tracks when option is set and tracklist is NOT empty, to prevent deletion
    // of older track entries
    if (d->sharedData->soundtrackRememberPlaylist && d->sharedData->soundtrackPlayListNeedsUpdate)
    {
        QString groupName(objectName() + QStringLiteral(" Soundtrack ") + d->sharedData->iface()->currentAlbum().url().toLocalFile());
        KConfigGroup soundGrp = d->config->group(groupName);
        soundGrp.writeEntry("Tracks", d->sharedData->soundtrackUrls);
    }

    d->config->sync();
}

void SlideShowConfig::slotStartClicked()
{
    saveSettings();

    if ( d->sharedData->mainPage->updateUrlList() )
        emit buttonStartClicked();

    return;
}

void SlideShowConfig::closeEvent(QCloseEvent* e)
{
    saveSettings();
    e->accept();
}

}  // namespace KIPIAdvancedSlideshowPlugin
