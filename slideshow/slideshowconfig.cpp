/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-02-17
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2006-2007 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

// Qt includes.

#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qmap.h>
#include <q3frame.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qcolor.h>
#include <qnamespace.h>
#include <q3listbox.h>
#include <qfileinfo.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QPixmap>

// Kde includes.

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kstandarddirs.h>
#include <kfontdialog.h>
#include <kcolorbutton.h>
#include <kio/previewjob.h>
#include <kurl.h>

// libkipi includes

#include <libkipi/interface.h>
#include <ktoolinvocation.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "listimageitems.h"
#include "slideshow.h"
#include "slideshowgl.h"
#include "slideshowkb.h"
#include "slideshowconfig.h"
#include "slideshowconfig.moc"

namespace KIPISlideShowPlugin
{

SlideShowConfig::SlideShowConfig(bool allowSelectedOnly, KIPI::Interface * interface,
                                 QWidget *parent, const char* name, bool ImagesHasComments,
                                 KUrl::List *urlList)
               : SlideShowConfigBase(parent, name)
{
    // About data and help button.

    KIPIPlugins::KPAboutData * about = new KIPIPlugins::KPAboutData(ki18n("Slide Show"),
                                        0,
                                        KAboutData::License_GPL,
                                        ki18n("A Kipi plugin for image slideshow"),
                                        ki18n( "(c) 2003-2004, Renchi Raju\n(c) 2007, Valerio Fuoglio" ));

    about->addAuthor(ki18n( "Renchi Raju" ), ki18n("Author"),
                     "renchi@pooh.tam.uiuc.edu");
    about->addAuthor(ki18n( "Valerio Fuoglio" ), ki18n("Author and maintainer"),
                     "valerio.fuoglio@gmail.com");

    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("SlideShow Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

    // Switch to selected files only (it depends on allowSelectedOnly)

    m_selectedFilesButton->setEnabled( allowSelectedOnly );

    m_delayMsMaxValue = 100000;
    m_delayMsMinValue = 100;
    m_delayMsLineStep = 10;

    m_delaySpinBox->setMinValue(m_delayMsMinValue);
    m_delaySpinBox->setMaxValue(m_delayMsMaxValue);
    m_delaySpinBox->setLineStep(m_delayMsLineStep);

    m_interface = interface;

    // Signal to Slot connections

    connect(m_openglCheckBox, SIGNAL(toggled(bool)), this, SLOT(slotOpenGLToggled()));
    connect(m_buttonStart, SIGNAL(clicked()), this, SLOT(slotStartClicked()));
    connect(m_printCommentsCheckBox, SIGNAL(toggled(bool)), this, SLOT(slotPrintCommentsToggled()));
    connect(m_commentsFontColor, SIGNAL(changed(const QColor &)), this, SLOT(slotCommentsFontColorChanged()));
    connect(m_commentsBgColor, SIGNAL(changed(const QColor &)), this, SLOT(slotCommentsBgColorChanged()));
    connect(m_useMillisecondsCheckBox, SIGNAL(toggled(bool)), SLOT(slotUseMillisecondsToggled()));
    connect(m_delaySpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotDelayChanged()));
    connect(m_effectsComboBox, SIGNAL(activated(int)), this,  SLOT(slotEffectChanged()));

    connect(m_fileSrcButtonGroup, SIGNAL(clicked(int)), this, SLOT(slotSelection()));

    connect( m_ImagesFilesListBox, SIGNAL( currentChanged( Q3ListBoxItem * ) ),
             this, SLOT( slotImagesFilesSelected(Q3ListBoxItem *) ) );
    connect(m_ImagesFilesListBox, SIGNAL( addedDropItems(KUrl::List) ),
            this, SLOT( slotAddDropItems(KUrl::List)));
    connect( m_ImagesFilesButtonAdd, SIGNAL( clicked() ),
             this, SLOT( slotImagesFilesButtonAdd() ) );
    connect( m_ImagesFilesButtonDelete, SIGNAL( clicked() ),
             this, SLOT( slotImagesFilesButtonDelete() ) );
    connect( m_ImagesFilesButtonUp, SIGNAL( clicked() ),
             this, SLOT( slotImagesFilesButtonUp() ) );
    connect( m_ImagesFilesButtonDown, SIGNAL( clicked() ),
             this, SLOT( slotImagesFilesButtonDown() ) );

    connect(m_cacheCheckBox, SIGNAL(toggled(bool)), this, SLOT(slotCacheToggled()));

    m_thumbJob = 0L;

    // Configuration file management

    m_config = new KConfig("kipirc");
    m_config->setGroup("SlideShow Settings");

    readSettings();

    slotUseMillisecondsToggled();

    // Comments tab management

    m_commentsFontChooser->setSampleText(
            i18n("Slideshow is part of KIPI-Plugins (http://www.kipi-plugins.org)"));

    // Host application images has comments
    if ( ! ImagesHasComments ) {
        m_printCommentsCheckBox->setEnabled(FALSE);
        m_tabWidget->setTabEnabled(commentsTab, FALSE);
    }

    m_urlList = urlList;

    slotSelection();
    slotEffectChanged();
}

SlideShowConfig::~SlideShowConfig()
{
    if ( m_thumbJob )   delete m_thumbJob;
    if ( m_config )     delete m_config;
}

void SlideShowConfig::loadEffectNames()
{
    m_effectsComboBox->clear();

    QMap<QString,QString> effectNames = SlideShow::effectNamesI18N();
    QStringList effects;

    QMap<QString,QString>::Iterator it;
    for (it = effectNames.begin(); it != effectNames.end(); ++it)
        effects.append(it.data());

    m_effectsComboBox->insertStringList(effects);

    for (int i=0; i<m_effectsComboBox->count(); i++) {
        if (effectNames[m_effectName] == m_effectsComboBox->text(i)) {
            m_effectsComboBox->setCurrentItem(i);
            break;
        }
    }
}

void SlideShowConfig::loadEffectNamesGL()
{
    m_effectsComboBox->clear();

    QStringList effects;
    QMap<QString,QString> effectNames;
    QMap<QString,QString>::Iterator it;

    // Load slideshowgl effects
    effectNames = SlideShowGL::effectNamesI18N();

    for (it = effectNames.begin(); it != effectNames.end(); ++it)
        effects.append(it.data());

    // Load Ken Burns effect
    effectNames = SlideShowKB::effectNamesI18N();
    for (it = effectNames.begin(); it != effectNames.end(); ++it)
      effects.append(it.data());

    // Update GUI

    effects.sort();
    m_effectsComboBox->insertStringList(effects);

    for (int i=0; i<m_effectsComboBox->count(); i++) {
      if (effectNames[m_effectNameGL] == m_effectsComboBox->text(i)) {
        m_effectsComboBox->setCurrentItem(i);
        break;
      }
    }
}


void SlideShowConfig::readSettings()
{
    bool  opengl;
    int   delay;
    bool  printFileName;
    bool  printProgress;
    bool  printFileComments;
    bool  loop;
    bool  shuffle;
    bool  showSelectedFilesOnly;
    bool  useMilliseconds;
    bool  enableMouseWheel;

    opengl                = m_config->readBoolEntry("OpenGL", false);
    delay                 = m_config->readNumEntry("Delay", 1500);
    printFileName         = m_config->readBoolEntry("Print Filename", true);
    printProgress         = m_config->readBoolEntry("Print Progress Inticator", true);
    printFileComments     = m_config->readBoolEntry("Print Comments", false);
    loop                  = m_config->readBoolEntry("Loop", false);
    shuffle               = m_config->readBoolEntry("Shuffle", false);
    showSelectedFilesOnly = m_config->readBoolEntry("Show Selected Files Only", false);
    m_effectName           = m_config->readEntry("Effect Name", "Random");
    m_effectNameGL         = m_config->readEntry("Effect Name (OpenGL)", "Random");

    useMilliseconds       = m_config->readBoolEntry("Use Milliseconds", false);
    enableMouseWheel      = m_config->readNumEntry("Enable Mouse Wheel", true);


    // Comments tab settings
    uint  commentsFontColor;
    uint  commentsBgColor;
    int   commentsLinesLength;

    QFont *savedFont = new QFont();
    savedFont->setFamily(m_config->readEntry("Comments Font Family"));
    savedFont->setPointSize(m_config->readNumEntry("Comments Font Size", 10 ));
    savedFont->setBold(m_config->readBoolEntry("Comments Font Bold", false));
    savedFont->setItalic(m_config->readBoolEntry("Comments Font Italic", false));
    savedFont->setUnderline(m_config->readBoolEntry("Comments Font Underline", false));
    savedFont->setOverline(m_config->readBoolEntry("Comments Font Overline", false));
    savedFont->setStrikeOut(m_config->readBoolEntry("Comments Font StrikeOut", false));
    savedFont->setFixedPitch(m_config->readBoolEntry("Comments Font FixedPitch", false));

    commentsFontColor     = m_config->readUnsignedNumEntry("Comments Font Color", 0xffffff);
    commentsBgColor       = m_config->readUnsignedNumEntry("Comments Bg Color", 0x000000);

    commentsLinesLength   = m_config->readNumEntry("Comments Lines Length", 72);

    // Advanced tab
    bool enableCache, kbDisableFadeInOut, kbDisableCrossFade;

    kbDisableFadeInOut = m_config->readBoolEntry("KB Disable FadeInOut", false);
    kbDisableCrossFade = m_config->readBoolEntry("KB Disable Crossfade", false);

    enableCache = m_config->readBoolEntry("Enable Cache", false);
    m_cacheSize  = m_config->readNumEntry("Cache Size", 5);


    // -- Apply Settings to widgets ------------------------------

    m_openglCheckBox->setChecked(opengl);

    m_delaySpinBox->setValue(delay);

    m_printNameCheckBox->setChecked(printFileName);

    m_printProgressCheckBox->setChecked(printProgress);

    m_printCommentsCheckBox->setChecked(printFileComments);

    m_loopCheckBox->setChecked(loop);

    m_shuffleCheckBox->setChecked(shuffle);

    m_enableMouseWheelCheckBox->setChecked(enableMouseWheel);
    m_useMillisecondsCheckBox->setChecked(useMilliseconds);

    if (showSelectedFilesOnly && m_selectedFilesButton->isEnabled() )
        m_selectedFilesButton->setChecked(true);
    else
        m_allFilesButton->setChecked(true);

    m_commentsLinesLengthSpinBox->setValue(commentsLinesLength);
    m_commentsFontColor->setColor(QColor(commentsFontColor));
    m_commentsBgColor->setColor(QColor(commentsBgColor));
    m_commentsFontChooser->setFont(*savedFont);
    delete savedFont;

    m_kbDisableFadeCheckBox->setChecked(kbDisableFadeInOut);
    m_kbDisableCrossfadeCheckBox->setChecked(kbDisableCrossFade);

    m_cacheCheckBox->setChecked(enableCache);

    slotOpenGLToggled();
    slotCacheToggled();
}

void SlideShowConfig::saveSettings()
{
    if (!m_config) return;

    m_config->writeEntry("OpenGL", m_openglCheckBox->isChecked());

    // Delay will be always saved as millisecond value, to keep compatibility
    if ( m_useMillisecondsCheckBox->isChecked() )
        m_config->writeEntry("Delay", m_delaySpinBox->value());
    else
        m_config->writeEntry("Delay", m_delaySpinBox->value()*1000);

    m_config->writeEntry("Print Filename", m_printNameCheckBox->isChecked());
    m_config->writeEntry("Print Progress Indicator", m_printProgressCheckBox->isChecked());
    m_config->writeEntry("Print Comments", m_printCommentsCheckBox->isChecked());
    m_config->writeEntry("Loop", m_loopCheckBox->isChecked());
    m_config->writeEntry("Shuffle", m_shuffleCheckBox->isChecked());
    m_config->writeEntry("Show Selected Files Only", m_selectedFilesButton->isChecked());

    m_config->writeEntry("Use Milliseconds", m_useMillisecondsCheckBox->isChecked());
    m_config->writeEntry("Enable Mouse Wheel", m_enableMouseWheelCheckBox->isChecked());

    // Comments tab settings
    QFont* commentsFont = new QFont(m_commentsFontChooser->font());
    m_config->writeEntry("Comments Font Family", commentsFont->family());
    m_config->writeEntry("Comments Font Size", commentsFont->pointSize());
    m_config->writeEntry("Comments Font Bold", commentsFont->bold());
    m_config->writeEntry("Comments Font Italic", commentsFont->italic());
    m_config->writeEntry("Comments Font Underline", commentsFont->underline());
    m_config->writeEntry("Comments Font Overline", commentsFont->overline());
    m_config->writeEntry("Comments Font StrikeOut", commentsFont->strikeOut());
    m_config->writeEntry("Comments Font FixedPitch", commentsFont->fixedPitch());
    delete commentsFont;

    QColor* fontColor = new QColor(m_commentsFontColor->color());
    uint commentsFontColorRGB = fontColor->rgb();
    delete fontColor;
    m_config->writeEntry("Comments Font Color", commentsFontColorRGB);

    QColor* bgColor = new QColor(m_commentsBgColor->color());
    uint commentsBgColorRGB = bgColor->rgb();
    delete bgColor;
    m_config->writeEntry("Comments Bg Color", commentsBgColorRGB);

    m_config->writeEntry("Comments Lines Length", m_commentsLinesLengthSpinBox->value());

    if (!m_openglCheckBox->isChecked()) {

        QString effect;
        QMap<QString,QString> effectNames = SlideShow::effectNamesI18N();
        QMap<QString,QString>::Iterator it;

        for (it = effectNames.begin(); it != effectNames.end(); ++it) {
            if (it.data() == m_effectsComboBox->currentText()) {
                effect = it.key();
                break;
            }
        }

        m_config->writeEntry("Effect Name", effect);

    }
    else
    {
      QStringList effects;
      QMap<QString,QString> effectNames;
      QMap<QString,QString>::Iterator it;

    // Load slideshowgl effects
      effectNames = SlideShowGL::effectNamesI18N();

      for (it = effectNames.begin(); it != effectNames.end(); ++it)
        effects.append(it.data());

    // Load Ken Burns effect
      effectNames = SlideShowKB::effectNamesI18N();
      for (it = effectNames.begin(); it != effectNames.end(); ++it)
        effects.append(it.data());

        QString effect;
        QStringList::Iterator it1;

        for (it1 = effects.begin(); it1 != effects.end(); ++it1) {
            if ( *it1 == m_effectsComboBox->currentText()) {
                effect = *it1;
                break;
            }
        }

        m_config->writeEntry("Effect Name (OpenGL)", effect);
    }

    // Advanced settings
    m_config->writeEntry("KB Disable FadeInOut", m_kbDisableFadeCheckBox->isChecked());
    m_config->writeEntry("KB Disable Crossfade", m_kbDisableCrossfadeCheckBox->isChecked());

    m_config->writeEntry("Enable Cache", m_cacheCheckBox->isChecked());
    m_config->writeEntry("Cache Size", m_cacheSizeSpinBox->value());

    m_config->sync();
}

void SlideShowConfig::addItems(const KUrl::List& fileList)
{
    if (fileList.isEmpty()) return;
    KUrl::List Files = fileList;

    for ( KUrl::List::Iterator it = Files.begin() ; it != Files.end() ; ++it )
    {
        KUrl currentFile = *it;

        QFileInfo fi(currentFile.path());
        QString Temp = fi.dirPath();
        QString albumName = Temp.section('/', -1);

        KIPI::ImageInfo info = m_interface->info(currentFile);
        QString comments = info.description();

        ImageItem *item = new ImageItem( m_ImagesFilesListBox,
                                         currentFile.path().section('/', -1 ),   // File name with extension.
                                         comments,                               // Image comments.
                                         currentFile.path().section('/', 0, -1), // Complete path with file name.
                                         albumName                               // Album name.
                                       );

        item->setName( currentFile.path().section('/', -1) );
    }

    ShowNumberImages( m_ImagesFilesListBox->count() );
    m_ImagesFilesListBox->setCurrentItem( m_ImagesFilesListBox->count()-1) ;
    slotImagesFilesSelected(m_ImagesFilesListBox->item(m_ImagesFilesListBox->currentItem()));
    m_ImagesFilesListBox->centerCurrentItem();
}

void SlideShowConfig::slotCommentsBgColorChanged()
{
    m_commentsFontChooser->setBackgroundColor(m_commentsBgColor->color());
}

void SlideShowConfig::slotCommentsFontColorChanged()
{
    m_commentsFontChooser->setColor(m_commentsFontColor->color());
}

void SlideShowConfig::slotPrintCommentsToggled()
{
    m_tabWidget->setTabEnabled(commentsTab, m_printCommentsCheckBox->isChecked());
}

void SlideShowConfig::slotUseMillisecondsToggled()
{

    int delayValue = m_delaySpinBox->value();

    m_delaySpinBox->setValue(0);

    if ( m_useMillisecondsCheckBox -> isChecked() ) {
        m_delayLabel->setText(QString("Delay between images (ms):"));

        m_delaySpinBox->setMinValue(m_delayMsMinValue);
        m_delaySpinBox->setMaxValue(m_delayMsMaxValue);
        m_delaySpinBox->setLineStep(m_delayMsLineStep);

        m_delaySpinBox->setValue(delayValue*1000);
    }
    else {
        m_delayLabel->setText(QString("Delay between images  (s):"));

        m_delaySpinBox->setMinValue(m_delayMsMinValue/1000);
        m_delaySpinBox->setMaxValue(m_delayMsMaxValue/100);
        m_delaySpinBox->setLineStep(m_delayMsLineStep/10);

        m_delaySpinBox->setValue(delayValue/1000);
    }
}

void SlideShowConfig::slotEffectChanged()
{
  bool isKB = m_effectsComboBox->currentText() == i18n("Ken Burns");

  m_printNameCheckBox->setEnabled(!isKB);
  m_printProgressCheckBox->setEnabled(!isKB);
  m_printCommentsCheckBox->setEnabled(!isKB);

  m_cacheButtonGroup->setEnabled(!isKB);
}

void SlideShowConfig::slotCacheToggled()
{
  bool isEnabled = m_cacheCheckBox->isChecked();

  m_cacheSizeLabel1->setEnabled(isEnabled);
  m_cacheSizeLabel2->setEnabled(isEnabled);
  m_cacheSizeSpinBox->setEnabled(isEnabled);
}

void SlideShowConfig::slotOpenGLToggled()
{
    if (m_openglCheckBox->isChecked()) {
        loadEffectNamesGL();
    }
    else {
        loadEffectNames();
    }

    ShowNumberImages( m_ImagesFilesListBox->count() );

    slotEffectChanged();
}

void SlideShowConfig::slotDelayChanged()
{
    ShowNumberImages( m_ImagesFilesListBox->count() );
}

void SlideShowConfig::slotSelection()
{
    KUrl::List urlList;
    if (m_selectedFilesButton->isChecked())
    {
        urlList = m_interface->currentSelection().images();

        m_ImagesFilesButtonAdd->setEnabled(FALSE);
        m_ImagesFilesButtonDelete->setEnabled(FALSE);
        m_ImagesFilesButtonUp->setEnabled(FALSE);
        m_ImagesFilesButtonDown->setEnabled(FALSE);
    }
    else
        if (m_allFilesButton->isChecked())
        {
            KUrl currentPath = m_interface->currentAlbum().path();
            Q3ValueList<KIPI::ImageCollection> albumList;
            albumList = m_interface->allAlbums();
            Q3ValueList<KIPI::ImageCollection>::iterator it;

            urlList = m_interface->currentAlbum().images();
            for ( it = albumList.begin(); it != albumList.end(); ++it )
                if (currentPath.isParentOf((*it).path()) && !((*it).path() == currentPath))
                    urlList += (*it).images();

            m_ImagesFilesButtonAdd->setEnabled(FALSE);
            m_ImagesFilesButtonDelete->setEnabled(FALSE);
            m_ImagesFilesButtonUp->setEnabled(FALSE);
            m_ImagesFilesButtonDown->setEnabled(FALSE);
        }

    if ( m_customButton->isChecked() )    // Custom selected
    {
        m_ImagesFilesButtonAdd->setEnabled(TRUE);
        m_ImagesFilesButtonDelete->setEnabled(TRUE);
        m_ImagesFilesButtonUp->setEnabled(TRUE);
        m_ImagesFilesButtonDown->setEnabled(TRUE);
    }
    else
    {
        if (!urlList.isEmpty())
        {
            m_ImagesFilesListBox->clear();
            addItems(urlList);
        }
    }
}

void SlideShowConfig::slotImagesFilesSelected( Q3ListBoxItem *item )
{

    if ( !item || m_ImagesFilesListBox->count() == 0 )
    {
        m_label7->setText("");
        m_ImageLabel->clear();
        return;
    }

    ImageItem *pitem = static_cast<ImageItem*>( item );

    if ( !pitem ) return;

    KUrl url;
    url.setPath(pitem->path());

    m_ImageLabel->clear();

    if ( m_thumbJob ) delete m_thumbJob;

    m_thumbJob = KIO::filePreview( url, m_ImageLabel->width() );

    connect(m_thumbJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            SLOT(slotGotPreview(const KFileItem*, const QPixmap&)));
    connect(m_thumbJob, SIGNAL(failed(const KFileItem*)),
            SLOT(slotFailedPreview(const KFileItem*)));

    int index = m_ImagesFilesListBox->index ( item );
    m_label7->setText(i18n("Image no. %1").arg(index + 1));
}

void SlideShowConfig::slotAddDropItems(KUrl::List filesUrl)
{
    addItems(filesUrl);
}

void SlideShowConfig::slotImagesFilesButtonAdd( void )
{
    KUrl::List ImageFilesList =
            KIPI::ImageDialog::getImageURLs( this, m_interface );
    if ( !ImageFilesList.isEmpty() )
        addItems( ImageFilesList );
}

void SlideShowConfig::slotImagesFilesButtonDelete( void )
{
    for (uint i = 0 ; i < m_ImagesFilesListBox->count() ; ++i)
    {
        if (m_ImagesFilesListBox->isSelected(i))
        {
            m_ImagesFilesListBox->removeItem(i);
            m_ImagesFilesListBox->setCurrentItem(i);
            --i;
        }
    }

    m_ImagesFilesListBox->setSelected(m_ImagesFilesListBox->item(m_ImagesFilesListBox->currentItem()), true);
    slotImagesFilesSelected(m_ImagesFilesListBox->item(m_ImagesFilesListBox->currentItem()));
    ShowNumberImages( m_ImagesFilesListBox->count() );
}

void SlideShowConfig::slotImagesFilesButtonUp( void )
{
    int Cpt = 0;

    for (uint i = 0 ; i < m_ImagesFilesListBox->count() ; ++i)
        if (m_ImagesFilesListBox->isSelected(i))
            ++Cpt;

    if  (Cpt == 0)
        return;

    if  (Cpt > 1)
    {
        KMessageBox::error(this, i18n("You can only move up one image file at once."));
        return;
    }

    unsigned int Index = m_ImagesFilesListBox->currentItem();

    if (Index == 0)
        return;

    ImageItem *pitem = static_cast<ImageItem*>( m_ImagesFilesListBox->item(Index) );
    QString path(pitem->path());
    QString comment(pitem->comments());
    QString name(pitem->name());
    QString album(pitem->album());
    m_ImagesFilesListBox->removeItem(Index);
    ImageItem *item = new ImageItem( 0, name, comment, path, album );
    item->setName( name );
    m_ImagesFilesListBox->insertItem(item, Index-1);
    m_ImagesFilesListBox->setSelected(Index-1, true);
    m_ImagesFilesListBox->setCurrentItem(Index-1);
}

void SlideShowConfig::slotImagesFilesButtonDown( void )
{
    int Cpt = 0;

    for (uint i = 0 ; i < m_ImagesFilesListBox->count() ; ++i)
        if (m_ImagesFilesListBox->isSelected(i))
            ++Cpt;

    if (Cpt == 0)
        return;

    if (Cpt > 1)
    {
        KMessageBox::error(this, i18n("You can only move down one image file at once."));
        return;
    }

    unsigned int Index = m_ImagesFilesListBox->currentItem();

    if (Index == m_ImagesFilesListBox->count())
        return;

    ImageItem *pitem = static_cast<ImageItem*>( m_ImagesFilesListBox->item(Index) );
    QString path(pitem->path());
    QString comment(pitem->comments());
    QString name(pitem->name());
    QString album(pitem->name());
    m_ImagesFilesListBox->removeItem(Index);
    ImageItem *item = new ImageItem( 0, name, comment, path, album );
    item->setName( name );
    m_ImagesFilesListBox->insertItem(item, Index+1);
    m_ImagesFilesListBox->setSelected(Index+1, true);
    m_ImagesFilesListBox->setCurrentItem(Index+1);
}

void SlideShowConfig::ShowNumberImages( int Number )
{
    QTime TotalDuration (0, 0, 0);

    int TransitionDuration = 2000;

    if ( m_openglCheckBox->isChecked() )
        TransitionDuration += 500;

    if ( m_useMillisecondsCheckBox->isChecked() )
        TotalDuration = TotalDuration.addMSecs(Number * m_delaySpinBox->text().toInt());
    else
        TotalDuration = TotalDuration.addSecs(Number * m_delaySpinBox->text().toInt());

    TotalDuration = TotalDuration.addMSecs((Number-1)*TransitionDuration);

    if ( Number < 2)
        m_label6->setText(i18n("%1 image [%2]").arg(Number).arg(TotalDuration.toString()));
    else
        m_label6->setText(i18n("%1 images [%2]").arg(Number).arg(TotalDuration.toString()));
}

void SlideShowConfig::slotGotPreview(const KFileItem*, const QPixmap &pixmap)
{
    m_ImageLabel->setPixmap(pixmap);
    m_thumbJob = 0L;
}

void SlideShowConfig::slotFailedPreview(const KFileItem*)
{
    m_thumbJob = 0L;
}

void SlideShowConfig::SlotPortfolioDurationChanged ( int )
{
    ShowNumberImages( m_ImagesFilesListBox->count() );
}

void SlideShowConfig::slotStartClicked()
{
    saveSettings();

    for (uint i=0 ; i < m_ImagesFilesListBox->count() ; ++i)
    {
        ImageItem *pitem = static_cast<ImageItem*>( m_ImagesFilesListBox->item(i) );
        if (!QFile::exists(pitem->path()))
        {
            KMessageBox::error(this,
                               i18n("Cannot access to file %1, please check the path is right.").arg(pitem->path()));
            return;
        }
        m_urlList->append(pitem->path());                              // Input images files.
    }

    emit buttonStartClicked();
}


void SlideShowConfig::slotHelp()
{
    KToolInvocation::invokeHelp("slideshow",
                                             "kipi-plugins");
}

}  // NameSpace KIPISlideShowPlugin
