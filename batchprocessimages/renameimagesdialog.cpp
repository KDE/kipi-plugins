//////////////////////////////////////////////////////////////////////////////
//
//    RENAMEIMAGESDIALOG.CPP
//
//    Copyright (C) 2003-2004 Gilles CAULIER <caulier dot gilles at free.fr>
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
#include <kdatewidget.h>

// Locale includes

#include "renameimagesdialog.h"
#include "renameimagesoptionsdialog.h"


//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

RenameImagesDialog::RenameImagesDialog( KURL::List urlList, KIPI::Interface* interface, QWidget *parent )
                 : BatchProcessImagesDialog( urlList, interface, parent )
{
    m_nbItem = m_selectedImageFiles.count();

    setCaption(i18n("Batch Rename Images options"));
    setHelp("plugin-renameimages.anchor", "digikam");

    //---------------------------------------------

    groupBox1->setTitle( i18n("Renaming images options") );

    m_labelType->hide();
    m_Type->hide();
    m_previewButton->hide();
    m_smallPreview->hide();

    //---------------------------------------------

    readSettings();
    listImageFiles();
    slotOverWriteModeActived(m_overWriteMode->currentText());
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

RenameImagesDialog::~RenameImagesDialog()
{
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void RenameImagesDialog::slotAbout( void )
{
    KMessageBox::about(this, i18n("A Digikam plugin for batch rename images\n\n"
                                  "Author: Gilles Caulier\n\n"
                                  "Email: caulier dot gilles at free.fr\n"),
                                  i18n("About Digikam batch rename images"));
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
    qDebug("RenameImagesDialog::slotResult is commented out!");
#ifdef TEMPORARILY_REMOVED
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
    Digikam::AlbumInfo *sourceAlbum = Digikam::AlbumManager::instance()
                                      ->findAlbum( item->pathSrc().section('/', -2, -2));
    Digikam::AlbumInfo *targetAlbum = Digikam::AlbumManager::instance()->findAlbum( m_albumList->currentText() );

    if (job->error())
        {
        item->changeResult(i18n("Failed !!!"));
        item->changeError(i18n("cannot copy original image file!"));
        item->changeOutputMess(i18n("Checked the file access right or the space disk!\n"
                                    "The internal error is:\n\n%1")
                                    .arg(job->errorString()));
        }
    else
        {
        // Save the comments if current image taken from Album database.

        if (sourceAlbum)
           {
           sourceAlbum->openDB();
           QString comments = sourceAlbum->getItemComments(item->nameSrc());
           sourceAlbum->closeDB();

           targetAlbum->openDB();
           targetAlbum->setItemComments(item->nameDest(), comments);
           targetAlbum->closeDB();
           }

        // Change image file timestamp if necessary.

        if ( m_dateChange == true )
           {
           if ( changeDate(targetAlbum->getPath() + "/" +  item->nameDest()) == false )
              {
              item->changeResult(i18n("Cannot change time stamp of destination file!"));
              item->changeError(i18n("cannot change time stamp of destination file!"));
              item->changeOutputMess(i18n("Checked the file access right!"));
              }
           }

        // Remove original image file if necessary.

        if ( m_removeOriginal->isChecked() == true )
           {
           KURL deleteImage(item->pathSrc());

           if ( KIO::NetAccess::del(deleteImage) == false )
                {
                item->changeResult(i18n("Cannot delete original !"));
                item->changeError(i18n("cannot remove original image file!"));
                item->changeOutputMess(i18n("Checked the file access right!"));
                }
           else
                {
                item->changeResult(i18n("OK"));
                item->changeError(i18n("no processing error"));
                item->changeOutputMess(i18n("None"));
                }
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
    else                                            // Copy is done...
        endProcess(i18n("Process finished!"));
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::slotTemplateSortListChanged(int index)
{
    updateOptions();
    listImageFiles();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::slotTemplateSortListChanged(bool on)
{
    updateOptions();
    listImageFiles();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::slotTemplateSortListChanged(const QString &text)
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
    m_newDate = optionsDialog->m_kDatePicker->date();

    m_changeModification = true;
    m_changeAccess = true;
    m_hour = 0;
    m_minute = 0;
    m_second = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool RenameImagesDialog::startProcess(void)
{
    qDebug("RenameImagesDialog::startProcess is commented out!");
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

    copyItemOperations();

    return true;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::copyItemOperations(void)
{
    qDebug("RenameImagesDialog::copyItemOperations is commented out!");

#ifdef TEMPORARILY_REMOVED
    Digikam::AlbumInfo *targetAlbum = Digikam::AlbumManager::instance()->findAlbum( m_albumList->currentText() );
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFile2Process_iterator->current() );
    m_listFiles->setCurrentItem(item);

    KIO::CopyJob* job = KIO::copy(item->pathSrc(),
                                  targetAlbum->getPath() + "/" +  item->nameDest(),
                                  true);

    connect(job, SIGNAL(result(KIO::Job*)),
            this, SLOT(slotResult(KIO::Job*)));
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenameImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    m_config = new KConfig("digikamrc");
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

    m_config = new KConfig("digikamrc");
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
    QListViewItemIterator it2( m_listFiles );
    int imageIndex = 0;

    while ( it2.current() )
       {
       BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*>(it2.current());
       QString currentAlbumName = pitem->pathSrc().section('/', 0, -2);

       if ( AlbumItemListed.find(currentAlbumName) == AlbumItemListed.end() )
          AlbumItemListed.append(currentAlbumName);

       ++it2;
       }

    m_listFiles->clear();

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

          if ( m_selectedImageFiles.find(fi->filePath()) != m_selectedImageFiles.end() )
             {
             BatchProcessImagesItem *item = new BatchProcessImagesItem(m_listFiles,
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


/////////////////////////////////////////////////////////////////////////////////////////////////////
// From ShowImg batch renamer

bool RenameImagesDialog::changeDate(QString file)
{
    FILE * f;
    struct utimbuf * t = new utimbuf();
    struct tm tmp;
    struct stat st;

    time_t ti;

    f = fopen(file.ascii (), "r");

    if( f == NULL )
       {
       return false;
       }

    fclose( f );

    tmp.tm_mday = m_newDate.day();
    tmp.tm_mon = m_newDate.month() - 1;
    tmp.tm_year = m_newDate.year() - 1900;

    tmp.tm_hour = m_hour;
    tmp.tm_min = m_minute;
    tmp.tm_sec = m_second;
    tmp.tm_isdst = -1;

    ti = mktime( &tmp );

    if( ti == -1 )
       {
       return false;
       }

    if( stat(file.ascii(), &st ) == -1 )
       {
       return false;
       }

    if(m_changeAccess)
        t->actime = ti;
    else
        t->actime = st.st_atime;

    if(m_changeModification)
        t->modtime = ti;
    else
        t->modtime = st.st_mtime;

    if(utime(file.ascii(), t ) != 0)
       {
       return false;
       }

    return true;
}
