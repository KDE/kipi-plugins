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
#include <QPointer>

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
#include "ui_batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

enum OverWriteMode
{
    OVERWRITE_ASK = 0,
    OVERWRITE_RENAME,
    OVERWRITE_SKIP,
    OVERWRITE_OVER
};

enum ProcessState
{
    NO_PROCESS = 0,
    UNDER_PROCESS,
    PROCESS_DONE,
    STOP_PROCESS
};

BatchProcessImagesDialog::BatchProcessImagesDialog(const KUrl::List& urlList, KIPI::Interface* interface,
                                                   const QString& caption, QWidget *parent)
                        : KDialog(parent),
                          m_listFile2Process_iterator(0),
                          m_selectedImageFiles(urlList),
                          m_interface(interface),
                          m_ui(new Ui::BatchProcessImagesDialog())
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

    setupUi();

    // Get the image files filters from the hosts app.

    m_ImagesFilesSort = m_interface->hostSetting("FileExtensions").toString();
}

void BatchProcessImagesDialog::setupUi()
{
    QWidget* widget = new QWidget(this);
    m_ui->setupUi(widget);
    setMainWidget(widget);

    //---------------------------------------------

    m_ui->m_optionsButton->setWhatsThis(i18n("You can choose here the options to use for the current process."));

    m_ui->m_smallPreview->setWhatsThis(i18n("If you enable this option, "
                                            "all preview effects will be calculated on a small zone "
                                            "of the image (300x300 pixels in the top left corner). "
                                            "Enable this option if you have a slow computer."));

    m_ui->m_previewButton->setWhatsThis(i18n("This button builds a process "
                                        "preview for the currently selected image on the list."));

    m_ui->m_overWriteMode->addItem(i18n("Ask"));
    m_ui->m_overWriteMode->addItem(i18n("Always Overwrite"));
    m_ui->m_overWriteMode->addItem(i18n("Rename"));
    m_ui->m_overWriteMode->addItem(i18n("Skip"));
    m_ui->m_overWriteMode->setCurrentItem(i18n("Rename"));
    m_ui->m_overWriteMode->setWhatsThis(i18n("Select here the overwrite mode used if your target's image "
                                        "files already exist."));

    m_ui->m_removeOriginal->setWhatsThis(i18n("If you enable this option, "
                                         "all original image files will be removed after processing."));

    m_ui->m_destinationUrl->setMode(KFile::Directory | KFile::LocalOnly);
    KIPI::ImageCollection album = m_interface->currentAlbum();
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
        m_ui->m_destinationUrl->lineEdit()->setText(url);
    }
    m_ui->m_destinationUrl->setWhatsThis(i18n("Here you can select the target folder which "
                                         "will used by the process."));

    m_ui->m_addImagesButton->setWhatsThis(i18n("Add images to the list."));
    m_ui->m_remImagesButton->setWhatsThis(i18n("Remove selected image from the list."));
    m_ui->m_imageLabel->setWhatsThis(i18n("The preview of the selected image on the list."));

    m_ui->m_progress->setVisible(false);

    // Adapt names for inherited dialogs
    // FIXME: Use accessors
    m_labelType     = m_ui->m_optionComboBoxLabel;
    m_Type          = m_ui->m_optionComboBox;
    m_optionsButton = m_ui->m_optionsButton;
    m_listFiles     = m_ui->m_listView;

    connect(m_listFiles, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            this, SLOT(slotListDoubleClicked(QTreeWidgetItem *)));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotProcessStart()));

    connect(m_ui->m_optionsButton, SIGNAL(clicked()),
            this, SLOT(slotOptionsClicked()));

    connect(m_ui->m_previewButton, SIGNAL(clicked()),
            this, SLOT(slotPreview()));

    connect(m_Type, SIGNAL(activated(int)),
            this, SLOT(slotTypeChanged(int)));

    connect(m_listFiles, SIGNAL(addedDropItems(QStringList)),
            this, SLOT(slotAddDropItems(QStringList)));

    connect(m_listFiles, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            this, SLOT(slotImageSelected(QTreeWidgetItem *)));

    connect(m_ui->m_addImagesButton, SIGNAL(clicked()),
            this, SLOT(slotImagesFilesButtonAdd()));

    connect(m_ui->m_remImagesButton, SIGNAL(clicked()),
            this, SLOT(slotImagesFilesButtonRem()));
}

BatchProcessImagesDialog::~BatchProcessImagesDialog()
{
    delete m_ui;
    delete m_listFile2Process_iterator;
}

void BatchProcessImagesDialog::slotImagesFilesButtonAdd()
{
    QStringList ImageFilesList;

    KUrl::List urls = KIPIPlugins::ImageDialog::getImageUrls(this, m_interface);

    for (KUrl::List::Iterator it = urls.begin(); it != urls.end(); ++it)
        ImageFilesList << (*it).path(); // PENDING(blackie) handle remote URLS

    if (urls.isEmpty())
        return;

    slotAddDropItems(ImageFilesList);
}

void BatchProcessImagesDialog::slotImagesFilesButtonRem()
{
    BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*>(m_listFiles->currentItem());

    if (pitem) {
        m_selectedImageFiles.removeOne(pitem->pathSrc());
        delete pitem;

        m_nbItem = m_selectedImageFiles.count();
    }
}

void BatchProcessImagesDialog::slotImageSelected(QTreeWidgetItem * item)
{
    if (!item || m_listFiles->topLevelItemCount() == 0)
    {
        m_ui->m_imageLabel->clear();
        return;
    }

    BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*>(item);
    if (!pitem)
        return;

    m_ui->m_imageLabel->clear();

    QString IdemIndexed = "file:" + pitem->pathSrc();

    KUrl url(IdemIndexed);

    KIO::PreviewJob* m_thumbJob = KIO::filePreview(url, m_ui->m_imageLabel->height());

    connect(m_thumbJob, SIGNAL(gotPreview(const KFileItem&, const QPixmap&)),
            this, SLOT(slotGotPreview(const KFileItem&, const QPixmap&)));
}

void BatchProcessImagesDialog::slotGotPreview(const KFileItem& item, const QPixmap& pixmap)
{
    QPixmap pix(pixmap);

    // Rotate the thumbnail compared to the angle the host application dictate
    KIPI::ImageInfo info = m_interface->info(item.url());
    if (info.angle() != 0)
    {
        QImage img = pix.toImage();
        QMatrix matrix;

        matrix.rotate(info.angle());
        img = img.transformed(matrix);
        pix.fromImage(img);
    }

    m_ui->m_imageLabel->setPixmap(pix);
}

void BatchProcessImagesDialog::slotAddDropItems(const QStringList& filesPath)
{
    if (filesPath.isEmpty())
        return;

    for (QStringList::const_iterator it = filesPath.constBegin(); it != filesPath.constEnd(); ++it)
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

void BatchProcessImagesDialog::slotProcessStart()
{
    if (m_selectedImageFiles.isEmpty() == true)
        return;

    if (m_ui->m_removeOriginal->isChecked() == true)
    {
        if (KMessageBox::warningContinueCancel(this, i18n(
                                                   "All original image files will be removed from the source Album.\nDo you want to continue?"),
                                               i18n("Delete Original Image Files"), KStandardGuiItem::cont(), KStandardGuiItem::cancel(),
                                               "KIPIplugin-BatchProcessImages-AlwaysRemomveOriginalFiles") != KMessageBox::Continue)
            return;
    }

    m_convertStatus = UNDER_PROCESS;

    disconnect(this, SIGNAL(user1Clicked()),
               this, SLOT(slotProcessStart()));

    showButton(KDialog::Cancel, false);
    setButtonText(User1, i18n("&Stop"));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotProcessStop()));

    enableWidgets(false);
    m_ui->m_progress->setVisible(true);

    m_listFile2Process_iterator = new QTreeWidgetItemIterator(m_listFiles);
    startProcess();
}

bool BatchProcessImagesDialog::startProcess()
{
    if (m_convertStatus == STOP_PROCESS)
    {
        endProcess();
        return true;
    }

    QString targetAlbum = m_ui->m_destinationUrl->url().path();

    //TODO check if it is valid also for remote URL's
    // this is a workarond for bug 117397
    QFileInfo dirInfo(targetAlbum + "/");
    if (!dirInfo.isDir() || !dirInfo.isWritable())
    {
        KMessageBox::error(this, i18n("You must specify a writable path for your output file."));
        endProcess();
        return true;
    }

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>(**m_listFile2Process_iterator);
    m_listFiles->setCurrentItem(item);

    if (prepareStartProcess(item, targetAlbum) == false)   // If there is a problem during the
    {                                                   // preparation -> pass to the next item!
        ++*m_listFile2Process_iterator;
        ++m_progressStatus;
        m_ui->m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));
        item = static_cast<BatchProcessImagesItem*>(**m_listFile2Process_iterator);
        m_listFiles->setCurrentItem(item);

        if (**m_listFile2Process_iterator)
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

    if (KIO::NetAccess::exists(desturl, KIO::NetAccess::DestinationSide,
                               kapp->activeWindow()) == true)
    {
        switch (overwriteMode())
        {
            case OVERWRITE_ASK:
            {
                int ValRet = KMessageBox::warningYesNoCancel(this,
                            i18n("The destination file \"%1\" already exists;\n"
                                  "do you want overwrite it?", item->nameDest()),
                            i18n("Overwrite Destination Image File"), KStandardGuiItem::cont());

                if (ValRet == KMessageBox::No)
                {
                    item->changeResult(i18n("Skipped."));
                    item->changeError(i18n("destination image file already exists (skipped by user)."));
                    ++*m_listFile2Process_iterator;
                    ++m_progressStatus;
                    m_ui->m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

                    if (**m_listFile2Process_iterator)
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
                else if (ValRet == KMessageBox::Cancel)
                {
                    processAborted(false);
                    return false;
                }
                else
                {
                    item->setDidOverWrite(true);
                }

                break;
            }

            case OVERWRITE_RENAME:
            {
                QFileInfo Target(targetAlbum + "/" + item->nameDest());
                QString newFileName = RenameTargetImageFile(&Target);

                if (newFileName.isNull())
                {
                    item->changeResult(i18n("Failed."));
                    item->changeError(i18n("destination image file already exists and cannot be renamed."));
                    ++*m_listFile2Process_iterator;
                    ++m_progressStatus;
                    m_ui->m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

                    if (**m_listFile2Process_iterator)
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
                m_ui->m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

                if (**m_listFile2Process_iterator)
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
                item->setDidOverWrite(true);
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

    connect(m_ProcessusProc, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(slotFinished()));

    connect(m_ProcessusProc, SIGNAL(readyRead()),
            this, SLOT(slotReadyRead()));

    m_ProcessusProc->start();
    if (!m_ProcessusProc->waitForStarted())
    {
        KMessageBox::error(this, i18n("Cannot start 'convert' program from 'ImageMagick' package;\n"
                                      "please check your installation."));
        delete m_ProcessusProc;
        m_ProcessusProc = 0;
        return false;
    }

    return true;
}

void BatchProcessImagesDialog::slotReadyRead()
{
    BatchProcessImagesItem *item =
        static_cast<BatchProcessImagesItem*>(**m_listFile2Process_iterator);
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

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>(**m_listFile2Process_iterator);
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
            m_ui->m_progress->setValue((int)((float) m_progressStatus * (float) 100 / (float) m_nbItem));

            if (**m_listFile2Process_iterator)
                startProcess();
            else
                endProcess();
        }
        return;
    }

    int ValRet = m_ProcessusProc->exitCode();
    kWarning() << "Convert exit (" << ValRet << ")";

    switch (ValRet)
    {
        case 0:
        { // Process finished successfully !
            item->changeResult(i18n("OK"));
            item->changeError(i18n("no processing error"));
            processDone();

            // Save the comments for the converted image
            KUrl src;
            src.setPath(item->pathSrc());
            KUrl dest = m_ui->m_destinationUrl->url();
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
                    {
                        item->changeResult(i18n("Failed."));
                    }
                }
            }

            if (src != dest)
            {
                KIPI::ImageInfo srcInfo = m_interface->info(src);
                KIPI::ImageInfo destInfo = m_interface->info(dest);
                destInfo.cloneData(srcInfo);
            }

            if (m_ui->m_removeOriginal->isChecked() && src != dest)
            {
                KUrl deleteImage(item->pathSrc());

                if (KIO::NetAccess::del(deleteImage, kapp->activeWindow()) == false)
                {
                    item->changeResult(i18n("Warning:"));
                    item->changeError(i18n("cannot remove original image file."));
                }
                else
                {
                    m_interface->delImage(item->pathSrc());
                }
            }
            break;
        }
        case 15:
        { //  process aborted !
            processAborted(true);
            break;
        }
        default :
        { // Processing error !
            item->changeResult(i18n("Failed."));
            item->changeError(i18n("cannot process original image file."));
            break;
        }
    }

    ++*m_listFile2Process_iterator;
    ++m_progressStatus;
    m_ui->m_progress->setValue((int)((float)m_progressStatus *(float)100 / (float)m_nbItem));

    if (**m_listFile2Process_iterator)
        startProcess();
    else
        endProcess();
}

void BatchProcessImagesDialog::slotListDoubleClicked(QTreeWidgetItem *itemClicked)
{
    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>(itemClicked);

    if (m_convertStatus == PROCESS_DONE)
    {
        QPointer<OutputDialog> infoDialog = new OutputDialog(this, i18n("Image processing error"),
                item->outputMess(),
                i18n("Image \"%1\": %2\n\nThe output messages are:\n",
                     item->nameSrc(),
                     item->error()));
        infoDialog->exec();
        delete infoDialog;
    }
}

void BatchProcessImagesDialog::slotPreview(void)
{
    kDebug() << "BatchProcessImagesDialog::slotPreview";

    if (m_listFiles->currentItem() == 0)
    {
        KMessageBox::error(this, i18n("You must select an item from the list to calculate the preview."));
        return;
    }

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>(m_listFiles->currentItem());

    enableWidgets(false);

    disconnect(this, SIGNAL(user1Clicked()),
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
    m_previewOutput.append(" "  + m_tmpFolder + "/" + QString::number(getpid()) + "preview.PNG\n\n");

    connect(m_PreviewProc, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(slotPreviewFinished()));

    connect(m_PreviewProc, SIGNAL(readyRead()),
            this, SLOT(slotPreviewReadyRead()));

    m_PreviewProc->start();
    if (!m_PreviewProc->waitForStarted())
    {
        KMessageBox::error(this, i18n("Cannot start 'convert' program from 'ImageMagick' package;\n"
                                      "please check your installation."));
        m_ui->m_previewButton->setEnabled(true);
        return;
    }
}

void BatchProcessImagesDialog::slotPreviewReadyRead()
{
    QByteArray output = m_PreviewProc->readAll();
    m_previewOutput.append(QString::fromLocal8Bit(output.data(), output.size()));
}

void BatchProcessImagesDialog::slotPreviewFinished()
{
    if (m_PreviewProc->exitStatus() == QProcess::CrashExit)
    {
        KMessageBox::error(this, i18n("Cannot run properly 'convert' program from 'ImageMagick' package."));
        m_ui->m_previewButton->setEnabled(true);
        return;
    }

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>(m_listFiles->currentItem());
    int ValRet                   = m_PreviewProc->exitCode();

    kDebug() << "Convert exit (" << ValRet << ")";

    if (ValRet == 0)
    {
        QString cropTitle = "";

        if (m_ui->m_smallPreview->isChecked())
            cropTitle = i18n(" - small preview");

        QPointer<ImagePreview> previewDialog = new ImagePreview(item->pathSrc(), m_tmpFolder + "/"
                + QString::number(getpid()) + "preview.PNG", m_tmpFolder, m_ui->m_smallPreview->isChecked(),
                false, m_Type->currentText() + cropTitle, item->nameSrc(), this);
        previewDialog->exec();
        delete previewDialog;

        KUrl deletePreviewImage(m_tmpFolder + "/" + QString::number(getpid()) + "preview.PNG");

        KIO::NetAccess::del(deletePreviewImage, kapp->activeWindow());
    }
    else
    {
        QPointer<OutputDialog> infoDialog = new OutputDialog(this, i18n("Preview processing error"),
                m_previewOutput, i18n("Cannot process preview for image \"%1\"."
                                      "\nThe output messages are:\n", item->nameSrc()));
        infoDialog->exec();
        delete infoDialog;
    }

    endPreview();
}

void BatchProcessImagesDialog::slotPreviewStop()
{
    m_PreviewProc->close();

    endPreview();
}

void BatchProcessImagesDialog::slotProcessStop()
{
    // Try to kill the current process !
    if (m_ProcessusProc)
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

void BatchProcessImagesDialog::listImageFiles()
{
    m_nbItem = m_selectedImageFiles.count();

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
            BatchProcessImagesItem *pitem = static_cast<BatchProcessImagesItem*>(*it2);

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

void BatchProcessImagesDialog::endPreview()
{
    enableWidgets(true);
    showButton(KDialog::Cancel, true);

    // Default status if 'slotTypeChanged' isn't re-implemented.
    m_ui->m_optionsButton->setEnabled(true);
    slotTypeChanged(m_Type->currentIndex());

    setButtonText(User1, i18n("&Start"));

    disconnect(this, SIGNAL(user1Clicked()),
               this, SLOT(slotPreviewStop()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotProcessStart()));
}

int BatchProcessImagesDialog::overwriteMode()
{
    QString OverWrite = m_ui->m_overWriteMode->currentText();

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
    kDebug() << "BatchProcessImagesDialog::processAborted";

    BatchProcessImagesItem *item = static_cast<BatchProcessImagesItem*>(**m_listFile2Process_iterator);
    m_listFiles->scrollToItem(m_listFiles->currentItem());

    item->changeResult(i18n("Aborted."));
    item->changeError(i18n("process aborted by user"));

    if (removeFlag == true)
    { // Try to delete de destination !
        KUrl deleteImage = m_ui->m_destinationUrl->url();
        deleteImage.addPath(item->nameDest());

        if (KIO::NetAccess::exists(deleteImage, KIO::NetAccess::DestinationSide, kapp->activeWindow())
                == true)
            KIO::NetAccess::del(deleteImage, kapp->activeWindow());
    }

    endProcess();
}

void BatchProcessImagesDialog::endProcess()
{
    m_convertStatus = PROCESS_DONE;
    enableWidgets(true);
    QTimer::singleShot(500, m_ui->m_progress, SLOT(hide()));
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
        NewDestUrl = fi->filePath().left(fi->filePath().lastIndexOf('.', -1)) + "_" + Temp + "."
                     + fi->filePath().section('.', -1);
    }
    while (Enumerator < 100 &&
           KIO::NetAccess::exists(NewDestUrl, KIO::NetAccess::SourceSide, kapp->activeWindow()) == true);

    if (Enumerator == 100)
        return QString();

    return (NewDestUrl.path());
}

void BatchProcessImagesDialog::enableWidgets(bool state)
{
    m_Type->setEnabled(state);
    m_ui->m_addImagesButton->setEnabled(state);
    m_ui->m_destinationUrl->setEnabled(state);
    m_labelType->setEnabled(state);
    m_listFiles->setEnabled(state);
    m_ui->m_optionsButton->setEnabled(state);
    m_ui->m_overWriteMode->setEnabled(state);
    m_ui->m_previewButton->setEnabled(state);
    m_ui->m_remImagesButton->setEnabled(state);
    m_ui->m_removeOriginal->setEnabled(state);
    m_ui->m_smallPreview->setEnabled(state);
}

void BatchProcessImagesDialog::readCommonSettings(const KConfigGroup& group)
{
    if (m_ui->m_smallPreview->isVisible())
    {
        m_ui->m_smallPreview->setChecked(group.readEntry("SmallPreview", "true") == "true");
    }

    m_ui->m_overWriteMode->setCurrentIndex(group.readEntry("OverWriteMode", 2));  // 'Rename' per default...

    m_ui->m_removeOriginal->setChecked(group.readEntry("RemoveOriginal", "false") == "true");
}

void BatchProcessImagesDialog::saveCommonSettings(KConfigGroup& group) const
{
    if (m_ui->m_smallPreview->isVisible())
    {
        group.writeEntry("SmallPreview", m_ui->m_smallPreview->isChecked());
    }
    group.writeEntry("OverWriteMode", m_ui->m_overWriteMode->currentIndex());
    group.writeEntry("RemoveOriginal", m_ui->m_removeOriginal->isChecked());
}

void BatchProcessImagesDialog::setPreviewOptionsVisible(bool visible)
{
    m_ui->m_previewButton->setVisible(visible);
    m_ui->m_smallPreview->setVisible(visible);
}

bool BatchProcessImagesDialog::smallPreview() const
{
    return m_ui->m_smallPreview->isChecked();
}

KUrl BatchProcessImagesDialog::destinationUrl() const
{
    return m_ui->m_destinationUrl->url();
}

void BatchProcessImagesDialog::setOptionBoxTitle(const QString& title)
{
    m_ui->m_optionGroupBox->setTitle(title);
}

QSize BatchProcessImagesDialog::sizeHint() const
{
    return QSize(650, 530);
}

}  // namespace KIPIBatchProcessImagesPlugin
