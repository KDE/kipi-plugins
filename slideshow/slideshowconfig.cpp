/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-09
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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
#include "slideshowconfig.moc"

// Qt includes.

#include <QFont>

// KDE includes.

#include <ktoolinvocation.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kicon.h>
#include <khelpmenu.h>
#include <kpushbutton.h>
#include <kmenu.h>
#include <kdebug.h>

// LibKIPI includes.

#include "kpaboutdata.h"

namespace KIPISlideShowPlugin
{

class SlideShowConfigPrivate
{

public:

    SlideShowConfigPrivate() {}

    ~SlideShowConfigPrivate()
    {
//        if (config) delete config;
    }

    KIPIPlugins::KPAboutData* about;
    SharedData*               sharedData;
    KConfig*                  config;

};


SlideShowConfig::SlideShowConfig (QWidget *parent, SharedData* sharedData)
        : KPageDialog(parent)
{
    setWindowTitle(i18n("SlideShow"));

    d = new SlideShowConfigPrivate();
    d->config = new KConfig("kipirc");

    d->sharedData = sharedData;

    setButtons(Help | Close | User1);
    setDefaultButton(User1);
    setButtonIcon(User1, KIcon("system-run"));
    setButtonText(User1, i18n("Start Slideshow"));
    setFaceType(List);
    setModal(true);

    // --- Pages settings ---

    d->sharedData->mainPage   = new MainDialog(this, d->sharedData);
    d->sharedData->page_main  = addPage(d->sharedData->mainPage, i18n("Main"));
    d->sharedData->page_main->setHeader(i18n("Main Settings"));
    d->sharedData->page_main->setIcon(KIcon("view-presentation"));

    d->sharedData->captionPage   = new CaptionDialog(this, d->sharedData);
    d->sharedData->page_caption  = addPage(d->sharedData->captionPage, i18n("Caption"));
    d->sharedData->page_caption->setHeader(i18n("Caption"));
    d->sharedData->page_caption->setIcon(KIcon("draw-freehand"));

    d->sharedData->soundtrackPage   = new SoundtrackDialog(this, d->sharedData);
    d->sharedData->page_soundtrack  = addPage(d->sharedData->soundtrackPage, i18n("Soundtrack"));
    d->sharedData->page_soundtrack->setHeader(i18n("Soundtrack"));
    d->sharedData->page_soundtrack->setIcon(KIcon("speaker"));

    d->sharedData->advancedPage   = new AdvancedDialog(this, d->sharedData);
    d->sharedData->page_advanced  = addPage(d->sharedData->advancedPage, i18n("Advanced"));
    d->sharedData->page_advanced->setHeader(i18n("Advanced"));
    d->sharedData->page_advanced->setIcon(KIcon("configure"));

    // --- About --

    d->about = new KIPIPlugins::KPAboutData(ki18n("SlideShow"),
                                            0,
                                            KAboutData::License_GPL,
                                            ki18n("A Kipi plugin for image slideshow"),
                                            ki18n("(c) 2003-2004, Renchi Raju\n(c) 2006-2008, Valerio Fuoglio"));

    d->about->addAuthor(ki18n( "Renchi Raju" ), ki18n("Author"),
                        "renchi@pooh.tam.uiuc.edu");
    d->about->addAuthor(ki18n( "Valerio Fuoglio" ), ki18n("Author and maintainer"),
                        "valerio.fuoglio@gmail.com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KPushButton *helpButton = button( Help );
    KHelpMenu* helpMenu     = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook       = new QAction(i18n("SlideShow Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)), this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    helpButton->setDelayedMenu( helpMenu->menu() );


    // Slot connections

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotStartClicked()));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    readSettings();

}

SlideShowConfig::~SlideShowConfig ()
{
    delete d;
}

void SlideShowConfig::readSettings()
{
    KConfigGroup grp = d->config->group("SlideShow Settings");

    d->sharedData->opengl                = grp.readEntry("OpenGL", false);
    d->sharedData->delay                 = grp.readEntry("Delay", 1500);
    d->sharedData->printFileName         = grp.readEntry("Print Filename", true);
    d->sharedData->printProgress         = grp.readEntry("Print Progress Inticator", true);
    d->sharedData->printFileComments     = grp.readEntry("Print Comments", false);
    d->sharedData->loop                  = grp.readEntry("Loop", false);
    d->sharedData->shuffle               = grp.readEntry("Shuffle", false);
    d->sharedData->showSelectedFilesOnly = grp.readEntry("Show Selected Files Only", false);
    d->sharedData->effectName            = grp.readEntry("Effect Name", "Random");
    d->sharedData->effectNameGL          = grp.readEntry("Effect Name (OpenGL)", "Random");

    d->sharedData->delayMsMaxValue = 10000;
    d->sharedData->delayMsMinValue = 100;
    d->sharedData->delayMsLineStep = 100;

    // Comments tab settings
    QFont *savedFont = new QFont();
    savedFont->setFamily(grp.readEntry("Comments Font Family"));
    savedFont->setPointSize(grp.readEntry("Comments Font Size", 10 ));
    savedFont->setBold(grp.readEntry("Comments Font Bold", false));
    savedFont->setItalic(grp.readEntry("Comments Font Italic", false));
    savedFont->setUnderline(grp.readEntry("Comments Font Underline", false));
    savedFont->setOverline(grp.readEntry("Comments Font Overline", false));
    savedFont->setStrikeOut(grp.readEntry("Comments Font StrikeOut", false));
    savedFont->setFixedPitch(grp.readEntry("Comments Font FixedPitch", false));

    d->sharedData->captionFont = savedFont;

    d->sharedData->commentsFontColor     = grp.readEntry("Comments Font Color", 0xffffff);
    d->sharedData->commentsBgColor       = grp.readEntry("Comments Bg Color", 0x000000);
    d->sharedData->transparentBg         = grp.readEntry("Transparent Bg", true);

    d->sharedData->commentsLinesLength   = grp.readEntry("Comments Lines Length", 72);

    // Soundtrack tab
    d->sharedData->soundtrackLoop        = grp.readEntry("Sountrack Loop", false);
    d->sharedData->soundtrackPath        = KUrl(grp.readEntry("Sountrack Path", "" ));

    // Advanced tab
    d->sharedData->useMilliseconds       = grp.readEntry("Use Milliseconds", false);
    d->sharedData->enableMouseWheel      = grp.readEntry("Enable Mouse Wheel", true);

    d->sharedData->kbDisableFadeInOut = grp.readEntry("KB Disable FadeInOut", false);
    d->sharedData->kbDisableCrossFade = grp.readEntry("KB Disable Crossfade", false);

    d->sharedData->enableCache = grp.readEntry("Enable Cache", false);
    d->sharedData->cacheSize  = grp.readEntry("Cache Size", 5);

    d->sharedData->mainPage->readSettings();
    d->sharedData->captionPage->readSettings();
    d->sharedData->soundtrackPage->readSettings();
    d->sharedData->advancedPage->readSettings();
}

void SlideShowConfig::saveSettings()
{
    if (!d->config) return;

    d->sharedData->mainPage->saveSettings();

    d->sharedData->captionPage->saveSettings();

    d->sharedData->soundtrackPage->saveSettings();

    d->sharedData->advancedPage->saveSettings();

    KConfigGroup grp = d->config->group("SlideShow Settings");

    grp.writeEntry("OpenGL", d->sharedData->opengl);

    grp.writeEntry("Delay", d->sharedData->delay);

    grp.writeEntry("Print Filename", d->sharedData->printFileName);

    grp.writeEntry("Print Progress Indicator", d->sharedData->printProgress);

    grp.writeEntry("Print Comments", d->sharedData->printFileComments);

    grp.writeEntry("Loop", d->sharedData->loop);

    grp.writeEntry("Shuffle", d->sharedData->shuffle);

    grp.writeEntry("Show Selected Files Only", d->sharedData->showSelectedFilesOnly);

    grp.writeEntry("Use Milliseconds", d->sharedData->useMilliseconds);

    grp.writeEntry("Enable Mouse Wheel", d->sharedData->enableMouseWheel);

    // Comments tab settings
    QFont* commentsFont = d->sharedData->captionFont;

    grp.writeEntry("Comments Font Family", commentsFont->family());

    grp.writeEntry("Comments Font Size", commentsFont->pointSize());

    grp.writeEntry("Comments Font Bold", commentsFont->bold());

    grp.writeEntry("Comments Font Italic", commentsFont->italic());

    grp.writeEntry("Comments Font Underline", commentsFont->underline());

    grp.writeEntry("Comments Font Overline", commentsFont->overline());

    grp.writeEntry("Comments Font StrikeOut", commentsFont->strikeOut());

    grp.writeEntry("Comments Font FixedPitch", commentsFont->fixedPitch());

    grp.writeEntry("Comments Font Color", d->sharedData->commentsFontColor);

    grp.writeEntry("Comments Bg Color", d->sharedData->commentsBgColor);

    grp.writeEntry("Transparent Bg", d->sharedData->transparentBg);

    grp.writeEntry("Comments Lines Length", d->sharedData->commentsLinesLength);

    grp.writeEntry("Effect Name (OpenGL)", d->sharedData->effectNameGL);

    grp.writeEntry("Effect Name", d->sharedData->effectName);

    // Sountrack tab
    grp.writeEntry("Sountrack Loop", d->sharedData->soundtrackLoop);

    grp.writeEntry("Sountrack Path", d->sharedData->soundtrackPath.path());

    // Advanced settings
    grp.writeEntry("KB Disable FadeInOut", d->sharedData->kbDisableFadeInOut);

    grp.writeEntry("KB Disable Crossfade", d->sharedData->kbDisableCrossFade);

    grp.writeEntry("Enable Cache", d->sharedData->enableCache);

    grp.writeEntry("Cache Size", d->sharedData->cacheSize);

    d->config->sync();
}

// -- SLOTS

void SlideShowConfig::slotStartClicked()
{
    saveSettings();

    if ( d->sharedData->mainPage->updateUrlList() )
        emit buttonStartClicked();

    return;
}

void SlideShowConfig::slotHelp()
{
    KToolInvocation::invokeHelp("slideshow", "kipi-plugins");
}

void SlideShowConfig::slotClose()
{
    close();
}

}  // namespace KIPISlideShowPlugin
