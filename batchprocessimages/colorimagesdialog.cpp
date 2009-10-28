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

#include "colorimagesdialog.h"
#include "colorimagesdialog.moc"

// Qt includes

#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QPointer>

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

#include "coloroptionsdialog.h"
#include "dialogutils.h"
#include "imagepreview.h"
#include "kpaboutdata.h"
#include "outputdialog.h"
#include "pluginsversion.h"

namespace KIPIBatchProcessImagesPlugin
{

ColorImagesDialog::ColorImagesDialog(const KUrl::List& urlList, KIPI::Interface* interface, QWidget *parent)
                 : BatchProcessImagesDialog(urlList, interface, i18n("Batch Image-Color Processing"), parent)
{
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Batch image-color enhancement"),
                                           QByteArray(),
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin for batch image-color enhancement.\n"
                                                 "This plugin use the \"convert\" program from the \"ImageMagick\" package."),
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

    setOptionBoxTitle(i18n("Image Coloring Options"));

    m_labelType->setText(i18n("Filter:"));

    m_Type->addItem(i18n("Decrease Contrast"));
    m_Type->addItem(i18n("Depth"));              // 1
    m_Type->addItem(i18n("Equalize"));
    m_Type->addItem(i18n("Fuzz"));               // 3
    m_Type->addItem(i18n("Gray Scales"));
    m_Type->addItem(i18n("Increase Contrast"));
    m_Type->addItem(i18n("Monochrome"));
    m_Type->addItem(i18n("Negate"));
    m_Type->addItem(i18n("Normalize"));
    m_Type->addItem(i18n("Segment"));            // 9
    m_Type->addItem(i18n("Trim"));
    m_Type->setCurrentItem(i18n("Normalize"));
    QString whatsThis = i18n(
                            "<p>Select here the color enhancement type for your images:</p>"
                            "<p>"
                            "<b>Decrease contrast</b>: reduce the image contrast. The algorithm "
                            "reduces the intensity difference between the lighter and darker elements "
                            "of the image.<br/>"
                            "<b>Depth</b>: change the color depth of the image.<br/>"
                            "<b>Equalize</b>: perform histogram equalization to the image.<br/>"
                            "<b>Fuzz</b>: merging colors within a distance, i.e. consider them to be equal.<br/>"
                            "<b>Gray scales</b>: convert color images to grayscale images.<br/>"
                            "<b>Increase contrast</b>: enhance the image contrast. The algorithm enhances "
                            "the intensity differences between the lighter and darker elements of the image.<br/>"
                            "<b>Monochrome</b>: transform the image to black and white.<br/>"
                            "<b>Negate</b>: replace every pixel with its complementary color. The red, green, "
                            "and blue intensities of an image are negated. White becomes black, yellow becomes "
                            "blue, etc.<br/>"
                            "<b>Normalize</b>: transform image to span the full range of color values. "
                            "This is a contrast enhancement technique. The algorithm enhances the contrast "
                            "of a colored image by adjusting the pixels color to span the entire range of "
                            "colors available.<br/>"
                            "<b>Segment</b>: segment an image by analyzing the histograms of the color "
                            "components and identifying units that are homogeneous with the fuzzy c-means "
                            "technique.<br/>"
                            "<b>Trim</b>: trim an image (fuzz reverse technique). The algorithm remove edges "
                            "that are the background color from the image."
                            "</p>");

    m_Type->setWhatsThis(whatsThis);

    //---------------------------------------------

    readSettings();
    listImageFiles();
    slotTypeChanged(m_Type->currentIndex());
}

ColorImagesDialog::~ColorImagesDialog()
{
    delete m_about;
}

void ColorImagesDialog::slotHelp()
{
    KToolInvocation::invokeHelp("colorimages", "kipi-plugins");
}

void ColorImagesDialog::slotTypeChanged(int type)
{
    if (type == 1 ||  // Depth
        type == 3 || // Fuzz
        type == 9    // Segment
       )
        m_optionsButton->setEnabled(true);
    else
        m_optionsButton->setEnabled(false);
}

void ColorImagesDialog::slotOptionsClicked()
{
    int Type = m_Type->currentIndex();
    QPointer<ColorOptionsDialog> optionsDialog = new ColorOptionsDialog(this, Type);

    if (Type == 1)  // Depth
    {
        int index = optionsDialog->m_depthValue->findText(m_depthValue);
        if (index != -1) optionsDialog->m_depthValue->setCurrentIndex(index);
    }

    if (Type == 3)  // Fuzz
        optionsDialog->m_fuzzDistance->setValue(m_fuzzDistance);

    if (Type == 9)
    { // Segment
        optionsDialog->m_segmentCluster->setValue(m_segmentCluster);
        optionsDialog->m_segmentSmooth->setValue(m_segmentSmooth);
    }

    if (optionsDialog->exec() == KMessageBox::Ok)
    {
        if (Type == 1)  // Depth
            m_depthValue = optionsDialog->m_depthValue->currentText();

        if (Type == 3)  // Fuzz
            m_fuzzDistance = optionsDialog->m_fuzzDistance->value();

        if (Type == 9)
        { // Segment
            m_segmentCluster = optionsDialog->m_segmentCluster->value();
            m_segmentSmooth = optionsDialog->m_segmentSmooth->value();
        }
    }

    delete optionsDialog;
}

void ColorImagesDialog::readSettings()
{
    // Read all settings from configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("ColorImages Settings");

    m_Type->setCurrentIndex(group.readEntry("ColorType", 8)); // Normalize per default.
    m_depthValue     = group.readEntry("DepthValue", "32");
    m_fuzzDistance   = group.readEntry("FuzzDistance", 3);
    m_segmentCluster = group.readEntry("SegmentCluster", 3);
    m_segmentSmooth  = group.readEntry("SegmentSmooth", 3);

    readCommonSettings(group);
}

void ColorImagesDialog::saveSettings()
{
    // Write all settings in configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("ColorImages Settings");

    group.writeEntry("ColorType", m_Type->currentIndex());
    group.writeEntry("DepthValue", m_depthValue);
    group.writeEntry("FuzzDistance", m_fuzzDistance);
    group.writeEntry("SegmentCluster", m_segmentCluster);
    group.writeEntry("SegmentSmooth", m_segmentSmooth);

    saveCommonSettings(group);
}

void ColorImagesDialog::initProcess(KProcess* proc, BatchProcessImagesItem *item,
                                    const QString& albumDest, bool previewMode)
{
    *proc << "convert";

    if (previewMode && smallPreview())
    {    // Preview mode and small preview enabled !
        *m_PreviewProc << "-crop" << "300x300+0+0";
        m_previewOutput.append(" -crop 300x300+0+0 ");
    }

    if (m_Type->currentIndex() == 0)
    { // Decrease contrast"
        *proc << "-contrast";
    }

    if (m_Type->currentIndex() == 1)
    { // Depth
        *proc << "-depth" << m_depthValue;
    }

    if (m_Type->currentIndex() == 2)
    { // Equalize
        *proc << "-equalize";
    }

    if (m_Type->currentIndex() == 3)
    { // Fuzz
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_fuzzDistance);
        *proc << "-fuzz" << Temp2;
    }

    if (m_Type->currentIndex() == 4)
    { // Gray scales
        *proc << "-type";
        *proc << "Grayscale";
    }

    if (m_Type->currentIndex() == 5)
    { // Increase contrast
        *proc << "+contrast";
    }

    if (m_Type->currentIndex() == 6)
    { // Monochrome
        *proc << "-monochrome";
    }

    if (m_Type->currentIndex() == 7)
    { // Negate
        *proc << "-negate";
    }

    if (m_Type->currentIndex() == 8)
    { // Normalize
        *proc << "-normalize";
    }

    if (m_Type->currentIndex() == 9)
    { // Segment
        *proc << "-segment";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_segmentCluster) + "x";
        Temp2.append(Temp.setNum(m_segmentSmooth));
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 10)
    { // Trim
        *proc << "-trim";
    }

    *proc << "-verbose";

    *proc << item->pathSrc();

    if (!previewMode)
    {   // No preview mode !
        *proc << albumDest + "/" + item->nameDest();
    }
}

}  // namespace KIPIBatchProcessImagesPlugin
