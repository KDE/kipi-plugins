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

#include "effectimagesdialog.h"
#include "effectimagesdialog.moc"

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
#include "effectoptionsdialog.h"
#include "imagepreview.h"
#include "kpaboutdata.h"
#include "outputdialog.h"
#include "pluginsversion.h"

namespace KIPIBatchProcessImagesPlugin
{

EffectImagesDialog::EffectImagesDialog(const KUrl::List& urlList, KIPI::Interface* interface, QWidget *parent)
                  : BatchProcessImagesDialog(urlList, interface, i18n("Batch Image Effects"), parent)
{
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Batch image effects"),
                                           QByteArray(),
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin for batch image-effect transformations.\n"
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

    setOptionBoxTitle(i18n("Image Effect Options"));

    m_labelType->setText(i18n("Effect:"));

    m_Type->addItem(i18n("Adaptive Threshold"));  // 0
    m_Type->addItem(i18n("Charcoal"));
    m_Type->addItem(i18n("Detect Edges"));
    m_Type->addItem(i18n("Emboss"));
    m_Type->addItem(i18n("Implode"));
    m_Type->addItem(i18n("Paint"));               // 5
    m_Type->addItem(i18n("Shade Light"));
    m_Type->addItem(i18n("Solarize"));
    m_Type->addItem(i18n("Spread"));
    m_Type->addItem(i18n("Swirl"));
    m_Type->addItem(i18n("Wave"));                // 10
    m_Type->setCurrentItem(i18n("Emboss"));
    QString whatsThis = i18n(
                            "<p>Select here the effect type for your images:</p>"
                            "<p>"
                            "<b>Adaptive threshold</b>: perform local adaptive thresholding. The algorithm "
                            "selects an individual threshold for each pixel based on the range of intensity "
                            "values in its local neighborhood. This allows for thresholding of an image whose "
                            "global intensity histogram does not contain distinctive peaks.<br/>"
                            "<b>Charcoal</b>: simulate a charcoal drawing.<br/>"
                            "<b>Detect edges</b>: detect edges within an image.<br/>"
                            "<b>Emboss</b>: returns a grayscale image with a three-dimensional effect. The "
                            "algorithm convolves the image with a Gaussian operator of the given radius and "
                            "standard deviation.<br/>"
                            "<b>Implode</b>: implode image pixels about the center.<br/>"
                            "<b>Paint</b>: applies a special effect filter that simulates an oil painting.<br/>"
                            "<b>Shade light</b>: shines a distant light on an image to create a three-dimensional "
                            "effect.<br/>"
                            "<b>Solarize</b>: negate all pixels above the threshold level. This algorithm produces a "
                            "solarization effect seen when exposing a photographic film to light during the development "
                            "process.<br/>"
                            "<b>Spread</b>: this is a special-effect method that randomly displaces each pixel in a "
                            "block defined by the radius parameter.<br/>"
                            "<b>Swirl</b>: swirls the pixels about the center of the image. <br/>"
                            "<b>Wave</b>: creates a \"ripple\" effect in the image by shifting the pixels vertically "
                            "along a sine wave."
                            "</p>");

    m_Type->setWhatsThis(whatsThis);

    //---------------------------------------------

    readSettings();
    listImageFiles();
}

EffectImagesDialog::~EffectImagesDialog()
{
    delete m_about;
}

void EffectImagesDialog::slotHelp(void)
{
    KToolInvocation::invokeHelp("effectimages",
                                "kipi-plugins");
}

void EffectImagesDialog::slotOptionsClicked(void)
{
    int Type = m_Type->currentIndex();
    EffectOptionsDialog *optionsDialog = new EffectOptionsDialog(this, Type);

    if (Type == 0)
    { // Adaptive threshold
        optionsDialog->m_latWidth->setValue(m_latWidth);
        optionsDialog->m_latHeight->setValue(m_latHeight);
        optionsDialog->m_latOffset->setValue(m_latOffset);
    }

    if (Type == 1)
    { // Charcoal
        optionsDialog->m_charcoalRadius->setValue(m_charcoalRadius);
        optionsDialog->m_charcoalDeviation->setValue(m_charcoalDeviation);
    }

    if (Type == 2)  // Detect edges
        optionsDialog->m_edgeRadius->setValue(m_edgeRadius);

    if (Type == 3)
    { // Emboss
        optionsDialog->m_embossRadius->setValue(m_embossRadius);
        optionsDialog->m_embossDeviation->setValue(m_embossDeviation);
    }

    if (Type == 4)  // Implode
        optionsDialog->m_implodeFactor->setValue(m_implodeFactor);

    if (Type == 5)  // Paint
        optionsDialog->m_paintRadius->setValue(m_paintRadius);

    if (Type == 6)
    { // Shade light
        optionsDialog->m_shadeAzimuth->setValue(m_shadeAzimuth);
        optionsDialog->m_shadeElevation->setValue(m_shadeElevation);
    }

    if (Type == 7)  // Solarize
        optionsDialog->m_solarizeFactor->setValue(m_solarizeFactor);

    if (Type == 8)  // Spread
        optionsDialog->m_spreadRadius->setValue(m_spreadRadius);

    if (Type == 9)  // Swirl
        optionsDialog->m_swirlDegrees->setValue(m_swirlDegrees);

    if (Type == 10)
    { // Wave
        optionsDialog->m_waveAmplitude->setValue(m_waveAmplitude);
        optionsDialog->m_waveLenght->setValue(m_waveLenght);
    }

    if (optionsDialog->exec() == KMessageBox::Ok)
    {
        if (Type == 0)
        { // Adaptive threshold
            m_latWidth = optionsDialog->m_latWidth->value();
            m_latHeight = optionsDialog->m_latHeight->value();
            m_latOffset = optionsDialog->m_latOffset->value();
        }

        if (Type == 1)
        { // Charcoal
            m_charcoalRadius = optionsDialog->m_charcoalRadius->value();
            m_charcoalDeviation = optionsDialog->m_charcoalDeviation->value();
        }

        if (Type == 2)  // Detect edges
            m_edgeRadius = optionsDialog->m_edgeRadius->value();

        if (Type == 3)
        { // Emboss
            m_embossRadius = optionsDialog->m_embossRadius->value();
            m_embossDeviation = optionsDialog->m_embossDeviation->value();
        }

        if (Type == 4)  // Implode
            m_implodeFactor = optionsDialog->m_implodeFactor->value();

        if (Type == 5)  // Paint
            m_paintRadius = optionsDialog->m_paintRadius->value();

        if (Type == 6)
        { // Shade light
            m_shadeAzimuth = optionsDialog->m_shadeAzimuth->value();
            m_shadeElevation = optionsDialog->m_shadeElevation->value();
        }

        if (Type == 7)  // Solarize
            m_solarizeFactor = optionsDialog->m_solarizeFactor->value();

        if (Type == 8)  // Spread
            m_spreadRadius = optionsDialog->m_spreadRadius->value();

        if (Type == 9)  // Swirl
            m_swirlDegrees = optionsDialog->m_swirlDegrees->value();

        if (Type == 10)
        { // Wave
            m_waveAmplitude = optionsDialog->m_waveAmplitude->value();
            m_waveLenght = optionsDialog->m_waveLenght->value();
        }
    }

    delete optionsDialog;
}

void EffectImagesDialog::readSettings(void)
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("EffectImages Settings");

    m_Type->setCurrentIndex(group.readEntry("EffectType", 3));   // Emboss per default.
    m_latWidth          = group.readEntry("LatWidth", 50);
    m_latHeight         = group.readEntry("LatHeight", 50);
    m_latOffset         = group.readEntry("LatOffset", 1);
    m_charcoalRadius    = group.readEntry("CharcoalRadius", 3);
    m_charcoalDeviation = group.readEntry("CharcoalDeviation", 3);
    m_edgeRadius        = group.readEntry("EdgeRadius", 3);
    m_embossRadius      = group.readEntry("EmbossRadius", 3);
    m_embossDeviation   = group.readEntry("EmbossDeviation", 3);
    m_implodeFactor     = group.readEntry("ImplodeFactor", 1);
    m_paintRadius       = group.readEntry("PaintRadius", 3);
    m_shadeAzimuth      = group.readEntry("ShadeAzimuth", 40);
    m_shadeElevation    = group.readEntry("ShadeElevation", 40);
    m_solarizeFactor    = group.readEntry("SolarizeFactor", 10);
    m_spreadRadius      = group.readEntry("SpreadRadius", 3);
    m_swirlDegrees      = group.readEntry("SwirlDegrees", 45);
    m_waveAmplitude     = group.readEntry("WaveAmplitude", 50);
    m_waveLenght        = group.readEntry("WaveLenght", 100);

    readCommonSettings(group);
}

void EffectImagesDialog::saveSettings(void)
{
    // Write all settings in configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("EffectImages Settings");
    group.writeEntry("EffectType", m_Type->currentIndex());

    group.writeEntry("LatWidth", m_latWidth);
    group.writeEntry("LatHeight", m_latHeight);
    group.writeEntry("LatOffset", m_latOffset);
    group.writeEntry("CharcoalRadius", m_charcoalRadius);
    group.writeEntry("CharcoalDeviation", m_charcoalDeviation);
    group.writeEntry("EdgeRadius", m_edgeRadius);
    group.writeEntry("EmbossRadius", m_embossRadius);
    group.writeEntry("EmbossDeviation", m_embossDeviation);
    group.writeEntry("ImplodeFactor", m_implodeFactor);
    group.writeEntry("PaintRadius", m_paintRadius);
    group.writeEntry("ShadeAzimuth", m_shadeAzimuth);
    group.writeEntry("ShadeElevation", m_shadeElevation);
    group.writeEntry("SolarizeFactor", m_solarizeFactor);
    group.writeEntry("SpreadRadius", m_spreadRadius);
    group.writeEntry("SwirlDegrees", m_swirlDegrees);
    group.writeEntry("WaveAmplitude", m_waveAmplitude);
    group.writeEntry("WaveLenght", m_waveLenght);

    saveCommonSettings(group);
}

void EffectImagesDialog::initProcess(KProcess* proc, BatchProcessImagesItem *item,
                                     const QString& albumDest, bool previewMode)
{
    *proc << "convert";

    if (previewMode && smallPreview())
    {    // Preview mode and small preview enabled !
        *m_PreviewProc << "-crop" << "300x300+0+0";
        m_previewOutput.append(" -crop 300x300+0+0 ");
    }

    if (m_Type->currentIndex() == 0)
    { // Adaptive threshold
        *proc << "-lat";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_latWidth) + "x";
        Temp2.append(Temp.setNum(m_latHeight) + "+");
        Temp2.append(Temp.setNum(m_latOffset));
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 1)
    { // Charcoal
        *proc << "-charcoal";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_charcoalRadius) + "x";
        Temp2.append(Temp.setNum(m_charcoalDeviation));
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 2)
    { // Detect edges
        *proc << "-edge";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_edgeRadius);
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 3)
    { // Emboss
        *proc << "-emboss";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_embossRadius) + "x";
        Temp2.append(Temp.setNum(m_embossDeviation));
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 4)
    { // Implode
        *proc << "-implode";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_implodeFactor);
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 5)
    { // Paint
        *proc << "-paint";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_paintRadius);
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 6)
    { // Shade light
        *proc << "-shade";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_shadeAzimuth) + "x";
        Temp2.append(Temp.setNum(m_shadeElevation));
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 7)
    { // Solarize
        *proc << "-solarize";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_solarizeFactor);
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 8)
    { // Spread
        *proc << "-spread";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_spreadRadius);
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 9)
    { // Swirl
        *proc << "-swirl";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_swirlDegrees);
        *proc << Temp2;
    }

    if (m_Type->currentIndex() == 10)
    { // Wave
        *proc << "-wave";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_waveAmplitude) + "x";
        Temp2.append(Temp.setNum(m_waveLenght));
        *proc << Temp2;
    }

    *proc << "-verbose";

    *proc << item->pathSrc();

    if (!previewMode) {   // No preview mode !
        *proc << albumDest + "/" + item->nameDest();
    }
}

}  // namespace KIPIBatchProcessImagesPlugin
