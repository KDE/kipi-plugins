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

#include "resizeimagesdialog.h"
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
#include <ktoolinvocation.h>

// Local includes

#include "dialogutils.h"
#include "kpaboutdata.h"
#include "outputdialog.h"
#include "pluginsversion.h"
#include "resizeoptionsdialog.h"
#include "resizecommandbuilder.h"

namespace KIPIBatchProcessImagesPlugin
{

class ResizeTool
{
public:

    ResizeTool(const QString& localizedName, ResizeCommandBuilder *commandBuilder,
               ResizeOptionsBaseDialog *dialog) :
        localizedName(localizedName), commandBuilder(commandBuilder), dialog(dialog)
    {
    }

    QString localizedName;
    ResizeCommandBuilder *commandBuilder;
    ResizeOptionsBaseDialog *dialog;
};

class ResizeImagesDialogPriv
{

public:

    const static QString RCNAME;
    const static QString RC_GROUP_NAME;

    ResizeImagesDialogPriv(ResizeImagesDialog *dialog) :
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
        D *optionDialog = new D(m_dialog, commandBuilder);
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
        foreach(ResizeTool tool, resizeTools)
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

    KIPIPlugins::KPAboutData *aboutData;

private:

    ResizeImagesDialog *m_dialog;

};

const QString ResizeImagesDialogPriv::RCNAME = "kipirc";
const QString ResizeImagesDialogPriv::RC_GROUP_NAME = "ResizeImages Settings";

typedef QList<ResizeTool>::iterator ResizeToolIterator;

ResizeImagesDialog::ResizeImagesDialog(const KUrl::List& urlList, KIPI::Interface* interface, QWidget *parent)
                  : BatchProcessImagesDialog(urlList, interface, i18n("Batch Resize Images"), parent),
                    d(new ResizeImagesDialogPriv(this))
{

    kDebug() << "Creating resize dialog";

    // set up resize types
    d->addResizeType<OneDimResizeCommandBuilder, OneDimResizeOptionsDialog>(i18n("Proportional (1 dim.)"));
    d->addResizeType<TwoDimResizeCommandBuilder, TwoDimResizeOptionsDialog>(i18n("Proportional (2 dim.)"));
    d->addResizeType<NonProportionalResizeCommandBuilder, NonProportionalResizeOptionsDialog>(i18n("Non-Proportional"));
    d->addResizeType<PrintPrepareResizeCommandBuilder, PrintPrepareResizeOptionsDialog>(i18n("Prepare to Print"));

    // About data and help button.
    d->aboutData = new KIPIPlugins::KPAboutData(ki18n("Batch resize images"),
                                           QByteArray(),
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to batch-resize images.\n"
                                                 "This plugin uses the \"convert\" program from the \"ImageMagick\" package."),
                                           ki18n("(c) 2003-2009, Gilles Caulier\n"
                                                 "(c) 2007-2009, Aurélien Gateau"));

    d->aboutData->addAuthor(ki18n("Gilles Caulier"), ki18n("Author"),
                       "caulier dot gilles at gmail dot com");
    d->aboutData->addAuthor(ki18n("Aurelien Gateau"), ki18n("Maintainer"),
                       "aurelien dot gateau at free dot fr");
    d->aboutData->addAuthor(ki18n("Johannes Wienke"), ki18n("Maintainer"),
                           "languitar at semipol dot de");

    DialogUtils::setupHelpButton(this, d->aboutData);

    //---------------------------------------------

    m_nbItem = m_selectedImageFiles.count();

    //---------------------------------------------

    setOptionBoxTitle(i18n("Image Resizing Options"));

    // insert resize tools in drop down box
    m_labelType->setText(i18n("Type:"));
    foreach(ResizeTool tool, d->resizeTools)
    {
        m_Type->addItem(tool.localizedName);
    }

    // build what's this text
    QString whatsThis = i18n("<p>Select here the image-resize type.</p>");
    foreach(ResizeTool tool, d->resizeTools)
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
    delete d->aboutData;
    delete d;
}

void ResizeImagesDialog::slotHelp()
{
    KToolInvocation::invokeHelp("resizeimages", "kipi-plugins");
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

    foreach(ResizeTool tool, d->resizeTools)
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

    foreach(ResizeTool tool, d->resizeTools)
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
