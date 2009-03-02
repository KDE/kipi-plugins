//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <QPixmap>
#include <Q3VBoxLayout>
#include <QCloseEvent>
/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C Ansi includes

extern "C"
{
#include <unistd.h>
}

// Include files for Qt

#include <q3vbox.h>
#include <qlayout.h>
#include <qdir.h>
#include <qwidget.h>
#include <qlabel.h>
#include <q3groupbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <q3process.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qimage.h>
#include <qevent.h>
#include <q3dragobject.h>
#include <qfileinfo.h>
#include <q3hgroupbox.h>
#include <q3vgroupbox.h>
#include <q3frame.h>
#include <qmatrix.h>

// Include files for KDE

#include <kstandarddirs.h>
#include <kcolorbutton.h>
#include <klocale.h>
#include <kprogressdialog.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kcomponentdata.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kdialog.h>
#include <k3listview.h>
#include <kimageio.h>
#include <k3process.h>
#include <kio/jobclasses.h>
#include <kio/netaccess.h>
#include <kio/global.h>
#include <kio/previewjob.h>
#include <k3buttonbox.h>
#include <kdiroperator.h>
#include <kdeversion.h>
#include <kurlrequester.h>
#include <klineedit.h>

// KIPI includes

#include <libkipi/uploadwidget.h>

// KIPIPlugins includes

#include "imagedialog.h"

// Local includes

#include "pluginsversion.h"
#include "outputdialog.h"
#include "imagepreview.h"
#include "batchprocessimagesdialog.h"
#include "batchprocessimagesdialog.moc"

namespace KIPIBatchProcessImagesPlugin
{

BatchProcessImagesDialog::BatchProcessImagesDialog( KUrl::List urlList, KIPI::Interface* interface,
                                                    QString caption, QWidget *parent )
                        : KDialog(parent)
                        , m_selectedImageFiles( urlList), m_interface( interface )
{
    setCaption(caption);
    setButtons(Help | User1 | Cancel);
    setButtonText(User1, i18n("&Start"));
    showButtonSeparator(false);

    // Init. Tmp folder

    KStandardDirs dir;
    m_tmpFolder = dir.saveLocation("tmp", "kipi-batchprocessimagesplugin-" +
                                   QString::number(getpid()) );

    m_convertStatus  = NO_PROCESS;
    m_progressStatus = 0;
    m_ProcessusProc  = 0;
    m_PreviewProc    = 0;

    

    QWidget* box = new QWidget(this);
    setMainWidget(box);
    Q3VBoxLayout *dvlay = new Q3VBoxLayout(box, 0, KDialog::spacingHint());

    //---------------------------------------------

    Q3HBoxLayout *hlay = new Q3HBoxLayout( dvlay );
    groupBox1 = new Q3GroupBox( 0, Qt::Vertical, box );
    groupBox1->layout()->setSpacing(KDialog::spacingHint());
    groupBox1->layout()->setMargin(KDialog::marginHint());
    Q3GridLayout* grid = new Q3GridLayout( groupBox1->layout(), 2, 3);
    m_labelType = new QLabel( groupBox1 );
    grid->addMultiCellWidget(m_labelType, 0, 0, 0, 0);

    m_Type = new QComboBox(false, groupBox1);
    grid->addMultiCellWidget(m_Type, 0, 0, 1, 1);

    m_optionsButton = new QPushButton (groupBox1, "OptionButton");
    m_optionsButton->setText(i18n("Options"));
    m_optionsButton->setWhatsThis(i18n("<p>You can choose here the options to use "
                                           "for the current process."));
    grid->addMultiCellWidget(m_optionsButton, 0, 0, 2, 2);

    m_smallPreview = new QCheckBox(i18n("Small preview"), groupBox1);
    m_smallPreview->setWhatsThis(i18n("<p>If you enable this option, "
                                          "all preview effects will be calculated on a small zone "
                                          "of the image (300x300 pixels in the top left corner). "
                                          "Enable this option if you have a slow computer.") );
    m_smallPreview->setChecked( true );
    grid->addMultiCellWidget(m_smallPreview, 1, 1, 0, 1);

    m_previewButton = new QPushButton (groupBox1, "PreviewButton");
    m_previewButton->setText(i18n("&Preview"));
    m_previewButton->setWhatsThis(i18n("<p>This button builds a process "
                                           "preview for the currently selected image on the list."));
    grid->addMultiCellWidget(m_previewButton, 1, 1, 2, 2);

    hlay->addWidget( groupBox1 );

    //---------------------------------------------

    groupBox2 = new Q3GroupBox( 2, Qt::Horizontal, i18n("File Operations"), box );

    m_labelOverWrite = new QLabel (i18n("Overwrite mode:"), groupBox2);
    m_overWriteMode = new QComboBox( false, groupBox2 );
    m_overWriteMode->insertItem(i18n("Ask"));
    m_overWriteMode->insertItem(i18n("Always Overwrite"));
    m_overWriteMode->insertItem(i18n("Rename"));
    m_overWriteMode->insertItem(i18n("Skip"));
    m_overWriteMode->setCurrentText (i18n("Rename"));
    m_overWriteMode->setWhatsThis(i18n("<p>Select here the overwrite mode used if your target's image "
                                           "files already exist.") );

    m_removeOriginal = new QCheckBox(i18n("Remove original"), groupBox2);
    m_removeOriginal->setWhatsThis(i18n("<p>If you enable this option, "
                                            "all original image files will be removed after processing.") );
    m_removeOriginal->setChecked( false );

    hlay->addWidget( groupBox2 );

    //---------------------------------------------

    groupBox3 = new Q3HGroupBox( i18n("Target Folder"), box );

    m_destinationURL = new KUrlRequester(groupBox3);
	m_destinationURL->setMode(KFile::Directory | KFile::LocalOnly);
	KIPI::ImageCollection album = interface->currentAlbum();
	if (album.isValid())
    {
		QString url;
		if (album.isDirectory())
        {
			url = album.uploadPath().path();
		}
        else
        {
			url = QDir::homePath();
		}
		m_destinationURL->lineEdit()->setText(url);
	}
    m_destinationURL->setWhatsThis(i18n("<p>Here you can select the target folder which "
                                            "will used by the process."));

    dvlay->addWidget( groupBox3 );

    //---------------------------------------------

    groupBox4         = new Q3HGroupBox( box );
    QWidget* box41    = new QWidget( groupBox4 );
    Q3HBoxLayout* lay2 = new Q3HBoxLayout( box41, 0, spacingHint() );
    m_listFiles       = new BatchProcessImagesList( box41 );
    lay2->addWidget( m_listFiles );

    m_listFiles->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    Q3VBoxLayout* lay3 = new Q3VBoxLayout( lay2 );
    m_addImagesButton = new QPushButton ( i18n( "&Add..." ), box41 );
    lay3->addWidget( m_addImagesButton );
    m_addImagesButton->setWhatsThis(i18n("<p>Add images to the list.") );

    m_remImagesButton = new QPushButton ( i18n( "&Remove" ), box41 );
    lay3->addWidget( m_remImagesButton );
    m_remImagesButton->setWhatsThis(i18n("<p>Remove selected image from the list.") );

    m_imageLabel = new QLabel( box41 );
    m_imageLabel->setFixedHeight( 80 );
    m_imageLabel->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_imageLabel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    lay3->addWidget( m_imageLabel );
    m_imageLabel->setWhatsThis(i18n( "<p>The preview of the selected image on the list." ) );
    lay3->addStretch( 1 );

    dvlay->addWidget( groupBox4 );

    //---------------------------------------------

    m_progress = new QProgressBar( box );
    m_progress->setMaximum(100);
    m_progress->setValue(0);
    m_progress->setWhatsThis(i18n("<p>This is the current percentage of the task completed.") );

    dvlay->addWidget( m_progress );

    //---------------------------------------------

    connect(m_listFiles, SIGNAL(doubleClicked(Q3ListViewItem *)),
            this, SLOT(slotListDoubleClicked(Q3ListViewItem *)));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotProcessStart()));

    connect(m_optionsButton, SIGNAL(clicked()),
            this, SLOT(slotOptionsClicked()));

    connect(m_previewButton, SIGNAL(clicked()),
            this, SLOT(slotPreview()));

    connect(m_Type, SIGNAL(activated(int)),
            this, SLOT(slotTypeChanged(int)));

    connect(m_listFiles, SIGNAL( addedDropItems(QStringList) ),
            this, SLOT( slotAddDropItems(QStringList)));

    connect(m_listFiles, SIGNAL( currentChanged( Q3ListViewItem * ) ),
            this, SLOT( slotImageSelected( Q3ListViewItem * )));

    connect(m_addImagesButton, SIGNAL(clicked()),
            this, SLOT(slotImagesFilesButtonAdd()));

    connect(m_remImagesButton, SIGNAL(clicked()),
            this, SLOT(slotImagesFilesButtonRem()));

   // Get the image files filters from the hosts app.

    m_ImagesFilesSort = m_interface->fileExtensions();

    dvlay->activate();
}

BatchProcessImagesDialog::~BatchProcessImagesDialog()
{
}

void BatchProcessImagesDialog::slotImagesFilesButtonAdd( void )
{
    QStringList ImageFilesList;

    KUrl::List urls = KIPIPlugins::ImageDialog::getImageURLs( this, m_interface );

    for ( KUrl::List::Iterator it = urls.begin() ; it != urls.end() ; ++it )
        ImageFilesList << (*it).path(); // PENDING(blackie) handle remote URLS

    if ( urls.isEmpty() ) return;

    slotAddDropItems(ImageFilesList);
}

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

        if (m_nbItem == 0)
            groupBox4->setTitle(i18n("Image Files List"));
        else
            groupBox4->setTitle(i18np("Image File List (1 item)", "Image File List (%1 items)", m_nbItem));
    }
}

void BatchProcessImagesDialog::slotImageSelected( Q3ListViewItem * item )
{
    if ( !item || m_listFiles->childCount() == 0 )
    {
       m_imageLabel->clear();
       return;
    }

    BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*>( item );
    if ( !pitem ) return;

    m_imageLabel->clear();

    QString IdemIndexed = "file:" + pitem->pathSrc();

    KUrl url(IdemIndexed);

    KIO::PreviewJob* m_thumbJob = KIO::filePreview( url, m_imageLabel->height() );

    connect(m_thumbJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            this, SLOT(slotGotPreview(const KFileItem*, const QPixmap&)));
}

void BatchProcessImagesDialog::slotGotPreview(const KFileItem* url, const QPixmap &pixmap)
{
    QPixmap pix( pixmap );

    // Rotate the thumbnail compared to the angle the host application dictate
    KIPI::ImageInfo info = m_interface->info( url->url() );
    if ( info.angle() != 0 )
    {
        QImage img = pix.convertToImage();
        QMatrix matrix;

        matrix.rotate( info.angle() );
        img = img.transformed( matrix );
        pix.convertFromImage( img );
    }

    m_imageLabel->setPixmap(pix);
}

void BatchProcessImagesDialog::slotAddDropItems(QStringList filesPath)
{
    if (filesPath.isEmpty()) return;

    for ( QStringList::Iterator it = filesPath.begin() ; it != filesPath.end() ; ++it )
    {
        QString currentDropFile = *it;
    
        // Check if the new item already exist in the list.
    
        bool findItem = false;
    
        for ( KUrl::List::Iterator it2 = m_selectedImageFiles.begin() ; it2 != m_selectedImageFiles.end() ; ++it2 )
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

void BatchProcessImagesDialog::closeEvent ( QCloseEvent *e )
{
    if (!e) return;

    if ( m_PreviewProc != 0 )
       if ( m_PreviewProc->isRunning() ) m_PreviewProc->kill(SIGTERM);

    if ( m_ProcessusProc != 0 )
       if ( m_ProcessusProc->isRunning() ) m_ProcessusProc->kill(SIGTERM);

    e->accept();
}

void BatchProcessImagesDialog::slotProcessStart( void )
{
    if ( m_selectedImageFiles.isEmpty() == true )
       return;

    if ( m_removeOriginal->isChecked() == true )
    {
        if ( KMessageBox::warningContinueCancel(this,
             i18n("All original image files will be removed from the source Album.\nDo you want to continue?"),
             i18n("Delete Original Image Files"), KStandardGuiItem::cont(), KStandardGuiItem::cancel(),
             "KIPIplugin-BatchProcessImages-AlwaysRemomveOriginalFiles") != KMessageBox::Continue )
           return;
    }

    m_convertStatus = UNDER_PROCESS;
    disconnect( this, SIGNAL(user1Clicked()), this, SLOT(slotProcessStart()));
    showButton(KDialog::Cancel, false );
    setButtonText( User1, i18n("&Stop") );
    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotProcessStop()));

    m_labelType->setEnabled(false);
    m_Type->setEnabled(false);
    m_optionsButton->setEnabled(false);
    m_previewButton->setEnabled(false);
    m_smallPreview->setEnabled(false);

    m_labelOverWrite->setEnabled(false);
    m_overWriteMode->setEnabled(false);
    m_removeOriginal->setEnabled(false);

    m_destinationURL->setEnabled(false);
    m_addImagesButton->setEnabled(false);
    m_remImagesButton->setEnabled(false);

    m_listFile2Process_iterator = new Q3ListViewItemIterator( m_listFiles );
    startProcess();
}

bool BatchProcessImagesDialog::startProcess(void)
{
    if ( m_convertStatus == STOP_PROCESS )
    {
       endProcess();
       return true;
    }

    QString targetAlbum = m_destinationURL->url().path();

    //TODO check if it is valid also for remote URL's
    // this is a workarond for bug 117397
    QFileInfo dirInfo(targetAlbum + "/");
    if (!dirInfo.isDir () || !dirInfo.isWritable())
    {
        KMessageBox::error(this, i18n("You must specify a writable path for your output file."));
        endProcess();
        return true;
    }

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
            endProcess();
            return true;
        }
    }

    KUrl desturl(targetAlbum + "/" + item->nameDest());

#if KDE_VERSION >= 0x30200
    if ( KIO::NetAccess::exists( desturl, false, kapp->activeWindow() ) == true )
#else
    if ( KIO::NetAccess::exists( desturl ) == true )
#endif
    {
       switch (overwriteMode())
       {
          case OVERWRITE_ASK:
          {
             int ValRet = KMessageBox::warningYesNoCancel(this,
                          i18n("The destination file \"%1\" already exists;\n"
                          "do you want overwrite it?").arg(item->nameDest()),
                          i18n("Overwrite Destination Image File"), KStandardGuiItem::cont());

             if ( ValRet == KMessageBox::No )
             {
                item->changeResult(i18n("Skipped."));
                item->changeError(i18n("destination image file already exists (skipped by user)."));
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
                   endProcess();
                   return true;
                }
            }
            else if ( ValRet == KMessageBox::Cancel )
            {
                processAborted(false);
                return false;
            }
            else
            {
                item->setDidOverWrite( true );
            }

             break;
          }

          case OVERWRITE_RENAME:
          {
             QFileInfo *Target = new QFileInfo(targetAlbum + "/" + item->nameDest());
             QString newFileName = RenameTargetImageFile(Target);

             if ( newFileName.isNull() )
             {
                item->changeResult(i18n("Failed."));
                item->changeError(i18n("destination image file already exists and cannot be renamed."));
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
                   endProcess();
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
             item->changeResult(i18n("Skipped."));
             item->changeError(i18n("destination image file already exists (skipped automatically)."));
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
                endProcess();
                return true;
             }
             break;
          }

          case OVERWRITE_OVER:   // In this case do nothing : 'convert' default mode...
              item->setDidOverWrite( true );
             break;

          default:
          {
             endProcess();
             return true;
             break;
          }
       }
    }

    m_ProcessusProc = new K3Process;
    initProcess(m_ProcessusProc, item, targetAlbum);
    m_commandLine = extractArguments(m_ProcessusProc);

    item->changeOutputMess(m_commandLine + "\n\n");

    connect(m_ProcessusProc, SIGNAL(processExited(K3Process *)),
            this, SLOT(slotProcessDone(K3Process*)));

    connect(m_ProcessusProc, SIGNAL(receivedStdout(K3Process *, char*, int)),
            this, SLOT(slotReadStd(K3Process*, char*, int)));

    connect(m_ProcessusProc, SIGNAL(receivedStderr(K3Process *, char*, int)),
            this, SLOT(slotReadStd(K3Process*, char*, int)));

    bool result = m_ProcessusProc->start(K3Process::NotifyOnExit, K3Process::All);
    if(!result)
    {
       KMessageBox::error(this, i18n("Cannot start 'convert' program from 'ImageMagick' package;\n"
                                     "please check your installation."));
       return false;
    }

    return true;
}

void BatchProcessImagesDialog::slotReadStd(K3Process* /*proc*/, char *buffer, int buflen)
{
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
    item->changeOutputMess( QString::fromLocal8Bit(buffer, buflen) );
}

void BatchProcessImagesDialog::slotProcessDone(K3Process* proc)
{
    if ( m_convertStatus == PROCESS_DONE )
    {
        // processAborted() has already been called. No need to show the warning.
        return;
    }
    
    BatchProcessImagesItem *item = dynamic_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
    m_listFiles->ensureItemVisible(m_listFiles->currentItem());
    
    if ( !m_ProcessusProc->normalExit() )
    {
        int code = KMessageBox::warningContinueCancel( this,
                                i18n("The 'convert' program from 'ImageMagick' package has been stopped abnormally"),
                                i18n("Error running 'convert'") );
    
        if ( code == KMessageBox::Cancel )
        {
            processAborted(true);
        }
        else
        {
            item->changeResult(i18n("Failed."));
            item->changeError(i18n("'convert' program from 'ImageMagick' package has been stopped abnormally."));
            ++*m_listFile2Process_iterator;
            ++m_progressStatus;
            m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));
    
            if ( m_listFile2Process_iterator->current() )
                startProcess();
            else
                endProcess();
        }
        return;
    }

    int ValRet = proc->exitStatus();
    kWarning() << "Convert exit (" << ValRet << ")" << endl;

    switch (ValRet)
    {
        case 0:  // Process finished successfully !
        {
            item->changeResult(i18n("OK"));
            item->changeError(i18n("no processing error"));
            processDone();
    
            // Save the comments for the converted image
            KUrl src;
            src.setPath( item->pathSrc() );
            KUrl dest = m_destinationURL->url();
            dest.addPath( item->nameDest() );
            QString errmsg;
    
            KUrl::List urlList;
            urlList.append(src);
            urlList.append(dest);
            m_interface->refreshImages( urlList );
    
            if ( !item->overWrote() )
            {
                // Do not add an entry if there was an image at the location already.
                bool ok = m_interface->addImage( dest, errmsg );
    
                if ( !ok )
                {
                    int code = KMessageBox::warningContinueCancel( this,
                                            i18n("<qt>Error adding image to application; error message was: "
                                            "<b>%1</b></qt>").arg( errmsg ),
                                            i18n("Error Adding Image to Application") );
    
                    if ( code == KMessageBox::Cancel )
                    {
                        slotProcessStop();
                        break;
                    }
                    else
                        item->changeResult(i18n("Failed."));
                }
            }

            if ( src != dest )
            {
                KIPI::ImageInfo srcInfo  = m_interface->info( src );
                KIPI::ImageInfo destInfo = m_interface->info( dest );
                destInfo.cloneData( srcInfo );
            }
    
            if ( m_removeOriginal->isChecked() && src != dest )
            {
                KUrl deleteImage(item->pathSrc());
    
#if KDE_VERSION >= 0x30200
                if ( KIO::NetAccess::del( deleteImage, kapp->activeWindow() ) == false )
#else
                if ( KIO::NetAccess::del( deleteImage ) == false )
#endif
                {
                    item->changeResult(i18n("Warning:"));
                    item->changeError(i18n("cannot remove original image file."));
                }
                else
                    m_interface->delImage( item->pathSrc() );
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
            item->changeResult(i18n("Failed."));
            item->changeError(i18n("cannot process original image file."));
            break;
        }
    }

    ++*m_listFile2Process_iterator;
    ++m_progressStatus;
    m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

    if ( m_listFile2Process_iterator->current() )
        startProcess();
    else
        endProcess();
}

void BatchProcessImagesDialog::slotListDoubleClicked(Q3ListViewItem *itemClicked)
{
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( itemClicked );

    if (m_convertStatus == PROCESS_DONE)
    {
       OutputDialog *infoDialog = new OutputDialog(this,
                                                   i18n("Image processing error"),
                                                   item->outputMess(),
                                                   i18n("Image \"%1\": %2\n\nThe output messages are:\n")
                                                        .arg(item->nameSrc()).arg(item->error())
                                                   );
       infoDialog->exec();
    }
}

void BatchProcessImagesDialog::slotPreview(void)
{
    kWarning() << "BatchProcessImagesDialog::slotPreview" << endl;

    if ( m_listFiles->currentItem() == 0 )
    {
       KMessageBox::error(this, i18n("You must select an item from the list to calculate the preview."));
       return;
    }

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFiles->currentItem() );

    m_listFiles->setEnabled(false);
    m_labelType->setEnabled(false);
    m_Type->setEnabled(false);
    m_optionsButton->setEnabled(false);
    m_previewButton->setEnabled(false);
    m_labelOverWrite->setEnabled(false);
    m_overWriteMode->setEnabled(false);
    m_removeOriginal->setEnabled(false);
    m_smallPreview->setEnabled(false);
    m_destinationURL->setEnabled(false);
    m_addImagesButton->setEnabled(false);
    m_remImagesButton->setEnabled(false);

    disconnect( this, SIGNAL(user1Clicked()),
                this, SLOT(slotProcessStart()));

    showButton(KDialog::Cancel, false );
    setButtonText( User1, i18n("&Stop") );

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotPreviewStop()));

    m_PreviewProc = new K3Process;
    initProcess(m_PreviewProc, item, QString(), true);

    m_previewOutput = extractArguments(m_PreviewProc);

    *m_PreviewProc << m_tmpFolder + "/" + QString::number(getpid()) + "preview.PNG";
    m_previewOutput.append( " "  + m_tmpFolder + "/" + QString::number(getpid()) + "preview.PNG\n\n");

    connect(m_PreviewProc, SIGNAL(processExited(K3Process *)),
            this, SLOT(slotPreviewProcessDone(K3Process*)));

    connect(m_PreviewProc, SIGNAL(receivedStdout(K3Process *, char*, int)),
            this, SLOT(slotPreviewReadStd(K3Process*, char*, int)));

    connect(m_PreviewProc, SIGNAL(receivedStderr(K3Process *, char*, int)),
            this, SLOT(slotPreviewReadStd(K3Process*, char*, int)));

    bool result = m_PreviewProc->start(K3Process::NotifyOnExit, K3Process::All);
    if(!result)
    {
        KMessageBox::error(this, i18n("Cannot start 'convert' program from 'ImageMagick' package;\n"
                                      "please check your installation."));
        m_previewButton->setEnabled(true);
        return;
    }
}

void BatchProcessImagesDialog::slotPreviewReadStd(K3Process* /*proc*/, char *buffer, int buflen)
{
    m_previewOutput.append( QString::fromLocal8Bit(buffer, buflen) );
}

void BatchProcessImagesDialog::slotPreviewProcessDone(K3Process* proc)
{
    if (!m_PreviewProc->normalExit())
    {
        KMessageBox::error(this, i18n("Cannot run properly 'convert' program from 'ImageMagick' package."));
        m_previewButton->setEnabled(true);
        return;
    }

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFiles->currentItem() );
    int ValRet = proc->exitStatus();

    kWarning() << "Convert exit (" << ValRet << ")" << endl;

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

       KUrl deletePreviewImage(m_tmpFolder + "/" + QString::number(getpid()) + "preview.PNG");

#if KDE_VERSION >= 0x30200
       KIO::NetAccess::del( deletePreviewImage, kapp->activeWindow() );
#else
       KIO::NetAccess::del( deletePreviewImage );
#endif
    }
    else
    {
       OutputDialog *infoDialog = new OutputDialog(this,
                                                   i18n("Preview processing error"),
                                                   m_previewOutput,
                                                   i18n("Cannot process preview for image \"%1\"."
                                                        "\nThe output messages are:\n")
                                                        .arg(item->nameSrc())
                                                   );
       infoDialog->exec();
    }

    endPreview();
}

void BatchProcessImagesDialog::slotPreviewStop( void )
{
    // Try to kill the current preview process !
    if ( m_PreviewProc->isRunning() == true ) m_PreviewProc->kill(SIGTERM);

    endPreview();
}

void BatchProcessImagesDialog::slotProcessStop( void )
{
    // Try to kill the current process !
    if ( m_ProcessusProc->isRunning() == true ) m_ProcessusProc->kill(SIGTERM);

    // If kill operation failed, Stop the process at the next image !
    if ( m_convertStatus == UNDER_PROCESS ) m_convertStatus = STOP_PROCESS;

    processAborted(true);
}

void BatchProcessImagesDialog::slotOk()
{
    close();
    saveSettings();
    delete this;
}

void BatchProcessImagesDialog::listImageFiles(void)
{
    m_nbItem = m_selectedImageFiles.count();

    if (m_nbItem == 0) groupBox4->setTitle(i18n("Image File List"));
    else
        groupBox4->setTitle(i18np("Image File List (%1 item)", "Image File List (%1 items)", m_nbItem));

    if (m_selectedImageFiles.isEmpty()) return;

    for ( KUrl::List::Iterator it = m_selectedImageFiles.begin() ; it != m_selectedImageFiles.end() ; ++it )
    {
        QString currentFile = (*it).path(); // PENDING(blackie) Handle URLS
        QFileInfo *fi = new QFileInfo(currentFile);
    
        // Check if the new item already exist in the list.
    
        bool findItem = false;
    
        Q3ListViewItemIterator it2( m_listFiles );
    
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
    
            new BatchProcessImagesItem(m_listFiles,
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

void BatchProcessImagesDialog::endPreview(void)
{
    m_listFiles->setEnabled(true);
    m_labelType->setEnabled(true);
    m_Type->setEnabled(true);
    m_previewButton->setEnabled(true);
    m_labelOverWrite->setEnabled(true);
    m_overWriteMode->setEnabled(true);
    m_destinationURL->setEnabled(true);
    m_addImagesButton->setEnabled(true);
    m_remImagesButton->setEnabled(true);
    m_smallPreview->setEnabled(true);
    m_removeOriginal->setEnabled(true);
    showButton(KDialog::Cancel, true );

    m_optionsButton->setEnabled(true);          // Default status if 'slotTypeChanged' isn't re-implemented.
    slotTypeChanged(m_Type->currentItem());

    setButtonText( User1, i18n("&Start") );

    disconnect(this, SIGNAL(user1Clicked()),
               this, SLOT(slotPreviewStop()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotProcessStart()));
}

int BatchProcessImagesDialog::overwriteMode(void)
{
    QString OverWrite = m_overWriteMode->currentText();

    if (OverWrite == i18n("Ask"))
        return OVERWRITE_ASK;

    if (OverWrite == i18n("Rename"))
        return OVERWRITE_RENAME;

    if (OverWrite == i18n("Skip"))
        return OVERWRITE_SKIP;

    if (OverWrite == i18n("Always Overwrite"))
        return OVERWRITE_OVER;

    return OVERWRITE_ASK;
}

void BatchProcessImagesDialog::processAborted(bool removeFlag)
{
    kWarning() << "BatchProcessImagesDialog::processAborted" << endl;

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
    m_listFiles->ensureItemVisible(m_listFiles->currentItem());

    item->changeResult(i18n("Aborted."));
    item->changeError(i18n("process aborted by user"));

    if (removeFlag == true) // Try to delete de destination !
    {
       KUrl deleteImage = m_destinationURL->url();
       deleteImage.addPath(item->nameDest());

#if KDE_VERSION >= 0x30200
       if ( KIO::NetAccess::exists( deleteImage, false, kapp->activeWindow() ) == true )
          KIO::NetAccess::del( deleteImage, kapp->activeWindow() );
#else
       if ( KIO::NetAccess::exists( deleteImage ) == true )
          KIO::NetAccess::del( deleteImage );
#endif
    }

    endProcess();
}

void BatchProcessImagesDialog::endProcess(void)
{
    m_convertStatus = PROCESS_DONE;
    setButtonText( User1, i18n("&Close") );

    disconnect(this, SIGNAL(user1Clicked()),
               this, SLOT(slotProcessStop()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotOk()));
}

QString BatchProcessImagesDialog::RenameTargetImageFile(QFileInfo *fi)
{
    QString Temp;
    int Enumerator = 0;
    KUrl NewDestUrl;

    do
    {
       ++Enumerator;
       Temp = Temp.setNum( Enumerator );
       NewDestUrl = fi->filePath().left( fi->filePath().findRev('.', -1)) + "_" + Temp
                    + "." + fi->filePath().section('.', -1 );
    }
    while ( Enumerator < 100 &&
#if KDE_VERSION >= 0x30200
            KIO::NetAccess::exists( NewDestUrl, true, kapp->activeWindow() )
#else
            KIO::NetAccess::exists( NewDestUrl )
#endif
            == true );

    if (Enumerator == 100) return QString();

    return (NewDestUrl.path());
}

QString BatchProcessImagesDialog::extractArguments(K3Process *proc)
{
    QString retArguments;
    QList<QByteArray> argumentsList = proc->args();

    Q_FOREACH(const QByteArray& arg, argumentsList) {
      retArguments.append(arg + " ");
    }

    return (retArguments);
}

}  // NameSpace KIPIBatchProcessImagesPlugin
