/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef BATCHPROCESSIMAGESDIALOG_H
#define BATCHPROCESSIMAGESDIALOG_H

// Qt includes

#include <QString>

// KDE includes

#include <kio/job.h>
#include <kurl.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kptooldialog.h"
#include "batchprocessimagesitem.h"

class QFileInfo;
class QLabel;

class KComboBox;
class KProcess;

namespace Ui
{
    class BatchProcessImagesDialog;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesList;

class BatchProcessImagesDialog : public KPToolDialog
{
    Q_OBJECT

public:

    // Don't forget to add the 'm_Type' and 'm_labelType' implementation in the constructor of
    // children dialog class.

    BatchProcessImagesDialog(const KUrl::List& urlList, const QString& caption, QWidget* const parent = 0);
    ~BatchProcessImagesDialog();

protected:

    virtual QSize sizeHint() const;

protected:

    // --------------------------------------------------------------------------------------------------------
    // Widgets used by inherited dialogs
    QLabel*                  m_labelType;
    KComboBox*               m_Type;
    QPushButton*             m_optionsButton;

    BatchProcessImagesList*  m_listFiles;

    KConfig*                 m_config;

    int                      m_convertStatus;
    int                      m_progressStatus;
    int                      m_nbItem;

    KProcess*                m_ProcessusProc;
    KProcess*                m_PreviewProc;

    QTreeWidgetItemIterator* m_listFile2Process_iterator;

    QString                  m_ImagesFilesSort;
    QString                  m_newDir;
    QString                  m_commandLine;
    QString                  m_tmpFolder;
    QString                  m_previewOutput;
    KUrl::List               m_selectedImageFiles;

    // --------------------------------------------------------------------------------------------------------
    // Standards functions

    /**
    * Init common widgets from settings
    */
    void readCommonSettings(const KConfigGroup& group);

    /**
    * Save common widgets settings
    */
    void saveCommonSettings(KConfigGroup& group) const;

    void setPreviewOptionsVisible(bool visible);

    /**
     * Whether we should use large or small previews
     */
    bool smallPreview() const;

    /**
     * @return destination url
     */
    KUrl destinationUrl() const;

    /**
     * Set title of option group box
     */
    void setOptionBoxTitle(const QString& title);

    // Functions for ImageMAgick call implementations.

    void    processAborted(bool removeFlag = false); // Called when ImageMagick process is stopped by user.
    void    endPreview();                      // Called when preview process is done or stopped by user.

    // Called for to parse the files overwriting mode selected by user.

    int     overwriteMode();

    // Called for rename the target current image file name if already exist.

    QString RenameTargetImageFile(QFileInfo* fi);

    // --------------------------------------------------------------------------------------------------------
    // Standard virtual functions for re-implementation.

    // Called when ImageMagick process is started or for another re-implementation without
    // ImageMagick calls (like 'RenameImages' plugin).

    virtual bool startProcess();

    // Called when ImageMagick process is done or stopped by user or for another re-implementation without
    // ImageMagick calls (like 'RenameImages' plugin).

    virtual void endProcess();

    //  Functions for ImageMagick implementation calls only.

    // Called by 'startProcess' function at start. You can checking here some things about the current
    // 'item' before the 'startprocess' function execution.

    virtual bool prepareStartProcess(BatchProcessImagesItem* /*item*/, const QString& /*albumDest*/)
    {
        return true;
    };

    // Called to construct the ImageMagick command line used for to process or preview the image traitements.
    // If 'albumDest' = 0L ==> preview process.

    virtual void initProcess(KProcess* /*proc*/, BatchProcessImagesItem* /*item*/,
                             const QString& /*albumDest*/ = QString(),
                             bool  /*previewMode*/ = false) = 0;

    virtual void processDone() {};

    // Functions used for to read and to save the settings in the configuration file.

    virtual void readSettings() {};
    virtual void saveSettings() {};

    // Called for add or update the list of images to process in the dialog.

    virtual void listImageFiles();

    // Called by 'listImageFiles' for renamed if necessary the target image file name.

    virtual QString oldFileName2NewFileName(const QString& fileName)
    {
        return (fileName);
    };

protected Q_SLOTS:

    void slotProcessStop();

private Q_SLOTS:

    // --------------------------------------------------------------------------------------------------------
    // Standards slots

    void slotOk();

    void slotProcessStart();
    void slotReadyRead();
    void slotFinished();

    void slotPreview();
    void slotPreviewFinished();
    void slotPreviewReadyRead();
    void slotPreviewStop();

    void slotListDoubleClicked(QTreeWidgetItem* itemClicked);

    void slotAddDropItems(const QStringList& filesPath);

    void slotImageSelected(QTreeWidgetItem* item);
    void slotGotPreview(const KFileItem&, const QPixmap& pixmap);

    void slotImagesFilesButtonAdd();
    void slotImagesFilesButtonRem();

    // --------------------------------------------------------------------------------------------------------
    // Standards virtual slots for re-implementation

    virtual void slotOptionsClicked() {};      // Called when 'Options' button is clicked.
    virtual void slotTypeChanged(int) {};      // Called when the current type option is changed.

private:

    void setupUi();
    void enableWidgets(bool state);

    // --------------------------------------------------------------------------------------------------------
    // Standards widgets in the dialog
    Ui::BatchProcessImagesDialog* const m_ui;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // BATCHPROCESSIMAGESDIALOG_H
