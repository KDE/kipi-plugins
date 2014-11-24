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

#include "resizeimagesdialog.moc"

// Qt includes

#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qimage.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlist.h>

// KDE includes

#include <kapplication.h>
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprocess.h>

// Local includes

#include "kpversion.h"
#include "resizeoptionsdialog.h"
#include "resizecommandbuilder.h"

namespace KIPIBatchProcessImagesPlugin
{

class ResizeTool
{
public:

    ResizeTool(const QString& localizedName, 
               ResizeCommandBuilder* const commandBuilder,
               ResizeOptionsBaseDialog* const dialog) :
        localizedName(localizedName), commandBuilder(commandBuilder), dialog(dialog)
    {
    }

    QString                  localizedName;
    ResizeCommandBuilder*    commandBuilder;
    ResizeOptionsBaseDialog* dialog;
};

// -----------------------------------------------------------------------------------------

class ResizeImagesDialog::ResizeImagesDialogPriv
{
public:

    const static QString RCNAME;
    const static QString RC_GROUP_NAME;

    ResizeImagesDialogPriv(ResizeImagesDialog* const dialog) :
        m_dialog(dialog)
    {
    }

    /**
     * Utility method that fills the type mapping. C defines the command builder
     * to use for the mapping, D the option dialog for this resize type.
     *
     * @param localizedName localized name of the mapping
     */
    template<class C, class D>
    void addResizeType(QString localizedName)
    {
        C *commandBuilder = new C(m_dialog);
        D *optionDialog   = new D(m_dialog, commandBuilder);
        optionDialog->layout();
        // somehow gcc needs these casts. Otherwise the templates will not work
        resizeTools << ResizeTool(localizedName,
                        dynamic_cast<ResizeCommandBuilder*> (commandBuilder),
                        dynamic_cast<ResizeOptionsBaseDialog*> (optionDialog));
    }

    /**
     * Returns the resize tool with the given localized name.
     *
     * @param name localized name of the resize tool
     * @return tool with that name or if there is no such tool a default one
     */
    ResizeTool getResizeToolByName(const QString& name)
    {
        foreach(const ResizeTool& tool, resizeTools)
        {
            if (tool.localizedName == name)
            {
                return tool;
            }
        }

        kError() << "Could not find a resize tool with localized name '"
                 << name << "'. Using first one.";

        return resizeTools[0];
    }

    /**
     * Maps the localized resize type names to their command builders and option
     * dialogs.
     */
    QList<ResizeTool> resizeTools;

private:

    ResizeImagesDialog* m_dialog;

};

const QString ResizeImagesDialog::ResizeImagesDialogPriv::RCNAME        = "kipirc";
const QString ResizeImagesDialog::ResizeImagesDialogPriv::RC_GROUP_NAME = "ResizeImages Settings";

typedef QList<ResizeTool>::iterator ResizeToolIterator;

ResizeImagesDialog::ResizeImagesDialog(const KUrl::List& urlList, QWidget* parent)
    : BatchProcessImagesDialog(urlList, i18n("Batch Resize Images"), parent),
    d(new ResizeImagesDialogPriv(this))
{

    kDebug() << "Creating resize dialog";

    // set up resize types
    d->addResizeType<OneDimResizeCommandBuilder, OneDimResizeOptionsDialog>(i18n("Proportional (1 dim.)"));
    d->addResizeType<TwoDimResizeCommandBuilder, TwoDimResizeOptionsDialog>(i18n("Proportional (2 dim.)"));
    d->addResizeType<NonProportionalResizeCommandBuilder, NonProportionalResizeOptionsDialog>(i18n("Non-Proportional"));
    d->addResizeType<PrintPrepareResizeCommandBuilder, PrintPrepareResizeOptionsDialog>(i18n("Prepare to Print"));

    //---------------------------------------------

    m_nbItem = m_selectedImageFiles.count();

    //---------------------------------------------

    setOptionBoxTitle(i18n("Image Resizing Options"));

    // insert resize tools in drop down box
    m_labelType->setText(i18n("Type:"));
    foreach(const ResizeTool &tool, d->resizeTools)
    {
        m_Type->addItem(tool.localizedName);
    }

    // build what's this text
    QString whatsThis = i18n("<p>Select here the image-resize type.</p>");
    foreach(const ResizeTool &tool, d->resizeTools)
    {
        whatsThis += tool.dialog->getWhatsThis();
    }
    m_Type->setWhatsThis(whatsThis);

    setPreviewOptionsVisible(false);

    //---------------------------------------------

    readSettings();
    listImageFiles();
}

ResizeImagesDialog::~ResizeImagesDialog()
{
    delete d;
}

void ResizeImagesDialog::slotOptionsClicked()
{
    d->getResizeToolByName(m_Type->currentText()).dialog->exec();
}

void ResizeImagesDialog::readSettings()
{
    // Read all settings from configuration file.

    KConfig config(ResizeImagesDialogPriv::RCNAME);
    KConfigGroup group = config.group(ResizeImagesDialogPriv::RC_GROUP_NAME);

    m_Type->setCurrentIndex(group.readEntry("ResizeType", 0));

    foreach(const ResizeTool &tool, d->resizeTools)
    {
        tool.dialog->readSettings(ResizeImagesDialogPriv::RCNAME,
                        ResizeImagesDialogPriv::RC_GROUP_NAME);
    }

    readCommonSettings(group);
}

void ResizeImagesDialog::saveSettings()
{
    // Write all settings in configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("ResizeImages Settings");

    group.writeEntry("ResizeType", m_Type->currentIndex());

    foreach(const ResizeTool &tool, d->resizeTools)
    {
        tool.dialog->saveSettings(ResizeImagesDialogPriv::RCNAME,
                        ResizeImagesDialogPriv::RC_GROUP_NAME);
    }

    saveCommonSettings(group);
}

void ResizeImagesDialog::initProcess(KProcess* proc, BatchProcessImagesItem *item,
                                     const QString& albumDest, bool)
{
    d->getResizeToolByName(m_Type->currentText()).commandBuilder->buildCommand(
                           proc, item, albumDest);
    kDebug() << "generated command line: " << proc->program();
}

bool ResizeImagesDialog::prepareStartProcess(BatchProcessImagesItem *item,
        const QString& albumDest)
{

    Q_UNUSED(albumDest);

    QImage img;
    if (!img.load(item->pathSrc()))
    {
        item->changeResult(i18n("Skipped."));
        item->changeError(i18n("image file format unsupported."));
        return false;
    }

    return true;
}

}  // namespace KIPIBatchProcessImagesPlugin
