//////////////////////////////////////////////////////////////////////////////
//
//    RENAMEIMAGESDIALOG.CPP
//
//    Copyright (C) 2003-2004 Gilles Caulier <caulier dot gilles at free.fr>
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

// C Ansi includes

extern "C"
{
#include <stdio.h>
#include <sys/types.h>
#include <utime.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
}

// Include files for Qt

#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qdir.h>

// Include files for KDE

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprocess.h>
#include <klineedit.h>
#include <kio/jobclasses.h>
#include <kio/netaccess.h>
#include <kprogress.h>
#include <kdeversion.h>
#if KDE_VERSION >= 0x30200
#include <kdatetimewidget.h>
#else
#include <kdatewidget.h>
#endif
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// Includes files for KIPI

#include <libkipi/version.h>
#include <libkipi/uploadwidget.h>

// Locale includes

#include "renameimagesoptionsdialog.h"
#include "renameimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

RenameImagesDialog::RenameImagesDialog( KURL::List urlList, KIPI::Interface* interface, QWidget *parent )
                  : BatchProcessImagesDialog( urlList, interface, i18n("Batch Rename Images"), parent )
{
    // About data and help button.
    
    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Batch-rename images"), 
                                       kipi_version,
                                       I18N_NOOP("A Kipi plugin to batch-rename images"),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2004, Gilles Caulier", 
                                       0,
                                       "http://extragear.kde.org/apps/kipi");
    
    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at free.fr");
                        
    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Batch-Rename Images Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

    //---------------------------------------------
    
    m_nbItem = m_selectedImageFiles.count();

    //---------------------------------------------

    groupBox1->setTitle( i18n("Image-Renaming Options") );

    m_labelType->hide();
    m_Type->hide();
    m_previewButton->hide();
    m_smallPreview->hide();

    //---------------------------------------------

    readSettings();
    listImageFiles();
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

RenameImagesDialog::~RenameImagesDialog()
{
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void RenameImagesDialog::slotHelp( void )
{
    KApplication::kApplication()->invokeHelp("renameimages",
                                             "kipi-plugins");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::slotOptionsClicked(void)
{
    optionsDialog = new RenameImagesOptionsDialog(this);

    optionsDialog->m_addOriginalFileName->setChecked(m_addOriginalFileName);
    optionsDialog->m_addImageFileDate->setChecked(m_addImageFileDate);
    optionsDialog->m_addImageFileSize->setChecked(m_addImageFileSize);
    optionsDialog->m_reverseOrder->setChecked(m_reverseOrder);
    optionsDialog->m_prefix->setText(m_prefix);
    optionsDialog->m_sortType->setCurrentItem(m_sortType);
    optionsDialog->m_enumeratorStart->setValue(m_enumeratorStart);
    optionsDialog->m_dateChange->setChecked(m_dateChange);

    connect(optionsDialog->m_prefix, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotTemplateSortListChanged(const QString &)));

    connect(optionsDialog->m_enumeratorStart, SIGNAL(valueChanged(int)),
            this, SLOT(slotTemplateSortListChanged(int)));

    connect(optionsDialog->m_sortType, SIGNAL(activated(int)),
            this, SLOT(slotTemplateSortListChanged(int)));

    connect(optionsDialog->m_reverseOrder, SIGNAL(toggled (bool)),
            this, SLOT(slotTemplateSortListChanged(bool)));

    connect(optionsDialog->m_addOriginalFileName, SIGNAL(toggled (bool)),
            this, SLOT(slotTemplateSortListChanged(bool)));

    connect(optionsDialog->m_addImageFileDate, SIGNAL(toggled (bool)),
            this, SLOT(slotTemplateSortListChanged(bool)));

    connect(optionsDialog->m_addImageFileSize, SIGNAL(toggled (bool)),
            this, SLOT(slotTemplateSortListChanged(bool)));

    if ( optionsDialog->exec() == KMessageBox::Ok )
       updateOptions();

    disconnect(optionsDialog->m_prefix, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotTemplateSortListChanged(const QString &)));

    disconnect(optionsDialog->m_enumeratorStart, SIGNAL(valueChanged(int)),
            this, SLOT(slotTemplateSortListChanged(int)));

    disconnect(optionsDialog->m_sortType, SIGNAL(activated(int)),
            this, SLOT(slotTemplateSortListChanged(int)));

    disconnect(optionsDialog->m_reverseOrder, SIGNAL(toggled (bool)),
            this, SLOT(slotTemplateSortListChanged(bool)));

    disconnect(optionsDialog->m_addOriginalFileName, SIGNAL(toggled (bool)),
            this, SLOT(slotTemplateSortListChanged(bool)));

    disconnect(optionsDialog->m_addImageFileDate, SIGNAL(toggled (bool)),
            this, SLOT(slotTemplateSortListChanged(bool)));

    disconnect(optionsDialog->m_addImageFileSize, SIGNAL(toggled (bool)),
            this, SLOT(slotTemplateSortListChanged(bool)));

    delete optionsDialog;

    listImageFiles();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::slotResult( KIO::Job *job )
{
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );

    if (job->error())
        {
        item->changeResult(i18n("Failed."));
        item->changeError(i18n("Cannot copy or move original image file."));
        item->changeOutputMess(i18n("Check the file's access rights or the amount of disk space.\n"
                                    "The internal error is:\n\n%1")
                                    .arg(job->errorString()));
        }
    else
        {
        // Save the comments if current image taken from Album database.

            KURL src;
            src.setPath( item->pathSrc() );
            KURL dest = m_upload->path();
            dest.addPath( item->nameDest() );

            QString errmsg;

            KURL::List urlList;
            urlList.append(src);
            urlList.append(dest);
            m_interface->refreshImages( urlList );

            bool ok = m_interface->addImage( dest, errmsg );

            if ( !ok ) 
                {
                int code = KMessageBox::warningContinueCancel( this,
                                        i18n("<qt>Error adding image to the host application; "
                                        "error message was: <b>%1</b></qt>").arg( errmsg ),
                                        i18n("Error Adding Image to Application") );
                if ( code == KMessageBox::Cancel ) 
                    {
                    slotProcessStop();
                    return;
                    }
                else
                    item->changeResult(i18n("Failed."));
                }

            KIPI::ImageInfo srcInfo = m_interface->info( src );
            KIPI::ImageInfo destInfo = m_interface->info( dest );
            destInfo.cloneData( srcInfo );

            // Change image file timestamp if necessary.

            if ( m_dateChange == true )
                {
                destInfo.setTime( m_newDateTime );
                }

            // Remove original image file from the host app. if necessary.

            if ( m_removeOriginal->isChecked() == true )
                {
                m_interface->delImage( src );
                }
        else
           {
           item->changeResult(i18n("OK"));
           item->changeError(i18n("no processing error"));
           item->changeOutputMess(i18n("None"));
           }
        }

    ++*m_listFile2Process_iterator;
    ++m_progressStatus;
    m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

    if (m_listFile2Process_iterator->current())     // Next image ?
        copyItemOperations();
    else                                            // Copy/move is done...
        endProcess();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::slotTemplateSortListChanged(int)
{
    updateOptions();
    listImageFiles();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::slotTemplateSortListChanged(bool)
{
    updateOptions();
    listImageFiles();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::slotTemplateSortListChanged(const QString &)
{
    updateOptions();
    listImageFiles();
}


////////////////////////////////////////////// FONCTIONS ////////////////////////////////////////////

void RenameImagesDialog::updateOptions(void)
{
    m_addOriginalFileName = optionsDialog->m_addOriginalFileName->isChecked();
    m_addImageFileDate = optionsDialog->m_addImageFileDate->isChecked();
    m_addImageFileSize = optionsDialog->m_addImageFileSize->isChecked();
    m_reverseOrder = optionsDialog->m_reverseOrder->isChecked();
    m_prefix = optionsDialog->m_prefix->text();
    m_sortType = optionsDialog->m_sortType->currentItem();
    m_enumeratorStart = optionsDialog->m_enumeratorStart->value();
    m_dateChange = optionsDialog->m_dateChange->isChecked();
#if KDE_VERSION >= 0x30200
    m_newDateTime = optionsDialog->m_kDatePicker->dateTime();
#else
    m_newDateTime = optionsDialog->m_kDatePicker->date();
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool RenameImagesDialog::startProcess(void)
{
    if ( m_convertStatus == STOP_PROCESS )
       {
       endProcess();
       return true;
       }

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
    m_listFiles->setCurrentItem(item);

    QString targetAlbum = m_upload->path().path(); // PENDING(blackie) handle remote URLS.
    
    if ( prepareStartProcess(item, targetAlbum) == false ) // If there is a problem during the
       {                                                                 //  preparation -> pass to the next item!
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

    KURL desturl(m_upload->path());
    desturl.addPath( item->nameDest() );

#if KDE_VERSION >= 0x30200
    if ( KIO::NetAccess::exists(desturl, false, kapp->activeWindow() ) == true )
#else
    if ( KIO::NetAccess::exists(desturl) == true )
#endif                   
       {
       switch (overwriteMode())
          {
          case OVERWRITE_ASK:
             {
             int ValRet = KMessageBox::warningYesNoCancel(this,
                          i18n("The destination file \"%1\" already exists.\n"
                          "Do you want overwrite it?").arg(item->nameDest()),
                          i18n("Overwrite Destination Image File"), KStdGuiItem::cont());

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

             if ( ValRet == KMessageBox::Cancel )
                {
                processAborted(false);
                return false;
                }

             break;
             }

          case OVERWRITE_RENAME:
             {
             QFileInfo *Target = new QFileInfo(desturl.path()); // handle remote URLS.
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
             break;

          default:
             {
             endProcess();
             return true;
             break;
             }
          }
       }

    copyItemOperations();
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::copyItemOperations(void)
{
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
    m_listFiles->setCurrentItem(item);

    KURL target = m_upload->path();
    target.addPath( item->nameDest() );
    KIO::CopyJob* job;
    
    if ( m_removeOriginal->isChecked() == true )
       job = KIO::move(item->pathSrc(), target, false);
    else
       job = KIO::copy(item->pathSrc(), target, false);

    connect(job, SIGNAL(result(KIO::Job*)),
            this, SLOT(slotResult(KIO::Job*)));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("RenameImages Settings");

    m_prefix = m_config->readEntry("PrefixString", "");

    if ( m_config->readEntry("AddOriginalFileName", "false") == "true")
       m_addOriginalFileName = true;
    else
       m_addOriginalFileName = false;

    if ( m_config->readEntry("AddImageFileDate", "false") == "true")
       m_addImageFileDate = true;
    else
       m_addImageFileDate = false;

    if ( m_config->readEntry("AddImageFileSize", "false") == "true")
       m_addImageFileSize = true;
    else
       m_addImageFileSize = false;

    m_enumeratorStart = m_config->readNumEntry("FirstRenameValue", 1);
    m_sortType = m_config->readNumEntry("SortMethod", 0);      // Sort by Name method by default...

    if ( m_config->readEntry("ReverseOrder", "false") == "true")
       m_reverseOrder = true;
    else
       m_reverseOrder = false;

    if ( m_config->readEntry("DateChange", "false") == "true")
       m_dateChange = true;
    else
       m_dateChange = false;

    m_overWriteMode->setCurrentItem(m_config->readNumEntry("OverWriteMode", 2));  // 'Rename' per default...

    if (m_config->readEntry("RemoveOriginal", "false") == "true")
        m_removeOriginal->setChecked( true );
    else
        m_removeOriginal->setChecked( false );

    delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::saveSettings(void)
{
    // Write all settings in configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("RenameImages Settings");

    m_config->writeEntry("PrefixString", m_prefix);
    m_config->writeEntry("AddOriginalFileName", m_addOriginalFileName);
    m_config->writeEntry("AddImageFileDate", m_addImageFileDate);
    m_config->writeEntry("AddImageFileSize", m_addImageFileSize);
    m_config->writeEntry("FirstRenameValue", m_enumeratorStart);
    m_config->writeEntry("SortMethod", m_sortType);
    m_config->writeEntry("ReverseOrder", m_reverseOrder);
    m_config->writeEntry("DateChange", m_dateChange);

    m_config->writeEntry("OverWriteMode", m_overWriteMode->currentItem());
    m_config->writeEntry("RemoveOriginal", m_removeOriginal->isChecked());

    m_config->sync();

    delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::listImageFiles(void)
{
    BatchProcessImagesDialog::listImageFiles();

    int sortMethod;

    switch (m_sortType)
       {
       case SORT_BY_NAME:
          {
          sortMethod = QDir::Name;
          break;
          }

       case SORT_BY_SIZE:
          {
          sortMethod = QDir::Size;
          break;
          }

       case SORT_BY_DATE:
          {
          sortMethod = QDir::Time;
          break;
          }

       default:
          sortMethod = QDir::Name;
          break;
       }
    
    QStringList AlbumItemListed;
    KURL::List images;
    
    for ( QListViewItem* it = m_listFiles->firstChild(); it; it = it->nextSibling() ) 
        {
        BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*>(it);
        images.append( pitem->pathSrc() );
        QString currentAlbumName = pitem->pathSrc().section('/', 0, -2);

        if ( AlbumItemListed.find(currentAlbumName) == AlbumItemListed.end() )
           AlbumItemListed.append(currentAlbumName);
        }

    m_listFiles->clear();

    int imageIndex = 0;
    
    QStringList selectedImageFiles;
    
    for( KURL::List::Iterator urlIt = images.begin(); urlIt != images.end(); ++urlIt )
        selectedImageFiles.append( (*urlIt).path() ); // PENDING(blackie) handle remote URLS

    for( QStringList::Iterator itAlbum = AlbumItemListed.begin() ; itAlbum != AlbumItemListed.end() ; ++itAlbum )
       {
       QString AlbumName = *itAlbum;
       QDir *dir = new QDir(AlbumName);
       const QFileInfoList* fileinfolist;

       if (m_reverseOrder == false)
          fileinfolist = dir->entryInfoList(QDir::Files | QDir::NoSymLinks, sortMethod);
       else
          fileinfolist = dir->entryInfoList(QDir::Files | QDir::NoSymLinks, sortMethod | QDir::Reversed );

       QFileInfoListIterator it(*fileinfolist);
       QFileInfo* fi;

       while( (fi = it.current() ) )
          {
          if (fi->fileName() == "." || fi->fileName() == ".." )
             {
             ++it;
             continue;
             }

          if ( selectedImageFiles.find(fi->filePath()) != selectedImageFiles.end() )
             {
             new BatchProcessImagesItem(m_listFiles,
                                        fi->filePath(),
                                        fi->fileName(),
                                        oldFileName2NewFileName(fi, imageIndex),
                                        ""
                                        );
             ++imageIndex;
             }

          ++it;
          }
       }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

QString RenameImagesDialog::oldFileName2NewFileName(QFileInfo *fi, int id)
{
    QString Temp, Temp2;
    int nbChar;

    // Fill the new filename index with '0' if necessary.

    Temp2 = Temp2.setNum( m_enumeratorStart + m_nbItem );
    nbChar = Temp2.length();
    Temp2 = Temp2.setNum( id + m_enumeratorStart );
    nbChar = nbChar - Temp2.length();
    Temp = m_prefix + Temp.fill('0', nbChar);
    Temp = Temp + Temp2;

    if ( m_addOriginalFileName == true )                                      // Add old filename.
       Temp = Temp + "-" + fi->fileName().left( fi->fileName().findRev('.',-1) );

    if ( m_addImageFileDate == true )
       Temp = Temp + "-" + fi->lastModified().toString("ddMMyyyy");           // Add last mofified date of file.

    if ( m_addImageFileSize == true )
       Temp = Temp + i18n("-(%1)").arg( KIO::convertSize(fi->size()));        // Add file size.

    Temp = Temp + "." + fi->extension();                                      // Add file extension.

    return Temp;
}

}  // NameSpace KIPIBatchProcessImagesPlugin

#include "renameimagesdialog.moc"
