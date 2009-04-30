/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "batchprocessimagesdialog.h"
#include "batchprocessimagesdialog.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// Qt includes

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QTimer>

// KDE includes

#include <kapplication.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <kfileitem.h>
#include <kio/netaccess.h>
#include <kio/previewjob.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>

// LibKIPI includes

#include <libkipi/uploadwidget.h>

// KIPI includes

#include "imagedialog.h"

// Local includes

#include "imagepreview.h"
#include "outputdialog.h"
#include "pluginsversion.h"

namespace KIPIBatchProcessImagesPlugin
{

BatchProcessImagesDialog::BatchProcessImagesDialog(KUrl::List urlList, KIPI::Interface* interface,
                                                   QString caption, QWidget *parent)
                        : KDialog(parent)
                        , m_selectedImageFiles(urlList), m_interface(interface)
{
    setCaption(caption);
    setButtons(Help | User1 | Cancel);
    setButtonText(User1, i18n("&Start"));
    showButtonSeparator(false);

    // Init. Tmp folder

    KStandardDirs dir;
    m_tmpFolder = dir.saveLocation("tmp", "kipi-batchprocessimagesplugin-" + QString::number(getpid()));

    m_convertStatus  = NO_PROCESS;
    m_progressStatus = 0;
    m_ProcessusProc  = 0;
    m_PreviewProc    = 0;

    //---------------------------------------------

    m_labelType     = new QLabel;
    m_Type          = new KComboBox;
    m_optionsButton = new QPushButton(this, "OptionButton");
    m_optionsButton->setText(i18n("Options"));
    m_optionsButton->setWhatsThis(i18n("You can choose here the options to use for the current process."));

    m_smallPreview = new QCheckBox(i18n("Small preview"));
    m_smallPreview->setWhatsThis(i18n("If you enable this option, "
                                      "all preview effects will be calculated on a small zone "
                                      "of the image (300x300 pixels in the top left corner). "
                                      "Enable this option if you have a slow computer."));
    m_smallPreview->setChecked(true);

    m_previewButton = new QPushButton;
    m_previewButton->setObjectName("PreviewButton");
    m_previewButton->setText(i18n("&Preview"));
    m_previewButton->setWhatsThis(i18n("This button builds a process "
                                       "preview for the currently selected image on the list."));

    groupBox1 = new QGroupBox;
    QGridLayout *gb1Layout = new QGridLayout;
    gb1Layout->addWidget(m_labelType,     0, 0, 1, 1);
    gb1Layout->addWidget(m_Type,          0, 1, 1, 1);
    gb1Layout->addWidget(m_optionsButton, 0, 3, 1, 1);
    gb1Layout->addWidget(m_smallPreview,  1, 0, 1, 2);
    gb1Layout->addWidget(m_previewButton, 1, 3, 1, 1);
    gb1Layout->setSpacing(KDialog::spacingHint());
    gb1Layout->setMargin(KDialog::marginHint());
    groupBox1->setLayout(gb1Layout);

    //---------------------------------------------

    m_labelOverWrite = new QLabel(i18n("Overwrite mode:"));
    m_overWriteMode  = new KComboBox;
    m_overWriteMode->insertItem(i18n("Ask"));
    m_overWriteMode->insertItem(i18n("Always Overwrite"));
    m_overWriteMode->insertItem(i18n("Rename"));
    m_overWriteMode->insertItem(i18n("Skip"));
    m_overWriteMode->setCurrentText(i18n("Rename"));
    m_overWriteMode->setWhatsThis(i18n("Select here the overwrite mode used if your target's image "
                                       "files already exist."));

    m_removeOriginal = new QCheckBox(i18n("Remove original"));
    m_removeOriginal->setWhatsThis(i18n("If you enable this option, "
                                        "all original image files will be removed after processing."));
    m_removeOriginal->setChecked(false);

    groupBox2              = new QGroupBox(i18n("File Operations"));
    QGridLayout *gb2Layout = new QGridLayout;
    gb2Layout->addWidget(m_labelOverWrite, 0, 0, 1, 1);
    gb2Layout->addWidget(m_overWriteMode,  0, 1, 1, 1);
    gb2Layout->addWidget(m_removeOriginal, 1, 0, 1,-1);
    groupBox2->setLayout(gb2Layout);

    //---------------------------------------------

    m_destinationURL = new KUrlRequester;
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
    m_destinationURL->setWhatsThis(i18n("Here you can select the target folder which "
                                        "will used by the process."));

    groupBox3 = new QGroupBox(i18n("Target Folder"));
    QGridLayout *gb3Layout = new QGridLayout;
    gb3Layout->addWidget(m_destinationURL,  0, 0, 1, 1);
    groupBox3->setLayout(gb3Layout);

    //---------------------------------------------

    m_listFiles = new BatchProcessImagesList(this);
    m_listFiles->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    m_addImagesButton = new QPushButton(i18n("&Add..."));
    m_addImagesButton->setWhatsThis(i18n("Add images to the list."));

    m_remImagesButton = new QPushButton(i18n("&Remove"));
    m_remImagesButton->setWhatsThis(i18n("Remove selected image from the list."));

    m_imageLabel = new QLabel;
    m_imageLabel->setFixedHeight(80);
    m_imageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_imageLabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    m_imageLabel->setWhatsThis(i18n("The preview of the selected image on the list."));

    groupBox4 = new QGroupBox;
    QGridLayout *gb4Layout = new QGridLayout;
    gb4Layout->addWidget(m_listFiles,        0, 0,-1, 1);
    gb4Layout->addWidget(m_addImagesButton,  0, 1, 1, 1);
    gb4Layout->addWidget(m_remImagesButton,  1, 1, 1, 1);
    gb4Layout->addWidget(m_imageLabel,       2, 1, 1, 1);
    gb4Layout->setColStretch(0, 10);
    gb4Layout->setRowStretch(3, 10);
    groupBox4->setLayout(gb4Layout);

    //---------------------------------------------

    m_progress = new QProgressBar;
    m_progress->setMaximum(100);
    m_progress->setValue(0);
    m_progress->setWhatsThis(i18n("This is the current percentage of the task completed."));
    m_progress->setVisible(false);

    QWidget* box            = new QWidget(this);
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(groupBox1,  0, 0, 1, 1);
    mainLayout->addWidget(groupBox2,  0, 1, 1, 1);
    mainLayout->addWidget(groupBox3,  1, 0, 1,-1);
    mainLayout->addWidget(groupBox4,  2, 0, 1,-1);
    mainLayout->addWidget(m_progress, 3, 0, 1,-1);
    box->setLayout(mainLayout);

    setMainWidget(box);

    //---------------------------------------------

    connect(m_listFiles, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            this, SLOT(slotListDoubleClicked(QTreeWidgetItem *)));

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

    connect(m_listFiles, SIGNAL( currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem *) ),
            this, SLOT( slotImageSelected( QTreeWidgetItem * )));

    connect(m_addImagesButton, SIGNAL(clicked()),
            this, SLOT(slotImagesFilesButtonAdd()));

    connect(m_remImagesButton, SIGNAL(clicked()),
            this, SLOT(slotImagesFilesButtonRem()));

   // Get the image files filters from the hosts app.

    m_ImagesFilesSort = m_interface->hostSetting("FileExtensions").toString();
}

BatchProcessImagesDialog::~BatchProcessImagesDialog()
{
}

void BatchProcessImagesDialog::slotImagesFilesButtonAdd(void)
{
    QStringList ImageFilesList;

    KUrl::List urls = KIPIPlugins::ImageDialog::getImageURLs(this, m_interface);

    for (KUrl::List::Iterator it = urls.begin(); it != urls.end(); ++it)
        ImageFilesList << (*it).path(); // PENDING(blackie) handle remote URLS

    if (urls.isEmpty())
        return;

    slotAddDropItems(ImageFilesList);
}

void BatchProcessImagesDialog::slotImagesFilesButtonRem(void)
{
    BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*> (m_listFiles->currentItem());

    if (pitem)
    {
        m_selectedImageFiles.removeOne(pitem->pathSrc());
        delete pitem;

        /*
        if (m_listFiles->currentItem()) {
            m_listFiles->currentItem()->setSelected(true);
        }
        */
        m_nbItem = m_selectedImageFiles.count();

        if (m_nbItem == 0)
            groupBox4->setTitle(i18n("Image Files List"));
        else
            groupBox4->setTitle(i18np("Image File List (1 item)", "Image File List (%1 items)", m_nbItem));
    }
}

void BatchProcessImagesDialog::slotImageSelected(QTreeWidgetItem * item)
{
    if (!item || m_listFiles->topLevelItemCount() == 0)
    {
        m_imageLabel->clear();
        return;
    }

    BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*> (item);
    if (!pitem)
        return;

    m_imageLabel->clear();

    QString IdemIndexed = "file:" + pitem->pathSrc();

    KUrl url(IdemIndexed);

    KIO::PreviewJob* m_thumbJob = KIO::filePreview(url, m_imageLabel->height());

    connect(m_thumbJob, SIGNAL(gotPreview(const KFileItem&, const QPixmap&)),
            this, SLOT(slotGotPreview(const KFileItem&, const QPixmap&)));
}

void BatchProcessImagesDialog::slotGotPreview(const KFileItem& item, const QPixmap &pixmap)
{
    QPixmap pix(pixmap);

    // Rotate the thumbnail compared to the angle the host application dictate
    KIPI::ImageInfo info = m_interface->info(item.url());
    if (info.angle() != 0)
    {
        QImage img = pix.convertToImage();
        QMatrix matrix;

        matrix.rotate(info.angle());
        img = img.transformed(matrix);
        pix.convertFromImage(img);
    }

    m_imageLabel->setPixmap(pix);
}

void BatchProcessImagesDialog::slotAddDropItems(QStringList filesPath)
{
    if (filesPath.isEmpty())
        return;

    for (QStringList::Iterator it = filesPath.begin(); it != filesPath.end(); ++it)
    {
        QString currentDropFile = *it;

        // Check if the new item already exist in the list.

        bool findItem = false;

        for (KUrl::List::Iterator it2 = m_selectedImageFiles.begin();
             it2 != m_selectedImageFiles.end(); ++it2)
        {
            QString currentFile = (*it2).path(); // PENDING(blackie) Handle URL's

            if (currentFile == currentDropFile)
                findItem = true;
        }

        if (findItem == false)
            m_selectedImageFiles.append(currentDropFile);
    }

    listImageFiles();
}

void BatchProcessImagesDialog::slotProcessStart(void)
{
    if (m_selectedImageFiles.isEmpty() == true)
        return;

    if (m_removeOriginal->isChecked() == true)
    {
        if (KMessageBox::warningContinueCancel(this, i18n(
                "All original image files will be removed from the source Album.\nDo you want to continue?"),
                i18n("Delete Original Image Files"), KStandardGuiItem::cont(), KStandardGuiItem::cancel(),
                "KIPIplugin-BatchProcessImages-AlwaysRemomveOriginalFiles") != KMessageBox::Continue)
            return;
    }

    m_convertStatus = UNDER_PROCESS;
    disconnect(this, SIGNAL(user1Clicked()), this, SLOT(slotProcessStart()));
    showButton(KDialog::Cancel, false);
    setButtonText(User1, i18n("&Stop"));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotProcessStop()));

    enableWidgets(false);
    m_progress->setVisible(true);

    m_listFile2Process_iterator = new QTreeWidgetItemIterator( m_listFiles );
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

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( **m_listFile2Process_iterator );
    m_listFiles->setCurrentItem(item);

    if ( prepareStartProcess(item, targetAlbum) == false ) // If there is a problem during the
    {                                                   // preparation -> pass to the next item!
        ++*m_listFile2Process_iterator;
        ++m_progressStatus;
        m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));
        item = static_cast<BatchProcessImagesItem*>( **m_listFile2Process_iterator );
        m_listFiles->setCurrentItem(item);

        if ( **m_listFile2Process_iterator )
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

    KUrl desturl(targetAlbum + '/' + item->nameDest());

    if ( KIO::NetAccess::exists( desturl, KIO::NetAccess::DestinationSide,
         kapp->activeWindow() ) == true )
    {
       switch (overwriteMode())
       {
          case OVERWRITE_ASK:
          {
             int ValRet = KMessageBox::warningYesNoCancel(this,
                          i18n("The destination file \"%1\" already exists;\n"
                          "do you want overwrite it?", item->nameDest()),
                          i18n("Overwrite Destination Image File"), KStandardGuiItem::cont());

             if ( ValRet == KMessageBox::No )
             {
                item->changeResult(i18n("Skipped."));
                item->changeError(i18n("destination image file already exists (skipped by user)."));
                ++*m_listFile2Process_iterator;
                ++m_progressStatus;
                m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

                if ( **m_listFile2Process_iterator )
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
             QFileInfo Target(targetAlbum + "/" + item->nameDest());
             QString newFileName = RenameTargetImageFile(&Target);

             if ( newFileName.isNull() )
             {
                item->changeResult(i18n("Failed."));
                item->changeError(i18n("destination image file already exists and cannot be renamed."));
                ++*m_listFile2Process_iterator;
                ++m_progressStatus;
                m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

                if ( **m_listFile2Process_iterator )
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
                QFileInfo newTarget(newFileName);
                item->changeNameDest(newTarget.fileName());
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

             if ( **m_listFile2Process_iterator )
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

    m_ProcessusProc = new KProcess(this);
    m_ProcessusProc->setOutputChannelMode(KProcess::MergedChannels);
    initProcess(m_ProcessusProc, item, targetAlbum);
    m_commandLine = m_ProcessusProc->program().join(" ");

    item->changeOutputMess(m_commandLine + "\n\n");

    connect(m_ProcessusProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(slotFinished()));

    connect(m_ProcessusProc, SIGNAL(readyRead()), SLOT(slotReadyRead()));

    m_ProcessusProc->start();
    if(!m_ProcessusProc->waitForStarted())
    {
        KMessageBox::error(this, i18n("Cannot start 'convert' program from 'ImageMagick' package;\n"
                                      "please check your installation."));
        delete m_ProcessusProc;
        m_ProcessusProc=0;
        return false;
    }

    return true;
}

void BatchProcessImagesDialog::slotReadyRead()
{
    BatchProcessImagesItem *item =
            static_cast<BatchProcessImagesItem*> (**m_listFile2Process_iterator);
    QByteArray output = m_ProcessusProc->readAll();
    item->changeOutputMess(QString::fromLocal8Bit(output.data(), output.size()));
}

void BatchProcessImagesDialog::slotFinished()
{
    if (m_convertStatus == PROCESS_DONE)
    {
        // processAborted() has already been called. No need to show the warning.
        return;
    }

    BatchProcessImagesItem *item =
            static_cast<BatchProcessImagesItem*> (**m_listFile2Process_iterator);
    m_listFiles->scrollToItem(m_listFiles->currentItem());

    if (m_ProcessusProc->exitStatus() == QProcess::CrashExit)
    {
        int code = KMessageBox::warningContinueCancel(this,
                                i18n("The 'convert' program from 'ImageMagick' package has been stopped abnormally"),
                                i18n("Error running 'convert'"));

        if (code == KMessageBox::Cancel)
        {
            processAborted(true);
        }
        else
        {
            item->changeResult(i18n("Failed."));
            item->changeError(i18n("'convert' program from 'ImageMagick' package "
                                   "has been stopped abnormally."));
            ++*m_listFile2Process_iterator;
            ++m_progressStatus;
            m_progress->setValue((int) ((float) m_progressStatus * (float) 100 / (float) m_nbItem));

            if (**m_listFile2Process_iterator)
                startProcess();
            else
                endProcess();
        }
        return;
    }

    int ValRet = m_ProcessusProc->exitCode();
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
            src.setPath(item->pathSrc());
            KUrl dest = m_destinationURL->url();
            dest.addPath(item->nameDest());
            QString errmsg;

            KUrl::List urlList;
            urlList.append(src);
            urlList.append(dest);
            m_interface->refreshImages(urlList);

            if (!item->overWrote())
            {
                // Do not add an entry if there was an image at the location already.
                bool ok = m_interface->addImage(dest, errmsg);

                if (!ok)
                {
                    int code = KMessageBox::warningContinueCancel(this, i18n(
                            "<qt>Error adding image to application; error message was: "
                            "<b>%1</b></qt>", errmsg),
                            i18n("Error Adding Image to Application"));

                    if (code == KMessageBox::Cancel)
                    {
                        slotProcessStop();
                        break;
                    }
                    else
                        item->changeResult(i18n("Failed."));
                }
            }

            if (src != dest)
            {
                KIPI::ImageInfo srcInfo = m_interface->info(src);
                KIPI::ImageInfo destInfo = m_interface->info(dest);
                destInfo.cloneData(srcInfo);
            }

            if (m_removeOriginal->isChecked() && src != dest)
            {
                KUrl deleteImage(item->pathSrc());

                if (KIO::NetAccess::del(deleteImage, kapp->activeWindow()) == false)
                {
                    item->changeResult(i18n("Warning:"));
                    item->changeError(i18n("cannot remove original image file."));
                }
                else
                    m_interface->delImage(item->pathSrc());
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

    if ( **m_listFile2Process_iterator )
        startProcess();
    else
        endProcess();
}

void BatchProcessImagesDialog::slotListDoubleClicked(QTreeWidgetItem *itemClicked)
{
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*> (itemClicked);

    if (m_convertStatus == PROCESS_DONE)
    {
        OutputDialog *infoDialog = new OutputDialog(this, i18n("Image processing error"),
                                                    item->outputMess(),
                                                    i18n("Image \"%1\": %2\n\nThe output messages are:\n",
                                                         item->nameSrc(),
                                                         item->error()));
        infoDialog->exec();
    }
}

void BatchProcessImagesDialog::slotPreview(void)
{
    kWarning() << "BatchProcessImagesDialog::slotPreview" << endl;

    if (m_listFiles->currentItem() == 0)
    {
        KMessageBox::error(this, i18n("You must select an item from the list to calculate the preview."));
        return;
    }

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*> (m_listFiles->currentItem());

    enableWidgets(false);

    disconnect( this, SIGNAL(user1Clicked()),
                this, SLOT(slotProcessStart()));

    showButton(KDialog::Cancel, false);
    setButtonText(User1, i18n("&Stop"));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotPreviewStop()));

    m_PreviewProc = new KProcess(this);
    m_PreviewProc->setOutputChannelMode(KProcess::MergedChannels);
    initProcess(m_PreviewProc, item, QString(), true);

    m_previewOutput = m_PreviewProc->program().join(" ");

    *m_PreviewProc << m_tmpFolder + "/" + QString::number(getpid()) + "preview.PNG";
    m_previewOutput.append( " "  + m_tmpFolder + "/" + QString::number(getpid()) + "preview.PNG\n\n");

    connect(m_PreviewProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(slotPreviewFinished()));
    connect(m_PreviewProc, SIGNAL(readyRead()), SLOT(slotPreviewReadyRead()));

    m_PreviewProc->start();
    if(!m_PreviewProc->waitForStarted())
    {
        KMessageBox::error(this, i18n("Cannot start 'convert' program from 'ImageMagick' package;\n"
                                      "please check your installation."));
        m_previewButton->setEnabled(true);
        return;
    }
}

void BatchProcessImagesDialog::slotPreviewReadyRead()
{
    QByteArray output = m_PreviewProc->readAll();
    m_previewOutput.append( QString::fromLocal8Bit(output.data(), output.size()));
}

void BatchProcessImagesDialog::slotPreviewFinished()
{
    if (m_PreviewProc->exitStatus() == QProcess::CrashExit)
    {
        KMessageBox::error(this, i18n("Cannot run properly 'convert' program from 'ImageMagick' package."));
        m_previewButton->setEnabled(true);
        return;
    }

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>( m_listFiles->currentItem() );
    int ValRet = m_PreviewProc->exitCode();

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

       KIO::NetAccess::del( deletePreviewImage, kapp->activeWindow() );
    }
    else
    {
       OutputDialog *infoDialog = new OutputDialog(this,
                                                   i18n("Preview processing error"),
                                                   m_previewOutput,
                                                   i18n("Cannot process preview for image \"%1\"."
                                                        "\nThe output messages are:\n",
                                                        item->nameSrc())
                                                   );
       infoDialog->exec();
    }

    endPreview();
}

void BatchProcessImagesDialog::slotPreviewStop(void)
{
    m_PreviewProc->close();

    endPreview();
}

void BatchProcessImagesDialog::slotProcessStop(void)
{
    // Try to kill the current process !
    m_ProcessusProc->close();

    // If kill operation failed, Stop the process at the next image !
    if (m_convertStatus == UNDER_PROCESS)
        m_convertStatus = STOP_PROCESS;

    processAborted(true);
}

void BatchProcessImagesDialog::slotOk()
{
    saveSettings();
    done(Close);
}

void BatchProcessImagesDialog::listImageFiles(void)
{
    m_nbItem = m_selectedImageFiles.count();

    if (m_nbItem == 0)
        groupBox4->setTitle(i18n("Image File List"));
    else
        groupBox4->setTitle(i18np("Image File List (%1 item)",
                                  "Image File List (%1 items)", m_nbItem));

    if (m_selectedImageFiles.isEmpty())
        return;

    for (KUrl::List::Iterator it = m_selectedImageFiles.begin();
         it != m_selectedImageFiles.end(); ++it)
    {
        QString currentFile = (*it).path(); // PENDING(blackie) Handle URLS
        QFileInfo *fi = new QFileInfo(currentFile);

        // Check if the new item already exist in the list.

        bool findItem = false;

        QTreeWidgetItemIterator it2(m_listFiles);

        while (*it2)
        {
            BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*> (*it2);

            if (pitem->pathSrc() == currentFile.section('/', 0, -1))
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

    QTreeWidgetItem* firstItem = m_listFiles->topLevelItem(0);
    if (firstItem)
    {
        m_listFiles->setCurrentItem(firstItem);
        slotImageSelected(firstItem);
        m_listFiles->scrollToItem(firstItem);
    }
}

void BatchProcessImagesDialog::endPreview(void)
{
    enableWidgets(true);
    showButton(KDialog::Cancel, true);

    // Default status if 'slotTypeChanged' isn't re-implemented.
    m_optionsButton->setEnabled(true);
    slotTypeChanged(m_Type->currentItem());

    setButtonText(User1, i18n("&Start"));

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

    BatchProcessImagesItem *item =
            static_cast<BatchProcessImagesItem*> (**m_listFile2Process_iterator);
    m_listFiles->scrollToItem(m_listFiles->currentItem());

    item->changeResult(i18n("Aborted."));
    item->changeError(i18n("process aborted by user"));

    if (removeFlag == true) // Try to delete de destination !
    {
        KUrl deleteImage = m_destinationURL->url();
        deleteImage.addPath(item->nameDest());

        if (KIO::NetAccess::exists(deleteImage, KIO::NetAccess::DestinationSide, kapp->activeWindow())
                == true)
            KIO::NetAccess::del(deleteImage, kapp->activeWindow());
    }

    endProcess();
}

void BatchProcessImagesDialog::endProcess(void)
{
    m_convertStatus = PROCESS_DONE;
    enableWidgets(true);
    QTimer::singleShot(500, m_progress, SLOT(hide()));
    setButtonText(User1, i18n("&Close"));

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
        Temp = Temp.setNum(Enumerator);
        NewDestUrl = fi->filePath().left(fi->filePath().findRev('.', -1)) + "_" + Temp + "."
                     + fi->filePath().section('.', -1);
    } while (Enumerator < 100 && KIO::NetAccess::exists(NewDestUrl, KIO::NetAccess::SourceSide,
             kapp->activeWindow()) == true);

    if (Enumerator == 100)
        return QString();

    return (NewDestUrl.path());
}

void BatchProcessImagesDialog::enableWidgets(bool state)
{
    m_Type->setEnabled(state);
    m_addImagesButton->setEnabled(state);
    m_destinationURL->setEnabled(state);
    m_labelOverWrite->setEnabled(state);
    m_labelType->setEnabled(state);
    m_listFiles->setEnabled(state);
    m_optionsButton->setEnabled(state);
    m_overWriteMode->setEnabled(state);
    m_previewButton->setEnabled(state);
    m_remImagesButton->setEnabled(state);
    m_removeOriginal->setEnabled(state);
    m_smallPreview->setEnabled(state);
}

}  // namespace KIPIBatchProcessImagesPlugin
