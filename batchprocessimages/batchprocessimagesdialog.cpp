//////////////////////////////////////////////////////////////////////////////
//
//    BATCHPROCESSIMAGESDIALOG.CPP
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
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

// Batch process images dialog implementation template. Fo more informations, read the header file.

// C Ansi includes

extern "C"
{
#include <unistd.h>
}

// Include files for Qt

#include <qvbox.h>
#include <qlayout.h>
#include <qdir.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qprocess.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qimage.h>
#include <qevent.h>
#include <qdragobject.h>
#include <qfileinfo.h>

// Include files for KDE

#include <kstandarddirs.h>
#include <kcolorbutton.h>
#include <klocale.h>
#include <kprogress.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kdialogbase.h>
#include <klistview.h>
#include <kimageio.h>
#include <kprocess.h>
#include <klineeditdlg.h>
#include <kio/jobclasses.h>
#include <kio/netaccess.h>
#include <kio/global.h>
#include <kbuttonbox.h>

// KIPI includes
#include <libkipi/thumbnailjob.h>

// Local includes

#include "batchprocessimagesdialog.h"
#include "outputdialog.h"
#include "imagepreview.h"
#include <kdiroperator.h>

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

BatchProcessImagesDialog::BatchProcessImagesDialog( KURL::List urlList, KIPI::Interface* interface, QWidget *parent )
    : KDialogBase( parent, "BatchProcessImagesDialog", false,
                   "", Help|User1|User2|Cancel,
                   Cancel, true, i18n("&About"), i18n("&Start")), m_selectedImageFiles( urlList), m_interface( interface )
{
    // Init. Tmp folder

    KStandardDirs dir;
    m_tmpFolder = dir.saveLocation("tmp");

    m_convertStatus = NO_PROCESS;
    m_progressStatus = 0;
    m_ProcessusProc = 0L;
    m_PreviewProc = 0L;

    KImageIO::registerFormats();

    QWidget* box = new QWidget( this );
    setMainWidget(box);
    QVBoxLayout *dvlay = new QVBoxLayout( box, 10, spacingHint() );

    //---------------------------------------------

    groupBox1 = new QGroupBox( 2, Qt::Horizontal, box );

    m_labelType = new QLabel( groupBox1 );

    m_Type = new QComboBox(false, groupBox1);

    m_labelType->setBuddy( m_Type );

    m_optionsButton = new QPushButton (groupBox1, "OptionButton");
    m_optionsButton->setText(i18n("Options"));
    QWhatsThis::add( m_optionsButton, i18n("<p>You can choose here the current options type "
                                           "using for the process."));

    m_previewButton = new QPushButton (groupBox1, "PreviewButton");
    m_previewButton->setText(i18n("&Preview"));
    QWhatsThis::add( m_previewButton, i18n("<p>You can build with this button an process "
                                           "preview for the current selected image in the list."));

    m_smallPreview = new QCheckBox(i18n("Small preview"), groupBox1);
    QWhatsThis::add( m_smallPreview, i18n("<p>If you enable this option, "
                                          "all preview effects will be calculed on a small zone "
                                          "of the image (300x300 pixels on the top left corner). "
                                          "Enabled this option if you have a slow computer.") );
    m_smallPreview->setChecked( true );

    dvlay->addWidget( groupBox1 );

    //---------------------------------------------

    QString title;
    if ( m_interface->hasFeature( KIPI::AlbumEQDir ) )
        title = i18n("Target Album");
    else
        title = i18n("Target Directory");

    groupBox3 = new QGroupBox( 2, Qt::Horizontal, title, box );

    m_albumList = new QComboBox( false, groupBox3 );
    QStringList albumsList;


#ifdef TEMPORARILY_REMOVED
    for (Digikam::AlbumInfo *album=Digikam::AlbumManager::instance()->firstAlbum() ;
         album ; album = album->nextAlbum())
        {
        album->openDB();
        albumsList.append (album->getTitle());
        album->closeDB();
        }
#endif

    albumsList.sort();
    m_albumList->insertStringList(albumsList);

#ifdef TEMPORARILY_REMOVED
    if ( Digikam::AlbumManager::instance()->currentAlbum() )
       m_albumList->setCurrentText (Digikam::AlbumManager::instance()->currentAlbum()->getTitle());
#endif

    QWhatsThis::add( m_albumList, i18n("<p>Select here the target Album used by the process.") );

    m_addNewAlbumButton = new QPushButton (groupBox3, "PushButton_AddNewAlbum");
    m_addNewAlbumButton->setText(i18n( "&Add new album") );
    m_addNewAlbumButton->setAutoRepeat( false );
    QWhatsThis::add( m_addNewAlbumButton, i18n( "Adding a new Album in the Digikam Albums library."));

    dvlay->addWidget( groupBox3 );

    //---------------------------------------------

    groupBox2 = new QGroupBox( 2, Qt::Horizontal, i18n("Files operations"), box );

    m_labelOverWrite = new QLabel (i18n("Overwrite mode:"), groupBox2);
    m_overWriteMode = new QComboBox( false, groupBox2 );
    m_overWriteMode->insertItem(i18n("Ask"));
    m_overWriteMode->insertItem(i18n("Always OverWrite"));
    m_overWriteMode->insertItem(i18n("Rename"));
    m_overWriteMode->insertItem(i18n("Skip"));
    m_overWriteMode->setCurrentText (i18n("Rename"));
    QWhatsThis::add( m_overWriteMode, i18n("<p>Select here the overwrite mode if your targets image "
                                           "files already exist.") );

    m_removeOriginal = new QCheckBox(i18n("Remove original"), groupBox2);
    QWhatsThis::add( m_removeOriginal, i18n("<p>If you enable this option, "
                                            "all originals images files will be removed.") );
    m_removeOriginal->setChecked( false );

    dvlay->addWidget( groupBox2 );

    //---------------------------------------------

    groupBox4 = new QGroupBox( 2, Qt::Horizontal, box );

    m_listFiles = new BatchProcessImagesList( groupBox4 );

    groupBox41 = new QGroupBox( 3, Qt::Vertical, groupBox4 );
    groupBox41->setLineWidth( 0 );
    m_addImagesButton = new QPushButton ( i18n( "&Add" ), groupBox41 );
    QWhatsThis::add( m_addImagesButton, i18n("<p>Add images to the list.") );
    m_remImagesButton = new QPushButton ( i18n( "&Remove" ), groupBox41 );
    QWhatsThis::add( m_remImagesButton, i18n("<p>Remove selected image on the list.") );

    m_imageLabel = new QLabel( groupBox41 );
    m_imageLabel->setFixedHeight( 80 );
    m_imageLabel->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_imageLabel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    QWhatsThis::add( m_imageLabel, i18n( "<p>The preview of the selected image in the list." ) );

    groupBox4->setMaximumHeight( 200 );
    dvlay->addWidget( groupBox4 );

    m_statusbar = new QLabel( box, "ProcessMessagesFrame" );
    m_statusbar->setMinimumHeight( 25 );
    m_statusbar->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_statusbar->setAlignment(AlignCenter|WordBreak|ExpandTabs);
    QWhatsThis::add( m_statusbar, i18n("<p>This is the current active task.") );

    dvlay->addWidget( m_statusbar );

    m_progress = new KProgress( box, "Progress" );
    m_progress->setMinimumHeight( 20 );
    m_progress->setRange(0, 100);
    m_progress->setValue(0);
    QWhatsThis::add( m_progress, i18n("<p>This is the current percent task released.") );

    dvlay->addWidget( m_progress );

    //---------------------------------------------

    connect(m_addNewAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotAddNewAlbum()));

    connect(m_listFiles, SIGNAL(doubleClicked(QListViewItem *)),
            this, SLOT(slotListDoubleClicked(QListViewItem *)));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotAbout()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotProcessStart()));

    connect(m_optionsButton, SIGNAL(clicked()),
            this, SLOT(slotOptionsClicked()));

    connect(m_previewButton, SIGNAL(clicked()),
            this, SLOT(slotPreview()));

    connect(m_overWriteMode, SIGNAL(activated(const QString &)),
            this, SLOT(slotOverWriteModeActived(const QString &)));

    connect(m_Type, SIGNAL(activated(const QString &)),
            this, SLOT(slotTypeChanged(const QString &)));

    connect(m_listFiles, SIGNAL( addedDropItems(QStringList) ),
            this, SLOT( slotAddDropItems(QStringList)));

    connect(m_listFiles, SIGNAL( currentChanged( QListViewItem * ) ),
            this, SLOT( slotImageSelected( QListViewItem * )));

    connect(m_addImagesButton, SIGNAL(clicked()),
            this, SLOT(slotImagesFilesButtonAdd()));

    connect(m_remImagesButton, SIGNAL(clicked()),
            this, SLOT(slotImagesFilesButtonRem()));

    // Read File Filter settings in digikamrc file.

    m_config = new KConfig("digikamrc");
    m_config->setGroup("Album Settings");
    m_ImagesFilesSort = m_config->readEntry("File Filter", "*.jpg *.jpeg *.tif *.tiff *.gif *.png *.bmp");

    delete m_config;
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

BatchProcessImagesDialog::~BatchProcessImagesDialog()
{
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void BatchProcessImagesDialog::slotImagesFilesButtonAdd( void )
{
    QStringList ImageFilesList;

#ifdef TEMPORARILY_REMOVED
    ImageFilesList = KFileDialog::getOpenFileNames( Digikam::AlbumManager::instance()->getLibraryPath(),
                                                    m_ImagesFilesSort,
                                                    this );
#endif

    slotAddDropItems(ImageFilesList);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotImagesFilesButtonRem( void )
{
    BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*>( m_listFiles->currentItem() );

    if ( pitem )
        {
        m_listFiles->takeItem(pitem);
        m_listFiles->setSelected( m_listFiles->currentItem(), true );
        m_selectedImageFiles.remove(m_selectedImageFiles.find(pitem->pathSrc()));
        delete pitem;
        m_nbItem = m_selectedImageFiles.count();

        if (m_nbItem == 0) groupBox4->setTitle(i18n("Image files list"));
        else
           groupBox4->setTitle(i18n("Image files list (1 item)", "Image files list (%n items)", m_nbItem));
        }
}


/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotImageSelected( QListViewItem * item )
{
    if (!m_thumbJob.isNull())
       delete m_thumbJob;

    if ( !item || m_listFiles->childCount() == 0 )
       {
       m_imageLabel->clear();
       return;
       }

    BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*>( item );
    if ( !pitem ) return;

    m_imageLabel->clear();

    QString IdemIndexed = "file:" + pitem->pathSrc();

    KURL url(IdemIndexed);

    m_thumbJob = new KIPI::ThumbnailJob( url, m_imageLabel->height(), false, true );

    connect(m_thumbJob, SIGNAL(signalThumbnail(const KURL&, const QPixmap&)),
            SLOT(slotGotPreview(const KURL&, const QPixmap&)));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotGotPreview(const KURL &url, const QPixmap &pixmap)
{
    m_imageLabel->setPixmap(pixmap);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotAddDropItems(QStringList filesPath)
{
    if (filesPath.isEmpty()) return;

    for ( QStringList::Iterator it = filesPath.begin() ; it != filesPath.end() ; ++it )
      {
      QString currentDropFile = *it;

      // Check if the new item already exist in the list.

      bool findItem = false;

      for ( KURL::List::Iterator it2 = m_selectedImageFiles.begin() ; it2 != m_selectedImageFiles.end() ; ++it2 )
         {
             QString currentFile = (*it2).path(); // PENDING(blackie) Handle URL's

         if ( currentFile == currentDropFile )
             findItem = true;
         }

      if (findItem == false)
         m_selectedImageFiles.append(currentDropFile);
      }

    listImageFiles();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::closeEvent ( QCloseEvent *e )
{
    if (!e) return;

    if ( m_PreviewProc != 0L )
       if ( m_PreviewProc->isRunning() ) m_PreviewProc->kill(SIGTERM);

    if ( m_ProcessusProc != 0L )
       if ( m_ProcessusProc->isRunning() ) m_ProcessusProc->kill(SIGTERM);

    e->accept();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotOverWriteModeActived( const QString &string )
{
    if ( string == i18n("Always OverWrite") )
       {
       m_removeOriginal->setEnabled(false);
       m_removeOriginal->setChecked(false);
       }
    else
       m_removeOriginal->setEnabled(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotAddNewAlbum( void )
{
    qDebug("BatchProcessImagesDialog::slotAddNewAlbum commented out");
#ifdef TEMPORARILY_REMOVED
    QDir libraryDir( Digikam::AlbumManager::instance()->getLibraryPath());

    if (!libraryDir.exists())
        {
        KMessageBox::error(this, i18n("Album Library has not been set correctly\n"
                                   "Please run Setup"));
        return;
        }

    bool ok;
    m_newDir = KLineEditDlg::getText(i18n("Enter New Album Name: "), "", &ok, this);
    if (!ok) return;

    KURL newAlbumURL(Digikam::AlbumManager::instance()->getLibraryPath());
    newAlbumURL.addPath(m_newDir);

    KIO::SimpleJob* job = KIO::mkdir(newAlbumURL);

    connect(job, SIGNAL(result(KIO::Job*)),
            this, SLOT(slot_onAlbumCreate(KIO::Job*)));
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slot_onAlbumCreate(KIO::Job* job)
{
    if (job->error())
        {
        job->showErrorDialog(this);
        }
    else
        {
        m_albumList->insertStringList(m_newDir, -1);
        m_albumList->setCurrentText (m_newDir);
        }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotProcessStart( void )
{
    if ( m_selectedImageFiles.isEmpty() == true )
       return;

    if ( m_removeOriginal->isChecked() == true )
        {
        if ( KMessageBox::warningContinueCancel(this,
             i18n("All original images files will be removed from the source Album!\nDo you want to continue?"),
             i18n("Delete original images files"), KStdGuiItem::cont(),
             "Digikamplugin-BatchProcessImages-AlwaysRemomveOriginalFiles") != KMessageBox::Continue )
           return;
        }

    m_statusbar->setText(i18n("Process under progress. Please wait..."));
    m_convertStatus = UNDER_PROCESS;
    disconnect( this, SIGNAL(user2Clicked()), this, SLOT(slotProcessStart()));
    showButtonCancel( false );
    setButtonText( User2, i18n("&Stop") );
    connect(this, SIGNAL(user2Clicked()), this, SLOT(slotProcessStop()));

    m_labelType->setEnabled(false);
    m_Type->setEnabled(false);
    m_optionsButton->setEnabled(false);
    m_previewButton->setEnabled(false);
    m_smallPreview->setEnabled(false);

    m_labelOverWrite->setEnabled(false);
    m_overWriteMode->setEnabled(false);
    m_removeOriginal->setEnabled(false);

    m_addNewAlbumButton->setEnabled(false);
    m_albumList->setEnabled(false);
    m_addImagesButton->setEnabled(false);
    m_remImagesButton->setEnabled(false);

    m_listFile2Process_iterator = new QListViewItemIterator( m_listFiles );
    startProcess();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool BatchProcessImagesDialog::startProcess(void)
{
    qDebug("BatchProcessImagesDialog::startProcess commented out");
#ifdef TEMPORARILY_REMOVED
    if ( m_convertStatus == STOP_PROCESS )
       {
       endProcess(i18n("Process aborted by user!"));
       return true;
       }

    Digikam::AlbumInfo *targetAlbum = Digikam::AlbumManager::instance()->findAlbum( m_albumList->currentText() );
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
    m_listFiles->setCurrentItem(item);

    if ( prepareStartProcess(item, targetAlbum) == false ) // If there is a problem during the
       {                                                   // preparation -> pass to the next item!
       ++*m_listFile2Process_iterator;
       ++m_progressStatus;
       m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));
       item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
       m_listFiles->setCurrentItem(item);

       if ( m_listFile2Process_iterator->current() )
          {
          startProcess();
          return true;
          }
       else
          {
          endProcess(i18n("Process finished!"));
          return true;
          }
       }

    KURL desturl(targetAlbum->getPath() + "/" + item->nameDest());

    if ( KIO::NetAccess::exists(desturl) == true )
       {
       switch (overwriteMode())
          {
          case OVERWRITE_ASK:
             {
             int ValRet = KMessageBox::warningYesNoCancel(this,
                          i18n("The destination file \"%1\" already exists!\n"
                          "Do you want overwrite it?").arg(item->nameDest()),
                          i18n("Overwrite destination image file"), KStdGuiItem::cont());

             if ( ValRet == KMessageBox::No )
                {
                item->changeResult(i18n("Skipped !"));
                item->changeError(i18n("destination image file already exists (skipped by user)!"));
                ++*m_listFile2Process_iterator;
                ++m_progressStatus;
                m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

                if ( m_listFile2Process_iterator->current() )
                   {
                   startProcess();
                   return true;
                   }
                else
                   {
                   endProcess(i18n("Process finished!"));
                   return true;
                   }
                }

             if ( ValRet == KMessageBox::Cancel )
                {
                processAborted(false);
                return false;
                }

             break;
             }

          case OVERWRITE_RENAME:
             {
             QFileInfo *Target = new QFileInfo(targetAlbum->getPath() + "/" + item->nameDest());
             QString newFileName = RenameTargetImageFile(Target);

             if ( newFileName == QString::null )
                {
                item->changeResult(i18n("Failed !"));
                item->changeError(i18n("destination image file already exists and cannot be renamed!"));
                ++*m_listFile2Process_iterator;
                ++m_progressStatus;
                m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

                if ( m_listFile2Process_iterator->current() )
                   {
                   startProcess();
                   return true;
                   }
                else
                   {
                   endProcess(i18n("Process finished!"));
                   return true;
                   }
                }
             else
                {
                QFileInfo *newTarget = new QFileInfo(newFileName);
                item->changeNameDest(newTarget->fileName());
                }

             break;
             }

          case OVERWRITE_SKIP:
             {
             item->changeResult(i18n("Skipped !"));
             item->changeError(i18n("destination image file already exists (skipped automaticly)!"));
             ++*m_listFile2Process_iterator;
             ++m_progressStatus;
             m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

             if ( m_listFile2Process_iterator->current() )
                {
                startProcess();
                return true;
                }
             else
                {
                endProcess(i18n("Process finished!"));
                return true;
                }
             break;
             }

          case OVERWRITE_OVER:   // In this case do nothing : 'convert' default mode...
             break;

          default:
             {
             endProcess(i18n("Process finished!"));
             return true;
             break;
             }
          }
       }

    m_commandLine = "";
    m_ProcessusProc = new KProcess;
    m_commandLine.append(makeProcess(m_ProcessusProc, item, targetAlbum));

    item->changeOutputMess(m_commandLine + "\n\n");

    connect(m_ProcessusProc, SIGNAL(processExited(KProcess *)),
            this, SLOT(slotProcessDone(KProcess*)));

    connect(m_ProcessusProc, SIGNAL(receivedStdout(KProcess *, char*, int)),
            this, SLOT(slotReadStd(KProcess*, char*, int)));

    connect(m_ProcessusProc, SIGNAL(receivedStderr(KProcess *, char*, int)),
            this, SLOT(slotReadStd(KProcess*, char*, int)));

    bool result = m_ProcessusProc->start(KProcess::NotifyOnExit, KProcess::All);

    if(!result)
       {
       KMessageBox::error(this, i18n("Cannot start 'convert' program from 'ImageMagick' package.\n"
                                     "Please, check your installation!"));
       return false;
       }

    return true;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotReadStd(KProcess* proc, char *buffer, int buflen)
{
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
    item->changeOutputMess( QString::fromLocal8Bit(buffer, buflen) );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotProcessDone(KProcess* proc)
{
    qDebug("BatchProcessImagesDialog::slotProcessDone commented out");
#ifdef TEMPORARILY_REMOVED
    int ValRet = proc->exitStatus();
    qDebug ("Convert exit (%i)", ValRet);

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
    Digikam::AlbumInfo *sourceAlbum = Digikam::AlbumManager::instance()
                                      ->findAlbum( item->pathSrc().section('/', -2, -2));
    Digikam::AlbumInfo *targetAlbum = Digikam::AlbumManager::instance()->findAlbum( m_albumList->currentText() );
    m_listFiles->ensureItemVisible(m_listFiles->currentItem());

    switch (ValRet)
       {
       case 0:  // Process finished successfully !
          {
          item->changeResult(i18n("OK"));
          item->changeError(i18n("no processing error"));

          // Save the comments for the converted image if current image taken from Album database.

          if (sourceAlbum)
             {
             sourceAlbum->openDB();
             QString comments = sourceAlbum->getItemComments(item->nameSrc());
             sourceAlbum->closeDB();

             targetAlbum->openDB();
             targetAlbum->setItemComments(item->nameDest(), comments);
             targetAlbum->closeDB();
             }

          if ( m_removeOriginal->isChecked() == true )
             {
             KURL deleteImage(item->pathSrc());

             if ( KIO::NetAccess::del(deleteImage) == false )
                {
                item->changeResult(i18n("Warning !"));
                item->changeError(i18n("cannot remove original image file!"));
                }
             }
          break;
          }
       case 15: //  process aborted !
          {
          processAborted(true);
          break;
          }
       default : // Processing error !
          {
          item->changeResult(i18n("Failed !!!"));
          item->changeError(i18n("cannot process original image file!"));
          break;
          }
       }

    ++*m_listFile2Process_iterator;
    ++m_progressStatus;
    m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

    if ( m_listFile2Process_iterator->current() )
       startProcess();
    else
       endProcess(i18n("Process finished!"));
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotListDoubleClicked(QListViewItem *itemClicked)
{
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( itemClicked );

    if (m_convertStatus == PROCESS_DONE)
       {
       OutputDialog *infoDialog = new OutputDialog(this,
                                                   i18n("Image error processing"),
                                                   item->outputMess(),
                                                   i18n("Image \"%1\": %2\n\nThe ouput messages are:\n")
                                                        .arg(item->nameSrc()).arg(item->error())
                                                   );
       infoDialog->exec();
       }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotPreview(void)
{
    qDebug("BatchProcessImagesDialog::slotPreview");
#ifdef TEMPORARILY_REMOVED
    if ( m_listFiles->currentItem() == 0 )
       {
       KMessageBox::error(this, i18n("You must selected an item in the list for calculate the preview!"));
       return;
       }

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFiles->currentItem() );
    m_statusbar->setText(i18n("Preview under progress. Please wait..."));

    m_listFiles->setEnabled(false);
    m_labelType->setEnabled(false);
    m_Type->setEnabled(false);
    m_optionsButton->setEnabled(false);
    m_previewButton->setEnabled(false);
    m_labelOverWrite->setEnabled(false);
    m_overWriteMode->setEnabled(false);
    m_removeOriginal->setEnabled(false);
    m_addNewAlbumButton->setEnabled(false);
    m_smallPreview->setEnabled(false);
    m_albumList->setEnabled(false);
    m_addImagesButton->setEnabled(false);
    m_remImagesButton->setEnabled(false);

    disconnect( this, SIGNAL(user2Clicked()), this, SLOT(slotProcessStart()));
    showButtonCancel( false );
    setButtonText( User2, i18n("&Stop") );
    connect(this, SIGNAL(user2Clicked()), this, SLOT(slotPreviewStop()));

    m_previewOutput = "";
    m_PreviewProc = new KProcess;

    m_previewOutput.append(makeProcess(m_PreviewProc, item));

    *m_PreviewProc << m_tmpFolder + "/" + QString::number(getpid()) + "preview.PNG";
    m_previewOutput.append( " "  + m_tmpFolder + "/" + QString::number(getpid()) + "preview.PNG\n\n");

    connect(m_PreviewProc, SIGNAL(processExited(KProcess *)),
            this, SLOT(slotPreviewProcessDone(KProcess*)));

    connect(m_PreviewProc, SIGNAL(receivedStdout(KProcess *, char*, int)),
            this, SLOT(slotPreviewReadStd(KProcess*, char*, int)));

    connect(m_PreviewProc, SIGNAL(receivedStderr(KProcess *, char*, int)),
            this, SLOT(slotPreviewReadStd(KProcess*, char*, int)));

    bool result = m_PreviewProc->start(KProcess::NotifyOnExit, KProcess::All);

    if(!result)
       {
       KMessageBox::error(this, i18n("Cannot start 'convert' program from 'ImageMagick' package.\n"
                                     "Please, check your installation!"));
       m_previewButton->setEnabled(true);
       return;
       }
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotPreviewReadStd(KProcess* proc, char *buffer, int buflen)
{
    m_previewOutput.append( QString::fromLocal8Bit(buffer, buflen) );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotPreviewProcessDone(KProcess* proc)
{
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFiles->currentItem() );
    int ValRet = proc->exitStatus();
    qDebug ("Convert exit (%i)", ValRet);

    if ( ValRet == 0 )
       {
       QString cropTitle = "";

       if ( m_smallPreview->isChecked() )
          cropTitle = i18n(" - small preview");

       ImagePreview *previewDialog = new ImagePreview(
                                         item->pathSrc(),
                                         m_tmpFolder + "/" + QString::number(getpid()) + "preview.PNG",
                                         m_tmpFolder,
                                         m_smallPreview->isChecked(),
                                         false,
                                         m_Type->currentText() + cropTitle,
                                         item->nameSrc(),
                                         this);
       previewDialog->exec();

       KURL deletePreviewImage(m_tmpFolder + "/" + QString::number(getpid()) + "preview.PNG");
       KIO::NetAccess::del(deletePreviewImage);
       }
    else
       {
       OutputDialog *infoDialog = new OutputDialog(this,
                                                   i18n("Preview error processing"),
                                                   m_previewOutput,
                                                   i18n("Cannot process preview for image \"%1\"."
                                                        "\nThe ouput messages are:\n")
                                                        .arg(item->nameSrc())
                                                   );
       infoDialog->exec();
       }

    endPreview();
    m_statusbar->setText("");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotPreviewStop( void )
{
    // Try to kill the current preview process !
    if ( m_PreviewProc->isRunning() == true ) m_PreviewProc->kill(SIGTERM);

    endPreview();
    m_statusbar->setText("Preview aborted by user!");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotProcessStop( void )
{
    // Try to kill the current process !
    if ( m_ProcessusProc->isRunning() == true ) m_ProcessusProc->kill(SIGTERM);

    // If kill operation failed, Stop the process at the next image !
    if ( m_convertStatus == UNDER_PROCESS ) m_convertStatus = STOP_PROCESS;

    processAborted(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::slotOk()
{
    close();
    saveSettings();
    delete this;
}


////////////////////////////////////////////// FONCTIONS ////////////////////////////////////////////

void BatchProcessImagesDialog::listImageFiles(void)
{
    m_nbItem = m_selectedImageFiles.count();

    if (m_nbItem == 0) groupBox4->setTitle(i18n("Image files list"));
    else
       groupBox4->setTitle(i18n("Image files list (1 item)", "Image files list (%n items)", m_nbItem));

    if (m_selectedImageFiles.isEmpty()) return;

    for ( KURL::List::Iterator it = m_selectedImageFiles.begin() ; it != m_selectedImageFiles.end() ; ++it )
      {
      QString currentFile = (*it).path(); // PENDING(blackie) Handle URLS
      QFileInfo *fi = new QFileInfo(currentFile);

      // Check if the new item already exist in the list.

      bool findItem = false;

      QListViewItemIterator it2( m_listFiles );

      while ( it2.current() )
         {
         BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*>(it2.current());

         if ( pitem->pathSrc() == currentFile.section('/', 0, -1) )
            findItem = true;

         ++it2;
         }

      if (findItem == false)
         {
         QString oldFileName = fi->fileName();
         QString newFileName = oldFileName2NewFileName(oldFileName);

         BatchProcessImagesItem *item = new BatchProcessImagesItem(m_listFiles,
                                                                   currentFile.section('/', 0, -1),
                                                                   oldFileName,
                                                                   newFileName,
                                                                   ""
                                                                   );
         }

       delete fi;
       }

    m_listFiles->setCurrentItem( m_listFiles->firstChild());
    m_listFiles->setSelected( m_listFiles->currentItem(), true );
    slotImageSelected(m_listFiles->currentItem());
    m_listFiles->ensureItemVisible(m_listFiles->currentItem());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::endPreview(void)
{
    m_listFiles->setEnabled(true);
    m_labelType->setEnabled(true);
    m_Type->setEnabled(true);
    m_previewButton->setEnabled(true);
    m_labelOverWrite->setEnabled(true);
    m_overWriteMode->setEnabled(true);
    m_addNewAlbumButton->setEnabled(true);
    m_albumList->setEnabled(true);
    m_addImagesButton->setEnabled(true);
    m_remImagesButton->setEnabled(true);
    m_smallPreview->setEnabled(true);
    showButtonCancel( true );

    m_optionsButton->setEnabled(true);          // Default status if 'slotTypeChanged' isn't re-implemented.
    slotTypeChanged(m_Type->currentText());
    slotOverWriteModeActived(m_overWriteMode->currentText());

    setButtonText( User2, i18n("&Start") );
    disconnect(this, SIGNAL(user2Clicked()), this, SLOT(slotPreviewStop()));
    connect(this, SIGNAL(user2Clicked()), this, SLOT(slotProcessStart()));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int BatchProcessImagesDialog::overwriteMode(void)
{
    QString OverWrite = m_overWriteMode->currentText();

    if (OverWrite == i18n("Ask"))
        return OVERWRITE_ASK;

    if (OverWrite == i18n("Rename"))
        return OVERWRITE_RENAME;

    if (OverWrite == i18n("Skip"))
        return OVERWRITE_SKIP;

    if (OverWrite == i18n("Always OverWrite"))
        return OVERWRITE_OVER;

    return OVERWRITE_ASK;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::processAborted(bool removeFlag)
{
    qDebug("BatchProcessImagesDialog::processAborted");

#ifdef TEMPORARILY_REMOVED
    Digikam::AlbumInfo *targetAlbum = Digikam::AlbumManager::instance()->findAlbum( m_albumList->currentText() );
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
    m_listFiles->ensureItemVisible(m_listFiles->currentItem());

    item->changeResult(i18n("Aborted !"));
    item->changeError(i18n("process aborted by user"));

    if (removeFlag == true) // Try to delete de destination !
       {
       KURL deleteImage(targetAlbum->getPath() + "/" + item->nameDest());
       if ( KIO::NetAccess::exists(deleteImage) == true )
          KIO::NetAccess::del(deleteImage);
       }

    endProcess(i18n("Process aborted by user!"));
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchProcessImagesDialog::endProcess(QString endMessage)
{
    qDebug("BatchProcessImagesDialog::endProcess");
#ifdef TEMPORARILY_REMOVED
    m_statusbar->setText(endMessage);
    Digikam::AlbumInfo *currentAlbum = Digikam::AlbumManager::instance()->currentAlbum();
    Digikam::AlbumManager::instance()->refreshItemHandler(currentAlbum->getTitle());
    Digikam::AlbumInfo *targetAlbum = Digikam::AlbumManager::instance()->findAlbum( m_albumList->currentText() );
    Digikam::AlbumManager::instance()->refreshItemHandler(targetAlbum->getTitle());
    m_convertStatus = PROCESS_DONE;
    setButtonText( User2, i18n("&Close") );

    disconnect(this, SIGNAL(user2Clicked()), this, SLOT(slotProcessStop()));

    connect(this, SIGNAL(user2Clicked()), this, SLOT(slotOk()));
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

QString BatchProcessImagesDialog::RenameTargetImageFile(QFileInfo *fi)
{
    QString Temp;
    int Enumerator = 0;
    KURL NewDestUrl;

    do
       {
       ++Enumerator;
       Temp = Temp.setNum( Enumerator );
       NewDestUrl = fi->filePath().left( fi->filePath().findRev('.', -1)) + "_" + Temp
                    + "." + fi->filePath().section('.', -1 );
       }
    while ( Enumerator < 100 && KIO::NetAccess::exists(NewDestUrl) == true );

    if (Enumerator == 100) return QString::null;

    return (NewDestUrl.path());
}



/////////////////////////////////////////////////////////////////////////////////////////////////////

QString BatchProcessImagesDialog::extractArguments(KProcess *proc)
{
    QString retArguments;
    QValueList<QCString> argumentsList = proc->args();

    for ( QValueList<QCString>::iterator it = argumentsList.begin() ; it != argumentsList.end() ; ++it )
      retArguments.append(*it + " ");

    return (retArguments);
}
