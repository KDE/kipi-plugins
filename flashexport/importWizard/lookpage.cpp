/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-13
 * Description : a plugin to export images to flash
 *
 * Copyright (C) 2011-2013 by Veaceslav Munteanu <slavuttici at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "lookpage.h"

// Qt includes

#include <QLabel>
#include <QGroupBox>
#include <QLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QApplication>
#include <QIcon>

// KDE includes

#include <kcolorbutton.h>
#include <klocalizedstring.h>

// Libkdcraw includes

#include <KDCRAW/RWidgetUtils>
#include <KDCRAW/RNumInput>

// Local includes

#include "simpleviewer.h"
#include "kipiplugins_debug.h"

using namespace KDcrawIface;

namespace KIPIFlashExportPlugin
{

class LookPage::Private
{
public:

    Private()
    {
        thumbnailPosition = 0;
        textColor         = 0;
        backgroundColor   = 0;
        frameColor        = 0;
        frameWidth        = 0;
        stagePadding      = 0;
        thumbnailColumns  = 0;
        thumbnailRows     = 0;
        displayTime       = 0;
        imagePadding      = 0;
        bkgndInnerColor   = 0;
        bkgndOuterColor   = 0;
        cellDimension     = 0;
        zoomInPerc        = 0;
        zoomOutPerc       = 0;
        vbox              = 0;
        showFlipButton    = 0;
        useReloadButton   = 0;
        backColor         = 0;

    }

    RVBox*        vbox;
    QComboBox*    thumbnailPosition;

    KColorButton* textColor;
    KColorButton* backgroundColor;
    KColorButton* frameColor;

    RIntNumInput* frameWidth;
    RIntNumInput* stagePadding;
    RIntNumInput* thumbnailColumns;
    RIntNumInput* thumbnailRows;

    // ---Autoviewer------
    RIntNumInput* displayTime;
    RIntNumInput* imagePadding;

    // ---Tiltviewer------
    KColorButton* bkgndInnerColor;
    KColorButton* bkgndOuterColor;
    KColorButton* backColor;
    QCheckBox*    useReloadButton;
    QCheckBox*    showFlipButton;

    // ---Postcardviewer----
    RIntNumInput* cellDimension;
    RIntNumInput* zoomInPerc;
    RIntNumInput* zoomOutPerc;
};

LookPage::LookPage(KAssistantDialog* const dlg)
    : KPWizardPage(dlg, i18n("Configure appearance")),
      d(new Private)
{
}

LookPage::~LookPage()
{
    delete d;
}

void LookPage::setPageContent(int plugType)
{
    if(d->vbox)
    {
       removePageWidget(d->vbox);
       delete d->vbox;
    }

    d->vbox= new RVBox(this);
    /* Every plugin have it's own settings
    * Keep a page for every plugin for a easier maintenance
    * even if some settings are common
    */
    switch(plugType)
    {
        case 0:
        {
            //---Navigation Options -------------------------
            QGroupBox* box    = new QGroupBox(i18nc("Settings for flash export navigation", "Navigation"), d->vbox);
            QVBoxLayout* vlay = new QVBoxLayout(box);

            RHBox* hbox       = new RHBox;
            QLabel* label     = new QLabel(i18n("Thumbnail &Rows:"), hbox);
            d->thumbnailRows  = new RIntNumInput(hbox);
            d->thumbnailRows->setRange(1, 10, 1);
            d->thumbnailRows->setValue(3);
            d->thumbnailRows->setWhatsThis(i18n("Number of thumbnails rows"));
            label->setBuddy(d->thumbnailRows);

            RHBox* hbox2        = new RHBox;
            QLabel* label2      = new QLabel(i18n("Thumbnail &Columns:"), hbox2);
            d->thumbnailColumns = new RIntNumInput(hbox2);
            d->thumbnailColumns->setRange(1, 10, 1);
            d->thumbnailColumns->setValue(3);
            d->thumbnailColumns->setWhatsThis(i18n("Number of thumbnails columns"));
            label2->setBuddy(d->thumbnailColumns);

            RHBox* hbox3         = new RHBox;
            QLabel* label3       = new QLabel(i18n("Thumbnail &Position:"), hbox3);
            d->thumbnailPosition = new QComboBox(hbox3);
            QString pos_right  = i18nc("thumbnail position: right",  "Right");
            QString pos_left   = i18nc("thumbnail position: left",   "Left");
            QString pos_top    = i18nc("thumbnail position: top",    "Top");
            QString pos_bottom = i18nc("thumbnail position: bottom", "Bottom");
            d->thumbnailPosition->insertItem(SimpleViewerSettingsContainer::RIGHT,  pos_right);
            d->thumbnailPosition->insertItem(SimpleViewerSettingsContainer::LEFT,   pos_left);
            d->thumbnailPosition->insertItem(SimpleViewerSettingsContainer::TOP,    pos_top);
            d->thumbnailPosition->insertItem(SimpleViewerSettingsContainer::BOTTOM, pos_bottom);
            d->thumbnailPosition->setCurrentIndex(SimpleViewerSettingsContainer::RIGHT);
            label3->setBuddy(d->thumbnailPosition);

            vlay->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay->addWidget(hbox);
            vlay->addWidget(hbox2);
            vlay->addWidget(hbox3);

            //---Colors Options -----------------------------------------------

            QGroupBox* box2    = new QGroupBox(i18n("Colors"), d->vbox);
            QVBoxLayout* vlay2 = new QVBoxLayout(box2);

            RHBox* hbox4    = new RHBox;
            QLabel* label4  = new QLabel(i18n("&Text Color:"), hbox4);
            d->textColor    = new KColorButton(hbox4);
            d->textColor->setColor(QColor("#FFFFFF"));
            d->textColor->setWhatsThis(i18n("Color of title and caption text"));
            label4->setBuddy(d->textColor);

            RHBox* hbox5       = new RHBox;
            QLabel* label5     = new QLabel(i18n("&Background Color:"), hbox5);
            d->backgroundColor = new KColorButton(hbox5);
            d->backgroundColor->setColor(QColor("#181818"));
            label5->setBuddy(d->backgroundColor);

            RHBox* hbox6    = new RHBox;
            QLabel* label6  = new QLabel(i18n("&Frame Color:"), hbox6);
            d->frameColor   = new KColorButton(QColor("#FFFFFF"), hbox6);
            d->frameColor->setColor(QColor("#FFFFFF"));
            d->frameColor->setWhatsThis(i18n("Color of image frame, viewed icon, load bars, thumbnail arrows"));
            label6->setBuddy(d->frameColor);

            vlay2->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay2->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay2->addWidget(hbox4);
            vlay2->addWidget(hbox5);
            vlay2->addWidget(hbox6);

            // ----Style Options------------------------------------------------

            QGroupBox* box3    = new QGroupBox(i18n("Style"), d->vbox);
            QVBoxLayout* vlay3 = new QVBoxLayout(box3);

            RHBox* hbox7   = new RHBox;
            QLabel* label7 = new QLabel(i18n("&Frame Width:"), hbox7);
            d->frameWidth  = new RIntNumInput(hbox7);
            d->frameWidth->setRange(0, 10, 1);
            d->frameWidth->setValue(1);
            d->frameWidth->setWhatsThis(i18n("Width of image frame in pixels."));
            label7->setBuddy(d->frameWidth);

            RHBox* hbox8    = new RHBox;
            QLabel* label8  = new QLabel(i18n("&Stage Padding:"), hbox8);
            d->stagePadding = new RIntNumInput(hbox8);
            d->stagePadding->setRange(10, 100, 1);
            d->stagePadding->setValue(20);
            d->stagePadding->setWhatsThis(i18n("Distance between image and thumbnails in pixels."));
            label8->setBuddy(d->stagePadding);

            vlay3->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay3->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay3->addWidget(hbox7);
            vlay3->addWidget(hbox8);

            break;
        }
        case 1:
        {
            //---Navigation Options -------------------------
            QGroupBox* box    = new QGroupBox(i18nc("Settings for flash export navigation", "Navigation"), d->vbox);
            QVBoxLayout* vlay = new QVBoxLayout(box);

            RHBox* hbox          = new RHBox;
            QLabel* label        = new QLabel(i18n("&Display Time:"), hbox);
            d->displayTime = new RIntNumInput(hbox);
            d->displayTime->setRange(1, 15, 1);
            d->displayTime->setValue(3);
            d->displayTime->setWhatsThis(i18n("Number of seconds each image will display in auto-play mode."));
            label->setBuddy(d->displayTime);

            vlay->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay->addWidget(hbox);

            //---Colors Options -----------------------------------------------

            QGroupBox* box2    = new QGroupBox(i18n("Colors"), d->vbox);
            QVBoxLayout* vlay2 = new QVBoxLayout(box2);

            RHBox* hbox2       = new RHBox;
            QLabel* label2     = new QLabel(i18n("&Background Color:"), hbox2);
            d->backgroundColor = new KColorButton(hbox2);
            d->backgroundColor->setColor(QColor("#181818"));
            label2->setBuddy(d->backgroundColor);

            RHBox* hbox3    = new RHBox;
            QLabel* label3  = new QLabel(i18n("&Frame Color:"), hbox3);
            d->frameColor   = new KColorButton(QColor("#FFFFFF"), hbox3);
            d->frameColor->setColor(QColor("#FFFFFF"));
            d->frameColor->setWhatsThis(i18n("Color of image frame, viewed icon, load bars, thumbnail arrows"));
            label3->setBuddy(d->frameColor);

            vlay2->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay2->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay2->addWidget(hbox2);
            vlay2->addWidget(hbox3);

            // ----Style Options------------------------------------------------

            QGroupBox* box3    = new QGroupBox(i18n("Style"), d->vbox);
            QVBoxLayout* vlay3 = new QVBoxLayout(box3);

            RHBox* hbox4    = new RHBox;
            QLabel* label4  = new QLabel(i18n("&Frame Width:"), hbox4);
            d->frameWidth = new RIntNumInput(hbox4);
            d->frameWidth->setRange(0, 10, 1);
            d->frameWidth->setValue(1);
            d->frameWidth->setWhatsThis(i18n("Width of image frame in pixels."));
            label4->setBuddy(d->frameWidth);

            RHBox* hbox5    = new RHBox;
            QLabel* label5  = new QLabel(i18n("&Image Padding:"), hbox5);
            d->imagePadding = new RIntNumInput(hbox5);
            d->imagePadding->setRange(10, 100, 1);
            d->imagePadding->setValue(20);
            d->imagePadding->setWhatsThis(i18n("Distance between images in pixels"));
            label5->setBuddy(d->imagePadding);

            vlay3->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay3->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay3->addWidget(hbox4);
            vlay3->addWidget(hbox5);

            break;
        }
        case 2:
        {
            //---Navigation Options -------------------------
            QGroupBox* box    = new QGroupBox(i18nc("Settings for flash export navigation", "Navigation"), d->vbox);
            QVBoxLayout* vlay = new QVBoxLayout(box);

            RHBox* hbox          = new RHBox;
            QLabel* label        = new QLabel(i18n(" &Rows:"), hbox);
            d->thumbnailRows = new RIntNumInput(hbox);
            d->thumbnailRows->setRange(1, 10, 1);
            d->thumbnailRows->setValue(3);
            d->thumbnailRows->setWhatsThis(i18n("Number of rows of images to display."));
            label->setBuddy(d->thumbnailRows);

            RHBox* hbox2          = new RHBox;
            QLabel* label2        = new QLabel(i18n(" &Columns:"), hbox2);
            d->thumbnailColumns = new RIntNumInput(hbox2);
            d->thumbnailColumns->setRange(1, 10, 1);
            d->thumbnailColumns->setValue(3);
            d->thumbnailColumns->setWhatsThis(i18n("Number of columns of images to display."));
            label2->setBuddy(d->thumbnailColumns);

            d->showFlipButton = new QCheckBox(i18n("Show Flip Button"), this);
            d->showFlipButton->setChecked(true);
            d->showFlipButton->setWhatsThis(i18n("If you enable this option, "
                                            "the images' orientations will be set according "
                                            "to their Exif information."));
            d->useReloadButton = new QCheckBox(i18n("Use reload button"), this);
            d->useReloadButton->setChecked(true);
            d->useReloadButton->setWhatsThis(i18n("If you enable this option, "
                                            "the images' orientations will be set according "
                                            "to their Exif information."));

            vlay->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay->addWidget(hbox);
            vlay->addWidget(hbox2);
            vlay->addWidget(d->showFlipButton);
            vlay->addWidget(d->useReloadButton);

            //---Colors Options -----------------------------------------------

            QGroupBox* box2    = new QGroupBox(i18nc("Settings for flash export navigation", "Colors"), d->vbox);
            QVBoxLayout* vlay2 = new QVBoxLayout(box2);

            RHBox* hbox3    = new RHBox;
            QLabel* label3  = new QLabel(i18n("&Frame Color:"), hbox3);
            d->frameColor   = new KColorButton(QColor("#FFFFFF"), hbox3);
            d->frameColor->setColor(QColor("#FFFFFF"));
            d->frameColor->setWhatsThis(i18n("Color of image frame, viewed icon, load bars, thumbnail arrows"));
            label3->setBuddy(d->frameColor);

            RHBox* hbox4    = new RHBox;
            QLabel* label4  = new QLabel(i18n("Background &Inner Color:"), hbox4);
            d->bkgndInnerColor    = new KColorButton(hbox4);
            d->bkgndInnerColor->setColor(QColor("#FFFFFF"));
            d->bkgndInnerColor->setWhatsThis(i18n("Color of the stage background gradient center."));
            label4->setBuddy(d->bkgndInnerColor);

            RHBox* hbox5    = new RHBox;
            QLabel* label5  = new QLabel(i18n("Background &Outer Color:"), hbox5);
            d->bkgndOuterColor    = new KColorButton(hbox5);
            d->bkgndOuterColor->setColor(QColor("#FFFFFF"));
            d->bkgndOuterColor->setWhatsThis(i18n("Color of the stage background gradient edge."));
            label5->setBuddy(d->bkgndOuterColor);

            RHBox* hbox6    = new RHBox;
            QLabel* label6  = new QLabel(i18n("Image Back Color:"), hbox6);
            d->backColor    = new KColorButton(hbox6);
            d->backColor->setColor(QColor("#FFFFFF"));
            d->backColor->setWhatsThis(i18n("Image's back color when pressing flip button"));
            label6->setBuddy(d->bkgndOuterColor);

            vlay2->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay2->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay2->addWidget(hbox3);
            vlay2->addWidget(hbox4);
            vlay2->addWidget(hbox5);
            vlay2->addWidget(hbox6);

            break;
        }
        case 3:
        {
            //---Navigation Options -------------------------
            QGroupBox* box    = new QGroupBox(i18nc("Settings for flash export navigation", "Navigation"), d->vbox);
            QVBoxLayout* vlay = new QVBoxLayout(box);

            RHBox* hbox          = new RHBox;
            QLabel* label        = new QLabel(i18n("Cell Dimension:"), hbox);
            d->cellDimension = new RIntNumInput(hbox);
            d->cellDimension->setRange(500, 1500, 1);
            d->cellDimension->setValue(800);
            d->cellDimension->setWhatsThis(i18n("Dimension of displayed image cells. Please use a higher value if you set high image size"));
            label->setBuddy(d->cellDimension);

            RHBox* hbox2          = new RHBox;
            QLabel* label2        = new QLabel(i18n(" &Columns:"), hbox2);
            d->thumbnailColumns = new RIntNumInput(hbox2);
            d->thumbnailColumns->setRange(1, 10, 1);
            d->thumbnailColumns->setValue(3);
            d->thumbnailColumns->setWhatsThis(i18n("Number of thumbnails columns"));
            label2->setBuddy(d->thumbnailColumns);

            vlay->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay->addWidget(hbox);
            vlay->addWidget(hbox2);


            //---Colors Options -----------------------------------------------

            QGroupBox* box2    = new QGroupBox(i18n("Colors"), d->vbox);
            QVBoxLayout* vlay2 = new QVBoxLayout(box2);

            RHBox* hbox3    = new RHBox;
            QLabel* label3  = new QLabel(i18n("&Caption Color:"), hbox3);
            d->textColor    = new KColorButton(hbox3);
            d->textColor->setColor(QColor("#FFFFFF"));
            d->textColor->setWhatsThis(i18n("Color of title and caption text"));
            label3->setBuddy(d->textColor);

            RHBox* hbox4       = new RHBox;
            QLabel* label4     = new QLabel(i18n("&Background Color:"), hbox4);
            d->backgroundColor = new KColorButton(hbox4);
            d->backgroundColor->setColor(QColor("#181818"));
            label4->setBuddy(d->backgroundColor);

            RHBox* hbox5    = new RHBox;
            QLabel* label5  = new QLabel(i18n("&Frame Color:"), hbox5);
            d->frameColor   = new KColorButton(QColor("#FFFFFF"), hbox5);
            d->frameColor->setColor(QColor("#FFFFFF"));
            d->frameColor->setWhatsThis(i18n("Color of image frame, viewed icon, load bars, thumbnail arrows"));
            label5->setBuddy(d->frameColor);

            vlay2->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay2->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay2->addWidget(hbox3);
            vlay2->addWidget(hbox4);
            vlay2->addWidget(hbox5);

            // ----Style Options------------------------------------------------

            QGroupBox* box3    = new QGroupBox(i18n("Style"), d->vbox);
            QVBoxLayout* vlay3 = new QVBoxLayout(box3);

            RHBox* hbox6   = new RHBox;
            QLabel* label6 = new QLabel(i18n("&Zoom In Percentage:"), hbox6);
            d->zoomInPerc  = new RIntNumInput(hbox6);
            d->zoomInPerc->setRange(0, 100, 1);
            d->zoomInPerc->setValue(100);
            d->zoomInPerc->setWhatsThis(i18n("The amount of scale when zoomed in (percentage)"));
            label6->setBuddy(d->zoomInPerc);

            RHBox* hbox7   = new RHBox;
            QLabel* label7 = new QLabel(i18n("&Zoom Out Percentage:"), hbox7);
            d->zoomOutPerc = new RIntNumInput(hbox7);
            d->zoomOutPerc->setRange(0, 100, 1);
            d->zoomOutPerc->setValue(15);
            d->zoomOutPerc->setWhatsThis(i18n("The amount of scale when zoomed out (percentage)"));
            label7->setBuddy(d->zoomOutPerc);

            RHBox* hbox8   = new RHBox;
            QLabel* label8 = new QLabel(i18n("&Frame Width:"), hbox8);
            d->frameWidth  = new RIntNumInput(hbox8);
            d->frameWidth->setRange(0, 15, 1);
            d->frameWidth->setValue(3);
            d->frameWidth->setWhatsThis(i18n("The frame width"));
            label8->setBuddy(d->frameWidth);

            vlay3->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay3->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
            vlay3->addWidget(hbox6);
            vlay3->addWidget(hbox7);
            vlay3->addWidget(hbox8);

            break;
        }

        default:
            qCDebug(KIPIPLUGINS_LOG) << "Unknown plugin type";
            break;
    }

    // ------------------------------------------------------------------------

    setPageWidget(d->vbox);
    setLeftBottomPix(QIcon::fromTheme("kipi-flash").pixmap(128));
}

void LookPage::setSettings(const SimpleViewerSettingsContainer* const settings)
{

   switch(settings->plugType)
   {
       case 0:
       {
           d->thumbnailRows->setValue(settings->thumbnailRows);
           d->thumbnailColumns->setValue(settings->thumbnailColumns);
           d->thumbnailPosition->setCurrentIndex(settings->thumbnailPosition);
           d->textColor->setColor(settings->textColor);
           d->backgroundColor->setColor(settings->backgroundColor);
           d->frameColor->setColor(settings->frameColor);
           d->frameWidth->setValue(settings->frameWidth);
           d->stagePadding->setValue(settings->stagePadding);
           break;
       }
      case 1:
      {
           d->frameColor->setColor(settings->frameColor);
           d->frameWidth->setValue(settings->frameWidth);
           d->displayTime->setValue(settings->displayTime);
           d->imagePadding->setValue(settings->imagePadding);
           d->backgroundColor->setColor(settings->backgroundColor);
           break;
      } 
      case 2:
      {
           d->thumbnailRows->setValue(settings->thumbnailRows);
           d->thumbnailColumns->setValue(settings->thumbnailColumns);
           d->bkgndInnerColor->setColor(settings->bkgndInnerColor);
           d->bkgndOuterColor->setColor(settings->bkgndOuterColor);
           d->frameColor->setColor(settings->frameColor);
           d->backColor->setColor(settings->backColor);
           d->useReloadButton->setChecked(settings->useReloadButton);
           d->showFlipButton->setChecked(settings->showFlipButton);
           break;        
      }
      case 3:
      {
           d->frameColor->setColor(settings->frameColor);
           d->frameWidth->setValue(settings->frameWidth);
           d->thumbnailColumns->setValue(settings->thumbnailColumns);
           d->cellDimension->setValue(settings->cellDimension);
           d->zoomInPerc->setValue(settings->zoomInPerc);
           d->zoomOutPerc->setValue(settings->zoomOutPerc);
           d->backgroundColor->setColor(settings->backgroundColor);
           d->textColor->setColor(settings->textColor);

           break;
      }
      default:
          break;
   }
}

void LookPage::settings(SimpleViewerSettingsContainer* const settings)
{   
    switch(settings->plugType)
    {
        case 0:
        {
            settings->thumbnailRows     = d->thumbnailRows->value();
            settings->thumbnailColumns  = d->thumbnailColumns->value();
            settings->thumbnailPosition = (SimpleViewerSettingsContainer::ThumbPosition)d->thumbnailPosition->currentIndex();
            settings->stagePadding      = d->stagePadding->value();
            settings->textColor         = d->textColor->color();
            settings->backgroundColor   = d->backgroundColor->color();
            settings->frameColor        = d->frameColor->color();
            settings->frameWidth        = d->frameWidth->value();
            break;
        }
        case 1:
        {
            settings->displayTime       = d->displayTime->value();
            settings->imagePadding      = d->imagePadding->value();
            settings->frameColor        = d->frameColor->color();
            settings->frameWidth        = d->frameWidth->value();
            settings->backgroundColor   = d->backgroundColor->color();
            break;
        }
        case 2:
        {
            settings->frameColor        = d->frameColor->color();
            settings->thumbnailRows     = d->thumbnailRows->value();
            settings->thumbnailColumns  = d->thumbnailColumns->value();
            settings->bkgndInnerColor   = d->bkgndInnerColor->color();
            settings->bkgndOuterColor   = d->bkgndOuterColor->color();
            settings->backColor			= d->backColor->color();
            settings->showFlipButton    = d->showFlipButton->isChecked();
            settings->useReloadButton   = d->useReloadButton->isChecked();
            break;
        } 
        case 3:
        {
    
            settings->cellDimension     = d->cellDimension->value();
            settings->zoomInPerc        = d->zoomInPerc->value();
            settings->zoomOutPerc       = d->zoomOutPerc->value();
            settings->frameColor        = d->frameColor->color();
            settings->frameWidth        = d->frameWidth->value();
            settings->thumbnailColumns  = d->thumbnailColumns->value();
            settings->backgroundColor   = d->backgroundColor->color();
            settings->textColor         = d->textColor->color();
            break;
        }
    }
}

}   // namespace KIPIFlashExportPlugin
