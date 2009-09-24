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

namespace KIPIBatchProcessImagesPlugin
{

ResizeImagesDialog::ResizeImagesDialog(KUrl::List urlList, KIPI::Interface* interface, QWidget *parent)
                  : BatchProcessImagesDialog(urlList, interface, i18n("Batch Resize Images"), parent)
{
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

    DialogUtils::setupHelpButton(this, m_about);

    //---------------------------------------------

    m_nbItem = m_selectedImageFiles.count();

    //---------------------------------------------

    setOptionBoxTitle(i18n("Image Resizing Options"));

    m_labelType->setText(i18n("Type:"));

    m_Type->insertItem(i18n("Proportional (1 dim.)"));  // 0
    m_Type->insertItem(i18n("Proportional (2 dim.)"));  // 1
    m_Type->insertItem(i18n("Non-Proportional"));       // 2
    m_Type->insertItem(i18n("Prepare to Print"));       // 3
    m_Type->setCurrentText(i18n("Proportional (1 dim.)"));
    QString whatsThis = i18n("<p>Select here the image-resize type.</p>");
    whatsThis = whatsThis + i18n("<p><b>Proportional (1 dim.)</b>: standard auto-resizing using one dimension. "
                                 "The width or the height of the images will be automatically "
                                 "selected, depending on the images' orientations. "
                                 "The images' aspect ratios are preserved.</p>");
    whatsThis = whatsThis + i18n("<p><b>Proportional (2 dim.)</b>: auto-resizing using two dimensions. "
                                 "The images' aspect ratio are preserved. You can use this, for example, "
                                 "to adapt your images' sizes to your screen size.</p>");
    whatsThis = whatsThis + i18n("<p><b>Non proportional</b>: non-proportional resizing using two dimensions. "
                                 "The images' aspect ratios are not preserved.</p>");
    whatsThis = whatsThis + i18n("<p><b>Prepare to print</b>: prepare the image for photographic printing. "
                                 "The user can set the print resolution and the photographic paper size. "
                                 "The target images will be adapted to the specified dimensions "
                                 "(included the background size, margin size, and background color).</p>");

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

void ResizeImagesDialog::slotHelp(void)
{
    KToolInvocation::invokeHelp("resizeimages", "kipi-plugins");
}

void ResizeImagesDialog::slotOptionsClicked(void)
{
    int Type = m_Type->currentItem();
    ResizeOptionsDialog *optionsDialog = new ResizeOptionsDialog(this, Type);

    if (Type == 0)
    {
        // Proportional (1 dim.)
        optionsDialog->m_quality->setValue(m_quality);
        optionsDialog->m_size->setValue(m_size);
        optionsDialog->m_resizeFilter->setCurrentText(m_resizeFilter);
    }
    if (Type == 1)
    {
        // Proportional (2 dim.)
        optionsDialog->m_quality->setValue(m_quality);
        optionsDialog->m_Width->setValue(m_Width);
        optionsDialog->m_Height->setValue(m_Height);
        optionsDialog->m_button_bgColor->setColor(m_bgColor);
        optionsDialog->m_resizeFilter->setCurrentText(m_resizeFilter);
        optionsDialog->m_Border->setValue(m_Border);
    }
    if (Type == 2)
    {
        // Non-proportional
        optionsDialog->m_quality->setValue(m_quality);
        optionsDialog->m_fixedWidth->setValue(m_fixedWidth);
        optionsDialog->m_fixedHeight->setValue(m_fixedHeight);
        optionsDialog->m_resizeFilter->setCurrentText(m_resizeFilter);
    }
    if (Type == 3)
    {
        // Prepare to print
        optionsDialog->m_quality->setValue(m_quality);
        optionsDialog->m_paperSize->setCurrentText(m_paperSize);
        optionsDialog->m_printDpi->setCurrentText(m_printDpi);
        optionsDialog->m_customXSize->setValue(m_customXSize);
        optionsDialog->m_customYSize->setValue(m_customYSize);
        optionsDialog->m_customDpi->setValue(m_customDpi);
        optionsDialog->m_button_backgroundColor->setColor(m_backgroundColor);
        optionsDialog->m_resizeFilter->setCurrentText(m_resizeFilter);
        optionsDialog->m_marging->setValue(m_marging);
        optionsDialog->m_customSettings->setChecked(m_customSettings);
    }

    if (optionsDialog->exec() == KMessageBox::Ok)
    {
        if (Type == 0)
        {
            // Proportional (1 dim.)
            m_quality = optionsDialog->m_quality->value();
            m_size = optionsDialog->m_size->value();
            m_resizeFilter = optionsDialog->m_resizeFilter->currentText();
        }
        if (Type == 1)
        {
            // Proportional (2 dim.)
            m_quality = optionsDialog->m_quality->value();
            m_Width = optionsDialog->m_Width->value();
            m_Height = optionsDialog->m_Height->value();
            m_bgColor = optionsDialog->m_button_bgColor->color();
            m_resizeFilter = optionsDialog->m_resizeFilter->currentText();
            m_Border = optionsDialog->m_Border->value();
        }
        if (Type == 2)
        {
            // Non-proportional
            m_quality = optionsDialog->m_quality->value();
            m_fixedWidth = optionsDialog->m_fixedWidth->value();
            m_fixedHeight = optionsDialog->m_fixedHeight->value();
            m_resizeFilter = optionsDialog->m_resizeFilter->currentText();
        }
        if (Type == 3)
        {
            // Prepare to print
            m_quality = optionsDialog->m_quality->value();
            m_paperSize = optionsDialog->m_paperSize->currentText();
            m_printDpi = optionsDialog->m_printDpi->currentText();
            m_customXSize = optionsDialog->m_customXSize->value();
            m_customYSize = optionsDialog->m_customYSize->value();
            m_customDpi = optionsDialog->m_customDpi->value();
            m_backgroundColor = optionsDialog->m_button_backgroundColor->color();
            m_resizeFilter = optionsDialog->m_resizeFilter->currentText();
            m_marging = optionsDialog->m_marging->value();
            m_customSettings = optionsDialog->m_customSettings->isChecked();
        }
    }

    delete optionsDialog;
}

void ResizeImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("ResizeImages Settings");

    m_Type->setCurrentIndex(group.readEntry("ResiseType", 3)); // Prepare to print per default.
    m_size = group.readEntry("Size", 640);
    m_resizeFilter = group.readEntry("ResizeFilter", "Lanczos");

    m_paperSize = group.readEntry("PaperSize", "10x15");
    m_printDpi = group.readEntry("PrintDpi", "300");
    m_customXSize = group.readEntry("CustomXSize", 10);
    m_customYSize = group.readEntry("CustomYSize", 15);
    m_customDpi = group.readEntry("CustomDpi", 300);
    m_backgroundColor = group.readEntry("BackgroundColor", QColor(Qt::white));
    m_marging = group.readEntry("MargingSize", 10);


    m_quality = group.readEntry("Quality", 75);
    m_Width = group.readEntry("Width", 1024);
    m_Height = group.readEntry("Height", 768);
    m_Border = group.readEntry("Border", 100);
    m_bgColor = group.readEntry("BgColor", QColor(Qt::black));

    m_fixedWidth = group.readEntry("FixedWidth", 640);
    m_fixedHeight = group.readEntry("FixedHeight", 480);

    if (group.readEntry("CustomSettings", "false") == "true")
        m_customSettings = true;
    else
        m_customSettings = false;

    readCommonSettings(group);
}

void ResizeImagesDialog::saveSettings(void)
{
    // Write all settings in configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("ResizeImages Settings");
    group.writeEntry("ResiseType", m_Type->currentItem());
    group.writeEntry("Size", m_size);
    group.writeEntry("ResizeFilter", m_resizeFilter);

    group.writeEntry("PaperSize", m_paperSize);
    group.writeEntry("PrintDpi", m_printDpi);
    group.writeEntry("CustomXSize", m_customXSize);
    group.writeEntry("CustomYSize", m_customYSize);
    group.writeEntry("CustomDpi", m_customDpi);
    group.writeEntry("BackgroundColor", m_backgroundColor);
    group.writeEntry("MargingSize", m_marging);
    group.writeEntry("CustomSettings", m_customSettings);

    group.writeEntry("Quality", m_quality);
    group.writeEntry("Width", m_Width);
    group.writeEntry("Height", m_Height);
    group.writeEntry("Border", m_Border);
    group.writeEntry("BgColor", m_bgColor);

    group.writeEntry("FixedWidth", m_fixedWidth);
    group.writeEntry("FixedHeight", m_fixedHeight);

    saveCommonSettings(group);
}

void ResizeImagesDialog::initProcess(KProcess* proc, BatchProcessImagesItem *item,
                                     const QString& albumDest, bool)
{
    QImage img;

    img.load(item->pathSrc());

    // Get image information.

    int w = img.width();
    int h = img.height();

    int Type = m_Type->currentItem();
    bool IncDec;
    int MargingSize;

    if (Type == 0)
    {
        // Proportional (1 dim.)
        *proc << "convert";
        IncDec = ResizeImage(w, h, m_size);

        *proc << "-resize";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(w) + "x";
        Temp2.append(Temp.setNum(h));
        *proc << Temp2;

        *proc << "-quality";
        QString Temp3;
        Temp3.setNum(m_quality);
        *proc << Temp3;

        if (IncDec == true)
        {
            // If the image is increased, enabled the filter.
            *proc << "-filter" << m_resizeFilter;
        }

        *proc << "-verbose";
        *proc << item->pathSrc() + "[0]";
        *proc << albumDest + "/" + item->nameDest();
    }

    if (Type == 1)
    {
        // Proportional (2 dim.)
        QString targetBackgroundSize;
        int ResizeCoeff;
        *proc << "convert";

        // Get the target image resizing dimensions with using the target size.
        ResizeCoeff = qMin(m_Width, m_Height);

        targetBackgroundSize = QString::number(m_Width) + "x" + QString::number(m_Height);

        *proc << "-verbose";

        *proc << item->pathSrc() + "[0]";

        // If the image is increased, enabled the filter.
        // jwienke: convert always uses a filter, so we can pass this option
        //          always with an appropriate default
        *proc << "-filter" << m_resizeFilter;

        // resize original image
        *proc << "-resize";
        QString Temp, Temp2;
        int MaxSize = qMax(1, ResizeCoeff - (2 * m_Border));
        QString MaxSizeString = QString::number(MaxSize);
        Temp2 = MaxSizeString + "x" + MaxSizeString;
        *proc << Temp2;

        // create border as desired
        *proc << "-bordercolor";
        Temp2 = "rgb(" + Temp.setNum(m_bgColor.red()) + ",";
        Temp2.append(Temp.setNum(m_bgColor.green()) + ",");
        Temp2.append(Temp.setNum(m_bgColor.blue()) + ")");
        *proc << Temp2;
        *proc << "-border" << QString::number(m_Width) + "x" + QString::number(m_Height);

        // center resized image on canvas
        *proc << "-gravity" << "Center";

        // set desired quality
        *proc << "-quality";
        QString Temp3;
        Temp3.setNum(m_quality);
        *proc << Temp3;

        // ImageMagick composite program do not preserve exif data from original.
        // Need to use "-profile" option for that.
        *proc << "-profile" << item->pathSrc();

        // crop image to canvas size
        *proc << "-crop" << targetBackgroundSize + "+0+0";

        // set destination
        *proc << albumDest + "/" + item->nameDest();

    }

    if (Type == 2)
    {
        // Non-proportional
        *proc << "convert";

        *proc << "-resize";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(m_fixedWidth) + "x";
        Temp2.append(Temp.setNum(m_fixedHeight) + "!");
        *proc << Temp2;

        if (m_fixedWidth > w || m_fixedHeight > h)
        {
            // If the image is increased, enabled the filter.
            *proc << "-filter" << m_resizeFilter;
        }

        *proc << "-quality";
        QString Temp3;
        Temp3.setNum(m_quality);
        *proc << Temp3;

        *proc << "-verbose";
        *proc << item->pathSrc() + "[0]";
        *proc << albumDest + "/" + item->nameDest();
    }

    if (Type == 3)
    {
        // Prepare to print
        if (m_customSettings == true)
        {
            MargingSize = (int)((float)(m_marging * m_customDpi) / (float)(25.4));

            if (w < h) 
            {
                // (w < h) because all paper dimensions are vertically gived !
                m_xPixels = (int)((float)(m_customXSize * m_customDpi) / (float)(2.54));
                m_yPixels = (int)((float)(m_customYSize * m_customDpi) / (float)(2.54));
            }
            else
            {
                m_yPixels = (int)((float)(m_customXSize * m_customDpi) / (float)(2.54));
                m_xPixels = (int)((float)(m_customYSize * m_customDpi) / (float)(2.54));
            }
        }
        else
        {
            QString Temp = m_printDpi;
            int Dpi = Temp.toInt();
            MargingSize = (int)((float)(m_marging * Dpi) / (float)(25.4));

            if (w < h)
            {
                // (w < h) because all paper dimensions are vertically given !
                Temp = m_paperSize.left(m_paperSize.find('x'));
                m_xPixels = (int)((float)(Temp.toInt() * Dpi) / (float)(2.54));
                Temp = m_paperSize.right(m_paperSize.find('x'));
                m_yPixels = (int)((float)(Temp.toInt() * Dpi) / (float)(2.54));
            }
            else
            {
                Temp = m_paperSize.left(m_paperSize.find('x'));
                m_yPixels = (int)((float)(Temp.toInt() * Dpi) / (float)(2.54));
                Temp = m_paperSize.right(m_paperSize.find('x'));
                m_xPixels = (int)((float)(Temp.toInt() * Dpi) / (float)(2.54));
            }
        }

        QString targetBackgroundSize;
        int ResizeCoeff;
        float RFactor;
        *proc << "composite";

        // Get the target image resizing dimensions with using the target paper size.

        if (m_xPixels < m_yPixels)
        {
            RFactor = (float)m_xPixels / (float)w;
            if (RFactor > 1.0) RFactor = (float)m_yPixels / (float)h;
            ResizeCoeff = (int)((float)h * RFactor);
        }
        else
        {
            RFactor = (float)m_yPixels / (float)h;
            if (RFactor > 1.0) RFactor = (float)m_xPixels / (float)w;
            ResizeCoeff = (int)((float)w * RFactor);
        }

        IncDec = ResizeImage(w, h, ResizeCoeff - MargingSize);
        targetBackgroundSize = QString::number(m_xPixels) + "x" + QString::number(m_yPixels);

        *proc << "-verbose" << "-gravity" << "Center";

        *proc << "-resize";
        QString Temp, Temp2;
        Temp2 = Temp.setNum(w) + "x";
        Temp2.append(Temp.setNum(h));
        *proc << Temp2;

        *proc << "-quality";
        QString Temp3;
        Temp3.setNum(m_quality);
        *proc << Temp3;

        if (IncDec == true)
        {
            // If the image is increased, enabled the filter.
            *proc << "-filter" << m_resizeFilter;
        }

        *proc << item->pathSrc();

        Temp2 = "xc:rgb(" + Temp.setNum(m_backgroundColor.red()) + ",";
        Temp2.append(Temp.setNum(m_backgroundColor.green()) + ",");
        Temp2.append(Temp.setNum(m_backgroundColor.blue()) + ")");
        *proc << Temp2;

        // ImageMagick composite program do not preserve exif data from original.
        // Need to use "-profile" option for that.

        *proc << "-profile" << item->pathSrc();

        *proc << "-resize" << targetBackgroundSize + "!";

        *proc << "-quality";
        QString Temp4;
        Temp4.setNum(m_quality);
        *proc << Temp4;

        *proc << albumDest + "/" + item->nameDest();
    }
}

bool ResizeImagesDialog::prepareStartProcess(BatchProcessImagesItem *item,
        const QString& /*albumDest*/)
{
    QImage img;

    if (img.load(item->pathSrc()) == false)
    {
        item->changeResult(i18n("Skipped."));
        item->changeError(i18n("image file format unsupported."));
        return false;
    }

    return true;
}

bool ResizeImagesDialog::ResizeImage(int &w, int &h, int SizeFactor)
{
    bool valRet;

    if (w > h)
    {
        h = (int)((double)(h * SizeFactor) / w);

        if (h == 0) h = 1;

        if (w < SizeFactor) valRet = true;
        else valRet = false;

        w = SizeFactor;
    }
    else
    {
        w = (int)((double)(w * SizeFactor) / h);

        if (w == 0) w = 1;

        if (h < SizeFactor) valRet = true;
        else valRet = false;

        h = SizeFactor;
    }

    return (valRet);  // Return true if image increased, else false.
}

}  // namespace KIPIBatchProcessImagesPlugin
