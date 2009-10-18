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

#include <QGroupBox>
#include <QCheckBox>
#include <QImage>
#include <QLabel>
#include <QPushButton>

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

const QString ResizeImagesDialog::RCNAME = "kipirc";
const QString ResizeImagesDialog::RC_GROUP_NAME = "ResizeImages Settings";

ResizeImagesDialog::ResizeImagesDialog(KUrl::List urlList, KIPI::Interface* interface, QWidget *parent)
                  : BatchProcessImagesDialog(urlList, interface, i18n("Batch Resize Images"), parent)
{

    kDebug(51000) << "Creating resize dialog";

    // set up resize types
    addResizeType<OneDimResizeCommandBuilder, OneDimResizeOptionsDialog>(i18n("Proportional (1 dim.)"));
    addResizeType<TwoDimResizeCommandBuilder, TwoDimResizeOptionsDialog>(i18n("Proportional (2 dim.)"));
    addResizeType<NonProportionalResizeCommandBuilder, NonProportionalResizeOptionsDialog>(i18n("Non-Proportional"));
    addResizeType<PrintPrepareResizeCommandBuilder, PrintPrepareResizeOptionsDialog>(i18n("Prepare to Print"));

    // About data and help button.
    m_about = new KIPIPlugins::KPAboutData(ki18n("Batch resize images"),
                                           QByteArray(),
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to batch-resize images.\n"
                                                 "This plugin uses the \"convert\" program from the \"ImageMagick\" package."),
                                           ki18n("(c) 2003-2009, Gilles Caulier\n"
                                                 "(c) 2007-2009, Aurélien Gateau"));

    m_about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author"),
                       "caulier dot gilles at gmail dot com");
    m_about->addAuthor(ki18n("Aurelien Gateau"), ki18n("Maintainer"),
                       "aurelien dot gateau at free dot fr");
    m_about->addAuthor(ki18n("Johannes Wienke"), ki18n("Maintainer"),
                           "languitar at semipol dot de");

    DialogUtils::setupHelpButton(this, m_about);

    //---------------------------------------------

    m_nbItem = m_selectedImageFiles.count();

    //---------------------------------------------

    setOptionBoxTitle(i18n("Image Resizing Options"));

    m_labelType->setText(i18n("Type:"));
    m_Type->insertItems(0, QStringList(m_resizeTypeMapping.keys()));

    QString whatsThis = i18n("<p>Select here the image-resize type.</p>");
    for (QMap<QString, QPair<ResizeCommandBuilder*, ResizeOptionsBaseDialog*> >::iterator it =
                    m_resizeTypeMapping.begin(); it
                    != m_resizeTypeMapping.end(); ++it)
    {
        whatsThis += it.value().second->getWhatsThis();
    }
    m_Type->setWhatsThis(whatsThis);

    setPreviewOptionsVisible(false);

    //---------------------------------------------

    readSettings();
    listImageFiles();
}

ResizeImagesDialog::~ResizeImagesDialog()
{
    delete m_about;
}

template<class C, class D>
void ResizeImagesDialog::addResizeType(QString localizedName)
{

    C *commandBuilder = new C(this);
    D *optionDialog = new D(this, commandBuilder);
    optionDialog->layout();
    // somehow gcc needs these casts. Otherwise the templates will not work
    m_resizeTypeMapping.insert(localizedName, qMakePair(
                    dynamic_cast<ResizeCommandBuilder*> (commandBuilder),
                    dynamic_cast<ResizeOptionsBaseDialog*> (optionDialog)));

}

void ResizeImagesDialog::slotHelp(void)
{
    KToolInvocation::invokeHelp("resizeimages", "kipi-plugins");
}

void ResizeImagesDialog::slotOptionsClicked(void)
{

    QMap<QString, QPair<ResizeCommandBuilder*, ResizeOptionsBaseDialog*> >::iterator
                    it = m_resizeTypeMapping.find(m_Type->currentText());
    if (it == m_resizeTypeMapping.end())
    {
        kError(51000)
                        << "Could not find a mapping from the selected resize type "
                        << "to the appropriate data classes.";
        return;
    }

    ResizeOptionsBaseDialog *dialog = it->second;
    dialog->exec();

}

void ResizeImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    KConfig config(RCNAME);
    KConfigGroup group = config.group(RC_GROUP_NAME);

    m_Type->setCurrentIndex(group.readEntry("ResizeType", 0));

    for (QMap<QString, QPair<ResizeCommandBuilder*, ResizeOptionsBaseDialog*> >::iterator
                    it = m_resizeTypeMapping.begin(); it
                    != m_resizeTypeMapping.end(); ++it)
    {
        it.value().second->readSettings(RCNAME, RC_GROUP_NAME);
    }

    readCommonSettings(group);
}

void ResizeImagesDialog::saveSettings(void)
{
    // Write all settings in configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("ResizeImages Settings");

    group.writeEntry("ResizeType", m_Type->currentItem());

    for (QMap<QString, QPair<ResizeCommandBuilder*, ResizeOptionsBaseDialog*> >::iterator
                    it = m_resizeTypeMapping.begin(); it
                    != m_resizeTypeMapping.end(); ++it)
    {
        it.value().second->saveSettings(RCNAME, RC_GROUP_NAME);
    }

    saveCommonSettings(group);
}

void ResizeImagesDialog::initProcess(KProcess* proc, BatchProcessImagesItem *item,
                                     const QString& albumDest, bool)
{

    QMap<QString, QPair<ResizeCommandBuilder*, ResizeOptionsBaseDialog*> >::iterator
                    it = m_resizeTypeMapping.find(m_Type->currentText());
    if (it == m_resizeTypeMapping.end())
    {
        kError(51000) << "Could not find a mapping from the selected resize type "
                        << "to the appropriate data classes.";
        // TODO is this a good way to handle this error?
        return;
    }

    ResizeCommandBuilder *commandBuilder = it->first;
    commandBuilder->buildCommand(proc, item, albumDest);
    kDebug(51000) << "generated command line: " << proc->program();

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
