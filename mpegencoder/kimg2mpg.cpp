//////////////////////////////////////////////////////////////////////////////
//
//    KIMG2MPG.CPP
//
//    Copyright (C) 2003 Gilles Caulier <caulier dot gilles at free.fr>
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
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

// C Ansi includes

extern "C"
{
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
}

// C++ includes

#include <iostream>

// Qt includes

#include <qwidget.h>
#include <qpixmap.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qprogressbar.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qheader.h>
#include <qwhatsthis.h>
#include <qprocess.h>
#include <qdatetime.h>
#include <qlayout.h>
#include <qdragobject.h>

// KDElib includes

#include <kprogress.h>
#include <kprocess.h>
#include <klocale.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kcolordialog.h>
#include <klineedit.h>
#include <kcolorbutton.h>
#include <klistbox.h>
#include <kbuttonbox.h>
#include <kurl.h>
#include <kimageio.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>
#include <kdebug.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// KIPI include files

#include <libkipi/imagedialog.h>

// Local includes

#include "pluginsversion.h"
#include "kpaboutdata.h"
#include "kshowdebuggingoutput.h"
#include "optionsdialog.h"
#include "checkbinprog.h"
#include "kimg2mpg.h"
#include "kimg2mpg.moc"

namespace KIPIMPEGEncoderPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////

class ImageItem : public QListBoxText
{
public:
    ImageItem(QListBox * parent, QString const & name, QString const & comments, QString const & path,
              QString const & album)
            : QListBoxText(parent), _name(name), _comments(comments), _path(path), _album(album)
    {}

    QString comments()                   { return _comments; }
    QString name()                       { return _name;     }
    QString path()                       { return _path;     }
    QString album()                      { return _album;    }
    void setName(const QString &newName) { setText(newName); }

private:
    QString _name;
    QString _comments;
    QString _path;
    QString _album;
};


/////////////////////////////////////////////////////////////////////////////////////////////

ListImageItems::ListImageItems(QWidget *parent, const char *name)
              : KListBox(parent, name)
{
    setSelectionMode (QListBox::Extended);
    setAcceptDrops(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void ListImageItems::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(QUriDrag::canDecode(e));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void ListImageItems::dropEvent(QDropEvent *e)
{
    QStrList strList;
    KURL::List filesUrl;

    if ( !QUriDrag::decode(e, strList) ) return;

    QStrList stringList;
    QStrListIterator it(strList);
    char *str;

    while ( (str = it.current()) != 0 )
       {
       QString filePath = QUriDrag::uriToLocalFile(str);
       QFileInfo fileInfo(filePath);

       if (fileInfo.isFile() && fileInfo.exists())
          {
          KURL url(fileInfo.filePath());
          filesUrl.append(url);
          }

       ++it;
       }

    if (filesUrl.isEmpty() == false)
       emit addedDropItems(filesUrl);
}


/////////////////////////////// CONSTRUCTOR /////////////////////////////////////////////////

KImg2mpgData::KImg2mpgData(KIPI::Interface* interface, QWidget *parent, const char *name)
            : KDialog( parent, name, false, Qt::WDestructiveClose ), m_interface( interface )
{
  m_TmpFolderConfig = "";
  m_Proc = 0L;
  m_thumbJob = 0L;
  m_Encoding = false;
  m_Abort = false;

  m_Icons = new KIconLoader( QString( "kipi" ) );
  m_NoneLabel = i18n ("none");

  QVBoxLayout* ml = new QVBoxLayout( this, 10 );

  //---------------------------------------------

  QFrame *headerFrame = new QFrame( this );
  headerFrame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  QHBoxLayout* layout = new QHBoxLayout( headerFrame );
  layout->setMargin( 2 ); // to make sure the frame gets displayed
  layout->setSpacing( 0 );
  QLabel *pixmapLabelLeft = new QLabel( headerFrame, "pixmapLabelLeft" );
  pixmapLabelLeft->setScaledContents( false );
  layout->addWidget( pixmapLabelLeft );
  QLabel *labelTitle = new QLabel( i18n("Create MPEG Slideshow"), headerFrame, "labelTitle" );
  layout->addWidget( labelTitle );
  layout->setStretchFactor( labelTitle, 1 );
  ml->addWidget( headerFrame );

  QString directory;
  KGlobal::dirs()->addResourceType("kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data");
  directory = KGlobal::dirs()->findResourceDir("kipi_banner_left", "banner_left.png");

  pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
  pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
  labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );

  //---------------------------------------------

  QHBoxLayout* h1 = new QHBoxLayout( ml );
  QVBoxLayout* v1 = new QVBoxLayout( h1 );
  h1->addSpacing( 5 );
  QGridLayout* g1 = new QGridLayout( v1, 3, 2 );

  // Video format selection.

  m_label1 = new QLabel(i18n("Video format, type and chroma mode:"), this);
  g1->addWidget( m_label1, 0, 0, AlignLeft );

  m_VideoFormatComboBox = new QComboBox( false, this, "Video_Format_ComboBox" );
  g1->addWidget( m_VideoFormatComboBox, 0, 1, AlignLeft );
  m_VideoFormatComboBox->insertItem("XVCD");
  m_VideoFormatComboBox->insertItem("SVCD");
  m_VideoFormatComboBox->insertItem("VCD");
  m_VideoFormatComboBox->insertItem("DVD");
  QWhatsThis::add( m_VideoFormatComboBox,
     i18n( "This option specifies the video format for your MPEG file. "
     "For a high photographic resolution on a TV screen, select "
     "'XVCD' (it is the same DVD resolution); "
     "although some old DVD players cannot read this format. "
     "'VCD'/'SVCD' are more compatible with DVD players, "
     "but they are only medium resolution. "
     "DVD is an experimental option." ) );

  // Video type selection.

  m_VideoTypeComboBox = new QComboBox( false, this, "Video_Type_ComboBox" );
  g1->addWidget( m_VideoTypeComboBox, 0, 2, AlignLeft );
  m_VideoTypeComboBox->insertItem("PAL");
  m_VideoTypeComboBox->insertItem("NTSC");
  m_VideoTypeComboBox->insertItem("SECAM");
  QWhatsThis::add( m_VideoTypeComboBox,
     i18n( "This option specifies the video type for your MPEG file. "
     "NTSC is an American TV standard; PAL/SECAM is European." ) );

  connect( m_VideoTypeComboBox, SIGNAL( activated(int ) ),
           this, SLOT( SlotPortfolioDurationChanged (int) ) );

  // Chroma subsampling mode
  m_ChromaComboBox = new QComboBox( false, this, "Chroma_ComboBox" );
  g1->addWidget( m_ChromaComboBox, 0, 3, AlignLeft );
  m_ChromaComboBox->insertItem("Default");
  m_ChromaComboBox->insertItem("444");
  m_ChromaComboBox->insertItem("420jpeg");
  m_ChromaComboBox->insertItem("420mpeg2");
  QWhatsThis::add( m_ChromaComboBox,
     i18n( "This option specifies the chroma subsampling mode. "
           "Change it if you have problems with the default value " ) );

  // Image duration.

  m_label3 = new QLabel(i18n("Image duration (seconds):"), this);
  g1->addWidget( m_label3, 2, 0, AlignLeft );

  m_DurationImageSpinBox = new QSpinBox( 1, 999, 1 , this, "Duration_Image_SpinBox" );
  g1->addWidget( m_DurationImageSpinBox, 2, 1, AlignLeft );
  QWhatsThis::add( m_DurationImageSpinBox,
     i18n( "This option specifies the duration for each image in your MPEG file. "
     "10 seconds is a good value for an image portfolio. "
     "Warning: you may have some problems with your DVD player if the "
     "total MPEG duration is under 3 seconds."));

  connect( m_DurationImageSpinBox, SIGNAL( valueChanged(int ) ),
           this, SLOT( SlotPortfolioDurationChanged (int) ) );

  // Transition between images selection.

  m_label4 = new QLabel(i18n("Transition speed between images:"), this);
  g1->addWidget( m_label4, 3, 0, AlignLeft );

  m_TransitionComboBox = new QComboBox( false, this, "Transition_ComboBox" );
  g1->addWidget( m_TransitionComboBox, 3, 1, AlignLeft );
  m_TransitionComboBox->insertItem(m_NoneLabel);
  m_TransitionComboBox->insertItem("1");
  m_TransitionComboBox->insertItem("2");
  m_TransitionComboBox->insertItem("4");
  m_TransitionComboBox->insertItem("5");
  m_TransitionComboBox->insertItem("10");
  m_TransitionComboBox->insertItem("20");
  QWhatsThis::add( m_TransitionComboBox,
     i18n( "This option specifies the transition speed between images in your MPEG file. "
     "'1' is a slow transition and '20' is a very fast transition. "
     "'2' is a good value for an image portfolio." ) );

  connect( m_TransitionComboBox, SIGNAL( activated(int ) ),
           this, SLOT( SlotPortfolioDurationChanged (int) ) );

  // Background color selection.

  m_label5 = new QLabel(i18n("Background color:"), this);
  g1->addWidget( m_label5, 4, 0, AlignLeft );

  // Black (default background color).

  QColor BackGroundCOLOR = QColor( 0, 0, 0 );

  m_BackgroundColorButton = new KColorButton( BackGroundCOLOR, this );
  g1->addWidget( m_BackgroundColorButton, 4, 1, AlignLeft );
  QWhatsThis::add( m_BackgroundColorButton,
     i18n( "You can select here the background color for your portfolio. "
     "This color is used to pad the image size to fit the TV screen size. "
     "Black is a good value for this. " ));

  // MPEG output filename selection.

  m_MPEGOutputFilename = new QGroupBox( 2, Qt::Horizontal, i18n( "MPEG Output Filename" ), this);
  v1->addWidget( m_MPEGOutputFilename );

  m_MPEGOutputEDITFilename = new KLineEdit( m_MPEGOutputFilename );
  m_MPEGOutputEDITFilename->setMinimumWidth( 300 );
  m_MPEGOutputBUTTONFilename = new QPushButton( m_MPEGOutputFilename );
  m_MPEGOutputBUTTONFilename->setIconSet( SmallIconSet( "fileopen" ) );
  QWhatsThis::add( m_MPEGOutputEDITFilename,
     i18n( "You can specify here the output MPEG filename. "
     "Warning : MPEG files are very big (if you have many images in your portfolio). "
     "Select a folder with a sufficient free disk space. ") );

  connect( m_MPEGOutputBUTTONFilename, SIGNAL( clicked() ),
           this, SLOT( slotMPEGFilenameDialog() ) );

  // Audio input filename selection.

  m_AudioInputFilename = new QGroupBox( 2, Qt::Horizontal, i18n( "Audio Input Filename" ), this);
  v1->addWidget( m_AudioInputFilename );

  m_AudioInputEDITFilename = new KLineEdit( m_AudioInputFilename );
  m_AudioInputEDITFilename->setMinimumWidth( 300 );
  m_AudioInputBUTTONFilename = new QPushButton( m_AudioInputFilename );
  m_AudioInputBUTTONFilename->setIconSet( SmallIconSet( "fileopen" ) );
  QWhatsThis::add( m_AudioInputEDITFilename,
     i18n( "You can specify here the input audio file name. "
     "This audio file name will be multiplexed with the portfolio video. "
     "Warning: if the audio duration is too long, it will be truncated." ) );

  connect( m_AudioInputBUTTONFilename, SIGNAL( clicked() ),
           this, SLOT( slotAudioFilenameDialog() ) );

  // Images files list and the control buttons.

  m_ImagesFilesGroup = new QGroupBox(3, Qt::Horizontal, i18n( "Image Files in Portfolio" ), this );
  v1->addWidget( m_ImagesFilesGroup );

  m_ImagesFilesListBox = new ListImageItems( m_ImagesFilesGroup, "ListImageItems");
  m_ImagesFilesListBox->setSelectionMode (QListBox::Extended);
  m_ImagesFilesListBox->setMinimumWidth( 300 );
  QWhatsThis::add( m_ImagesFilesListBox,
     i18n( "This is the list of the image files for your portfolio. "
     "The portfolio's first image is on the top; the last image is on the bottom. "
     "If you want to add some images, click on the 'Add' "
     "button or use the drag-and-drop." ) );

  connect( m_ImagesFilesListBox, SIGNAL( currentChanged( QListBoxItem * ) ),
           this, SLOT( slotImagesFilesSelected(QListBoxItem *) ) );

  connect(m_ImagesFilesListBox, SIGNAL( addedDropItems(KURL::List) ),
          this, SLOT( slotAddDropItems(KURL::List)));

  m_ImagesFilesButtonBox = new KButtonBox( m_ImagesFilesGroup, Vertical );
  m_ImagesFilesButtonAdd = m_ImagesFilesButtonBox->addButton( i18n( "&Add..." ) );
  QWhatsThis::add( m_ImagesFilesButtonAdd, i18n( "Add some image files to the portfolio list." ) );
  m_ImagesFilesButtonDelete = m_ImagesFilesButtonBox->addButton( i18n( "&Delete" ) );
  QWhatsThis::add( m_ImagesFilesButtonDelete, i18n( "Remove some image files from the portfolio list." ) );
  m_ImagesFilesButtonUp = m_ImagesFilesButtonBox->addButton( i18n( "Image &Up" ) );
  QWhatsThis::add( m_ImagesFilesButtonUp, i18n( "Moving the current image up on the portfolio list." ) );
  m_ImagesFilesButtonDown = m_ImagesFilesButtonBox->addButton( i18n( "Image D&own" ) );
  QWhatsThis::add( m_ImagesFilesButtonDown, i18n( "Moving the current image down on the portfolio list." ) );
  m_ImagesFilesButtonBox->layout();

  connect( m_ImagesFilesButtonAdd, SIGNAL( clicked() ),
           this, SLOT( slotImagesFilesButtonAdd() ) );

  connect( m_ImagesFilesButtonDelete, SIGNAL( clicked() ),
           this, SLOT( slotImagesFilesButtonDelete() ) );

  connect( m_ImagesFilesButtonUp, SIGNAL( clicked() ),
           this, SLOT( slotImagesFilesButtonUp() ) );

  connect( m_ImagesFilesButtonDown, SIGNAL( clicked() ),
           this, SLOT( slotImagesFilesButtonDown() ) );

  m_ImageLabel = new QLabel( m_ImagesFilesGroup );
  m_ImageLabel->setMinimumWidth( 120 );
  m_ImageLabel->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  m_ImageLabel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
  QWhatsThis::add( m_ImageLabel, i18n( "Preview the currently selected image." ) );

  m_label6 = new QLabel(m_ImagesFilesGroup);
  QWhatsThis::add( m_label6, i18n( "Total number of images in the portfolio and sequence duration." ) );
  m_label7 = new QLabel(m_ImagesFilesGroup);
  QWhatsThis::add( m_label7, i18n( "Currently selected image in the portfolio list." ) );

  // Encode push button.

  QVBoxLayout* v3 = new QVBoxLayout( h1 );
  m_Encodebutton = new QPushButton( this, "PushButton_Encode" );
  m_Encodebutton->setText(i18n( "&Encode") );
  m_Encodebutton->setAutoRepeat( false );
  QWhatsThis::add( m_Encodebutton, i18n( "Start the portfolio MPEG encoding. "
                                         "The program uses the 'images2mpg' bash script. " ) );

  connect(m_Encodebutton, SIGNAL(clicked()),
          this, SLOT(slotEncode()));

  v3->addWidget( m_Encodebutton );
  v3->addStretch( 1 );

  // About data and help button.

  m_helpButton = new QPushButton( this, "HelpButton_Options" );
  m_helpButton->setText( i18n( "&Help") );
  v3->addWidget( m_helpButton );

  m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("MPEG Slideshow"),
                                      NULL,
                                      KAboutData::License_GPL,
                                      I18N_NOOP("A Kipi plugin for encoding images to an MPEG file."),
  "(c) 2003-2004, Gilles Caulier");

  m_about->addAuthor("Gilles Caulier", I18N_NOOP("Author"),
                    "caulier dot gilles at free.fr");

  m_about->addAuthor("Angelo Naselli", I18N_NOOP("Contributor"),
                    "anaselli at linux dot it");

  KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
  helpMenu->menu()->removeItemAt(0);
  helpMenu->menu()->insertItem(i18n("MPEG SlideShow Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
  m_helpButton->setPopup( helpMenu->menu() );

  // Options button.

  m_optionsbutton = new QPushButton( this, "PushButton_Settings" );
  m_optionsbutton->setText( i18n( "&Settings") );

  connect(m_optionsbutton, SIGNAL(clicked()),
          this, SLOT(slotOptions()));

  v3->addWidget( m_optionsbutton );

  // Quit push button.

  m_quitbutton = new QPushButton( this, "PushButton_Quit" );
  m_quitbutton->setText(i18n( "&Close") );
  m_quitbutton->setAutoRepeat( false );

  connect(m_quitbutton, SIGNAL(clicked()),
          this, SLOT(slotClose()));

  v3->addWidget( m_quitbutton );
  QWhatsThis::add( m_quitbutton, i18n( "Abort the current encoding and exit." ) );

  // Process messages frame.

  m_frame = new QLabel( this, "Process_Messages_frame" );
  m_frame->setMinimumHeight( 25 );
  m_frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  m_frame->setAlignment(AlignCenter|WordBreak|ExpandTabs);
  ml->addWidget( m_frame );
  QWhatsThis::add( m_frame, i18n( "Current encoding task." ) );

  // Progress bar.

  m_progress = new KProgress( this, "Progress" );
  m_progress->setMinimumHeight( 25 );
  ml->addWidget( m_progress );
  QWhatsThis::add( m_progress, i18n( "Encoding progress bar." ) );

  readSettings();

  int maxW = QMAX( m_VideoFormatComboBox->sizeHint().width(),
                   m_ChromaComboBox->sizeHint().width() );

  m_ChromaComboBox->setMinimumWidth( maxW ); 
  m_VideoFormatComboBox->setMinimumWidth( maxW );
  m_VideoTypeComboBox->setMinimumWidth( maxW );
  m_TransitionComboBox->setMinimumWidth( maxW );
}


/////////////////////////////// DESTRUCTOR //////////////////////////////////////////////////

KImg2mpgData::~KImg2mpgData()
{
  if ( m_thumbJob ) delete m_thumbJob;

  delete m_about;
}


///////////////////////////////////////// SLOTS /////////////////////////////////////////////

void KImg2mpgData::SlotPortfolioDurationChanged ( int )
{
  ShowNumberImages( m_ImagesFilesListBox->count() );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotAddDropItems(KURL::List filesUrl)
{
  addItems(filesUrl);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotMPEGFilenameDialog( void )
{
  QString temp;

  temp = KFileDialog::getOpenFileName(KGlobalSettings::documentPath(),
                                      QString( "*.mpg" ),
                                      this,
                                      i18n("Select MPEG Output File") );
  if( temp.isEmpty() )
    return;

  m_MPEGOutputEDITFilename->setText( temp );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotAudioFilenameDialog( void )
{
  QString temp;

  temp = KFileDialog::getOpenFileName(KGlobalSettings::documentPath(),
                                      QString( "*.wav *.mp2 *.mp3 *.ogg" ),
                                      this,
                                      i18n("Select Audio Input File") );
  if( temp.isEmpty() )
    return;

  m_AudioInputEDITFilename->setText( temp );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotImagesFilesButtonAdd( void )
{
    KURL::List ImageFilesList =
        KIPI::ImageDialog::getImageURLs( this, m_interface );
    if ( !ImageFilesList.isEmpty() )
        addItems( ImageFilesList );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotImagesFilesButtonDelete( void )
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


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotImagesFilesButtonUp( void )
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

/////////////////////////////////////////////////////////////////////////////////////////////


void KImg2mpgData::slotImagesFilesSelected( QListBoxItem *item )
{
  if ( !item || m_ImagesFilesListBox->count() == 0 )
      {
      m_label7->setText("");
      m_ImageLabel->clear();
      return;
      }

  ImageItem *pitem = static_cast<ImageItem*>( item );

  if ( !pitem ) return;

  KURL url;
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


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotGotPreview(const KFileItem*, const QPixmap &pixmap)
{
  m_ImageLabel->setPixmap(pixmap);
  m_thumbJob = 0L;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotFailedPreview(const KFileItem*)
{
  m_thumbJob = 0L;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotImagesFilesButtonDown( void )
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


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotClose()
{
  close();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotEncode( void )
{
  m_Proc = new KProcess;
  QColor BackGroundColor;
  QString HexColor;
  QString Temp;
  QString OutputFileName, InputAudioFileName;
  bool ResultOk;

  if (m_Encoding)
    {
    int Ret = KMessageBox::questionYesNo(this,
        i18n("Do you really abort this encoding process ?\n\n"
             "Warning: all work so-far will be lost."));
    if (Ret == KMessageBox::Yes)
      {
      m_Abort = true;
      reset();
      }
    return;
    }

  // Init. Tmp folder
  KStandardDirs dir;
  m_TmpFolderConfig = dir.saveLocation("tmp", "kipi-mpegencoderplugin-" +
                                       QString::number(getpid()) );

  m_DebugOuputMessages = "";
  m_DurationTime.start();
  InputAudioFileName = m_AudioInputEDITFilename->text();

  if (InputAudioFileName.isEmpty() == false && !QFile::exists(InputAudioFileName))
  {
    KMessageBox::error(this, i18n("You must specify an existing audio file."));
    return;
  }

  OutputFileName = m_MPEGOutputEDITFilename->text();
  QFileInfo fileInfo(OutputFileName);
  if (OutputFileName.isEmpty() || fileInfo.isDir())
    {
    KMessageBox::error(this, i18n("You must specify an MPEG output file name."));
    return;
    }

   QFileInfo dirInfo(fileInfo.dir().path());
   if (!dirInfo.exists () || !dirInfo.isWritable())
   {
     KMessageBox::error(this, i18n("You must specify a writable path for your output file."));
     return;
   }

  if (m_ImagesFilesListBox->count() == 0)
    {
    KMessageBox::error(this, i18n("You must specify some input images files in the portfolio."));
    return;
    }

  if (QFile::exists(OutputFileName))
    {
    int Ret=KMessageBox::questionYesNo(this, i18n("The output MPEG file '%1' already exists.\n"
                                                  "Do you want overwrite this file?").arg(OutputFileName));
    if (Ret == KMessageBox::No)
      return;
    }

  m_ImagesFilesListBox->clearSelection();
  m_Encodebutton->setText(i18n("A&bort"));
  QWhatsThis::add( m_Encodebutton, i18n( "Abort the portfolio MPEG encoding. "
                                         "Warning: all work so-far will be lost...." ) );

  m_optionsbutton->setEnabled(false);
  m_VideoFormatComboBox->setEnabled(false);
  m_VideoTypeComboBox->setEnabled(false);
  m_ChromaComboBox->setEnabled(false);
  m_DurationImageSpinBox->setEnabled(false);
  m_TransitionComboBox->setEnabled(false);
  m_MPEGOutputEDITFilename->setEnabled(false);
  m_MPEGOutputBUTTONFilename->setEnabled(false);
  m_BackgroundColorButton->setEnabled(false);
  m_AudioInputEDITFilename->setEnabled(false);
  m_AudioInputBUTTONFilename->setEnabled(false);
  m_ImagesFilesListBox->setEnabled(false);
  m_ImagesFilesButtonBox->setEnabled(false);

  m_Abort = false;
  m_Encoding = true;

  m_progress->setTotalSteps(100);
  m_progress->setValue(0);

  BackGroundColor = m_BackgroundColorButton->color();
  HexColor = "";
  Temp.setNum (BackGroundColor.red(), 16);
  HexColor = HexColor + Temp.rightJustify(2,'0');
  Temp.setNum (BackGroundColor.green(), 16);
  HexColor = HexColor + Temp.rightJustify(2,'0');
  Temp.setNum (BackGroundColor.blue(), 16);
  HexColor = HexColor + Temp.rightJustify(2,'0');

  // This is for debuging output in debug dialog box.

  m_CommandLine = i18n("THE COMMAND LINE IS :\n\n");
  m_CommandLine = m_CommandLine + "images2mpg --with-gui ";

  *m_Proc << "images2mpg";                                        // The script...
  *m_Proc << "--with-gui";                                        // Running with the GUI.
  *m_Proc << "-f" << m_VideoFormatComboBox->currentText();        // Video format option.
  m_CommandLine = m_CommandLine + " -f " + m_VideoFormatComboBox->currentText();

  *m_Proc << "-n" << m_VideoTypeComboBox->currentText();          // Video type option.
  m_CommandLine = m_CommandLine + " -n " + m_VideoTypeComboBox->currentText();

  if (m_ChromaComboBox->currentText() != "Default")
  {
    *m_Proc << "-S" << m_ChromaComboBox->currentText();          // Chroma subsampling mode option.
    m_CommandLine = m_CommandLine + " -S " + m_ChromaComboBox->currentText();
  }

  *m_Proc << "-d" << m_DurationImageSpinBox->text();              // Image duration.
  m_CommandLine = m_CommandLine + " -d " + m_DurationImageSpinBox->text();

  m_TransitionComboBox->currentText().toInt(&ResultOk);

  if (ResultOk == true)
      {
      *m_Proc << "-t" << m_TransitionComboBox->currentText();     // Transitions duration.
      m_CommandLine = m_CommandLine + " -t " + m_TransitionComboBox->currentText();
      }

  *m_Proc << "-c" << HexColor;                                    // Background color.
  m_CommandLine = m_CommandLine + " -c " + HexColor;

  *m_Proc << "-T" << m_TmpFolderConfig;                           // Temporary folder.
  m_CommandLine = m_CommandLine + " -T " + m_TmpFolderConfig;

  *m_Proc << "-M" << m_MJBinFolderConfig;                         // MJPEGTools binary folder.
  m_CommandLine = m_CommandLine + " -M " + m_MJBinFolderConfig;

  *m_Proc << "-I" << m_IMBinFolderConfig;                         // ImageMagick binary folder.
  m_CommandLine = m_CommandLine + " -I " + m_IMBinFolderConfig;


  if ( InputAudioFileName.isEmpty() == false )
    {
    if ( InputAudioFileName.findRev(".mp2", -1, false) == -1 )
      {
      *m_Proc << "-w" << InputAudioFileName;                      // Input WAV/OGG/MP3 audio file name.
      m_CommandLine = m_CommandLine + " -w \"" + InputAudioFileName + "\"";
      }
    else
      {
      *m_Proc << "-a" << InputAudioFileName;                      // Input MP2 audio file name.
      m_CommandLine = m_CommandLine + " -a \"" + InputAudioFileName + "\"";
      }
    }

  *m_Proc << "-o" << OutputFileName;                              // Output MPEG file.
  m_CommandLine = m_CommandLine + " -o \"" + OutputFileName + "\"";

  *m_Proc << "-i";                                                // Input images option.
  m_CommandLine = m_CommandLine + " -i ";

  for (uint i=0 ; i < m_ImagesFilesListBox->count() ; ++i)
    {
    QString FileName="";
    ImageItem *pitem = static_cast<ImageItem*>( m_ImagesFilesListBox->item(i) );
    FileName.append (pitem->path());                              // Input images files.
    *m_Proc << FileName;
    m_CommandLine = m_CommandLine + " \"" + FileName + "\" ";
    }

  connect(m_Proc, SIGNAL(processExited(KProcess *)),this,
          SLOT(EncodeDone(KProcess*)));

  connect(m_Proc, SIGNAL(receivedStderr(KProcess *,char*,int)),this,
          SLOT(readStderr(KProcess*,char*,int)));

  bool result = m_Proc->start(KProcess::NotifyOnExit , KProcess::All);

  if(!result)
    {
    QString str = i18n("Cannot start 'images2mpg' bash script : fork failed.");
    KMessageBox::error(this, str);
    reset();
    }

  m_Img2mpgPidNum = m_Proc->pid();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotOptions( void )
{
  m_OptionDlg = new OptionsDialog(this);
  m_OptionDlg->IMBinFolderEditFilename->setText(m_IMBinFolderConfig);
  m_OptionDlg->MJBinFolderEditFilename->setText(m_MJBinFolderConfig);
  m_OptionDlg->show();

  connect( m_OptionDlg, SIGNAL( okClicked () ),
           this, SLOT( slotOptionDlgOkClicked() ));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotOptionDlgOkClicked( void )
{
  m_Encodebutton->setEnabled(true);

  m_IMBinFolderConfig = m_OptionDlg->IMBinFolderEditFilename->text();

  if (m_IMBinFolderConfig.endsWith("/"))
    m_IMBinFolderConfig.truncate(m_IMBinFolderConfig.length()-1);

  m_MJBinFolderConfig = m_OptionDlg->MJBinFolderEditFilename->text();

  if (m_MJBinFolderConfig.endsWith("/"))
    m_MJBinFolderConfig.truncate(m_MJBinFolderConfig.length()-1);

  writeSettings();
  CheckBinProg* CheckExternalPrograms = new CheckBinProg(this);
  int ValRet = CheckExternalPrograms->findExecutables();

  m_Encodebutton->setEnabled(true);
  m_AudioInputFilename->setEnabled(true);

  if (ValRet == 0)
      m_Encodebutton->setEnabled(false);

  if (ValRet == 2)
      m_AudioInputFilename->setEnabled(false);

  disconnect( m_OptionDlg, SIGNAL( okClicked() ),
              this, SLOT( slotOptionDlgOkClicked() ));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::slotHelp()
{
    KApplication::kApplication()->invokeHelp("mpegencoder",
                                             "kipi-plugins");
}


//////////////////////////////  FONCTIONS ///////////////////////////////////////////////////

void KImg2mpgData::ShowNumberImages( int Number )
{
  QTime TotalDuration (0, 0, 0);
  bool ResultOk;
  int TransitionDuration;

  int DurationImage = m_DurationImageSpinBox->text().toInt();
  int TransitionSpeed = m_TransitionComboBox->currentText().toInt(&ResultOk);

  if (ResultOk == false)
     TransitionDuration = 0;
  else
    {
    QString VidFormat=m_VideoTypeComboBox->currentText();

    if (VidFormat == "NTSC")
      TransitionDuration = (int)((((float)100 / (float)TransitionSpeed) / (float)30) * (float)1000);  // in ms
    else
      TransitionDuration = (int)((((float)100 / (float)TransitionSpeed) / (float)25) * (float)1000);  // in ms
    }

  TotalDuration = TotalDuration.addSecs(Number*DurationImage);
  TotalDuration = TotalDuration.addMSecs((Number+1)*TransitionDuration);

  if ( Number < 2)
    m_label6->setText(i18n("%1 image [%2]").arg(Number).arg(TotalDuration.toString()));
  else
    m_label6->setText(i18n("%1 images [%2]").arg(Number).arg(TotalDuration.toString()));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::readStderr(KProcess *, char *buffer, int buflen)
{
  QString NewString2;
  int ImgNum;
  m_EncodeString = QString::fromLocal8Bit(buffer, buflen);

  QString BufferTmp = buffer;

  m_DebugOuputMessages.append(BufferTmp.left(buflen));

  if (m_EncodeString.contains("Images encoding (%) :"))
    {
    int pos1 = m_EncodeString.find(':');

    if (pos1 != -1)
      {
      QString newstring = m_EncodeString.mid(pos1+1, 4);
      m_progress->setValue(newstring.toUInt());
      }

    int pos2 = m_EncodeString.find('[');

    if (pos2 != -1)
      {
      NewString2 = m_EncodeString.mid(pos2+1, 4);
      ImgNum = NewString2.toInt();

      if (ImgNum == 0)
        ImgNum = 1;

      m_frame->setText(i18n("Encoding image file [%1/%2]...")
                       .arg(ImgNum).arg(m_ImagesFilesListBox->count()));

      if (ImgNum > 1)
        m_ImagesFilesListBox->setSelected(ImgNum-2, false);

      m_ImagesFilesListBox->setSelected(ImgNum-1, true);
      m_ImagesFilesListBox->setCurrentItem(ImgNum-1);
      }
    }
  else
    {
    // Print on the GUI the actual 'images2mpg' process.

    if (m_EncodeString.contains("Initialising..."))
      m_frame->setText(i18n("Initialising..."));

    if (m_EncodeString.contains("Merging MPEG flux..."))
      {
      m_frame->setText(i18n("Merging MPEG flux..."));
      m_progress->setValue(100);
      }

    if (m_EncodeString.contains("Encoding audio file..."))
      {
      m_frame->setText(i18n("Encoding audio file..."));
      m_progress->setValue(100);
      }

    // Or errors detections...

    if (m_EncodeString.contains("cat:") ||
        m_EncodeString.contains("ERROR:") ||
        m_EncodeString.contains("Broken pipe") ||
        m_EncodeString.contains("No such file or directory"))
      {

      m_Abort = true;
      reset();
      int Ret=KMessageBox::warningYesNo(this,
                                 i18n("The 'images2mpg' script has returned an error during the MPEG encoding;\n"
                                      "the process has been aborted.\n\n"
                                      "Send a mail to the author..."),
                                 i18n("'images2mpg' Script-Execution Problem"),
                                 i18n("&OK"),
                                 i18n("Show Debugging Output"));
      if (Ret == KMessageBox::No)
        {
        m_DebuggingDialog = new KShowDebuggingOutput(m_DebugOuputMessages, m_CommandLine,
                            i18n ("\nEXIT STATUS : error during encoding process."), this);
        m_DebuggingDialog->exec();
        }
      }
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::EncodeDone(KProcess*)
{
  reset();

  int msecDur = m_DurationTime.elapsed();
  m_EncodingDuration.setHMS(0, 0, 0);
  QTime Duration = m_EncodingDuration.addMSecs (msecDur);
  QString Encoding = Duration.toString("hh:mm:ss");

  if ( m_Abort == false )
    {
    m_frame->setText(i18n("Encoding terminated..."));
    int Ret=KMessageBox::warningYesNo(this,
                         i18n("The encoding process has terminated...\n\n"
                         "Encoding duration: %1").arg(Encoding),
                         i18n("'images2mpg' Script Execution Terminated"),
                         i18n("&OK"),
                         i18n("Show Process Messages"));

    if (Ret == KMessageBox::No)
      {
      m_DebuggingDialog = new KShowDebuggingOutput(m_DebugOuputMessages, m_CommandLine,
                          i18n ("\nEXIT STATUS : encoding process finished successfully."),
                          this);
      m_DebuggingDialog->exec();
      }
    }
  else
    {
    m_frame->setText(i18n("Encoding aborted..."));
    int Ret=KMessageBox::warningYesNo(this,
                         i18n("The encoding process has been aborted...\n\n"
                         "Encoding duration: %1").arg(Encoding),
                         i18n("'images2mpg' Script Execution Aborted"),
                         i18n("&OK"),
                         i18n("Show Process Messages"));

    if (Ret == KMessageBox::No)
      {
      m_DebuggingDialog = new KShowDebuggingOutput(m_DebugOuputMessages, m_CommandLine,
                        i18n ("\nEXIT STATUS : encoding process aborted by user."), this);
      m_DebuggingDialog->exec();
      }
    }

  RemoveTmpFiles();
}


/////////////////////////////////////////////////////////////////////////////////////////////

QPixmap KImg2mpgData::LoadIcon( QString Name, int Group )
{
  return m_Icons->loadIcon( Name, (KIcon::Group)Group );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::closeEvent(QCloseEvent* e)
{
    if (!e) return;

    if (m_Encoding)
    {
        int Ret =
            KMessageBox::questionYesNo(this, i18n("An encoding process is active;\n"
                                                  "abort this process and exit ?"));
        if (Ret == KMessageBox::Yes)
        {
            m_Abort = true;
            reset();
        }
        else {
            e->ignore();
            return;
        }
    }

    RemoveTmpFiles();
    writeSettings();
    e->accept();
}


/////////////////////////////////////////////////////////////////////////////////////////////


void KImg2mpgData::show()
{
  setCaption(i18n("Create MPEG Slideshow"));
  KDialog::show();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::reset()
  {
  m_Encoding = false;

  if (m_Proc)
    {
        ::kill(m_Img2mpgPidNum, SIGKILL);
    }

  delete m_Proc;
  m_Proc = 0L;

  m_progress->setValue(0);
  m_frame->clear();

  m_Encodebutton->setText(i18n("&Encode"));
  m_optionsbutton->setEnabled(true);
  m_VideoFormatComboBox->setEnabled(true);
  m_ChromaComboBox->setEnabled(true);
  m_VideoTypeComboBox->setEnabled(true);
  m_DurationImageSpinBox->setEnabled(true);
  m_TransitionComboBox->setEnabled(true);
  m_MPEGOutputEDITFilename->setEnabled(true);
  m_MPEGOutputBUTTONFilename->setEnabled(true);
  m_BackgroundColorButton->setEnabled(true);
  m_AudioInputEDITFilename->setEnabled(true);
  m_AudioInputBUTTONFilename->setEnabled(true);
  m_ImagesFilesListBox->setEnabled(true);
  m_ImagesFilesButtonBox->setEnabled(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::writeSettings()
{
  m_config = new KConfig("kipirc");
  m_config->setGroup("MPEGEncoder Settings");

  m_VideoFormatConfig = m_VideoFormatComboBox->currentText();
  m_config->writeEntry("VideoFormat", m_VideoFormatConfig);

  m_VideoTypeConfig = m_VideoTypeComboBox->currentText();
  m_config->writeEntry("VideoType", m_VideoTypeConfig);

  m_ChromaConfig = m_ChromaComboBox->currentText();
  m_config->writeEntry("ChromaMode", m_ChromaConfig);

  m_ImageDurationConfig = m_DurationImageSpinBox->text();
  m_config->writeEntry("ImageDuration", m_ImageDurationConfig);

  m_TransitionSpeedConfig = m_TransitionComboBox->currentText();
  m_config->writeEntry("TransitionSpeed", m_TransitionSpeedConfig);

  m_BackgroundColorConfig = m_BackgroundColorButton->color();
  m_config->writeEntry("BackgroundColor", m_BackgroundColorConfig);

  m_AudioInputFileConfig = m_AudioInputEDITFilename->text();
  m_config->writePathEntry("AudioInputFile", m_AudioInputFileConfig);

  m_MPEGOutputFileConfig = m_MPEGOutputEDITFilename->text();
  m_config->writePathEntry("MPEGOutputFile", m_MPEGOutputFileConfig);

  m_config->writePathEntry("ImageMagickBinFolder", m_IMBinFolderConfig);
  m_config->writePathEntry("MjpegToolsBinFolder", m_MJBinFolderConfig);

  m_config->sync();
  delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::readSettings()
{
  m_config = new KConfig("kipirc");
  m_config->setGroup("MPEGEncoder Settings");

  m_VideoFormatConfig = m_config->readEntry("VideoFormat", "XVCD");

  for (int i = 0 ; i < m_VideoFormatComboBox->count() ; ++i)
    if ( (QString) m_VideoFormatComboBox->text(i) == m_VideoFormatConfig)
      m_VideoFormatComboBox->setCurrentItem(i);

  m_VideoTypeConfig = m_config->readEntry("VideoType", "PAL");

  for (int i = 0 ; i < m_VideoTypeComboBox->count() ; ++i)
    if ( (QString) m_VideoTypeComboBox->text(i) == m_VideoTypeConfig)
      m_VideoTypeComboBox->setCurrentItem(i);

  m_ChromaConfig = m_config->readEntry("ChromaMode", "420mpeg2");

  for (int i = 0 ; i < m_ChromaComboBox->count() ; ++i)
    if ( (QString) m_ChromaComboBox->text(i) == m_ChromaConfig)
      m_ChromaComboBox->setCurrentItem(i);

  m_ImageDurationConfig = m_config->readEntry("ImageDuration", "10");
  m_DurationImageSpinBox->setValue(m_ImageDurationConfig.toInt());

  m_TransitionSpeedConfig = m_config->readEntry("TransitionSpeed", m_NoneLabel);

  for (int i = 0 ; i < m_TransitionComboBox->count() ; ++i)
    if ( (QString) m_TransitionComboBox->text(i) == m_TransitionSpeedConfig)
      m_TransitionComboBox->setCurrentItem(i);

  QColor *ColorB = new QColor( 0, 0, 0 );
  m_BackgroundColorConfig = m_config->readColorEntry("BackgroundColor", ColorB);
  m_BackgroundColorButton->setColor(m_BackgroundColorConfig);

  m_AudioInputFileConfig = m_config->readPathEntry("AudioInputFile");
  m_AudioInputEDITFilename->setText(m_AudioInputFileConfig);

  m_MPEGOutputFileConfig = m_config->readPathEntry("MPEGOutputFile", KGlobalSettings::documentPath() + "output.mpg");
  m_MPEGOutputEDITFilename->setText(m_MPEGOutputFileConfig);

  m_IMBinFolderConfig = m_config->readPathEntry("ImageMagickBinFolder", "/usr/bin");
  m_MJBinFolderConfig = m_config->readPathEntry("MjpegToolsBinFolder", "/usr/bin");

  delete ColorB;
  delete m_config;

  // Get the image files filters from the hosts app.

  m_ImagesFilesSort = m_interface->fileExtensions();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::RemoveTmpFiles(void)
{
    QDir tmpFolder(m_TmpFolderConfig);

    if (m_TmpFolderConfig.isEmpty() != true && tmpFolder.exists() == true)
       if (DeleteDir(m_TmpFolderConfig) == false)
          KMessageBox::error(this, i18n("Cannot remove temporary folder %1!").arg(m_TmpFolderConfig));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KImg2mpgData::addItems(const KURL::List& fileList)
{
    if (fileList.isEmpty()) return;

    KURL::List Files = fileList;

    for ( KURL::List::Iterator it = Files.begin() ; it != Files.end() ; ++it )
      {
      KURL currentFile = *it;

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


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool KImg2mpgData::DeleteDir(QString dirname)
{
if ( !dirname.isEmpty() )
    {
    QDir dir;

    if (dir.exists ( dirname ) == true)
       {
       if (deldir(dirname) == false)
           return false;

       if (dir.rmdir( dirname ) == false )
           return false;
       }
    else
       return false;
    }
else
    return false;

return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool KImg2mpgData::deldir(QString dirname)
{
QDir *dir = new QDir(dirname);
dir->setFilter ( QDir::Dirs | QDir::Files | QDir::NoSymLinks );

const QFileInfoList* fileinfolist = dir->entryInfoList();
QFileInfoListIterator it(*fileinfolist);
QFileInfo* fi;

while( (fi = it.current() ) )
     {
     if(fi->fileName() == "." || fi->fileName() == ".." )
          {
          ++it;
          continue;
          }

     if( fi->isDir() )
          {
          if (deldir( fi->absFilePath() ) == false)
              return false;
          if (dir->rmdir( fi->absFilePath() ) == false)
              return false;
          }
     else
          if( fi->isFile() )
               if (dir->remove(fi->absFilePath() ) == false)
                   return false;

     kapp->processEvents();
     ++it;
     }

return true;
}

}  // NameSpace KIPIMPEGEncoderPlugin

