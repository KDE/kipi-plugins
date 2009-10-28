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

#include "filterimagesdialog.h"
#include "filterimagesdialog.moc"

// Qt includes

#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

// KDE includes

#include <kapplication.h>
#include <kcombobox.h>
#include <kconfig.h>
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
#include "pluginsversion.h"
#include "filteroptionsdialog.h"
#include "outputdialog.h"
#include "imagepreview.h"

namespace KIPIBatchProcessImagesPlugin
{

FilterImagesDialog::FilterImagesDialog(const KUrl::List& urlList, KIPI::Interface* interface, QWidget *parent)
        : BatchProcessImagesDialog(urlList, interface, i18n("Batch Image Filtering"), parent)
{
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Batch image filtering"),
                                           QByteArray(),
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to batch filter images.\n"
                                                 "This plugin uses the \"convert\" program from the \"ImageMagick\" package."),
                                           ki18n("(c) 2003-2009, Gilles Caulier\n"
                                                 "(c) 2007-2009, Aurélien Gateau"));

    m_about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author"),
                       "caulier dot gilles at gmail dot com");

    m_about->addAuthor(ki18n("Aurelien Gateau"), ki18n("Maintainer"),
                       "aurelien dot gateau at free dot fr");

    DialogUtils::setupHelpButton(this, m_about);
    //---------------------------------------------

    m_nbItem = m_selectedImageFiles.count();

    //---------------------------------------------

    setOptionBoxTitle(i18n("Image Filtering Options"));

    m_labelType->setText(i18n("Filter:"));

    m_Type->addItem(i18n("Add Noise"));       // 0
    m_Type->addItem(i18n("Antialias"));
    m_Type->addItem(i18n("Blur"));
    m_Type->addItem(i18n("Despeckle"));
    m_Type->addItem(i18n("Enhance"));         // 4
    m_Type->addItem(i18n("Median"));
    m_Type->addItem(i18n("Noise Reduction"));
    m_Type->addItem(i18n("Sharpen"));
    m_Type->addItem(i18n("Unsharp"));         // 8
    m_Type->setCurrentItem(i18n("Sharpen"));
    QString whatsThis = i18n(
                            "<p>Select here the filter type for your images:</p>"
                            "<p>"
                            "<b>Add noise</b>: add artificial noise to an image.<br/>"
                            "<b>Antialias</b>: remove pixel aliasing.<br/>"
                            "<b>Blur</b>: blur the image with a Gaussian operator.<br/>"
                            "<b>Despeckle</b>: reduces the speckle noise in an image while preserving the "
                            "edges of the original image.<br/>"
                            "<b>Enhance</b>: apply a digital filter to enhance a noisy image.<br/>"
                            "<b>Median</b>: apply a median filter to an image.<br/>"
                            "<b>Noise reduction</b>: reduce noise in an image. <br/>"
                            "<b>Sharpen</b>: sharpen the image with a Gaussian operator.<br/>"
                            "<b>Unsharp</b>: sharpen the image with an unsharp mask operator."
                            "</p>");

    m_Type->setWhatsThis(whatsThis);

    //---------------------------------------------

    readSettings();
    listImageFiles();
    slotTypeChanged(m_Type->currentIndex());
}

FilterImagesDialog::~FilterImagesDialog()
{
    delete m_about;
}

void FilterImagesDialog::slotHelp()
{
    KToolInvocation::invokeHelp("filterimages", "kipi-plugins");
}

void FilterImagesDialog::slotTypeChanged(int type)
{
    if (type == 1 ||       // Antialias
            type == 3 ||      // Despeckle
            type == 4)        // Enhance
        m_optionsButton->setEnabled(false);
    else
        m_optionsButton->setEnabled(true);
}

void FilterImagesDialog::slotOptionsClicked()
{
    int Type                           = m_Type->currentIndex();
    FilterOptionsDialog *optionsDialog = new FilterOptionsDialog(this, Type);

    if (Type == 0)  // Add noise
    {
        int index = optionsDialog->m_noiseType->findText(m_noiseType);
        if (index != -1) optionsDialog->m_noiseType->setCurrentIndex(index);
    }

    if (Type == 2)
    { // Blur
        optionsDialog->m_blurRadius->setValue(m_blurRadius);
        optionsDialog->m_blurDeviation->setValue(m_blurDeviation);
    }

    if (Type == 5)  // Median
        optionsDialog->m_medianRadius->setValue(m_medianRadius);

    if (Type == 6)  // Noise reduction
        optionsDialog->m_noiseRadius->setValue(m_noiseRadius);

    if (Type == 7)
    { // Sharpen
        optionsDialog->m_sharpenRadius->setValue(m_sharpenRadius);
        optionsDialog->m_sharpenDeviation->setValue(m_sharpenDeviation);
    }

    if (Type == 8)
    { // Unsharp
        optionsDialog->m_unsharpenRadius->setValue(m_unsharpenRadius);
        optionsDialog->m_unsharpenDeviation->setValue(m_unsharpenDeviation);
        optionsDialog->m_unsharpenPercent->setValue(m_unsharpenPercent);
        optionsDialog->m_unsharpenThreshold->setValue(m_unsharpenThreshold);
    }

    if (optionsDialog->exec() == KMessageBox::Ok)
    {
        if (Type == 0)  // Add noise
            m_noiseType = optionsDialog->m_noiseType->currentText();

        if (Type == 2)
        { // Blur
            m_blurRadius = optionsDialog->m_blurRadius->value();
            m_blurDeviation = optionsDialog->m_blurDeviation->value();
        }

        if (Type == 5)  // Median
            m_medianRadius = optionsDialog->m_medianRadius->value();

        if (Type == 6)  // Noise reduction
            m_noiseRadius = optionsDialog->m_noiseRadius->value();

        if (Type == 7) { // Sharpen
            m_sharpenRadius = optionsDialog->m_sharpenRadius->value();
            m_sharpenDeviation = optionsDialog->m_sharpenDeviation->value();
        }

        if (Type == 8)
        { // Unsharp
            m_unsharpenRadius = optionsDialog->m_unsharpenRadius->value();
            m_unsharpenDeviation = optionsDialog->m_unsharpenDeviation->value();
            m_unsharpenPercent = optionsDialog->m_unsharpenPercent->value();
            m_unsharpenThreshold = optionsDialog->m_unsharpenThreshold->value();
        }
    }

    delete optionsDialog;
}

void FilterImagesDialog::readSettings()
{
    // Read all settings from configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("FilterImages Settings");

    m_Type->setCurrentIndex(group.readEntry("FilterType", 7));      // Sharpen per default
    m_noiseType          = group.readEntry("NoiseType", i18n("Gaussian"));
    m_blurRadius         = group.readEntry("BlurRadius", 3);
    m_blurDeviation      = group.readEntry("BlurDeviation", 1);
    m_medianRadius       = group.readEntry("MedianRadius", 3);
    m_noiseRadius        = group.readEntry("NoiseRadius", 3);
    m_sharpenRadius      = group.readEntry("SharpenRadius", 3);
    m_sharpenDeviation   = group.readEntry("SharpenDeviation", 1);
    m_unsharpenRadius    = group.readEntry("UnsharpenRadius", 3);
    m_unsharpenDeviation = group.readEntry("UnsharpenDeviation", 1);
    m_unsharpenPercent   = group.readEntry("UnsharpenPercent", 3);
    m_unsharpenThreshold = group.readEntry("UnsharpenThreshold", 1);

    readCommonSettings(group);
}

void FilterImagesDialog::saveSettings()
{
    // Write all settings in configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("FilterImages Settings");

    group.writeEntry("FilterType", m_Type->currentIndex());
    group.writeEntry("NoiseType", m_noiseType);
    group.writeEntry("BlurRadius", m_blurRadius);
    group.writeEntry("BlurDeviation", m_blurDeviation);
    group.writeEntry("MedianRadius", m_medianRadius);
    group.writeEntry("NoiseRadius", m_noiseRadius);
    group.writeEntry("SharpenRadius", m_sharpenRadius);
    group.writeEntry("SharpenDeviation", m_sharpenDeviation);
    group.writeEntry("UnsharpenRadius", m_unsharpenRadius);
    group.writeEntry("UnsharpenDeviation", m_unsharpenDeviation);
    group.writeEntry("UnsharpenPercent", m_unsharpenPercent);
    group.writeEntry("UnsharpenThreshold", m_unsharpenThreshold);

    saveCommonSettings(group);
}

void FilterImagesDialog::initProcess(KProcess* proc, BatchProcessImagesItem *item,
                                     const QString& albumDest, bool previewMode)
{
    *proc << "convert";

    if (previewMode && smallPreview())
    {    // Preview mode and small preview enabled !
        *m_PreviewProc << "-crop" << "300x300+0+0";
        m_previewOutput.append(" -crop 300x300+0+0 ");
    }

    if (m_Type->currentIndex() == 0)
    { // Add noise
        QString Temp;
        if (m_noiseType == i18n("Uniform")) Temp = "Uniform";
        if (m_noiseType == i18n("Gaussian")) Temp = "Gaussian";
        if (m_noiseType == i18n("Multiplicative")) Temp = "Multiplicative";
        if (m_noiseType == i18n("Impulse")) Temp = "Impulse";
        if (m_noiseType == i18n("Laplacian")) Temp = "Laplacian";
        if (m_noiseType == i18n("Poisson")) Temp = "Poisson";
        *proc << "+noise" << Temp;
    }

    if (m_Type->currentIndex() == 1)
    { // Antialias
        *proc << "-antialias";
    }

    if (m_Type->currentIndex() == 2)
    { // Blur
        *proc << "-blur";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_blurRadius) + "x";
        Temp2.append(Temp.setNum(m_blurDeviation));
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 3)
    { // Despeckle
        *proc << "-despeckle";
    }

    if (m_Type->currentIndex() == 4)
    { // Enhance
        *proc << "-enhance";
    }

    if (m_Type->currentIndex() == 5)
    { // Median
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_medianRadius);
        *proc << "-median" << Temp2;
    }

    if (m_Type->currentIndex() == 6)
    { // Noise reduction
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_noiseRadius);
        *proc << "-noise" << Temp2;
    }

    if (m_Type->currentIndex() == 7)
    { // Sharpen
        *proc << "-sharpen";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_sharpenRadius) + "x";
        Temp2.append(Temp.setNum(m_sharpenDeviation));
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 8)
    { // Unsharp
        QString arg = QString("%1x%2+%3+%4")
            .arg(m_unsharpenRadius)
            .arg(m_unsharpenDeviation)
            .arg(m_unsharpenPercent / 100.)
            .arg(m_unsharpenThreshold / 100.);
        *proc << "-unsharp" << arg;
    }

    *proc << "-verbose";

    *proc << item->pathSrc();

    if (!previewMode)
    {   // No preview mode !
        *proc << albumDest + "/" + item->nameDest();
    }
}

}  // namespace KIPIBatchProcessImagesPlugin
